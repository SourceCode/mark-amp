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

    // ── Renderable icon characters (used by wxWidgets text drawing) ────────
    static constexpr const char* kFileIcon       = "\xF0\x9F\x93\x84"; // 📄
    static constexpr const char* kEditIcon       = "\xE2\x9C\x8F\xEF\xB8\x8F"; // ✏️
    static constexpr const char* kViewIcon       = "\xF0\x9F\x91\x81"; // 👁
    static constexpr const char* kNavigationIcon = "\xF0\x9F\xA7\xAD"; // 🧭
    static constexpr const char* kTerminalIcon   = "\xF0\x9F\x92\xBB"; // 💻
    static constexpr const char* kExtensionIcon  = "\xF0\x9F\xA7\xA9"; // 🧩
    static constexpr const char* kEditorIcon     = "\xF0\x9F\x93\x9D"; // 📝
    static constexpr const char* kSearchIcon     = "\xF0\x9F\x94\x8D"; // 🔍
    static constexpr const char* kDebugIcon      = "\xF0\x9F\x90\x9B"; // 🐛
    static constexpr const char* kSettingsIcon   = "\xE2\x9A\x99\xEF\xB8\x8F"; // ⚙️
    static constexpr const char* kDefaultIcon    = "\xE2\x96\xAA"; // ▪

    // ── V27: Canonical MUI icon identifiers (for future icon renderer) ───
    static constexpr const char* kMuiFileIcon       = "mui-file-text";
    static constexpr const char* kMuiEditIcon       = "mui-pencil";
    static constexpr const char* kMuiViewIcon       = "mui-eye";
    static constexpr const char* kMuiNavigationIcon = "mui-compass";
    static constexpr const char* kMuiTerminalIcon   = "mui-terminal";
    static constexpr const char* kMuiExtensionIcon  = "mui-puzzle-piece";
    static constexpr const char* kMuiEditorIcon     = "mui-edit-3";
    static constexpr const char* kMuiSearchIcon     = "mui-search";
    static constexpr const char* kMuiDebugIcon      = "mui-bug";
    static constexpr const char* kMuiSettingsIcon   = "mui-settings";
    static constexpr const char* kMuiDefaultIcon    = "mui-circle-dot";

    // ── V27: Additional category icons ───────────────────────────────────
    static constexpr const char* kCanvasIcon     = "\xF0\x9F\x96\xBC"; // 🖼
    static constexpr const char* kNotebookIcon   = "\xF0\x9F\x93\x93"; // 📓
    static constexpr const char* kThemeIcon      = "\xF0\x9F\x8E\xA8"; // 🎨
    static constexpr const char* kExportIcon     = "\xF0\x9F\x93\xA4"; // 📤
    static constexpr const char* kAIIcon         = "\xE2\x9C\xA8"; // ✨
    static constexpr const char* kMuiCanvasIcon     = "mui-layout";
    static constexpr const char* kMuiNotebookIcon   = "mui-book-open";
    static constexpr const char* kMuiThemeIcon      = "mui-palette";
    static constexpr const char* kMuiExportIcon     = "mui-share";
    static constexpr const char* kMuiAIIcon         = "mui-sparkles";

    /// V27: Total number of category icon constants.
    [[nodiscard]] static constexpr auto v27_category_icon_count() noexcept -> int { return 16; }

private:
    std::unordered_map<std::string, std::string> specific_icons_;
    std::unordered_map<std::string, std::string> category_icons_;
};

} // namespace markamp::ui
