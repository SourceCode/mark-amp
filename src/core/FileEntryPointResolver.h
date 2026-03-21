/// @file FileEntryPointResolver.h
/// @brief V20 P02-T02: Unify all new-file entry points and workspace placement.
///
/// Normalizes file creation from different entry points (menu, toolbar, palette,
/// explorer, tab bar, welcome screen) into a single resolved placement context.
/// Handles target-directory resolution, workspace awareness, and inline rename rules.
#pragma once

#include "ArtifactRegistry.h"

#include <optional>
#include <string>

namespace markamp::core
{

/// Entry point identifiers for file creation.
enum class FileCreationEntryPoint
{
    kMenu,          ///< File > New File
    kToolbar,       ///< Toolbar "New" button
    kPalette,       ///< Command palette
    kExplorer,      ///< File tree context menu
    kTabBar,        ///< Tab bar "+" button
    kWelcome,       ///< Welcome/empty-state screen
    kShortcut,      ///< Keyboard shortcut (Cmd+N)
    kDragDrop,      ///< Drag-and-drop of external file
    kProgrammatic,  ///< API/service call
};

/// Resolved placement for a new file.
struct ResolvedPlacement
{
    std::string target_directory;                  ///< Where to create/save the file
    std::string base_name;                         ///< Suggested filename
    bool has_workspace{false};                     ///< Whether a workspace is active
    bool should_focus{true};                       ///< Whether to focus the new file
    bool should_inline_edit{false};                ///< Whether to start inline name editing
    FileCreationEntryPoint source{FileCreationEntryPoint::kMenu};
};

/// Resolves file creation context from any entry point into a canonical placement.
class FileEntryPointResolver
{
public:
    FileEntryPointResolver() = default;

    /// Resolve placement for a new file from a given entry point.
    [[nodiscard]] auto resolve(FileCreationEntryPoint entry_point,
                                const std::string& workspace_root = {},
                                const std::string& selected_tree_path = {},
                                const std::string& explicit_directory = {}) -> ResolvedPlacement;

    /// Convert entry point enum to string label.
    [[nodiscard]] static auto entry_point_label(FileCreationEntryPoint entry_point) -> std::string;

    /// Whether the entry point should trigger inline rename after creation.
    [[nodiscard]] static auto should_inline_edit(FileCreationEntryPoint entry_point) -> bool;

    /// Total number of resolutions performed.
    [[nodiscard]] auto resolution_count() const noexcept -> int { return resolution_count_; }

private:
    int resolution_count_{0};
};

} // namespace markamp::core
