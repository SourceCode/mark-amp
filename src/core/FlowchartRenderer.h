// ============================================================================
// File: src/core/FlowchartRenderer.h
// Phase 30: Diagram Rendering Extended — Flowchart.js renderer (stub)
// ============================================================================
#pragma once

#include "IDiagramRenderer.h"

#include <string>

namespace markamp::core
{

/// Stub renderer for Flowchart.js diagrams.
/// Flowchart.js requires a JavaScript engine; this renderer outputs
/// an HTML container with embedded JS to be rendered in the webview.
class FlowchartRenderer : public IDiagramRenderer
{
public:
    FlowchartRenderer() = default;

    // IDiagramRenderer interface
    [[nodiscard]] auto render(const std::string& source) const
        -> std::expected<DiagramResult, std::string> override;
    [[nodiscard]] auto is_available() const -> bool override;
    [[nodiscard]] auto language_id() const -> std::string override;
    [[nodiscard]] auto display_name() const -> std::string override;
};

} // namespace markamp::core
