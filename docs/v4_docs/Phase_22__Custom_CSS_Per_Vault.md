# Phase 22 -- Custom CSS Per Vault

## Objective

Allow each vault to have custom CSS that styles the preview panel, editor highlights, and UI elements. Users can place a `styles.css` file in their vault's `.markamp/` directory to customize the rendering. Supports CSS variables for easy theming, live reload on file change, and a CSS editor panel.

## Prerequisites

- Phase 02 (VaultService -- vault directory structure)
- Existing ThemeEngine, PreviewPanel

## Feature References (PRD)

- PRD #47: Custom CSS per Vault
- PRD #12: Themes

## Data Structures to Implement

### File: `src/core/VaultStyleService.h`

```cpp
#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class VaultService;
class ThemeEngine;

struct CssVariable
{
    std::string name;           // --primary-color
    std::string value;          // #ff0000
    std::string description;
};

struct VaultStyle
{
    std::string css_content;
    std::filesystem::path file_path;
    std::vector<CssVariable> variables;
    bool is_valid{false};
    std::string error;          // CSS parse error if any
};

class VaultStyleService
{
public:
    VaultStyleService(EventBus& event_bus, Config& config,
                      VaultService& vault_service, ThemeEngine& theme_engine);

    /// Load the vault's custom CSS file.
    [[nodiscard]] auto load_vault_style() -> std::expected<VaultStyle, std::string>;

    /// Get the current vault's merged CSS (theme + vault custom).
    [[nodiscard]] auto get_merged_css() const -> std::string;

    /// Save custom CSS content to the vault.
    [[nodiscard]] auto save_vault_style(const std::string& css_content)
        -> std::expected<void, std::string>;

    /// Validate CSS content (basic syntax check).
    [[nodiscard]] auto validate_css(const std::string& css) const
        -> std::pair<bool, std::string>;

    /// Extract CSS variables from content.
    [[nodiscard]] auto extract_variables(const std::string& css) const
        -> std::vector<CssVariable>;

    /// Get the path to the vault's CSS file.
    [[nodiscard]] auto vault_css_path() const -> std::filesystem::path;

    /// Create a default CSS file with common variable overrides.
    auto create_default_css() -> void;

    /// Watch for CSS file changes (live reload).
    auto start_css_watcher() -> void;
    auto stop_css_watcher() -> void;

private:
    EventBus& event_bus_;
    Config& config_;
    VaultService& vault_service_;
    ThemeEngine& theme_engine_;

    VaultStyle current_style_;
    bool watcher_active_{false};

    auto on_css_file_changed() -> void;

    Subscription vault_opened_sub_;
    Subscription theme_changed_sub_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`load_vault_style()`** -- Check if `.markamp/styles.css` exists. Read content. Validate CSS. Extract variables. Store in current_style_. Publish VaultStyleLoadedEvent.

2. **`get_merged_css()`** -- Combine base theme CSS from ThemeEngine with vault custom CSS. Custom CSS overrides theme styles. Return merged string.

3. **`validate_css(css)`** -- Check for balanced braces, valid selectors, valid property names. Return (valid, error_message).

4. **`extract_variables(css)`** -- Parse `:root { --name: value; }` declarations. Return list of CssVariable.

5. **`create_default_css()`** -- Write a template CSS file with common customization points documented as comments.

6. **`start_css_watcher()`** -- Watch the vault CSS file for changes. On change, reload and publish VaultStyleChangedEvent for live preview.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultStyleLoadedEvent)
std::string vault_path;
bool has_custom_css{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultStyleChangedEvent)
std::string css_content;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_vault_style.cpp`

1. **Load existing CSS** -- Vault with styles.css. Verify loaded.
2. **No CSS file** -- Vault without styles.css. Verify empty style, no error.
3. **Merge with theme** -- Theme provides base CSS. Vault adds overrides. Verify merged.
4. **CSS validation valid** -- Valid CSS. Verify valid=true.
5. **CSS validation invalid** -- Unbalanced braces. Verify error returned.
6. **Extract variables** -- CSS with 3 `--custom-*` variables. Verify extracted.
7. **Save CSS** -- Save new CSS content. Verify file written.
8. **Create default** -- Create default CSS. Verify file has template content.
9. **Live reload** -- Modify CSS file. Verify VaultStyleChangedEvent published.
10. **Variables override theme** -- CSS variable overrides theme variable. Verify new value used.

## Acceptance Criteria

- [ ] Vault custom CSS loads from `.markamp/styles.css`
- [ ] Custom CSS merges with and overrides base theme
- [ ] CSS variables are extracted and can override theme variables
- [ ] CSS validation catches syntax errors
- [ ] Live reload updates preview on CSS file save
- [ ] Default CSS template includes documentation
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/VaultStyleService.h` | VaultStyleService, VaultStyle, CssVariable |
| CREATE | `src/core/VaultStyleService.cpp` | Full implementation |
| MODIFY | `src/ui/PreviewPanel.cpp` | Apply vault CSS to preview rendering |
| MODIFY | `src/core/Events.h` | Add 2 vault style events |
| MODIFY | `src/CMakeLists.txt` | Add VaultStyleService.cpp |
| CREATE | `tests/unit/test_vault_style.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_vault_style target |

## Estimated Complexity

**M** -- CSS loading/merging, variable extraction, validation, file watching, 10 tests.
