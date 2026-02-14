#pragma once

#include "ExtensionManifest.h"

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

/// Result of reading a VSIX package without installing it.
struct VsixPackageInfo
{
    ExtensionManifest manifest;
    std::vector<std::string> file_list; // All files inside the VSIX
    std::size_t total_size_bytes{0};
};

/// Result of installing a VSIX package.
struct VsixInstallResult
{
    ExtensionManifest manifest;
    std::filesystem::path install_path; // Where the extension was extracted
};

/// Service for reading and inspecting VSIX packages (ZIP files).
/// A VSIX is a ZIP archive containing `extension/package.json` plus assets.
class VsixPackageService
{
public:
    /// Inspect a VSIX file: read the manifest and file listing without extracting.
    [[nodiscard]] static auto inspect(const std::filesystem::path& vsix_path)
        -> std::expected<VsixPackageInfo, std::string>;

    /// Read just the package.json manifest from a VSIX file.
    [[nodiscard]] static auto read_manifest(const std::filesystem::path& vsix_path)
        -> std::expected<ExtensionManifest, std::string>;

    /// Validate that a file is a valid VSIX (ZIP with extension/package.json).
    [[nodiscard]] static auto validate(const std::filesystem::path& vsix_path)
        -> std::expected<void, std::string>;
};

/// Service for installing VSIX packages to the extensions directory.
class VsixInstallService
{
public:
    /// Construct with the extensions root directory.
    explicit VsixInstallService(std::filesystem::path extensions_root);

    /// Default constructor uses `~/.markamp/extensions/`.
    VsixInstallService();

    /// Install a VSIX file: extract to `<extensions_root>/<publisher>.<name>-<version>/`.
    [[nodiscard]] auto install(const std::filesystem::path& vsix_path)
        -> std::expected<VsixInstallResult, std::string>;

    /// Uninstall an extension by ID (publisher.name format).
    /// Removes the newest matching directory.
    [[nodiscard]] auto uninstall(const std::string& extension_id)
        -> std::expected<void, std::string>;

    /// Get the extensions root directory.
    [[nodiscard]] auto extensions_root() const -> const std::filesystem::path&
    {
        return extensions_root_;
    }

private:
    std::filesystem::path extensions_root_;
};

/// Service for exporting installed extensions back to VSIX format.
class VsixExportService
{
public:
    /// Export an installed extension directory to a VSIX file.
    [[nodiscard]] static auto export_to_vsix(const std::filesystem::path& extension_dir,
                                             const std::filesystem::path& output_path)
        -> std::expected<void, std::string>;
};

} // namespace markamp::core
