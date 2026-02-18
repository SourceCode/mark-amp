// ============================================================================
// File: src/core/AccessibilityCommandProvider.cpp
// Phase 31: Accessibility System — Command palette commands
// ============================================================================

#include "AccessibilityCommandProvider.h"

#include <algorithm>

namespace markamp::core
{

AccessibilityCommandProvider::AccessibilityCommandProvider()
{
    register_commands();
}

auto AccessibilityCommandProvider::commands() const -> const std::vector<AccessibilityCommand>&
{
    return commands_;
}

auto AccessibilityCommandProvider::find_command(const std::string& cmd_id) const
    -> std::optional<AccessibilityCommand>
{
    auto iter =
        std::find_if(commands_.begin(),
                     commands_.end(),
                     [&cmd_id](const AccessibilityCommand& cmd) { return cmd.id == cmd_id; });
    if (iter != commands_.end())
    {
        return *iter;
    }
    return std::nullopt;
}

auto AccessibilityCommandProvider::commands_in_category(const std::string& category) const
    -> std::vector<AccessibilityCommand>
{
    std::vector<AccessibilityCommand> result;
    for (const auto& cmd : commands_)
    {
        if (cmd.category == category)
        {
            result.push_back(cmd);
        }
    }
    return result;
}

auto AccessibilityCommandProvider::categories() const -> std::vector<std::string>
{
    std::vector<std::string> cats;
    for (const auto& cmd : commands_)
    {
        if (std::find(cats.begin(), cats.end(), cmd.category) == cats.end())
        {
            cats.push_back(cmd.category);
        }
    }
    return cats;
}

auto AccessibilityCommandProvider::command_count() const -> std::size_t
{
    return commands_.size();
}

auto AccessibilityCommandProvider::register_commands() -> void
{
    commands_ = {
        {"a11y.toggleHighContrast",
         "Accessibility: Toggle High Contrast",
         "Accessibility",
         "",
         "Toggle high contrast mode for improved visibility"},
        {"a11y.toggleReducedMotion",
         "Accessibility: Toggle Reduced Motion",
         "Accessibility",
         "",
         "Toggle reduced motion to disable all animations"},
        {"a11y.runAudit",
         "Accessibility: Run Audit",
         "Accessibility",
         "",
         "Scan current view for accessibility issues"},
        {"a11y.zoomIn",
         "Accessibility: Increase UI Scale",
         "Zoom",
         "Cmd+=",
         "Increase UI scale by 10%"},
        {"a11y.zoomOut",
         "Accessibility: Decrease UI Scale",
         "Zoom",
         "Cmd+-",
         "Decrease UI scale by 10%"},
        {"a11y.zoomReset",
         "Accessibility: Reset UI Scale",
         "Zoom",
         "Cmd+0",
         "Reset UI scale to 100%"},
        {"a11y.readLine",
         "Accessibility: Read Current Line",
         "Screen Reader",
         "Ctrl+Alt+Space",
         "Announce the current line to screen reader"},
        {"a11y.navigateHeading",
         "Accessibility: Navigate to Next Heading",
         "Screen Reader",
         "Alt+H",
         "Jump to the next heading in the document"},
    };
}

} // namespace markamp::core
