/// @file WorkbenchCommands.h
/// @brief P02-T02: Central registration of core workbench commands.
///
/// Registers the canonical set of first-party commands into a CommandRegistry.
/// Each command has a unique ID, display metadata, and an execute function that
/// publishes the corresponding event through the EventBus.
#pragma once

namespace markamp::core
{
class CommandRegistry;
class EventBus;

/// Register the core workbench commands (File, Edit, View, Window categories).
/// These are the ~30 most-used commands that previously had duplicate inline
/// lambdas in MainFrame, Toolbar, and CommandPalette.
void register_workbench_commands(CommandRegistry& registry, EventBus& event_bus);

} // namespace markamp::core
