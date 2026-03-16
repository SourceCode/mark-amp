/// @file ThemeMarketplace.cpp
/// @brief V4 Phase 23 – Theme Marketplace / Sharing implementation.

#include "core/ThemeMarketplace.h"

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/HttpClient.h"
#include "core/ThemeRegistry.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

ThemeMarketplace::ThemeMarketplace(EventBus& event_bus,
                                   Config& config,
                                   ThemeRegistry& theme_registry,
                                   HttpClient& http_client)
    : event_bus_(event_bus)
    , config_(config)
    , theme_registry_(theme_registry)
    , http_client_(http_client)
{
}

// ============================================================================
// Fetch listings
// ============================================================================

auto ThemeMarketplace::fetch_listings(const std::string& search, const std::string& /*category*/) const
    -> std::vector<ThemeListingInfo>
{
    // (#50) Filter installed themes by search term and category.
    // ThemeMarketplace stores only installed IDs; full metadata lookup
    // would require the ThemeRegistry.  For now, return IDs that match search.
    std::vector<ThemeListingInfo> results;

    auto to_lower = [](std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
        return str;
    };

    const auto lower_search = to_lower(search);

    for (const auto& theme_id : installed_theme_ids_)
    {
        if (!lower_search.empty() &&
            to_lower(theme_id).find(lower_search) == std::string::npos)
        {
            continue;
        }
        ThemeListingInfo info;
        info.id = theme_id;
        info.name = theme_id;
        results.push_back(std::move(info));
    }

    return results;
}

// ============================================================================
// Install theme
// ============================================================================

auto ThemeMarketplace::install_theme(const ThemeListingInfo& listing)
    -> std::expected<void, std::string>
{
    if (listing.id.empty())
    {
        return std::unexpected("Theme ID is empty");
    }

    if (listing.theme_content.empty() && listing.download_url.empty())
    {
        return std::unexpected("No theme content or download URL available");
    }

    // Mark as installed
    mark_installed(listing.id);

    // Publish event
    events::ThemeInstalledEvent event;
    event.theme_id = listing.id;
    event.theme_name = listing.name;
    event_bus_.publish(event);

    return {};
}

// ============================================================================
// Uninstall theme
// ============================================================================

auto ThemeMarketplace::uninstall_theme(const std::string& theme_id)
    -> std::expected<void, std::string>
{
    auto iter = std::find(installed_theme_ids_.begin(), installed_theme_ids_.end(), theme_id);

    if (iter == installed_theme_ids_.end())
    {
        return std::unexpected("Theme '" + theme_id + "' is not installed");
    }

    installed_theme_ids_.erase(iter);

    events::ThemeUninstalledEvent event;
    event.theme_id = theme_id;
    event_bus_.publish(event);

    return {};
}

// ============================================================================
// Check updates
// ============================================================================

auto ThemeMarketplace::check_updates() const -> std::vector<ThemeListingInfo>
{
    // In a real implementation, compare installed vs marketplace versions.
    // Returns themes that have has_update = true.
    std::vector<ThemeListingInfo> updatable;
    return updatable;
}

// ============================================================================
// Export theme
// ============================================================================

auto ThemeMarketplace::export_theme(const std::string& theme_id,
                                    const std::string& theme_content) const -> ThemeExportPackage
{
    ThemeExportPackage package;
    package.theme_content = theme_content;
    package.metadata_json = "{\"id\":\"" + theme_id + "\"}";
    return package;
}

// ============================================================================
// Import theme
// ============================================================================

auto ThemeMarketplace::import_theme(const ThemeExportPackage& package)
    -> std::expected<void, std::string>
{
    if (package.theme_content.empty())
    {
        return std::unexpected("Theme package has no content");
    }

    // Extract theme_id from metadata
    std::string theme_id;
    auto id_pos = package.metadata_json.find("\"id\":\"");
    if (id_pos != std::string::npos)
    {
        auto id_start = id_pos + 6;
        auto id_end = package.metadata_json.find('"', id_start);
        if (id_end != std::string::npos)
        {
            theme_id = package.metadata_json.substr(id_start, id_end - id_start);
        }
    }

    if (theme_id.empty())
    {
        theme_id = "imported_theme";
    }

    mark_installed(theme_id);

    events::ThemeInstalledEvent event;
    event.theme_id = theme_id;
    event.theme_name = theme_id;
    event_bus_.publish(event);

    return {};
}

// ============================================================================
// Rate theme
// ============================================================================

auto ThemeMarketplace::rate_theme(const std::string& theme_id, int rating) -> void
{
    // In a real implementation, POST to marketplace API
    (void)theme_id;
    (void)rating;
}

// ============================================================================
// Installed themes management
// ============================================================================

auto ThemeMarketplace::installed_themes() const -> const std::vector<std::string>&
{
    return installed_theme_ids_;
}

auto ThemeMarketplace::mark_installed(const std::string& theme_id) -> void
{
    if (std::find(installed_theme_ids_.begin(), installed_theme_ids_.end(), theme_id) ==
        installed_theme_ids_.end())
    {
        installed_theme_ids_.push_back(theme_id);
    }
}

auto ThemeMarketplace::mark_uninstalled(const std::string& theme_id) -> void
{
    auto iter = std::find(installed_theme_ids_.begin(), installed_theme_ids_.end(), theme_id);
    if (iter != installed_theme_ids_.end())
    {
        installed_theme_ids_.erase(iter);
    }
}

// (#155) Return the number of installed marketplace themes.
auto ThemeMarketplace::installed_count() const -> std::size_t
{
    return installed_theme_ids_.size();
}

// (#156) Check if any installed theme has an available update.
auto ThemeMarketplace::has_updates() const -> bool
{
    return !check_updates().empty();
}

// (#157) Check if a specific theme ID is installed.
auto ThemeMarketplace::is_installed(const std::string& theme_id) const -> bool
{
    return std::find(installed_theme_ids_.begin(), installed_theme_ids_.end(), theme_id) !=
           installed_theme_ids_.end();
}

} // namespace markamp::core
