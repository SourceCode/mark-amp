/// @file IconProvider.cpp
/// @brief V16 Phase 07 — Icon provider with manifest-based command icon resolution.
///
/// Upgraded to try manifest-based command icon lookup first,
/// falling back to emoji-based category defaults.

#include "ui/IconProvider.h"

#include "ui/IconManifest.h"

#include <filesystem>

namespace markamp::ui
{

namespace
{

/// Shared manifest for command icon resolution (same as FileTypeIconResolver).
auto get_command_manifest() -> const IconManifest&
{
    static const IconManifest manifest = []() -> IconManifest
    {
        IconManifest result;
        const std::filesystem::path manifest_path = "resources/icons/icon_manifest.json";
        if (std::filesystem::exists(manifest_path))
        {
            [[maybe_unused]] auto loaded = result.load_from_file(manifest_path);
        }
        return result;
    }();
    return manifest;
}

} // namespace

IconProvider::IconProvider()
{
    // Register default category icons (emoji fallback when manifest not available)
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
    // V16: Try manifest-based command icon resolution first
    if (!icon_id.empty())
    {
        const auto& manifest = get_command_manifest();
        if (manifest.icon_count() > 0)
        {
            auto canonical_id = manifest.resolve_command_icon(icon_id);
            if (canonical_id != IconManifest::kFallbackCommandIcon)
            {
                // Return the canonical ID — callers can use this with IconManager
                return canonical_id;
            }
        }

        // Legacy: try specific icon registry
        auto specific_it = specific_icons_.find(icon_id);
        if (specific_it != specific_icons_.end())
        {
            return specific_it->second;
        }
    }

    // Fall back to category default
    return icon_for_category(category);
}

auto IconProvider::icon_for_category(const std::string& category) const -> std::string
{
    auto category_it = category_icons_.find(category);
    if (category_it != category_icons_.end())
    {
        return category_it->second;
    }
    return kDefaultIcon;
}

void IconProvider::register_icon(const std::string& icon_id, const std::string& icon_char)
{
    specific_icons_[icon_id] = icon_char;
}

auto IconProvider::has_icon(const std::string& icon_id) const -> bool
{
    // Check manifest first, then specific icons
    const auto& manifest = get_command_manifest();
    if (manifest.icon_count() > 0)
    {
        auto canonical_id = manifest.resolve_command_icon(icon_id);
        if (canonical_id != IconManifest::kFallbackCommandIcon)
        {
            return true;
        }
    }
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
