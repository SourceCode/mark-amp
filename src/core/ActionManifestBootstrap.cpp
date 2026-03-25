/// @file ActionManifestBootstrap.cpp
/// @brief V25 P04: Action manifest bootstrap implementation.
#include "core/ActionManifestBootstrap.h"

namespace markamp::core
{

static constexpr int kReleaseActionCount = 34;

void bootstrap_release_actions(ActionManifest& manifest)
{
    auto reg = [&](const std::string& id, const std::string& label,
                   ActionCategory cat, int order) {
        ManifestAction action;
        action.action_id = id;
        action.label = label;
        action.category = cat;
        action.sort_order = order;
        action.is_visible = true;
        action.handler = []() {};
        [[maybe_unused]] auto ok = manifest.register_action(std::move(action));
    };

    // File actions
    reg("file.new",        "New File",       ActionCategory::kFile, 1);
    reg("file.open",       "Open File",      ActionCategory::kFile, 2);
    reg("file.save",       "Save",           ActionCategory::kFile, 5);
    reg("file.saveAs",     "Save As",        ActionCategory::kFile, 6);
    reg("file.close",      "Close",          ActionCategory::kFile, 7);
    reg("file.closeAll",   "Close All",      ActionCategory::kFile, 8);

    // Edit actions
    reg("edit.undo",       "Undo",           ActionCategory::kEdit, 1);
    reg("edit.redo",       "Redo",           ActionCategory::kEdit, 2);
    reg("edit.cut",        "Cut",            ActionCategory::kEdit, 3);
    reg("edit.copy",       "Copy",           ActionCategory::kEdit, 4);
    reg("edit.paste",      "Paste",          ActionCategory::kEdit, 5);
    reg("edit.selectAll",  "Select All",     ActionCategory::kEdit, 6);
    reg("edit.find",       "Find",           ActionCategory::kEdit, 7);
    reg("edit.replace",    "Replace",        ActionCategory::kEdit, 8);

    // View actions
    reg("view.sidebar",    "Toggle Sidebar", ActionCategory::kView, 1);
    reg("view.panel",      "Toggle Panel",   ActionCategory::kView, 2);
    reg("view.fullscreen", "Fullscreen",     ActionCategory::kView, 3);
    reg("view.zoomIn",     "Zoom In",        ActionCategory::kView, 4);
    reg("view.zoomOut",    "Zoom Out",       ActionCategory::kView, 5);
    reg("view.zoomReset",  "Reset Zoom",     ActionCategory::kView, 6);

    // Navigate actions
    reg("navigate.back",   "Go Back",        ActionCategory::kNavigate, 1);
    reg("navigate.forward","Go Forward",     ActionCategory::kNavigate, 2);
    reg("navigate.goToLine","Go to Line",    ActionCategory::kNavigate, 3);
    reg("navigate.goToFile","Go to File",    ActionCategory::kNavigate, 4);

    // Search actions
    reg("search.find",     "Find in Files",  ActionCategory::kSearch, 1);
    reg("search.replace",  "Replace in Files",ActionCategory::kSearch, 2);

    // Terminal/Debug
    reg("terminal.new",    "New Terminal",    ActionCategory::kTerminal, 1);
    reg("debug.start",     "Start Debugging", ActionCategory::kDebug, 1);
    reg("debug.stop",      "Stop Debugging",  ActionCategory::kDebug, 2);

    // Source Control
    reg("sc.commit",       "Commit",          ActionCategory::kSourceControl, 1);
    reg("sc.push",         "Push",            ActionCategory::kSourceControl, 2);
    reg("sc.pull",         "Pull",            ActionCategory::kSourceControl, 3);
    reg("sc.diff",         "View Diff",       ActionCategory::kSourceControl, 4);

    // Help
    reg("help.about",      "About MarkAmp",   ActionCategory::kHelp, 1);
    reg("help.docs",       "Documentation",   ActionCategory::kHelp, 2);
}

auto release_action_count() noexcept -> int
{
    return kReleaseActionCount;
}

} // namespace markamp::core
