/// @file CanvasShellIntegration.h
/// @brief V20 P04-T04: Canvas shell integration for tabs, workspace tree,
///        focus, and commands.
///
/// Makes canvas boards visible and controllable from the same shell structures
/// as files and notebooks — tabs, explorer tree, command palette, status bar.
#pragma once

#include "ArtifactRegistry.h"

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Tab descriptor for a canvas board in the workbench tab bar.
struct CanvasTabDescriptor
{
    ArtifactId artifact_id;
    std::string display_name;
    std::string board_id;
    bool is_dirty{false};
    bool is_active{false};

    [[nodiscard]] auto has_board() const noexcept -> bool
    {
        return !board_id.empty();
    }
};

/// Explorer tree node for a canvas board.
struct CanvasTreeNode
{
    ArtifactId artifact_id;
    std::string display_name;
    std::string file_path;
    int object_count{0};
    bool is_open{false};

    [[nodiscard]] auto has_file() const noexcept -> bool
    {
        return !file_path.empty();
    }
};

/// Manages canvas presence in shell structures (tabs, tree, palette, status).
class CanvasShellIntegration
{
public:
    explicit CanvasShellIntegration(ArtifactRegistry& registry);

    /// Build tab descriptors for all open canvas boards.
    [[nodiscard]] auto build_tab_descriptors() const -> std::vector<CanvasTabDescriptor>;

    /// Build explorer tree nodes for canvas boards.
    [[nodiscard]] auto build_tree_nodes() const -> std::vector<CanvasTreeNode>;

    /// Get the active board tab descriptor (if canvas is active).
    [[nodiscard]] auto active_board_tab() const -> std::optional<CanvasTabDescriptor>;

    /// Count of open canvas tabs.
    [[nodiscard]] auto tab_count() const -> int;

    /// Whether a canvas file is visible in the explorer.
    [[nodiscard]] auto is_visible_in_tree(const ArtifactId& artifact_id) const -> bool;

    /// Total queries performed.
    [[nodiscard]] auto query_count() const noexcept -> int { return query_count_; }

private:
    ArtifactRegistry& registry_;
    mutable int query_count_{0};
};

} // namespace markamp::core
