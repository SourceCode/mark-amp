/// @file PresentationCommandProvider.cpp
/// @brief V9 Phase 43 — PresentationCommandProvider implementation.

#include "PresentationCommandProvider.h"

namespace markamp::core
{

void PresentationCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto PresentationCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "pres.create",
        "pres.open",
        "pres.start",
        "pres.stop",
        "pres.presenterMode",
        "pres.export",
        "pres.setTheme",
        "pres.rehearse",
    };
}

auto PresentationCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto PresentationCommandProvider::command_count() -> int
{
    return 8;
}

auto PresentationCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "pres.create";
        cmd.title = "Create Presentation";
        cmd.category = "Presentation";
        cmd.description = "Create a new presentation from Markdown";
        cmd.icon = "slides";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pres.open";
        cmd.title = "Open Presentation";
        cmd.category = "Presentation";
        cmd.description = "Open an existing presentation";
        cmd.icon = "folder-open";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pres.start";
        cmd.title = "Start Slideshow";
        cmd.category = "Presentation";
        cmd.description = "Begin fullscreen slideshow";
        cmd.icon = "play";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pres.stop";
        cmd.title = "Stop Slideshow";
        cmd.category = "Presentation";
        cmd.description = "Stop the current slideshow";
        cmd.icon = "stop";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pres.presenterMode";
        cmd.title = "Toggle Presenter Mode";
        cmd.category = "Presentation";
        cmd.description = "Show presenter view with notes and timer";
        cmd.icon = "presenter";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pres.export";
        cmd.title = "Export Presentation";
        cmd.category = "Presentation";
        cmd.description = "Export slides to PDF, HTML, or images";
        cmd.icon = "export";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pres.setTheme";
        cmd.title = "Set Slide Theme";
        cmd.category = "Presentation";
        cmd.description = "Choose a theme for the presentation";
        cmd.icon = "palette";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pres.rehearse";
        cmd.title = "Rehearse Presentation";
        cmd.category = "Presentation";
        cmd.description = "Rehearse with timer and note prompts";
        cmd.icon = "clock";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
