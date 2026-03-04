/// @file IconProvider.h
/// @brief V13 Phase 31 Task 8 — Maps command icon IDs to category defaults.
///
/// Provides icon resolution for command palette items. Commands with specific
/// icon IDs get their icon; others fall back to category-based defaults.
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
/// auto icon = icons.icon_for_command("bold", "Editor");
/// // Returns the "Editor" category icon if no specific "bold" icon exists
/// ```
class IconProvider
{
public:
    IconProvider();

    /// Get the icon character for a command by its icon ID.
    /// Falls back to category-based default if no specific icon is registered.
    [[nodiscard]] auto icon_for_command(const std::string& icon_id,
                                        const std::string& category) const -> std::string;

    /// Get the default icon for a category.
    [[nodiscard]] auto icon_for_category(const std::string& category) const -> std::string;

    /// Register a specific icon for an icon ID.
    void register_icon(const std::string& icon_id, const std::string& icon_char);

    /// Check if a specific icon is registered.
    [[nodiscard]] auto has_icon(const std::string& icon_id) const -> bool;

    /// Get the count of registered specific icons.
    [[nodiscard]] auto specific_icon_count() const -> size_t;

    /// Get the count of category defaults.
    [[nodiscard]] auto category_count() const -> size_t;

    // ── Default category icon strings ──
    static constexpr const char* kFileIcon = "📄";
    static constexpr const char* kEditIcon = "✏️";
    static constexpr const char* kViewIcon = "👁";
    static constexpr const char* kNavigationIcon = "🧭";
    static constexpr const char* kTerminalIcon = "💻";
    static constexpr const char* kExtensionIcon = "🧩";
    static constexpr const char* kEditorIcon = "📝";
    static constexpr const char* kSearchIcon = "🔍";
    static constexpr const char* kDebugIcon = "🐛";
    static constexpr const char* kSettingsIcon = "⚙️";
    static constexpr const char* kDefaultIcon = "▪";

private:
    std::unordered_map<std::string, std::string> specific_icons_;
    std::unordered_map<std::string, std::string> category_icons_;
};

} // namespace markamp::ui
