#pragma once

#include "EventBus.h"
#include "Types.h"

#include <expected>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Interface for file system operations.
/// All file access goes through this abstraction for App Sandbox compatibility.
/// On macOS: uses security-scoped bookmarks for persisted file access.
/// On Windows: uses broker-based file access for MSIX apps.
class IFileSystem
{
public:
    virtual ~IFileSystem() = default;
    [[nodiscard]] virtual auto read_file(const std::filesystem::path& path)
        -> std::expected<std::string, std::string> = 0;
    [[nodiscard]] virtual auto write_file(const std::filesystem::path& path,
                                          std::string_view content)
        -> std::expected<void, std::string> = 0;
    [[nodiscard]] virtual auto list_directory(const std::filesystem::path& path)
        -> std::expected<std::vector<FileNode>, std::string> = 0;
    [[nodiscard]] virtual auto watch_file(const std::filesystem::path& path,
                                          std::function<void()> callback) -> Subscription = 0;
};

} // namespace markamp::core
