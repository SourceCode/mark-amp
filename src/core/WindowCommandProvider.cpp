/// @file WindowCommandProvider.cpp
/// @brief V9 Phase 46 — WindowCommandProvider implementation.

#include "WindowCommandProvider.h"

namespace markamp::core
{

void WindowCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto WindowCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "window.new",
        "window.close",
        "window.toggleFullscreen",
        "window.minimize",
        "window.maximize",
        "window.splitRight",
        "window.splitDown",
        "window.focusNextGroup",
    };
}

auto WindowCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
{
    for (auto& entry : build_entries())
    {
        if (entry.id == command_id)
        {
            return entry;
        }
    }
    return {};
}

auto WindowCommandProvider::command_count() -> int
{
    return 8;
}

auto WindowCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    auto make = [&](const std::string& cmd_id,
                    const std::string& cmd_title,
                    const std::string& desc,
                    const std::string& icon)
    {
        CommandEntry cmd;
        cmd.id = cmd_id;
        cmd.title = cmd_title;
        cmd.category = "Window";
        cmd.description = desc;
        cmd.icon = icon;
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    };

    make("window.new", "New Window", "Open a new application window", "plus");
    make("window.close", "Close Window", "Close the current window", "x");
    make("window.toggleFullscreen", "Toggle Fullscreen", "Toggle fullscreen mode", "maximize");
    make("window.minimize", "Minimize Window", "Minimize the current window", "minus");
    make("window.maximize", "Maximize Window", "Maximize the current window", "square");
    make("window.splitRight", "Split Right", "Split editor to the right", "columns");
    make("window.splitDown", "Split Down", "Split editor below", "rows");
    make("window.focusNextGroup",
         "Focus Next Group",
         "Move focus to the next editor group",
         "arrow-right");

    return entries;
}

} // namespace markamp::core
