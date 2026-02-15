// ============================================================================
// File: src/core/GraphvizRenderer.h
// Phase 30: Diagram Rendering Extended — Graphviz renderer
// ============================================================================
#pragma once

#include "IDiagramRenderer.h"

#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Renders Graphviz (DOT language) diagrams via the `dot` CLI tool.
class GraphvizRenderer : public IDiagramRenderer
{
public:
    GraphvizRenderer() = default;

    /// Set the path to the `dot` binary. If empty, searches PATH.
    void set_dot_path(const std::string& path);

    // IDiagramRenderer interface
    [[nodiscard]] auto render(const std::string& source) const
        -> std::expected<DiagramResult, std::string> override;
    [[nodiscard]] auto is_available() const -> bool override;
    [[nodiscard]] auto language_id() const -> std::string override;
    [[nodiscard]] auto display_name() const -> std::string override;

private:
    std::string dot_path_{"dot"};
    mutable std::unordered_map<std::string, std::string> cache_;

    [[nodiscard]] static auto compute_hash(const std::string& source) -> std::string;
};

} // namespace markamp::core
