#include "VsixService.h"

#include "Logger.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <zip.h>

namespace markamp::core
{

namespace fs = std::filesystem;

namespace
{

/// RAII wrapper for zip_t* (libzip archive handle).
class ZipArchive
{
public:
    explicit ZipArchive(const fs::path& path, int flags = ZIP_RDONLY)
    {
        int zip_error = 0;
        archive_ = zip_open(path.c_str(), flags, &zip_error);
    }

    ~ZipArchive()
    {
        if (archive_ != nullptr)
        {
            zip_close(archive_);
        }
    }

    ZipArchive(const ZipArchive&) = delete;
    auto operator=(const ZipArchive&) -> ZipArchive& = delete;
    ZipArchive(ZipArchive&&) = delete;
    auto operator=(ZipArchive&&) -> ZipArchive& = delete;

    [[nodiscard]] auto is_open() const -> bool
    {
        return archive_ != nullptr;
    }
    [[nodiscard]] auto get() const -> zip_t*
    {
        return archive_;
    }

private:
    zip_t* archive_{nullptr};
};

/// Read a specific file from a ZIP archive as a string.
auto read_zip_entry(zip_t* archive, const std::string& entry_name)
    -> std::expected<std::string, std::string>
{
    const auto index = zip_name_locate(archive, entry_name.c_str(), 0);
    if (index < 0)
    {
        return std::unexpected("Entry not found in ZIP: " + entry_name);
    }

    zip_stat_t stat;
    zip_stat_init(&stat);
    if (zip_stat_index(archive, static_cast<zip_uint64_t>(index), 0, &stat) != 0)
    {
        return std::unexpected("Cannot stat ZIP entry: " + entry_name);
    }

    auto* file_handle = zip_fopen_index(archive, static_cast<zip_uint64_t>(index), 0);
    if (file_handle == nullptr)
    {
        return std::unexpected("Cannot open ZIP entry: " + entry_name);
    }

    std::string content;
    content.resize(stat.size);

    const auto bytes_read =
        zip_fread(file_handle, content.data(), static_cast<zip_uint64_t>(content.size()));
    zip_fclose(file_handle);

    if (bytes_read < 0 || static_cast<zip_uint64_t>(bytes_read) != stat.size)
    {
        return std::unexpected("Failed to read ZIP entry: " + entry_name);
    }

    return content;
}

/// Extract all files from a ZIP archive to a target directory.
auto extract_all(zip_t* archive, const fs::path& target_dir, const std::string& prefix)
    -> std::expected<void, std::string>
{
    const auto num_entries = zip_get_num_entries(archive, 0);

    for (zip_int64_t idx = 0; idx < num_entries; ++idx)
    {
        zip_stat_t stat;
        zip_stat_init(&stat);
        if (zip_stat_index(archive, static_cast<zip_uint64_t>(idx), 0, &stat) != 0)
        {
            continue;
        }

        std::string name(stat.name);

        // Only extract entries under the prefix (e.g. "extension/")
        if (!prefix.empty() && name.find(prefix) != 0)
        {
            continue;
        }

        // Strip the prefix to get relative path
        std::string relative = name.substr(prefix.size());
        if (relative.empty())
        {
            continue;
        }

        const auto target_path = target_dir / relative;

        // Directory entry (name ends with '/')
        if (name.back() == '/')
        {
            std::error_code dir_err;
            fs::create_directories(target_path, dir_err);
            continue;
        }

        // Ensure parent directory exists
        std::error_code parent_err;
        fs::create_directories(target_path.parent_path(), parent_err);

        // Extract file
        auto* file_handle = zip_fopen_index(archive, static_cast<zip_uint64_t>(idx), 0);
        if (file_handle == nullptr)
        {
            return std::unexpected("Cannot extract: " + name);
        }

        std::ofstream out_file(target_path, std::ios::binary);
        if (!out_file.is_open())
        {
            zip_fclose(file_handle);
            return std::unexpected("Cannot create file: " + target_path.string());
        }

        constexpr std::size_t kBufferSize = 8192;
        std::vector<char> buffer(kBufferSize);
        zip_int64_t bytes_read = 0;

        while ((bytes_read = zip_fread(
                    file_handle, buffer.data(), static_cast<zip_uint64_t>(kBufferSize))) > 0)
        {
            out_file.write(buffer.data(), bytes_read);
        }

        zip_fclose(file_handle);
    }

    return {};
}

/// Add a file to a ZIP archive being written.
auto add_file_to_zip(zip_t* archive, const fs::path& file_path, const std::string& archive_name)
    -> bool
{
    auto* source = zip_source_file(archive, file_path.c_str(), 0, -1);
    if (source == nullptr)
    {
        return false;
    }

    const auto result = zip_file_add(archive, archive_name.c_str(), source, ZIP_FL_OVERWRITE);
    if (result < 0)
    {
        zip_source_free(source);
        return false;
    }

    return true;
}

auto default_extensions_root() -> fs::path
{
    const char* home_dir = std::getenv("HOME"); // NOLINT(concurrency-mt-unsafe)
    if (home_dir != nullptr)
    {
        return fs::path(home_dir) / ".markamp" / "extensions";
    }
    return fs::path(".markamp") / "extensions";
}

} // anonymous namespace

// ── VsixPackageService ──

auto VsixPackageService::inspect(const fs::path& vsix_path)
    -> std::expected<VsixPackageInfo, std::string>
{
    ZipArchive archive(vsix_path);
    if (!archive.is_open())
    {
        return std::unexpected("Cannot open VSIX file: " + vsix_path.string());
    }

    // Read package.json from extension/ prefix
    auto manifest_result = read_zip_entry(archive.get(), "extension/package.json");
    if (!manifest_result)
    {
        return std::unexpected(manifest_result.error());
    }

    ExtensionManifest manifest;
    try
    {
        manifest = ManifestParser::parse(manifest_result.value());
    }
    catch (const std::exception& parse_err)
    {
        return std::unexpected(std::string("Invalid package.json in VSIX: ") + parse_err.what());
    }

    // Build file list and total size
    VsixPackageInfo info;
    info.manifest = std::move(manifest);

    const auto num_entries = zip_get_num_entries(archive.get(), 0);
    for (zip_int64_t idx = 0; idx < num_entries; ++idx)
    {
        zip_stat_t stat;
        zip_stat_init(&stat);
        if (zip_stat_index(archive.get(), static_cast<zip_uint64_t>(idx), 0, &stat) == 0)
        {
            info.file_list.emplace_back(stat.name);
            info.total_size_bytes += stat.size;
        }
    }

    return info;
}

auto VsixPackageService::read_manifest(const fs::path& vsix_path)
    -> std::expected<ExtensionManifest, std::string>
{
    ZipArchive archive(vsix_path);
    if (!archive.is_open())
    {
        return std::unexpected("Cannot open VSIX file: " + vsix_path.string());
    }

    auto manifest_result = read_zip_entry(archive.get(), "extension/package.json");
    if (!manifest_result)
    {
        return std::unexpected(manifest_result.error());
    }

    try
    {
        return ManifestParser::parse(manifest_result.value());
    }
    catch (const std::exception& parse_err)
    {
        return std::unexpected(std::string("Invalid package.json: ") + parse_err.what());
    }
}

auto VsixPackageService::validate(const fs::path& vsix_path) -> std::expected<void, std::string>
{
    if (!fs::exists(vsix_path))
    {
        return std::unexpected("VSIX file does not exist: " + vsix_path.string());
    }

    ZipArchive archive(vsix_path);
    if (!archive.is_open())
    {
        return std::unexpected("Not a valid ZIP file: " + vsix_path.string());
    }

    // Check for extension/package.json
    if (zip_name_locate(archive.get(), "extension/package.json", 0) < 0)
    {
        return std::unexpected("VSIX missing extension/package.json");
    }

    return {};
}

// ── VsixInstallService ──

VsixInstallService::VsixInstallService(fs::path extensions_root)
    : extensions_root_(std::move(extensions_root))
{
}

VsixInstallService::VsixInstallService()
    : extensions_root_(default_extensions_root())
{
}

auto VsixInstallService::install(const fs::path& vsix_path)
    -> std::expected<VsixInstallResult, std::string>
{
    // Validate first
    auto validation = VsixPackageService::validate(vsix_path);
    if (!validation)
    {
        return std::unexpected(validation.error());
    }

    // Read manifest
    auto manifest_result = VsixPackageService::read_manifest(vsix_path);
    if (!manifest_result)
    {
        return std::unexpected(manifest_result.error());
    }

    auto manifest = std::move(manifest_result.value());
    const auto install_dir_name = manifest.publisher + "." + manifest.name + "-" + manifest.version;
    const auto install_path = extensions_root_ / install_dir_name;

    // Remove existing installation if present
    if (fs::exists(install_path))
    {
        std::error_code remove_err;
        fs::remove_all(install_path, remove_err);
        if (remove_err)
        {
            return std::unexpected("Cannot remove existing installation: " + remove_err.message());
        }
    }

    // Create extensions root if needed
    std::error_code mkdir_err;
    fs::create_directories(extensions_root_, mkdir_err);
    if (mkdir_err)
    {
        return std::unexpected("Cannot create extensions directory: " + mkdir_err.message());
    }

    // Extract extension/ contents to install path
    ZipArchive archive(vsix_path);
    if (!archive.is_open())
    {
        return std::unexpected("Cannot reopen VSIX for extraction");
    }

    auto extract_result = extract_all(archive.get(), install_path, "extension/");
    if (!extract_result)
    {
        // Clean up partial extraction
        std::error_code cleanup_err;
        fs::remove_all(install_path, cleanup_err);
        return std::unexpected(extract_result.error());
    }

    MARKAMP_LOG_INFO("Installed extension: {}.{} v{} to {}",
                     manifest.publisher,
                     manifest.name,
                     manifest.version,
                     install_path.string());

    VsixInstallResult result;
    result.manifest = std::move(manifest);
    result.install_path = install_path;
    return result;
}

auto VsixInstallService::uninstall(const std::string& extension_id)
    -> std::expected<void, std::string>
{
    if (!fs::exists(extensions_root_))
    {
        return std::unexpected("Extensions directory does not exist");
    }

    // Find matching directory (publisher.name-version)
    bool found = false;
    std::error_code dir_err;

    for (const auto& entry : fs::directory_iterator(extensions_root_, dir_err))
    {
        if (!entry.is_directory())
        {
            continue;
        }

        const auto dirname = entry.path().filename().string();
        // Check if directory starts with extension_id followed by '-'
        if (dirname.find(extension_id) == 0 && dirname.size() > extension_id.size() &&
            dirname[extension_id.size()] == '-')
        {
            std::error_code remove_err;
            fs::remove_all(entry.path(), remove_err);
            if (remove_err)
            {
                return std::unexpected("Failed to remove: " + remove_err.message());
            }

            MARKAMP_LOG_INFO(
                "Uninstalled extension: {} from {}", extension_id, entry.path().string());
            found = true;
        }
    }

    if (!found)
    {
        return std::unexpected("Extension not found: " + extension_id);
    }

    return {};
}

// ── VsixExportService ──

auto VsixExportService::export_to_vsix(const fs::path& extension_dir, const fs::path& output_path)
    -> std::expected<void, std::string>
{
    if (!fs::exists(extension_dir) || !fs::is_directory(extension_dir))
    {
        return std::unexpected("Extension directory does not exist: " + extension_dir.string());
    }

    // Verify package.json exists
    if (!fs::exists(extension_dir / "package.json"))
    {
        return std::unexpected("Missing package.json in extension directory");
    }

    // Ensure output parent directory exists
    std::error_code mkdir_err;
    fs::create_directories(output_path.parent_path(), mkdir_err);

    int zip_error = 0;
    auto* archive = zip_open(output_path.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &zip_error);
    if (archive == nullptr)
    {
        return std::unexpected("Cannot create VSIX file: " + output_path.string());
    }

    // Add all files from extension_dir under "extension/" prefix
    std::error_code walk_err;
    for (const auto& entry : fs::recursive_directory_iterator(extension_dir, walk_err))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        const auto relative = fs::relative(entry.path(), extension_dir);
        const auto archive_name = "extension/" + relative.string();

        if (!add_file_to_zip(archive, entry.path(), archive_name))
        {
            zip_discard(archive);
            return std::unexpected("Failed to add file to VSIX: " + relative.string());
        }
    }

    if (zip_close(archive) != 0)
    {
        return std::unexpected("Failed to finalize VSIX file");
    }

    MARKAMP_LOG_INFO("Exported extension to: {}", output_path.string());
    return {};
}

} // namespace markamp::core
