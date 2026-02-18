/// @file SlideCommandProvider.cpp
/// @brief V9 Phase 43 — SlideCommandProvider implementation.

#include "SlideCommandProvider.h"

namespace markamp::core
{

void SlideCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto SlideCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "slide.add",
        "slide.remove",
        "slide.duplicate",
        "slide.moveUp",
        "slide.moveDown",
        "slide.setLayout",
        "slide.addBlock",
        "slide.setTransition",
    };
}

auto SlideCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto SlideCommandProvider::command_count() -> int
{
    return 8;
}

auto SlideCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "slide.add";
        cmd.title = "Add Slide";
        cmd.category = "Slides";
        cmd.description = "Add a new slide after the current one";
        cmd.icon = "plus";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "slide.remove";
        cmd.title = "Remove Slide";
        cmd.category = "Slides";
        cmd.description = "Remove the current slide";
        cmd.icon = "trash";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "slide.duplicate";
        cmd.title = "Duplicate Slide";
        cmd.category = "Slides";
        cmd.description = "Duplicate the current slide";
        cmd.icon = "copy";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "slide.moveUp";
        cmd.title = "Move Slide Up";
        cmd.category = "Slides";
        cmd.description = "Move the current slide up in order";
        cmd.icon = "arrow-up";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "slide.moveDown";
        cmd.title = "Move Slide Down";
        cmd.category = "Slides";
        cmd.description = "Move the current slide down in order";
        cmd.icon = "arrow-down";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "slide.setLayout";
        cmd.title = "Set Slide Layout";
        cmd.category = "Slides";
        cmd.description = "Choose a layout for the current slide";
        cmd.icon = "layout";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "slide.addBlock";
        cmd.title = "Add Content Block";
        cmd.category = "Slides";
        cmd.description = "Add a new content block to the slide";
        cmd.icon = "block";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "slide.setTransition";
        cmd.title = "Set Transition";
        cmd.category = "Slides";
        cmd.description = "Set the transition effect for this slide";
        cmd.icon = "transition";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
