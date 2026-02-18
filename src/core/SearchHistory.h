#pragma once

/// @file SearchHistory.h
/// @brief V9 Phase 10 – Search history manager with persistence.
///
/// Header-only implementation of search history tracking with:
///   - Capped history (100 entries max)
///   - Deduplication (repeated queries update timestamp/count)
///   - Prefix-based autocomplete
///   - Serialize/deserialize for persistence
///   - History merging for workspace combination

#include "Search.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace markamp::core
{

// ────────────────────────────────────────────────────────────
// SearchHistoryEntry (compatible with SearchPanel.h definition)
// ────────────────────────────────────────────────────────────

/// A single search history entry.
struct SearchHistoryEntry
{
    /// The search query text.
    std::string query;

    /// The search method used.
    SearchMethod method{SearchMethod::Keyword};

    /// Number of results returned.
    int result_count{0};

    /// How many times this query has been executed.
    int execution_count{1};

    /// When the search was first executed.
    std::chrono::system_clock::time_point created_at;

    /// When the search was last executed.
    std::chrono::system_clock::time_point last_used_at;
};

// ────────────────────────────────────────────────────────────
// SearchHistoryManager
// ────────────────────────────────────────────────────────────

/// Manages search history with deduplication, capping, and persistence.
class SearchHistoryManager
{
public:
    /// Maximum number of history entries kept.
    static constexpr std::size_t kMaxEntries = 100;

    SearchHistoryManager() = default;

    // ── Core Operations ──────────────────────────────────────

    /// Add a search to history. If the same query+method already exists,
    /// updates its timestamp, result count, and increments execution_count.
    void add_entry(const std::string& query, SearchMethod method, int result_count)
    {
        if (query.empty())
        {
            return;
        }

        const auto now = std::chrono::system_clock::now();

        // Check for existing duplicate
        auto iter = std::find_if(entries_.begin(),
                                 entries_.end(),
                                 [&](const SearchHistoryEntry& entry)
                                 { return entry.query == query && entry.method == method; });

        if (iter != entries_.end())
        {
            // Update existing entry
            iter->result_count = result_count;
            iter->execution_count++;
            iter->last_used_at = now;

            // Move to front (most recent)
            auto updated = std::move(*iter);
            entries_.erase(iter);
            entries_.insert(entries_.begin(), std::move(updated));
            return;
        }

        // Create new entry
        SearchHistoryEntry entry;
        entry.query = query;
        entry.method = method;
        entry.result_count = result_count;
        entry.execution_count = 1;
        entry.created_at = now;
        entry.last_used_at = now;

        entries_.insert(entries_.begin(), std::move(entry));

        // Enforce cap
        if (entries_.size() > kMaxEntries)
        {
            entries_.resize(kMaxEntries);
        }
    }

    /// Get all history entries (most recent first).
    [[nodiscard]] auto get_history() const -> const std::vector<SearchHistoryEntry>&
    {
        return entries_;
    }

    /// Get the number of history entries.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return entries_.size();
    }

    /// Check if history is empty.
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return entries_.empty();
    }

    /// Clear all history entries.
    void clear()
    {
        entries_.clear();
    }

    /// Remove a specific entry by index.
    void remove_at(std::size_t index)
    {
        if (index < entries_.size())
        {
            entries_.erase(entries_.begin() + static_cast<std::ptrdiff_t>(index));
        }
    }

    /// Find entries matching a prefix (for autocomplete).
    [[nodiscard]] auto find_matching(const std::string& prefix) const
        -> std::vector<SearchHistoryEntry>
    {
        if (prefix.empty())
        {
            return entries_;
        }

        std::vector<SearchHistoryEntry> matches;
        const auto lower_prefix = to_lower(prefix);

        for (const auto& entry : entries_)
        {
            const auto lower_query = to_lower(entry.query);
            if (lower_query.find(lower_prefix) != std::string::npos)
            {
                matches.push_back(entry);
            }
        }

        return matches;
    }

    /// Get the most recent entry, if any.
    [[nodiscard]] auto most_recent() const -> std::optional<SearchHistoryEntry>
    {
        if (entries_.empty())
        {
            return std::nullopt;
        }
        return entries_.front();
    }

    // ── Persistence ──────────────────────────────────────────

    /// Serialize history to a string format.
    /// Format: one entry per line, fields separated by \t
    /// Fields: query \t method \t result_count \t execution_count \t created_epoch \t
    /// last_used_epoch
    [[nodiscard]] auto serialize() const -> std::string
    {
        std::ostringstream oss;
        oss << "SEARCH_HISTORY_V1\n";
        oss << "entries=" << entries_.size() << "\n";

        for (const auto& entry : entries_)
        {
            const auto created_epoch = std::chrono::duration_cast<std::chrono::seconds>(
                                           entry.created_at.time_since_epoch())
                                           .count();
            const auto last_used_epoch = std::chrono::duration_cast<std::chrono::seconds>(
                                             entry.last_used_at.time_since_epoch())
                                             .count();

            oss << escape_tabs(entry.query) << "\t" << static_cast<int>(entry.method) << "\t"
                << entry.result_count << "\t" << entry.execution_count << "\t" << created_epoch
                << "\t" << last_used_epoch << "\n";
        }

        return oss.str();
    }

    /// Deserialize history from a string.
    /// Returns true if deserialization was successful.
    auto deserialize(const std::string& data) -> bool
    {
        entries_.clear();

        if (data.empty())
        {
            return false;
        }

        std::istringstream iss(data);
        std::string line;

        // Check header
        if (!std::getline(iss, line) || line != "SEARCH_HISTORY_V1")
        {
            return false;
        }

        // Read entry count
        if (!std::getline(iss, line) || line.find("entries=") != 0)
        {
            return false;
        }

        const auto entry_count_str = line.substr(8);
        int entry_count = 0;
        try
        {
            entry_count = std::stoi(entry_count_str);
        }
        catch (...)
        {
            return false;
        }

        // Read entries
        for (int idx = 0; idx < entry_count && std::getline(iss, line); ++idx)
        {
            auto entry = parse_entry_line(line);
            if (entry.has_value())
            {
                entries_.push_back(std::move(entry.value()));
            }
        }

        return true;
    }

    /// Merge another history into this one, deduplicating by query+method.
    /// Keeps the entry with the most recent last_used_at.
    void merge(const SearchHistoryManager& other)
    {
        for (const auto& their_entry : other.entries_)
        {
            auto iter = std::find_if(entries_.begin(),
                                     entries_.end(),
                                     [&](const SearchHistoryEntry& entry) {
                                         return entry.query == their_entry.query &&
                                                entry.method == their_entry.method;
                                     });

            if (iter != entries_.end())
            {
                // Keep the more recently used entry
                if (their_entry.last_used_at > iter->last_used_at)
                {
                    iter->last_used_at = their_entry.last_used_at;
                    iter->result_count = their_entry.result_count;
                }
                iter->execution_count += their_entry.execution_count;
            }
            else
            {
                entries_.push_back(their_entry);
            }
        }

        // Sort by most recent and enforce cap
        std::sort(entries_.begin(),
                  entries_.end(),
                  [](const SearchHistoryEntry& lhs, const SearchHistoryEntry& rhs)
                  { return lhs.last_used_at > rhs.last_used_at; });

        if (entries_.size() > kMaxEntries)
        {
            entries_.resize(kMaxEntries);
        }
    }

