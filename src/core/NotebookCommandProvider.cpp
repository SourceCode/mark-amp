/// @file NotebookCommandProvider.cpp
/// @brief P08-T05: Notebook commands, menus, and activity bar entry points.

#include "NotebookCommandProvider.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

NotebookCommandProvider::NotebookCommandProvider(EventBus& bus)
    : event_bus_(bus)
    , notebook_open_sub_(bus.subscribe<events::ActiveFileChangedEvent>(
          [](const events::ActiveFileChangedEvent& /*evt*/)
          {
              // Could check file extension to auto-set notebook_active_
          }))
{
    register_built_in_commands();
}

void NotebookCommandProvider::register_command(const NotebookCommand& cmd)
{
    commands_.push_back(cmd);
    MARKAMP_LOG_DEBUG("Notebook command registered: {}", cmd.command_id);
}

auto NotebookCommandProvider::is_enabled(const std::string& command_id) const -> bool
{
    for (const auto& cmd : commands_)
    {
        if (cmd.command_id == command_id)
        {
            if (cmd.requires_active_notebook && !notebook_active_)
            {
                return false;
            }
            return true;
        }
    }
    return false;
}

void NotebookCommandProvider::register_built_in_commands()
{
    register_command({"notebook.create", "Create Notebook", "Notebook", false});
    register_command({"notebook.open", "Open Notebook", "Notebook", false});
    register_command({"notebook.save", "Save Notebook", "Notebook", true});
    register_command({"notebook.runCell", "Run Cell", "Notebook", true});
    register_command({"notebook.runAll", "Run All Cells", "Notebook", true});
    register_command({"notebook.addCell", "Add Cell", "Notebook", true});
    register_command({"notebook.deleteCell", "Delete Cell", "Notebook", true});
    register_command({"notebook.moveCellUp", "Move Cell Up", "Notebook", true});
    register_command({"notebook.moveCellDown", "Move Cell Down", "Notebook", true});
    register_command({"notebook.clearOutputs", "Clear All Outputs", "Notebook", true});
    register_command({"notebook.export", "Export Notebook", "Notebook", true});
    register_command({"notebook.cancelExecution", "Cancel Execution", "Notebook", true});

    MARKAMP_LOG_INFO("Notebook commands registered: {}", commands_.size());
    event_bus_.publish(events::NotificationEvent{
        "Notebook commands ready",
        events::NotificationLevel::Info, 0});
}

} // namespace markamp::core
