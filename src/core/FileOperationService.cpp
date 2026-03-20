/// @file FileOperationService.cpp
/// @brief P05-T01: Shared service for file and workspace operations.

#include "FileOperationService.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

#include <filesystem>
#include <fstream>

namespace markamp::core
{

namespace fs = std::filesystem;

FileOperationService::FileOperationService(EventBus& bus, Config& cfg)
    : event_bus_(bus)
    , config_(cfg)
{
}

auto FileOperationService::create_file(const std::string& path) -> FileOpResult
{
    if (fs::exists(path))
    {
        notify_result(FileOpResult::kAlreadyExists, "create file", path);
        return FileOpResult::kAlreadyExists;
    }

    std::error_code err_code;
    // Ensure parent directory exists
    const auto parent = fs::path(path).parent_path();
    if (!parent.empty() && !fs::exists(parent))
    {
        fs::create_directories(parent, err_code);
        if (err_code)
        {
            MARKAMP_LOG_ERROR("Failed to create parent directory: {}", err_code.message());
            notify_result(FileOpResult::kError, "create file", path);
            return FileOpResult::kError;
        }
    }

    // Create empty file
    std::ofstream ofs(path);
    if (!ofs.is_open())
    {
        notify_result(FileOpResult::kPermissionDenied, "create file", path);
        return FileOpResult::kPermissionDenied;
    }
    ofs.close();

    publish_tree_refresh();
    notify_result(FileOpResult::kSuccess, "create file", path);
    MARKAMP_LOG_INFO("File created: {}", path);
    return FileOpResult::kSuccess;
}

auto FileOperationService::create_folder(const std::string& path) -> FileOpResult
{
    if (fs::exists(path))
    {
        notify_result(FileOpResult::kAlreadyExists, "create folder", path);
        return FileOpResult::kAlreadyExists;
    }

    std::error_code err_code;
    fs::create_directories(path, err_code);
    if (err_code)
    {
        MARKAMP_LOG_ERROR("Failed to create folder: {}", err_code.message());
        notify_result(FileOpResult::kError, "create folder", path);
        return FileOpResult::kError;
    }

    publish_tree_refresh();
    notify_result(FileOpResult::kSuccess, "create folder", path);
    MARKAMP_LOG_INFO("Folder created: {}", path);
    return FileOpResult::kSuccess;
}

auto FileOperationService::rename(const std::string& old_path,
                                   const std::string& new_path) -> FileOpResult
{
    if (!fs::exists(old_path))
    {
        notify_result(FileOpResult::kNotFound, "rename", old_path);
        return FileOpResult::kNotFound;
    }
    if (fs::exists(new_path))
    {
        notify_result(FileOpResult::kAlreadyExists, "rename", new_path);
        return FileOpResult::kAlreadyExists;
    }

    std::error_code err_code;
    fs::rename(old_path, new_path, err_code);
    if (err_code)
    {
        MARKAMP_LOG_ERROR("Rename failed: {}", err_code.message());
        notify_result(FileOpResult::kError, "rename", old_path);
        return FileOpResult::kError;
    }

    // Notify file renamed for tab path updates
    events::FileSavedEvent renamed_evt;
    renamed_evt.file_path = new_path;
    event_bus_.publish(renamed_evt);

    publish_tree_refresh();
    notify_result(FileOpResult::kSuccess, "rename", old_path);
    MARKAMP_LOG_INFO("Renamed: {} -> {}", old_path, new_path);
    return FileOpResult::kSuccess;
}

auto FileOperationService::remove(const std::string& path) -> FileOpResult
{
    if (!fs::exists(path))
    {
        notify_result(FileOpResult::kNotFound, "delete", path);
        return FileOpResult::kNotFound;
    }

    std::error_code err_code;
    fs::remove_all(path, err_code);
    if (err_code)
    {
        MARKAMP_LOG_ERROR("Delete failed: {}", err_code.message());
        notify_result(FileOpResult::kError, "delete", path);
        return FileOpResult::kError;
    }

    // Close tab if the file was open
    const events::TabCloseRequestEvent close_evt{path};
    event_bus_.publish(close_evt);

    publish_tree_refresh();
    notify_result(FileOpResult::kSuccess, "delete", path);
    MARKAMP_LOG_INFO("Deleted: {}", path);
    return FileOpResult::kSuccess;
}

void FileOperationService::reveal_in_finder(const std::string& path)
{
    const events::RevealInFinderRequestEvent reveal_evt;
    event_bus_.publish(reveal_evt);
    MARKAMP_LOG_DEBUG("Reveal in finder: {}", path);
}

void FileOperationService::open_terminal(const std::string& directory)
{
    MARKAMP_LOG_INFO("Open terminal at: {}", directory);
    // Platform-specific terminal opening handled by platform abstraction
}

void FileOperationService::refresh_workspace()
{
    publish_tree_refresh();
    MARKAMP_LOG_INFO("Workspace refresh requested");
}

auto FileOperationService::move_to_trash(const std::string& path) -> FileOpResult
{
    if (!fs::exists(path))
    {
        notify_result(FileOpResult::kNotFound, "trash", path);
        return FileOpResult::kNotFound;
    }

    // Move to trash via platform abstraction (fallback to remove)
    // For now, delegate to remove
    return remove(path);
}

auto FileOperationService::duplicate_file(const std::string& path) -> FileOpResult
{
    if (!fs::exists(path))
    {
        notify_result(FileOpResult::kNotFound, "duplicate", path);
        return FileOpResult::kNotFound;
    }

    const auto src = fs::path(path);
    auto dst = src;
    dst.replace_filename(src.stem().string() + " (copy)" + src.extension().string());

    std::error_code err_code;
    fs::copy_file(src, dst, err_code);
    if (err_code)
    {
        MARKAMP_LOG_ERROR("Duplicate failed: {}", err_code.message());
        notify_result(FileOpResult::kError, "duplicate", path);
        return FileOpResult::kError;
    }

    publish_tree_refresh();
    notify_result(FileOpResult::kSuccess, "duplicate", path);
    MARKAMP_LOG_INFO("Duplicated: {} -> {}", path, dst.string());
    return FileOpResult::kSuccess;
}

void FileOperationService::publish_tree_refresh()
{
    const events::WorkspaceRefreshRequestEvent refresh_evt;
    event_bus_.publish(refresh_evt);
}

void FileOperationService::notify_result(FileOpResult result, const std::string& operation,
                                          const std::string& path)
{
    const auto filename = fs::path(path).filename().string();
    switch (result)
    {
    case FileOpResult::kSuccess:
        event_bus_.publish(events::NotificationEvent{
            operation + ": " + filename,
            events::NotificationLevel::Success, 2000});
        break;
    case FileOpResult::kPermissionDenied:
        event_bus_.publish(events::NotificationEvent{
            "Permission denied: " + filename,
            events::NotificationLevel::Error, 3000});
        break;
    case FileOpResult::kNotFound:
        event_bus_.publish(events::NotificationEvent{
            "Not found: " + filename,
            events::NotificationLevel::Warning, 3000});
        break;
    case FileOpResult::kAlreadyExists:
        event_bus_.publish(events::NotificationEvent{
            "Already exists: " + filename,
            events::NotificationLevel::Warning, 3000});
        break;
    case FileOpResult::kCancelled:
        break; // Silent
    case FileOpResult::kError:
        event_bus_.publish(events::NotificationEvent{
            "Failed: " + operation + " " + filename,
            events::NotificationLevel::Error, 3000});
        break;
    }
}

} // namespace markamp::core
