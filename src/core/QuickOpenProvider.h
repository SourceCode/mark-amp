/// @file QuickOpenProvider.h
/// @brief V9 Phase 36 Task 7 — Quick Open file picker mode for command palette.
///
/// Provides workspace file listing with fuzzy search, recent-files boosting,
/// and inline file preview (first N lines). Used by CommandPalette in
/// kQuickOpen mode (Cmd+P).
#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// QuickOpenItem — a single file entry in the quick open picker
// ============================================================================

struct QuickOpenItem
{
    std::filesystem::path file_path;                     // Full path to the file
    std::string display_name;                            // File basename
    std::string relative_path;                           // Path relative to workspace root
    std::string file_type_icon;                          // Icon identifier based on extension
    std::chrono::system_clock::time_point last_accessed; // For recent-files boost
};

// ============================================================================
// QuickOpenProvider — lists and searches workspace files
// ============================================================================

/// Provides file listing and fuzzy search for the Quick Open palette mode.
///
/// Usage:
/// ```cpp
/// QuickOpenProvider provider;
/// provider.set_workspace_root("/path/to/workspace");
/// provider.scan_workspace();
/// auto results = provider.fuzzy_match("readme");
/// auto preview = provider.get_file_preview(results[0].file_path, 10);
/// ```
class QuickOpenProvider
{
public:
    QuickOpenProvider() = default;

    /// Set the workspace root directory.
    void set_workspace_root(const std::filesystem::path& root);

    /// Scan the workspace and populate the file list.
    void scan_workspace();

    /// Get all workspace files.
    [[nodiscard]] auto all_files() const -> const std::vector<QuickOpenItem>&;

    /// Fuzzy match against file names. Returns results sorted by relevance.
    /// Recent files are boosted in ranking.
    [[nodiscard]] auto fuzzy_match(const std::string& query, int max_results = 50) const
        -> std::vector<QuickOpenItem>;

    /// Get the first N lines of a file for inline preview.
    [[nodiscard]] static auto get_file_preview(const std::filesystem::path& file_path,
                                               int max_lines = 10) -> std::vector<std::string>;

    /// Record that a file was accessed (for recent-files ranking).
    void record_access(const std::filesystem::path& file_path);

    /// Get the icon identifier for a file extension.
    [[nodiscard]] static auto icon_for_extension(const std::string& extension) -> std::string;

    /// Total number of files in the workspace.
    [[nodiscard]] auto file_count() const -> std::size_t;

private:
    std::filesystem::path workspace_root_;
    std::vector<QuickOpenItem> files_;

    /// Fuzzy score a file name against a query.
    [[nodiscard]] static auto score_match(const std::string& query, const std::string& filename)
        -> int;
};

} // namespace markamp::core
