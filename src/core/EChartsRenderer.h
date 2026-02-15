// ============================================================================
// File: src/core/EChartsRenderer.h
// Phase 30: Diagram Rendering Extended — ECharts renderer (stub)
// ============================================================================
#pragma once

#include "IDiagramRenderer.h"

#include <string>

namespace markamp::core
{

/// Stub renderer for Apache ECharts diagrams.
/// ECharts requires a JavaScript engine; this renderer outputs
/// an HTML container with embedded JS configuration.
class EChartsRenderer : public IDiagramRenderer
{
public:
    EChartsRenderer() = default;

    // IDiagramRenderer interface
    [[nodiscard]] auto render(const std::string& source) const
        -> std::expected<DiagramResult, std::string> override;
    [[nodiscard]] auto is_available() const -> bool override;
    [[nodiscard]] auto language_id() const -> std::string override;
    [[nodiscard]] auto display_name() const -> std::string override;
};

} // namespace markamp::core
