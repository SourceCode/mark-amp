/// @file FileOperationService.h
/// @brief P05-T01: Shared service for file and workspace operations.
///
/// Routes rename, delete, new file/folder, reveal, and terminal open through
/// a single service that handles validation, confirmation, and event publication.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;
class Config;

/// Result of a file operation.
enum class FileOpResult
{
    kSuccess,
    kPermissionDenied,
    kNotFound,
    kAlreadyExists,
    kCancelled,
    kError,
};

/// Shared service for file-system operations with consistent UI updates.
class FileOperationService
{
public:
    FileOperationService(EventBus& bus, Config& cfg);

    /// Create a new file at the given path.
    [[nodiscard]] auto create_file(const std::string& path) -> FileOpResult;

    /// Create a new folder at the given path.
    [[nodiscard]] auto create_folder(const std::string& path) -> FileOpResult;

    /// Rename a file or folder. Updates tabs if the file is open.
    [[nodiscard]] auto rename(const std::string& old_path,
                              const std::string& new_path) -> FileOpResult;

    /// Delete a file or folder. Closes tab if the file is open.
    [[nodiscard]] auto remove(const std::string& path) -> FileOpResult;

    /// Reveal a file in the system file manager.
    void reveal_in_finder(const std::string& path);

    /// Open a terminal at the given directory.
    void open_terminal(const std::string& directory);

    /// Refresh the workspace file tree.
    void refresh_workspace();

    /// Move a file to the trash instead of permanent delete.
    [[nodiscard]] auto move_to_trash(const std::string& path) -> FileOpResult;

    /// Duplicate a file.
    [[nodiscard]] auto duplicate_file(const std::string& path) -> FileOpResult;

private:
    void publish_tree_refresh();
    void notify_result(FileOpResult result, const std::string& operation,
                       const std::string& path);

    EventBus& event_bus_;
    Config& config_;
};

} // namespace markamp::core
