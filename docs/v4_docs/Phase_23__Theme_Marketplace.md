# Phase 23 -- Theme Marketplace / Sharing

## Objective

Extend the existing ThemeGallery with a theme marketplace: browse, install, and share community themes. Themes are Markdown files with YAML frontmatter (existing format) distributed via a theme registry. Supports theme preview, one-click install, star ratings, and export/import.

## Prerequisites

- Existing ThemeEngine, ThemeGallery, ThemeRegistry, ThemePreviewCard
- Existing GalleryService, HttpClient
- Phase 22 (VaultStyleService -- CSS integration)

## Feature References (PRD)

- PRD #34: Theme Marketplace / Sharing
- PRD #12: Themes

## Data Structures to Implement

### File: `src/core/ThemeMarketplace.h`

```cpp
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

struct ThemeListingInfo
{
    std::string id;
    std::string name;
    std::string author;
    std::string description;
    std::string version;
    std::string preview_url;        // Screenshot URL
    std::string download_url;
    int download_count{0};
    double rating{0.0};
    int rating_count{0};
    std::string license;
    std::vector<std::string> tags;  // "dark", "minimal", "colorful"
    bool is_installed{false};
    bool has_update{false};
};

struct ThemeExportPackage
{
    std::string theme_content;      // Theme markdown file
    std::string custom_css;         // Optional vault CSS
    std::string preview_image;      // Base64-encoded screenshot
    std::string metadata_json;      // Author, version, description
};

class ThemeMarketplace
{
public:
    ThemeMarketplace(EventBus& event_bus, Config& config,
                     ThemeRegistry& theme_registry, HttpClient& http_client);

    /// Fetch the list of available themes from the marketplace.
    [[nodiscard]] auto fetch_listings(const std::string& search = "",
                                       const std::string& category = "") const
        -> std::expected<std::vector<ThemeListingInfo>, std::string>;

    /// Install a theme from the marketplace.
    [[nodiscard]] auto install_theme(const std::string& theme_id)
        -> std::expected<void, std::string>;

    /// Uninstall a marketplace theme.
    [[nodiscard]] auto uninstall_theme(const std::string& theme_id)
        -> std::expected<void, std::string>;

    /// Check for theme updates.
    [[nodiscard]] auto check_updates() const
        -> std::vector<ThemeListingInfo>;

    /// Update a theme to the latest version.
    [[nodiscard]] auto update_theme(const std::string& theme_id)
        -> std::expected<void, std::string>;

    /// Export a theme as a shareable package.
    [[nodiscard]] auto export_theme(const std::string& theme_id) const
        -> std::expected<ThemeExportPackage, std::string>;

    /// Import a theme from a package.
    [[nodiscard]] auto import_theme(const ThemeExportPackage& package)
        -> std::expected<void, std::string>;

    /// Rate a theme.
    [[nodiscard]] auto rate_theme(const std::string& theme_id, int rating)
        -> std::expected<void, std::string>;

private:
    EventBus& event_bus_;
    Config& config_;
    ThemeRegistry& theme_registry_;
    HttpClient& http_client_;

    std::string marketplace_url_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`fetch_listings(search, category)`** -- HTTP GET to marketplace API. Parse JSON response into ThemeListingInfo vector. Mark installed themes.

2. **`install_theme(theme_id)`** -- Download theme file from download_url. Validate content. Save to themes directory. Register with ThemeRegistry. Publish ThemeInstalledEvent.

3. **`export_theme(theme_id)`** -- Read theme file. Capture preview screenshot. Bundle into ThemeExportPackage.

4. **`import_theme(package)`** -- Validate package. Save theme file. Register. Apply if desired.

5. **`check_updates()`** -- Compare installed theme versions with marketplace versions. Return themes with available updates.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeInstalledEvent)
std::string theme_id;
std::string theme_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeUninstalledEvent)
std::string theme_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeMarketplaceRefreshedEvent)
int theme_count{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_theme_marketplace.cpp`

1. **Fetch listings** -- Mock HTTP response with 5 themes. Verify 5 ThemeListingInfo returned.
2. **Search filter** -- Fetch with search="dark". Verify only dark themes returned.
3. **Install theme** -- Install from listing. Verify theme file saved and registered.
4. **Uninstall theme** -- Uninstall. Verify file removed and unregistered.
5. **Check updates** -- Installed v1.0, marketplace has v1.1. Verify has_update=true.
6. **Export theme** -- Export theme. Verify package contains theme content and metadata.
7. **Import theme** -- Import package. Verify theme registered.
8. **Rate theme** -- Rate theme 5 stars. Verify HTTP request sent.
9. **Installed flag** -- Installed theme appears with is_installed=true in listings.
10. **Error handling** -- HTTP failure. Verify error returned, not crash.

## Acceptance Criteria

- [ ] Browse marketplace themes with search and category filter
- [ ] One-click install/uninstall of marketplace themes
- [ ] Update detection for installed themes
- [ ] Theme export/import for sharing
- [ ] Rating system
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/ThemeMarketplace.h` | ThemeMarketplace, ThemeListingInfo |
| CREATE | `src/core/ThemeMarketplace.cpp` | Full implementation |
| MODIFY | `src/ui/ThemeGallery.cpp` | Add marketplace tab |
| MODIFY | `src/core/Events.h` | Add 3 marketplace events |
| MODIFY | `src/CMakeLists.txt` | Add ThemeMarketplace.cpp |
| CREATE | `tests/unit/test_theme_marketplace.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_theme_marketplace target |

## Estimated Complexity

**M** -- HTTP integration, package format, existing theme infrastructure reuse, 10 tests.
