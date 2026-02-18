/// WriteAheadLog.cpp — V7 Phase 32: Write-ahead logging

#include "WriteAheadLog.h"

#include <fstream>

namespace markamp::core
{

auto WriteAheadLog::open(const std::filesystem::path& path) -> Result<void>
{
    std::lock_guard lock(mutex_);

    if (is_open_)
    {
        close();
    }

    path_ = path;
    entries_.clear();
    sequence_ = 0;
    is_open_ = true;

    // If file exists, read existing entries
    if (std::filesystem::exists(path_))
    {
        std::ifstream file(path_);
        if (!file.is_open())
        {
            return std::unexpected(
                make_io_error("Cannot open WAL file: " + path_.string(), ErrorCode::FileReadError));
        }

        // Simple line-based format: SEQ|TS|OP|KEY|VALUE
        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty())
            {
                continue;
            }

            WalEntry entry{};
            size_t pos = 0;

            // Parse sequence
            auto sep1 = line.find('|', pos);
            if (sep1 == std::string::npos)
            {
                continue;
            }
            entry.sequence = std::stoull(line.substr(pos, sep1 - pos));
            pos = sep1 + 1;

            // Parse timestamp
            auto sep2 = line.find('|', pos);
            if (sep2 == std::string::npos)
            {
                continue;
            }
            entry.timestamp_ms = std::stoll(line.substr(pos, sep2 - pos));
            pos = sep2 + 1;

            // Parse operation
            auto sep3 = line.find('|', pos);
            if (sep3 == std::string::npos)
            {
                continue;
            }
            entry.operation = static_cast<WalOperation>(std::stoi(line.substr(pos, sep3 - pos)));
            pos = sep3 + 1;

            // Parse key
            auto sep4 = line.find('|', pos);
            if (sep4 == std::string::npos)
            {
                continue;
            }
            entry.key = line.substr(pos, sep4 - pos);
            pos = sep4 + 1;

            // Rest is value
            entry.value = line.substr(pos);

            entries_.push_back(std::move(entry));
            if (entry.sequence > sequence_)
            {
                sequence_ = entry.sequence;
            }
        }
    }

    return {};
}

auto WriteAheadLog::append(WalOperation operation, const std::string& key, const std::string& value)
    -> Result<uint64_t>
{
    std::lock_guard lock(mutex_);

    if (!is_open_)
    {
        return std::unexpected(make_io_error("WAL is not open", ErrorCode::IoError));
    }

    ++sequence_;
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();

    WalEntry entry{
        .sequence = sequence_,
        .timestamp_ms = now_ms,
        .operation = operation,
        .key = key,
        .value = value,
    };

    // Append to file
    std::ofstream file(path_, std::ios::app);
    if (!file.is_open())
    {
        return std::unexpected(
            make_io_error("Cannot write to WAL: " + path_.string(), ErrorCode::FileWriteError));
    }

    file << entry.sequence << '|' << entry.timestamp_ms << '|' << static_cast<int>(entry.operation)
         << '|' << entry.key << '|' << entry.value << '\n';
    file.flush();

    entries_.push_back(std::move(entry));

    // Auto-compact if needed
    if (entries_.size() > max_entries_)
    {
        // Ignoring compact result during auto-compact
        (void)compact();
    }

    return sequence_;
}

auto WriteAheadLog::replay(std::function<void(const WalEntry&)> handler) const -> Result<size_t>
{
    std::lock_guard lock(mutex_);

    for (const auto& entry : entries_)
    {
        handler(entry);
    }

    return entries_.size();
}

auto WriteAheadLog::compact() -> Result<size_t>
{
    // Keep only the latest entry per key (for Set/Delete ops)
    std::unordered_map<std::string, size_t> latest_by_key;
    for (size_t idx = 0; idx < entries_.size(); ++idx)
    {
        if (entries_[idx].operation == WalOperation::Set ||
            entries_[idx].operation == WalOperation::Delete)
        {
            latest_by_key[entries_[idx].key] = idx;
        }
    }

    std::vector<WalEntry> compacted;
    compacted.reserve(latest_by_key.size());

    for (size_t idx = 0; idx < entries_.size(); ++idx)
    {
        auto key_it = latest_by_key.find(entries_[idx].key);
        if (key_it != latest_by_key.end() && key_it->second == idx)
        {
            // Keep latest entry for this key, skip Deletes
            if (entries_[idx].operation != WalOperation::Delete)
            {
                compacted.push_back(entries_[idx]);
            }
        }
        else if (entries_[idx].operation == WalOperation::Checkpoint ||
                 entries_[idx].operation == WalOperation::Clear)
        {
            compacted.push_back(entries_[idx]);
        }
    }

    auto removed = entries_.size() - compacted.size();
    entries_ = std::move(compacted);

    // Rewrite WAL file
    if (!path_.empty())
    {
        std::ofstream file(path_, std::ios::trunc);
        if (file.is_open())
        {
            for (const auto& entry : entries_)
            {
                file << entry.sequence << '|' << entry.timestamp_ms << '|'
                     << static_cast<int>(entry.operation) << '|' << entry.key << '|' << entry.value
                     << '\n';
            }
        }
    }

    return removed;
}

void WriteAheadLog::close()
{
    is_open_ = false;
    entries_.clear();
    sequence_ = 0;
}

} // namespace markamp::core
