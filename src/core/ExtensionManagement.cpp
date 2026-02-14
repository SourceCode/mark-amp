#include "ExtensionManagement.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

namespace fs = std::filesystem;

// ── Construction ──

ExtensionManagementService::ExtensionManagementService(VsixInstallService& vsix_service,
                                                       ExtensionScannerService& scanner_service,
                                                       IExtensionGalleryService& gallery_service,
                                                       EventBus& event_bus)
    : vsix_service_(vsix_service)
    , scanner_service_(scanner_service)
    , gallery_service_(gallery_service)
    , event_bus_(event_bus)
{
    refresh_cache();
}

// ── Install from local VSIX ──

auto ExtensionManagementService::install(const fs::path& vsix_path)
    -> std::expected<LocalExtension, std::string>
{
    auto install_result = vsix_service_.install(vsix_path);
    if (!install_result)
    {
        return std::unexpected(install_result.error());
    }

    // Refresh cache to pick up the new extension
    refresh_cache();

    // Find the newly installed extension in cache
    const auto ext_id =
        install_result.value().manifest.publisher + "." + install_result.value().manifest.name;

    LocalExtension local_ext;
    local_ext.manifest = std::move(install_result.value().manifest);
    local_ext.location = std::move(install_result.value().install_path);
    local_ext.is_builtin = false;

    // Publish event
    events::ExtensionInstalledEvent event;
    event.extension_id = ext_id;
    event.version = local_ext.manifest.version;
    event_bus_.publish(event);

    MARKAMP_LOG_INFO("Extension installed: {} v{}", ext_id, local_ext.manifest.version);

    return local_ext;
}

// ── Install from gallery ──

auto ExtensionManagementService::install_from_gallery(const GalleryExtension& gallery_ext)
    -> std::expected<LocalExtension, std::string>
{
    // Create a temp path for the downloaded VSIX
    const auto temp_dir = fs::temp_directory_path() / "markamp_downloads";
    std::error_code mkdir_err;
    fs::create_directories(temp_dir, mkdir_err);

    const auto vsix_filename = gallery_ext.identifier + "-" + gallery_ext.version + ".vsix";
    const auto vsix_path = temp_dir / vsix_filename;

    // Download from gallery
    auto download_result = gallery_service_.download(gallery_ext, vsix_path.string());
    if (!download_result)
    {
        return std::unexpected("Download failed: " + download_result.error());
    }

    // Install the downloaded VSIX
    auto result = install(vsix_path);

    // Clean up temp file
    std::error_code cleanup_err;
    fs::remove(vsix_path, cleanup_err);

    return result;
}

// ── Uninstall ──

auto ExtensionManagementService::uninstall(const std::string& extension_id)
    -> std::expected<void, std::string>
{
    // Check for dependents before uninstalling
    auto dependents = find_dependents(extension_id);
    if (!dependents.empty())
    {
        std::string dep_list;
        for (const auto& dep : dependents)
        {
            if (!dep_list.empty())
            {
                dep_list += ", ";
            }
            dep_list += dep;
        }
        return std::unexpected("Cannot uninstall '" + extension_id +
                               "': the following extensions depend on it: " + dep_list);
    }

    auto result = vsix_service_.uninstall(extension_id);
    if (!result)
    {
        return std::unexpected(result.error());
    }

    // Refresh cache
    refresh_cache();

    // Publish event
    events::ExtensionUninstalledEvent event;
    event.extension_id = extension_id;
    event_bus_.publish(event);

    MARKAMP_LOG_INFO("Extension uninstalled: {}", extension_id);

    return {};
}

// ── Query installed ──

auto ExtensionManagementService::get_installed() -> std::vector<LocalExtension>
{
    refresh_cache();
    return installed_cache_;
}

// ── Update checking ──

auto ExtensionManagementService::check_updates()
    -> std::expected<std::vector<ExtensionUpdateInfo>, std::string>
{
    refresh_cache();

    if (installed_cache_.empty())
    {
        return std::vector<ExtensionUpdateInfo>{};
    }

    // Collect identifiers for batch lookup
    std::vector<std::string> identifiers;
    identifiers.reserve(installed_cache_.size());
    for (const auto& ext : installed_cache_)
    {
        identifiers.push_back(ext.manifest.publisher + "." + ext.manifest.name);
    }

    auto gallery_result = gallery_service_.get_extensions(identifiers);
    if (!gallery_result)
    {
        return std::unexpected(gallery_result.error());
    }

    std::vector<ExtensionUpdateInfo> updates;

    for (const auto& gallery_ext : gallery_result.value())
    {
        // Find the matching installed extension
        auto local_it = std::find_if(installed_cache_.begin(),
                                     installed_cache_.end(),
                                     [&gallery_ext](const LocalExtension& local)
                                     {
                                         const auto local_id =
                                             local.manifest.publisher + "." + local.manifest.name;
                                         return local_id == gallery_ext.identifier;
                                     });

        if (local_it != installed_cache_.end() && local_it->manifest.version != gallery_ext.version)
        {
            ExtensionUpdateInfo update_info;
            update_info.extension_id = gallery_ext.identifier;
            update_info.current_version = local_it->manifest.version;
            update_info.available_version = gallery_ext.version;
            update_info.gallery_entry = gallery_ext;
            updates.push_back(std::move(update_info));
        }
    }

    return updates;
}

// ── Update single extension ──

auto ExtensionManagementService::update(const std::string& extension_id)
    -> std::expected<LocalExtension, std::string>
{
    // Look up the extension in the gallery
    auto gallery_result = gallery_service_.get_extensions({extension_id});
    if (!gallery_result || gallery_result.value().empty())
    {
        return std::unexpected("Extension not found in gallery: " + extension_id);
    }

    // Install the new version (VSIX install overwrites existing)
    return install_from_gallery(gallery_result.value().front());
}

// ── Private helpers ──

void ExtensionManagementService::refresh_cache()
{
    installed_cache_ = scanner_service_.scan_extensions();
}

auto ExtensionManagementService::find_dependents(const std::string& extension_id) const
    -> std::vector<std::string>
{
    std::vector<std::string> dependents;

    for (const auto& ext : installed_cache_)
    {
        for (const auto& dep : ext.manifest.extension_dependencies)
        {
            if (dep == extension_id)
            {
                dependents.push_back(ext.manifest.publisher + "." + ext.manifest.name);
                break;
            }
        }
    }

    return dependents;
}

// ── Auto-update scheduler (#43) ──

void ExtensionManagementService::schedule_update_check(std::chrono::seconds interval)
{
    update_check_interval_ = interval;
    update_check_scheduled_ = true;
    MARKAMP_LOG_INFO("Auto-update check scheduled every {}s", interval.count());
}

void ExtensionManagementService::cancel_update_check()
{
    update_check_scheduled_ = false;
    update_check_interval_ = std::chrono::seconds{0};
    MARKAMP_LOG_INFO("Auto-update check cancelled");
}

auto ExtensionManagementService::is_update_check_scheduled() const -> bool
{
    return update_check_scheduled_;
}

auto ExtensionManagementService::update_check_interval() const -> std::chrono::seconds
{
    return update_check_interval_;
}

} // namespace markamp::core
