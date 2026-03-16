/// @file ThemeMarketplace.h
/// @brief V4 Phase 23 – Theme Marketplace / Sharing.

#pragma once

#include <expected>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class ThemeRegistry;
class HttpClient;

// ============================================================================
// ThemeListingInfo: metadata for one marketplace theme
// ============================================================================

struct ThemeListingInfo
{
    std::string id;
    std::string name;
    std::string author;
    std::string description;
    std::string version;
    std::string preview_url;
    std::string download_url;
    int download_count{0};
    double rating{0.0};
    int rating_count{0};
    std::string license;
    std::vector<std::string> tags;
    bool is_installed{false};
    bool has_update{false};
    std::string theme_content; // Populated on install
};

// ============================================================================
// ThemeExportPackage: shareable theme bundle
// ============================================================================

struct ThemeExportPackage
{
    std::string theme_content;
    std::string custom_css;
    std::string preview_image;
    std::string metadata_json;
};

// ============================================================================
// ThemeMarketplace
// ============================================================================

class ThemeMarketplace
{
public:
    ThemeMarketplace(EventBus& event_bus,
                     Config& config,
                     ThemeRegistry& theme_registry,
                     HttpClient& http_client);

    /// Fetch the list of available themes (optionally filtered).
    [[nodiscard]] auto fetch_listings(const std::string& search = "",
                                      const std::string& category = "") const
        -> std::vector<ThemeListingInfo>;

    /// Install a theme by ID.
    [[nodiscard]] auto install_theme(const ThemeListingInfo& listing)
        -> std::expected<void, std::string>;

    /// Uninstall a theme by ID.
    [[nodiscard]] auto uninstall_theme(const std::string& theme_id)
        -> std::expected<void, std::string>;

    /// Check for theme updates among installed themes.
    [[nodiscard]] auto check_updates() const -> std::vector<ThemeListingInfo>;

    /// Export a theme as a shareable package.
    [[nodiscard]] auto export_theme(const std::string& theme_id,
                                    const std::string& theme_content) const -> ThemeExportPackage;

    /// Import a theme from a package.
    [[nodiscard]] auto import_theme(const ThemeExportPackage& package)
        -> std::expected<void, std::string>;

    /// Rate a theme.
    auto rate_theme(const std::string& theme_id, int rating) -> void;

    /// Access installed themes list.
    [[nodiscard]] auto installed_themes() const -> const std::vector<std::string>&;

    /// Add a theme ID to the installed list.
    auto mark_installed(const std::string& theme_id) -> void;

    /// Remove a theme ID from the installed list.
    auto mark_uninstalled(const std::string& theme_id) -> void;

    /// (#155) Return the number of installed marketplace themes.
    [[nodiscard]] auto installed_count() const -> std::size_t;

    /// (#156) Check if any installed theme has an available update.
    [[nodiscard]] auto has_updates() const -> bool;

    /// (#157) Check if a specific theme ID is installed.
    [[nodiscard]] auto is_installed(const std::string& theme_id) const -> bool;

private:
    EventBus& event_bus_;
    Config& config_;
    ThemeRegistry& theme_registry_;
    HttpClient& http_client_;

    std::vector<std::string> installed_theme_ids_;
    std::string marketplace_url_{"https://themes.markamp.io/api/v1"};
};

} // namespace markamp::core
