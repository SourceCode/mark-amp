/// @file CrossPlatformPath.h
/// @brief V9 Phase 32 – Cross-platform path handling, normalization, and file dialog helpers.

#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

/// File dialog filter entry (e.g. "Markdown Files (*.md)").
struct FileDialogFilter
{
    std::string description;
    std::vector<std::string> extensions; ///< Without leading dot, e.g. {"md", "markdown"}
};

/// Cross-platform path handling service.
/// Respects platform conventions for case sensitivity, path separators,
/// max path length, and symlink resolution.
class CrossPlatformPath
{
public:
    /// Returns true if the current platform's filesystem is case-sensitive (Linux).
    [[nodiscard]] static auto is_case_sensitive() -> bool;

    /// Normalize a path to the platform's canonical form.
    [[nodiscard]] static auto normalize(const std::string& path) -> std::string;

    /// Compare two paths respecting platform case sensitivity.
    [[nodiscard]] static auto compare(const std::string& path_a, const std::string& path_b) -> bool;

    /// Get the platform path separator ('/' or '\\').
    [[nodiscard]] static auto get_separator() -> char;

    /// Get the maximum path length for the platform.
    [[nodiscard]] static auto get_max_path_length() -> int;

    /// Check if Windows extended path support (\\?\) is available.
    [[nodiscard]] static auto supports_long_paths() -> bool;

    /// Resolve symlinks in a path.
    [[nodiscard]] static auto resolve_symlinks(const std::string& path) -> std::string;

    /// Get default file dialog filters for MarkAmp.
    [[nodiscard]] static auto get_default_dialog_filters() -> std::vector<FileDialogFilter>;

    /// Get the last-used directory for file dialogs.
    [[nodiscard]] auto get_last_directory() const -> std::string;

    /// Set the last-used directory for file dialogs.
    void set_last_directory(const std::string& path);

private:
    std::string last_directory_;
};

} // namespace markamp::core
