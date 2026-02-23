#include "TokenInheritanceResolver.h"

#include <unordered_set>

namespace markamp::core
{

TokenInheritanceResolver::TokenInheritanceResolver()
{
    apply_default_rules();
}

void TokenInheritanceResolver::register_fallback(const std::string& token_name,
                                                 const std::string& fallback_token)
{
    fallback_rules_[token_name] = fallback_token;
}

auto TokenInheritanceResolver::get_fallback(const std::string& token_name) const
    -> std::optional<std::string>
{
    if (const auto it = fallback_rules_.find(token_name); it != fallback_rules_.end())
    {
        return it->second;
    }
    return std::nullopt;
}

auto TokenInheritanceResolver::get_resolution_chain(const std::string& token_name) const
    -> std::vector<std::string>
{
    std::vector<std::string> chain;
    std::unordered_set<std::string> visited;

    std::string current = token_name;

    while (!current.empty() && !visited.contains(current))
    {
        chain.push_back(current);
        visited.insert(current);

        if (const auto fallback = get_fallback(current))
        {
            current = *fallback;
        }
        else
        {
            break; // No further fallback
        }
    }

    return chain;
}

void TokenInheritanceResolver::apply_default_rules()
{
    // Global Window
    register_fallback("window.background", "editor.background");
    register_fallback("window.foreground", "editor.foreground");
    register_fallback("window.border", "contrastBorder");

    // Title Bar
    register_fallback("titleBar.activeBackground", "window.background");
    register_fallback("titleBar.activeForeground", "window.foreground");
    register_fallback("titleBar.inactiveBackground", "titleBar.activeBackground");
    register_fallback("titleBar.inactiveForeground", "titleBar.activeForeground");

    // Activity Bar
    register_fallback("activityBar.background", "window.background");
    register_fallback("activityBar.foreground", "window.foreground");
    register_fallback("activityBar.inactiveForeground", "activityBar.foreground");
    register_fallback("activityBarBadge.background", "focusBorder");
    register_fallback("activityBarBadge.foreground", "window.foreground");

    // Sidebar
    register_fallback("sideBar.background", "window.background");
    register_fallback("sideBar.foreground", "window.foreground");
    register_fallback("sideBar.border", "window.border");
    register_fallback("sideBarSectionHeader.background", "sideBar.background");
    register_fallback("sideBarSectionHeader.foreground", "sideBar.foreground");
    register_fallback("sideBarSectionHeader.border", "window.border");

    // Tab Bar / Editor Group
    register_fallback("editorGroupHeader.tabsBackground", "window.background");
    register_fallback("editorGroupHeader.tabsBorder", "window.border");
    register_fallback("tab.activeBackground", "editor.background");
    register_fallback("tab.activeForeground", "editor.foreground");
    register_fallback("tab.activeBorder", "focusBorder");
    register_fallback("tab.inactiveBackground", "editorGroupHeader.tabsBackground");
    register_fallback("tab.inactiveForeground", "window.foreground");
    register_fallback("tab.hoverBackground", "list.hoverBackground");
    register_fallback("tab.border", "window.border");

    // Editor
    // 'editor.background' and 'editor.foreground' have no fallback; they are the root for text.
    register_fallback("editor.selectionBackground", "selection.background");
    register_fallback("editor.lineHighlightBackground", "list.hoverBackground");
    register_fallback("editorLineNumber.foreground",
                      "window.foreground"); // Will be overriden or dimmed
    register_fallback("editorLineNumber.activeForeground", "editor.foreground");
    register_fallback("editorCursor.foreground", "editor.foreground");

    // Status Bar
    register_fallback("statusBar.background", "window.background");
    register_fallback("statusBar.foreground", "window.foreground");
    register_fallback("statusBar.border", "window.border");
    register_fallback("statusBar.noFolderBackground", "statusBar.background");

    // Lists / Trees
    register_fallback("list.activeSelectionBackground", "selection.background");
    register_fallback("list.activeSelectionForeground", "window.foreground");
    register_fallback("list.inactiveSelectionBackground", "list.activeSelectionBackground");
    register_fallback("list.inactiveSelectionForeground", "list.activeSelectionForeground");
    register_fallback("list.hoverBackground",
                      "editor.background"); // Needs dimming/highlighting in practice
    register_fallback("list.hoverForeground", "window.foreground");

    // Inputs
    register_fallback("input.background", "editor.background");
    register_fallback("input.foreground", "editor.foreground");
    register_fallback("input.border", "window.border");

    // Buttons
    register_fallback("button.background", "focusBorder"); // Often accent color
    register_fallback("button.foreground", "window.foreground");
    register_fallback("button.hoverBackground", "button.background"); // Usually lightened

    // Scrollbar
    register_fallback("scrollbarSlider.background", "window.border");
    register_fallback("scrollbarSlider.hoverBackground", "scrollbarSlider.background");
    register_fallback("scrollbarSlider.activeBackground", "scrollbarSlider.hoverBackground");

    // Minimap
    register_fallback("minimap.background", "editor.background");

    // Panels (Bottom)
    register_fallback("panel.background", "editor.background");
    register_fallback("panel.border", "window.border");
    register_fallback("panelTitle.activeForeground", "panel.foreground");
    register_fallback("panelTitle.inactiveForeground", "panel.foreground");
    register_fallback("panelTitle.activeBorder", "focusBorder");

    // Badges
    register_fallback("badge.background", "activityBarBadge.background");
    register_fallback("badge.foreground", "activityBarBadge.foreground");

    // Breadcrumbs
    register_fallback("breadcrumb.foreground", "window.foreground");
    register_fallback("breadcrumb.background", "editor.background");
    register_fallback("breadcrumb.focusForeground", "editor.foreground");
    register_fallback("breadcrumb.activeSelectionForeground", "editor.foreground");

    // Notifications
    register_fallback("notificationCenterHeader.background", "editor.background");
    register_fallback("notificationCenterHeader.foreground", "editor.foreground");
    register_fallback("notifications.background", "editor.background");
    register_fallback("notifications.foreground", "editor.foreground");
    register_fallback("notifications.border", "window.border");
}

} // namespace markamp::core
