#pragma once

#include "EncodingDetector.h"
#include "EventBus.h"
#include "FileNode.h"
#include "IFileSystem.h"

#include <filesystem>
#include <set>
#include <string>
#include <vector>

// Forward declare wxWindow and wxTimer to avoid wx header in this header
class wxWindow;
class wxTimer;
class wxTimerEvent;

namespace markamp::core
{

/// Concrete file system implementation.
/// Reads/writes files from disk, scans directories to FileNode trees,
/// tracks file watches via polling timer.
class FileSystem : public IFileSystem
{
public:
    explicit FileSystem(EventBus& event_bus);
    ~FileSystem() override;

    // ── IFileSystem interface ──
    [[nodiscard]] auto read_file(const std::filesystem::path& path)
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto write_file(const std::filesystem::path& path, std::string_view content)
        -> std::expected<void, std::string> override;
    [[nodiscard]] auto list_directory(const std::filesystem::path& path)
        -> std::expected<std::vector<FileNode>, std::string> override;
    [[nodiscard]] auto watch_file(const std::filesystem::path& path, std::function<void()> callback)
        -> Subscription override;

    // ── Extended operations ──

    /// Read a file and detect its encoding.
    [[nodiscard]] auto read_file_with_encoding(const std::filesystem::path& path)
        -> std::expected<std::pair<std::string, DetectedEncoding>, std::string>;

    /// Recursively scan a directory into a FileNode tree.
    /// Only includes files with matching extensions. Skips hidden and excluded dirs.
    [[nodiscard]] auto scan_directory_to_tree(const std::filesystem::path& dir_path,
                                              int max_depth = 10)
        -> std::expected<FileNode, std::string>;

    /// Show native open-file dialog (returns nullopt on cancel).
    [[nodiscard]] auto
    open_file_dialog(wxWindow* parent,
                     const std::string& wildcard = "Markdown files (*.md)|*.md|All files (*.*)|*.*")
        -> std::optional<std::filesystem::path>;

    /// Show native open-folder dialog (returns nullopt on cancel).
    [[nodiscard]] auto open_folder_dialog(wxWindow* parent) -> std::optional<std::filesystem::path>;

    /// Show native save-file dialog (returns nullopt on cancel).
    [[nodiscard]] auto save_file_dialog(wxWindow* parent,
                                        const std::string& default_name = "untitled.md")
        -> std::optional<std::filesystem::path>;

    // ── Constants ──
    static const std::set<std::string> kMarkdownExtensions;
    static const std::set<std::string> kExcludedDirectories;

private:
    EventBus& event_bus_;

    // ── File watching ──
    struct WatchEntry
    {
        std::filesystem::path path;
        std::filesystem::file_time_type last_modified;
        std::function<void()> callback;
        std::size_t id{0};
    };

    std::vector<WatchEntry> watch_entries_;
    std::size_t next_watch_id_{1};

    // Directory scan helper
    void scan_recursive(const std::filesystem::path& dir_path,
                        FileNode& parent_node,
                        int current_depth,
                        int max_depth);
};

} // namespace markamp::core
