#include "RecentFiles.h"

#include <nlohmann/json.hpp>

#include <algorithm>

namespace markamp::core
{

RecentFiles::RecentFiles(Config& config)
    : config_(config)
{
    load();
}

void RecentFiles::add(const std::filesystem::path& path)
{
    // R19 Fix 14: weakly_canonical throws on invalid/inaccessible paths
    std::filesystem::path canonical;
    try
    {
        canonical = std::filesystem::weakly_canonical(path);
    }
    catch (const std::filesystem::filesystem_error&)
    {
        canonical = path; // Fallback to raw path
    }

    // Remove if already present (will re-add at front)
    std::erase_if(entries_,
                  [&canonical](const std::filesystem::path& entry)
                  {
                      std::error_code ec;
                      return std::filesystem::weakly_canonical(entry, ec) == canonical;
                  });

    // Insert at front (most recent first)
    entries_.insert(entries_.begin(), canonical);

    // Trim to max
    if (entries_.size() > kMaxEntries)
    {
        entries_.resize(kMaxEntries);
    }

    save();
}

void RecentFiles::remove(const std::filesystem::path& path)
{
    // R19 Fix 15: weakly_canonical throws on invalid/inaccessible paths
    std::filesystem::path canonical;
    try
    {
        canonical = std::filesystem::weakly_canonical(path);
    }
    catch (const std::filesystem::filesystem_error&)
    {
        canonical = path;
    }
    std::erase_if(entries_,
                  [&canonical](const std::filesystem::path& entry)
                  {
                      std::error_code ec;
                      return std::filesystem::weakly_canonical(entry, ec) == canonical;
                  });
    save();
}

void RecentFiles::clear()
{
    entries_.clear();
    save();
}

auto RecentFiles::list() const -> const std::vector<std::filesystem::path>&
{
    return entries_;
}

auto RecentFiles::max_entries() const -> std::size_t
{
    return kMaxEntries;
}

void RecentFiles::load()
{
    entries_.clear();
    auto json_str = config_.get_string("recent_files", "[]");

    try
    {
        auto json_array = nlohmann::json::parse(json_str);
        if (json_array.is_array())
        {
            for (const auto& item : json_array)
            {
                if (item.is_string())
                {
                    entries_.emplace_back(item.get<std::string>());
                }
            }
        }
    }
    catch (const nlohmann::json::exception&)
    {
        // Corrupted config — start fresh
        entries_.clear();
    }

    prune_nonexistent();
}

void RecentFiles::save()
{
    nlohmann::json json_array = nlohmann::json::array();
    for (const auto& entry : entries_)
    {
        json_array.push_back(entry.string());
    }
    config_.set("recent_files", json_array.dump());
    (void)config_.save(); // best-effort persist
}

void RecentFiles::prune_nonexistent()
{
    std::erase_if(entries_,
                  [](const std::filesystem::path& p)
                  {
                      std::error_code ec;
                      return !std::filesystem::exists(p, ec);
                  });
}

} // namespace markamp::core
