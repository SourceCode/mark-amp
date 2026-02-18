/// @file CrossPlatformPath.cpp
/// @brief V9 Phase 32 – Cross-platform path implementation.

#include "CrossPlatformPath.h"

#include <algorithm>
#include <cctype>

namespace markamp::core
{

auto CrossPlatformPath::is_case_sensitive() -> bool
{
#if defined(__linux__)
    return true;
#else
    return false; // macOS and Windows are case-insensitive
#endif
}

auto CrossPlatformPath::normalize(const std::string& path) -> std::string
{
    if (path.empty())
    {
        return path;
    }

    std::string result = path;

    // Normalize separators to platform-native
#if defined(_WIN32) || defined(_WIN64)
    std::replace(result.begin(), result.end(), '/', '\\');
#else
    std::replace(result.begin(), result.end(), '\\', '/');
#endif

    // Remove trailing separator (unless it's the root)
    if (result.size() > 1 && (result.back() == '/' || result.back() == '\\'))
    {
        result.pop_back();
    }

    // Collapse consecutive separators
    const char sep = get_separator();
    std::string collapsed;
    collapsed.reserve(result.size());
    bool prev_was_sep = false;
    for (char chr : result)
    {
        const bool is_sep = (chr == sep);
        if (is_sep && prev_was_sep)
        {
            continue;
        }
        collapsed += chr;
        prev_was_sep = is_sep;
    }

    return collapsed;
}

auto CrossPlatformPath::compare(const std::string& path_a, const std::string& path_b) -> bool
{
    if (is_case_sensitive())
    {
        return path_a == path_b;
    }

    // Case-insensitive comparison
    if (path_a.size() != path_b.size())
    {
        return false;
    }
    for (size_t idx = 0; idx < path_a.size(); ++idx)
    {
        if (std::tolower(static_cast<unsigned char>(path_a[idx])) !=
            std::tolower(static_cast<unsigned char>(path_b[idx])))
        {
            return false;
        }
    }
    return true;
}

auto CrossPlatformPath::get_separator() -> char
{
#if defined(_WIN32) || defined(_WIN64)
    return '\\';
#else
    return '/';
#endif
}

auto CrossPlatformPath::get_max_path_length() -> int
{
#if defined(_WIN32) || defined(_WIN64)
    return 260; // MAX_PATH
#else
    return 4096;  // PATH_MAX on most Unix systems
#endif
}

auto CrossPlatformPath::supports_long_paths() -> bool
{
#if defined(_WIN32) || defined(_WIN64)
    // Windows 10 1607+ supports long paths when enabled
    return true;
#else
    return false; // Not applicable on Unix
#endif
}

auto CrossPlatformPath::resolve_symlinks(const std::string& path) -> std::string
{
    std::error_code err_code;
    auto resolved = std::filesystem::canonical(path, err_code);
    if (err_code)
    {
        return path; // Return original on error
    }
    return resolved.string();
}

auto CrossPlatformPath::get_default_dialog_filters() -> std::vector<FileDialogFilter>
{
    return {
        {"Markdown Files", {"md", "markdown", "mdown", "mkd"}},
        {"MarkAmp Notebooks", {"markamp-nb"}},
        {"MarkAmp Boards", {"markamp-board"}},
        {"All Files", {"*"}},
    };
}

auto CrossPlatformPath::get_last_directory() const -> std::string
{
    return last_directory_;
}

void CrossPlatformPath::set_last_directory(const std::string& path)
{
    last_directory_ = path;
}

} // namespace markamp::core
