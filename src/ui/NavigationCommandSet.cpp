#include "NavigationCommandSet.h"

#include <algorithm>
#include <iterator>

namespace markamp::ui
{

auto NavigationCommand::scope_label() const -> std::string
{
    switch (scope)
    {
        case NavigationScope::kGlobal:
            return "Global";
        case NavigationScope::kPane:
            return "Pane";
        case NavigationScope::kEditor:
            return "Editor";
    }
    return "Unknown";
}

auto NavigationCommandSet::standard_commands() -> std::vector<NavigationCommand>
{
    return {
        // Global navigation
        {"nav.back", "Go Back", "Alt+Left", "arrow-left", NavigationScope::kGlobal, true},
        {"nav.forward", "Go Forward", "Alt+Right", "arrow-right", NavigationScope::kGlobal, true},
        {"nav.recent_locations",
         "Recent Locations",
         "Ctrl+Shift+E",
         "history",
         NavigationScope::kGlobal,
         true},
        {"nav.quick_switch", "Quick Switch", "Ctrl+Tab", "switch", NavigationScope::kGlobal, true},

        // File navigation
        {"nav.go_to_file", "Go to File", "Ctrl+P", "file-search", NavigationScope::kGlobal, true},
        {"nav.go_to_symbol",
         "Go to Symbol",
         "Ctrl+Shift+O",
         "symbol-search",
         NavigationScope::kGlobal,
         true},
        {"nav.go_to_line", "Go to Line", "Ctrl+G", "goto-line", NavigationScope::kEditor, true},

        // Pane navigation
        {"nav.focus_next_pane",
         "Focus Next Pane",
         "Ctrl+K Ctrl+Right",
         "pane-right",
         NavigationScope::kPane,
         true},
        {"nav.focus_previous_pane",
         "Focus Previous Pane",
         "Ctrl+K Ctrl+Left",
         "pane-left",
         NavigationScope::kPane,
         true},

        // Editor-specific
        {"nav.go_to_definition",
         "Go to Definition",
         "F12",
         "definition",
         NavigationScope::kEditor,
         true},
        {"nav.peek_definition",
         "Peek Definition",
         "Alt+F12",
         "peek",
         NavigationScope::kEditor,
         true},
        {"nav.go_to_references",
         "Go to References",
         "Shift+F12",
         "references",
         NavigationScope::kEditor,
         true},
        // Editor-specific (go-to-bracket)
        {"nav.go_to_bracket",
         "Go to Bracket",
         "Ctrl+Shift+\\",
         "bracket",
         NavigationScope::kEditor,
         true},
    };
}

auto NavigationCommandSet::commands_for_scope(NavigationScope scope)
    -> std::vector<NavigationCommand>
{
    auto all_cmds = standard_commands();
    std::vector<NavigationCommand> filtered;
    std::copy_if(all_cmds.begin(),
                 all_cmds.end(),
                 std::back_inserter(filtered),
                 [scope](const NavigationCommand& cmd) { return cmd.scope == scope; });
    return filtered;
}

auto NavigationCommandSet::find_command(const std::string& command_id) -> const NavigationCommand*
{
    static auto commands = standard_commands();
    for (const auto& cmd : commands)
    {
        if (cmd.command_id == command_id)
        {
            return &cmd;
        }
    }
    return nullptr;
}

auto NavigationCommandSet::command_count() -> int
{
    return static_cast<int>(standard_commands().size());
}

auto NavigationCommandSet::available_scopes() -> std::vector<NavigationScope>
{
    return {NavigationScope::kGlobal,
            NavigationScope::kPane,
            NavigationScope::kEditor};
}

} // namespace markamp::ui
