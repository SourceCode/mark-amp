#pragma once

#include "ExtensionManifest.h"

#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

/// Represents a locally installed extension on disk.
struct LocalExtension
{
    ExtensionManifest manifest;
    std::filesystem::path location; // Directory containing the extension
    bool is_builtin{false};         // True for bundled built-in extensions
};

/// Interface for scanning the filesystem for installed extensions.
class IExtensionScannerService
{
public:
    virtual ~IExtensionScannerService() = default;

    /// Scan the extensions directory and return all valid extensions found.
    [[nodiscard]] virtual auto scan_extensions() -> std::vector<LocalExtension> = 0;

    /// Scan a specific directory for extensions (useful for testing).
    [[nodiscard]] virtual auto scan_directory(const std::filesystem::path& dir)
        -> std::vector<LocalExtension> = 0;
};

/// Scans `~/.markamp/extensions/` (or a configurable path) for installed
/// extensions by reading each subdirectory's `package.json`.
class ExtensionScannerService : public IExtensionScannerService
{
public:
    /// Construct with a custom extensions root directory.
    explicit ExtensionScannerService(std::filesystem::path extensions_root);

    /// Default constructor uses `~/.markamp/extensions/`.
    ExtensionScannerService();

    [[nodiscard]] auto scan_extensions() -> std::vector<LocalExtension> override;

    [[nodiscard]] auto scan_directory(const std::filesystem::path& dir)
        -> std::vector<LocalExtension> override;

    /// Get the configured extensions root directory.
    [[nodiscard]] auto extensions_root() const -> const std::filesystem::path&
    {
        return extensions_root_;
    }

private:
    std::filesystem::path extensions_root_;
};

} // namespace markamp::core
