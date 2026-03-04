/// @file IconProvider.cpp
/// @brief V13 Phase 31 Task 8 — Icon provider implementation.

#include "ui/IconProvider.h"

namespace markamp::ui
{

IconProvider::IconProvider()
{
    // Register default category icons
    category_icons_["File"] = kFileIcon;
    category_icons_["Edit"] = kEditIcon;
    category_icons_["View"] = kViewIcon;
    category_icons_["Navigation"] = kNavigationIcon;
    category_icons_["Go To"] = kNavigationIcon;
    category_icons_["Terminal"] = kTerminalIcon;
    category_icons_["Extensions"] = kExtensionIcon;
    category_icons_["Editor"] = kEditorIcon;
    category_icons_["Search"] = kSearchIcon;
    category_icons_["Debug"] = kDebugIcon;
    category_icons_["Settings"] = kSettingsIcon;
    category_icons_["Preferences"] = kSettingsIcon;
    category_icons_["Help"] = "❓";
    category_icons_["Window"] = "🪟";
    category_icons_["Selection"] = "🔲";
    category_icons_["Format"] = "📐";
}

auto IconProvider::icon_for_command(const std::string& icon_id, const std::string& category) const
    -> std::string
{
    // Try specific icon first
    if (!icon_id.empty())
    {
        auto it = specific_icons_.find(icon_id);
        if (it != specific_icons_.end())
        {
            return it->second;
        }
    }

    // Fall back to category default
    return icon_for_category(category);
}

auto IconProvider::icon_for_category(const std::string& category) const -> std::string
{
    auto it = category_icons_.find(category);
    if (it != category_icons_.end())
    {
        return it->second;
    }
    return kDefaultIcon;
}

void IconProvider::register_icon(const std::string& icon_id, const std::string& icon_char)
{
    specific_icons_[icon_id] = icon_char;
}

auto IconProvider::has_icon(const std::string& icon_id) const -> bool
{
    return specific_icons_.count(icon_id) > 0;
}

auto IconProvider::specific_icon_count() const -> size_t
{
    return specific_icons_.size();
}

auto IconProvider::category_count() const -> size_t
{
    return category_icons_.size();
}

} // namespace markamp::ui
