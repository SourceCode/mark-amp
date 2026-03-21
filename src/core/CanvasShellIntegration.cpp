/// @file CanvasShellIntegration.cpp
/// @brief V20 P04-T04: Canvas shell integration implementation.

#include "CanvasShellIntegration.h"

#include "Logger.h"

namespace markamp::core
{

CanvasShellIntegration::CanvasShellIntegration(ArtifactRegistry& registry)
    : registry_(registry)
{
}

auto CanvasShellIntegration::build_tab_descriptors() const -> std::vector<CanvasTabDescriptor>
{
    ++query_count_;
    std::vector<CanvasTabDescriptor> tabs;

    auto canvas_artifacts = registry_.artifacts_by_kind(ArtifactKind::kCanvas);
    tabs.reserve(canvas_artifacts.size());

    const auto& active_id = registry_.active_artifact();

    for (const auto& record : canvas_artifacts)
    {
        CanvasTabDescriptor tab;
        tab.artifact_id = record.id;
        tab.display_name = record.display_name;
        tab.board_id = record.id.value;
        tab.is_dirty = record.is_dirty() || record.is_unsaved();
        tab.is_active = (record.id == active_id);
        tabs.push_back(std::move(tab));
    }

    return tabs;
}

auto CanvasShellIntegration::build_tree_nodes() const -> std::vector<CanvasTreeNode>
{
    ++query_count_;
    std::vector<CanvasTreeNode> nodes;

    auto canvas_artifacts = registry_.artifacts_by_kind(ArtifactKind::kCanvas);
    nodes.reserve(canvas_artifacts.size());

    for (const auto& record : canvas_artifacts)
    {
        CanvasTreeNode node;
        node.artifact_id = record.id;
        node.display_name = record.display_name;
        node.file_path = record.file_path.value_or("");
        node.is_open = true; // All registered canvases are open
        nodes.push_back(std::move(node));
    }

    return nodes;
}

auto CanvasShellIntegration::active_board_tab() const -> std::optional<CanvasTabDescriptor>
{
    ++query_count_;

    const auto& active_id = registry_.active_artifact();
    if (active_id.empty())
    {
        return std::nullopt;
    }

    const auto* record = registry_.find(active_id);
    if (record == nullptr || !record->is_canvas())
    {
        return std::nullopt;
    }

    CanvasTabDescriptor tab;
    tab.artifact_id = record->id;
    tab.display_name = record->display_name;
    tab.board_id = record->id.value;
    tab.is_dirty = record->is_dirty() || record->is_unsaved();
    tab.is_active = true;
    return tab;
}

auto CanvasShellIntegration::tab_count() const -> int
{
    ++query_count_;
    return registry_.count_by_kind(ArtifactKind::kCanvas);
}

auto CanvasShellIntegration::is_visible_in_tree(const ArtifactId& artifact_id) const -> bool
{
    ++query_count_;
    const auto* record = registry_.find(artifact_id);
    return record != nullptr && record->is_canvas() && record->has_path();
}

} // namespace markamp::core
