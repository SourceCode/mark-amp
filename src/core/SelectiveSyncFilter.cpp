// ============================================================================
// File: src/core/SelectiveSyncFilter.cpp
// Phase 27: Cloud Sync & Collaboration — Selective sync & ignore patterns
// ============================================================================

#include "SelectiveSyncFilter.h"

#include "Config.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

SelectiveSyncFilter::SelectiveSyncFilter(Config& config)
    : config_(config)
{
}

auto SelectiveSyncFilter::load_patterns(const std::string& syncignore_content) -> void
{
    user_patterns_.clear();

    std::istringstream stream(syncignore_content);
    std::string line;

    while (std::getline(stream, line))
    {
        // Trim whitespace.
        auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
        {
            continue;
        }
        auto end = line.find_last_not_of(" \t\r\n");
        line = line.substr(start, end - start + 1);

        // Skip comments and empty lines.
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        user_patterns_.push_back(line);
    }
}

auto SelectiveSyncFilter::add_pattern(const std::string& pattern) -> void
{
    auto iter = std::find(user_patterns_.begin(), user_patterns_.end(), pattern);
    if (iter == user_patterns_.end())
    {
        user_patterns_.push_back(pattern);
    }
}

auto SelectiveSyncFilter::remove_pattern(const std::string& pattern) -> bool
{
    auto iter = std::find(user_patterns_.begin(), user_patterns_.end(), pattern);
    if (iter != user_patterns_.end())
    {
        user_patterns_.erase(iter);
        return true;
    }
    return false;
}

auto SelectiveSyncFilter::patterns() const -> const std::vector<std::string>&
{
    return user_patterns_;
}

auto SelectiveSyncFilter::pattern_count() const -> int32_t
{
    return static_cast<int32_t>(user_patterns_.size());
}

auto SelectiveSyncFilter::should_sync(const std::string& relative_path) const -> bool
{
    const auto effective = effective_patterns();

    // Check negation patterns first (patterns starting with '!').
    bool negated = false;
    bool matched_ignore = false;

    for (const auto& pattern : effective)
    {
        if (!pattern.empty() && pattern[0] == '!')
        {
            // Negation pattern — re-include if it matches.
            const auto actual_pattern = pattern.substr(1);
            if (matches_pattern(relative_path, actual_pattern))
            {
                negated = true;
            }
        }
        else if (matches_pattern(relative_path, pattern))
        {
            matched_ignore = true;
        }
    }

    // If negated, it should be synced regardless of ignore.
    if (negated)
    {
        return true;
    }

    if (matched_ignore)
    {
        return false;
    }

    // Check folder exclusions.
    for (const auto& folder : excluded_folders_)
    {
        if (relative_path.starts_with(folder))
        {
            return false;
        }
    }

    return true;
}

auto SelectiveSyncFilter::exclude_folder(const std::string& folder_path) -> void
{
    auto iter = std::find(excluded_folders_.begin(), excluded_folders_.end(), folder_path);
    if (iter == excluded_folders_.end())
    {
        excluded_folders_.push_back(folder_path);
    }
}

auto SelectiveSyncFilter::include_folder(const std::string& folder_path) -> void
{
    auto iter = std::find(excluded_folders_.begin(), excluded_folders_.end(), folder_path);
    if (iter != excluded_folders_.end())
    {
        excluded_folders_.erase(iter);
    }
}

auto SelectiveSyncFilter::excluded_folders() const -> const std::vector<std::string>&
{
    return excluded_folders_;
}

auto SelectiveSyncFilter::is_folder_excluded(const std::string& folder_path) const -> bool
{
    return std::find(excluded_folders_.begin(), excluded_folders_.end(), folder_path) !=
           excluded_folders_.end();
}

auto SelectiveSyncFilter::add_cloud_only_file(const std::string& file_path) -> void
{
    auto iter = std::find(cloud_only_files_.begin(), cloud_only_files_.end(), file_path);
    if (iter == cloud_only_files_.end())
    {
        cloud_only_files_.push_back(file_path);
    }
}

auto SelectiveSyncFilter::remove_cloud_only_file(const std::string& file_path) -> bool
{
    auto iter = std::find(cloud_only_files_.begin(), cloud_only_files_.end(), file_path);
    if (iter != cloud_only_files_.end())
    {
        cloud_only_files_.erase(iter);
        return true;
    }
    return false;
}

auto SelectiveSyncFilter::cloud_only_files() const -> const std::vector<std::string>&
{
    return cloud_only_files_;
}

auto SelectiveSyncFilter::is_cloud_only(const std::string& file_path) const -> bool
{
    return std::find(cloud_only_files_.begin(), cloud_only_files_.end(), file_path) !=
           cloud_only_files_.end();
}

auto SelectiveSyncFilter::set_use_defaults(bool use_defaults) -> void
{
    use_defaults_ = use_defaults;
}

auto SelectiveSyncFilter::use_defaults() const -> bool
{
    return use_defaults_;
}

auto SelectiveSyncFilter::effective_patterns() const -> std::vector<std::string>
{
    std::vector<std::string> result;

    if (use_defaults_)
    {
        auto defaults = SyncIgnorePatterns::default_patterns();
        result.insert(result.end(), defaults.begin(), defaults.end());
    }

    result.insert(result.end(), user_patterns_.begin(), user_patterns_.end());
    return result;
}

auto SelectiveSyncFilter::serialize() const -> std::string
{
    std::ostringstream output;
    output << "# .markamp-syncignore\n";
    output << "# Files and patterns to exclude from cloud sync\n\n";

    if (!user_patterns_.empty())
    {
        for (const auto& pattern : user_patterns_)
        {
            output << pattern << "\n";
        }
    }

    return output.str();
}

auto SelectiveSyncFilter::matches_pattern(const std::string& path, const std::string& pattern)
    -> bool
{
    // Handle directory patterns (ending with '/').
    if (!pattern.empty() && pattern.back() == '/')
    {
        const auto dir_pattern = pattern.substr(0, pattern.size() - 1);
        // Check if path starts with directory name.
        if (path.starts_with(dir_pattern + "/") || path == dir_pattern)
        {
            return true;
        }
        // Also check if any path component matches.
        if (path.find("/" + dir_pattern + "/") != std::string::npos)
        {
            return true;
        }
        return false;
    }

    // Handle extension patterns (starting with '*').
    if (pattern.size() >= 2 && pattern[0] == '*' && pattern[1] == '.')
    {
        const auto extension = pattern.substr(1); // ".tmp"
        if (path.size() >= extension.size() &&
            path.substr(path.size() - extension.size()) == extension)
        {
            return true;
        }
        return false;
    }

    // Handle prefix patterns (starting with '~$').
    if (pattern.size() >= 2 && pattern[0] == '~' && pattern[1] == '$')
    {
        const auto prefix = pattern.substr(0, pattern.find('*'));
        // Check filename portion.
        auto last_slash = path.rfind('/');
        const auto filename =
            (last_slash != std::string::npos) ? path.substr(last_slash + 1) : path;
        return filename.starts_with(prefix);
    }

    // Handle ** glob (matches any path depth).
    if (pattern.find("**") != std::string::npos)
    {
        auto parts_pattern = pattern.substr(0, pattern.find("**"));
        return path.starts_with(parts_pattern);
    }

    // Exact match.
    if (path == pattern)
    {
        return true;
    }

    // Filename match — check just the filename component.
    auto last_slash = path.rfind('/');
    const auto filename = (last_slash != std::string::npos) ? path.substr(last_slash + 1) : path;
    return filename == pattern;
}

} // namespace markamp::core
