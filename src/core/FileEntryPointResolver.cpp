/// @file FileEntryPointResolver.cpp
/// @brief V20 P02-T02: File entry point resolver implementation.

#include "FileEntryPointResolver.h"

#include "Logger.h"

#include <filesystem>

namespace markamp::core
{

namespace fs = std::filesystem;

auto FileEntryPointResolver::resolve(FileCreationEntryPoint entry_point,
                                       const std::string& workspace_root,
                                       const std::string& selected_tree_path,
                                       const std::string& explicit_directory) -> ResolvedPlacement
{
    ++resolution_count_;
    ResolvedPlacement placement;
    placement.source = entry_point;
    placement.has_workspace = !workspace_root.empty();

    // Resolve target directory with priority:
    //   1. Explicit directory (if provided)
    //   2. Selected tree path's parent (for explorer context)
    //   3. Workspace root
    //   4. Empty (unsaved, no directory)

    if (!explicit_directory.empty())
    {
        placement.target_directory = explicit_directory;
    }
    else if (!selected_tree_path.empty())
    {
        // If selected path is a directory, use it; otherwise use its parent
        auto sel_path = fs::path(selected_tree_path);
        if (sel_path.has_extension())
        {
            placement.target_directory = sel_path.parent_path().string();
        }
        else
        {
            placement.target_directory = selected_tree_path;
        }
    }
    else if (!workspace_root.empty())
    {
        placement.target_directory = workspace_root;
    }

    // Set focus and inline edit behavior based on entry point
    placement.should_focus = true;
    placement.should_inline_edit = should_inline_edit(entry_point);

    MARKAMP_LOG_DEBUG("Resolved placement: entry={} dir='{}' workspace={}", 
                      entry_point_label(entry_point), placement.target_directory,
                      placement.has_workspace);

    return placement;
}

auto FileEntryPointResolver::entry_point_label(FileCreationEntryPoint entry_point) -> std::string
{
    switch (entry_point)
    {
        case FileCreationEntryPoint::kMenu:
            return "menu";
        case FileCreationEntryPoint::kToolbar:
            return "toolbar";
        case FileCreationEntryPoint::kPalette:
            return "palette";
        case FileCreationEntryPoint::kExplorer:
            return "explorer";
        case FileCreationEntryPoint::kTabBar:
            return "tab-bar";
        case FileCreationEntryPoint::kWelcome:
            return "welcome";
        case FileCreationEntryPoint::kShortcut:
            return "shortcut";
        case FileCreationEntryPoint::kDragDrop:
            return "drag-drop";
        case FileCreationEntryPoint::kProgrammatic:
            return "programmatic";
    }
    return "unknown";
}

auto FileEntryPointResolver::should_inline_edit(FileCreationEntryPoint entry_point) -> bool
{
    switch (entry_point)
    {
        case FileCreationEntryPoint::kExplorer:
            return true;  // Explorer creates start inline name editing
        case FileCreationEntryPoint::kMenu:
        case FileCreationEntryPoint::kToolbar:
        case FileCreationEntryPoint::kPalette:
        case FileCreationEntryPoint::kTabBar:
        case FileCreationEntryPoint::kWelcome:
        case FileCreationEntryPoint::kShortcut:
        case FileCreationEntryPoint::kDragDrop:
        case FileCreationEntryPoint::kProgrammatic:
            return false;
    }
    return false;
}

} // namespace markamp::core
