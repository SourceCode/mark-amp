/// @file IconProvider.h
/// @brief V13 Phase 31 Task 8 + V27-P03-T03 — Maps command icon IDs to category defaults.
///
/// Provides icon resolution for command palette items. V27 replaces all
/// emoji constants with canonical MUI icon identifiers and deprecates the
/// legacy icon_for_command API.
#pragma once

#include <string>
#include <unordered_map>

namespace markamp::ui
{

/// Category-based default icon mappings for the command palette.
///
/// Usage:
/// ```cpp
/// IconProvider icons;
/// auto icon = icons.icon_for_command_v27("bold", "Editor");
/// // Returns the "Editor" category MUI icon if no specific "bold" icon exists
/// ```
class IconProvider
{
public:
    IconProvider();

    /// V27: Get the MUI icon ID for a command.
    /// Falls back to category-based default if no specific icon is registered.
    [[nodiscard]] auto icon_for_command_v27(const std::string& icon_id,
                                            const std::string& category) const -> std::string;

    /// @deprecated Use icon_for_command_v27 instead.
    [[deprecated("Use icon_for_command_v27 — V27 canonical MUI icons")]]
    [[nodiscard]] auto icon_for_command(const std::string& icon_id,
                                        const std::string& category) const -> std::string;

    /// Get the default MUI icon for a category.
    [[nodiscard]] auto icon_for_category(const std::string& category) const -> std::string;

    /// Register a specific icon for an icon ID.
    void register_icon(const std::string& icon_id, const std::string& icon_char);

    /// Check if a specific icon is registered.
    [[nodiscard]] auto has_icon(const std::string& icon_id) const -> bool;

    /// Get the count of registered specific icons.
    [[nodiscard]] auto specific_icon_count() const -> size_t;

    /// Get the count of category defaults.
    [[nodiscard]] auto category_count() const -> size_t;

    // ── V27: Canonical MUI icon identifiers replacing legacy emoji ───────
    static constexpr const char* kFileIcon       = "mui-file-text";
    static constexpr const char* kEditIcon       = "mui-pencil";
    static constexpr const char* kViewIcon       = "mui-eye";
    static constexpr const char* kNavigationIcon = "mui-compass";
    static constexpr const char* kTerminalIcon   = "mui-terminal";
    static constexpr const char* kExtensionIcon  = "mui-puzzle-piece";
    static constexpr const char* kEditorIcon     = "mui-edit-3";
    static constexpr const char* kSearchIcon     = "mui-search";
    static constexpr const char* kDebugIcon      = "mui-bug";
    static constexpr const char* kSettingsIcon   = "mui-settings";
    static constexpr const char* kDefaultIcon    = "mui-circle-dot";

    // ── V27: Additional category icons ───────────────────────────────────
    static constexpr const char* kCanvasIcon     = "mui-layout";
    static constexpr const char* kNotebookIcon   = "mui-book-open";
    static constexpr const char* kThemeIcon      = "mui-palette";
    static constexpr const char* kExportIcon     = "mui-share";
    static constexpr const char* kAIIcon         = "mui-sparkles";

    /// V27: Total number of category icon constants.
    [[nodiscard]] static constexpr auto v27_category_icon_count() noexcept -> int { return 16; }

private:
    std::unordered_map<std::string, std::string> specific_icons_;
    std::unordered_map<std::string, std::string> category_icons_;
};

} // namespace markamp::ui
