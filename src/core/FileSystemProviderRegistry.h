#pragma once

#include <cstdint>
#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// File type enumeration for virtual file systems.
enum class FileType : std::uint8_t
{
    kUnknown = 0,
    kFile = 1,
    kDirectory = 2,
    kSymbolicLink = 64
};

/// File stat information.
struct FileStat
{
    FileType type{FileType::kUnknown};
    std::uint64_t size{0};
    std::uint64_t ctime{0}; // Creation time (ms since epoch)
    std::uint64_t mtime{0}; // Modification time (ms since epoch)
};

/// A directory entry.
struct DirectoryEntry
{
    std::string name;
    FileType type{FileType::kUnknown};
};

/// Interface for virtual file system providers (mirrors VS Code's FileSystemProvider).
class IFileSystemProvider
{
public:
    virtual ~IFileSystemProvider() = default;

    [[nodiscard]] virtual auto stat(const std::string& uri)
        -> std::expected<FileStat, std::string> = 0;

    [[nodiscard]] virtual auto read_file(const std::string& uri)
        -> std::expected<std::vector<std::uint8_t>, std::string> = 0;

    [[nodiscard]] virtual auto write_file(const std::string& uri,
                                          const std::vector<std::uint8_t>& content)
        -> std::expected<void, std::string> = 0;

    [[nodiscard]] virtual auto read_directory(const std::string& uri)
        -> std::expected<std::vector<DirectoryEntry>, std::string> = 0;

    [[nodiscard]] virtual auto create_directory(const std::string& uri)
        -> std::expected<void, std::string> = 0;

    [[nodiscard]] virtual auto delete_entry(const std::string& uri, bool recursive)
        -> std::expected<void, std::string> = 0;

    [[nodiscard]] virtual auto rename(const std::string& old_uri, const std::string& new_uri)
        -> std::expected<void, std::string> = 0;
};

/// Registry that maps URI schemes to file system providers.
class FileSystemProviderRegistry
{
public:
    FileSystemProviderRegistry() = default;

    /// Register a provider for a URI scheme. Takes ownership via shared_ptr.
    void register_provider(const std::string& scheme,
                           std::shared_ptr<IFileSystemProvider> provider);

    /// Get the provider for a URI scheme. Returns nullptr if not registered.
    [[nodiscard]] auto get_provider(const std::string& scheme) const
        -> std::shared_ptr<IFileSystemProvider>;

    /// Check if a provider is registered for a scheme.
    [[nodiscard]] auto has_provider(const std::string& scheme) const -> bool;

    /// Unregister a provider.
    void unregister_provider(const std::string& scheme);

    /// Get all registered schemes.
    [[nodiscard]] auto schemes() const -> std::vector<std::string>;

private:
    std::unordered_map<std::string, std::shared_ptr<IFileSystemProvider>> providers_;
};

} // namespace markamp::core