private:
    std::vector<SearchHistoryEntry> entries_;

    /// Case-insensitive helper.
    [[nodiscard]] static auto to_lower(const std::string& str) -> std::string
    {
        std::string result = str;
        std::transform(result.begin(),
                       result.end(),
                       result.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
        return result;
    }

    /// Escape tab characters in a string for serialization.
    [[nodiscard]] static auto escape_tabs(const std::string& str) -> std::string
    {
        std::string result;
        result.reserve(str.size());
        for (char chr : str)
        {
            if (chr == '\t')
            {
                result += "\\t";
            }
            else if (chr == '\\')
            {
                result += "\\\\";
            }
            else if (chr == '\n')
            {
                result += "\\n";
            }
            else
            {
                result += chr;
            }
        }
        return result;
    }

    /// Unescape tab characters from serialized string.
    [[nodiscard]] static auto unescape_tabs(const std::string& str) -> std::string
    {
        std::string result;
        result.reserve(str.size());
        for (std::size_t idx = 0; idx < str.size(); ++idx)
        {
            if (str[idx] == '\\' && idx + 1 < str.size())
            {
                if (str[idx + 1] == 't')
                {
                    result += '\t';
                    ++idx;
                }
                else if (str[idx + 1] == '\\')
                {
                    result += '\\';
                    ++idx;
                }
                else if (str[idx + 1] == 'n')
                {
                    result += '\n';
                    ++idx;
                }
                else
                {
                    result += str[idx];
                }
            }
            else
            {
                result += str[idx];
            }
        }
        return result;
    }

    /// Parse a single serialized entry line.
    [[nodiscard]] static auto parse_entry_line(const std::string& line)
        -> std::optional<SearchHistoryEntry>
    {
        // Split by tabs
        std::vector<std::string> fields;
        std::size_t start = 0;
        for (std::size_t pos = 0; pos <= line.size(); ++pos)
        {
            if (pos == line.size() || line[pos] == '\t')
            {
                fields.push_back(line.substr(start, pos - start));
                start = pos + 1;
            }
        }

        if (fields.size() < 6)
        {
            return std::nullopt;
        }

        SearchHistoryEntry entry;
        entry.query = unescape_tabs(fields[0]);

        try
        {
            entry.method = static_cast<SearchMethod>(std::stoi(fields[1]));
            entry.result_count = std::stoi(fields[2]);
            entry.execution_count = std::stoi(fields[3]);

            const auto created_secs = std::stoll(fields[4]);
            const auto last_used_secs = std::stoll(fields[5]);

            entry.created_at =
                std::chrono::system_clock::time_point(std::chrono::seconds(created_secs));
            entry.last_used_at =
                std::chrono::system_clock::time_point(std::chrono::seconds(last_used_secs));
        }
        catch (...)
        {
            return std::nullopt;
        }

        return entry;
    }
};

} // namespace markamp::core
