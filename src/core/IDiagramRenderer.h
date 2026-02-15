// ============================================================================
// File: src/core/IDiagramRenderer.h
// Phase 30: Diagram Rendering Extended — Renderer interface and types
// ============================================================================
#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <vector>

namespace markamp::core
{

/// Diagnostic severity for diagram rendering issues.
enum class DiagramDiagnosticLevel : uint8_t
{
    kInfo,
    kWarning,
    kError
};

/// A diagnostic message from the diagram renderer.
struct DiagramDiagnostic
{
    DiagramDiagnosticLevel level{DiagramDiagnosticLevel::kInfo};
    std::string message;
    int line{-1}; // Source line, -1 if unknown
};

/// Result of rendering a diagram.
struct DiagramResult
{
    std::string svg;      // SVG output
    std::string png_data; // PNG bytes (if SVG not available)
    std::vector<DiagramDiagnostic> diagnostics;
    int64_t render_time_ms{0}; // Time taken to render in milliseconds

    [[nodiscard]] auto has_errors() const -> bool
    {
        for (const auto& diag : diagnostics)
        {
            if (diag.level == DiagramDiagnosticLevel::kError)
            {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] auto has_output() const -> bool
    {
        return !svg.empty() || !png_data.empty();
    }
};

/// Interface for diagram language renderers.
class IDiagramRenderer
{
public:
    virtual ~IDiagramRenderer() = default;

    /// Render diagram source code to SVG/PNG.
    [[nodiscard]] virtual auto render(const std::string& source) const
        -> std::expected<DiagramResult, std::string> = 0;

    /// Check if the external tool (if any) is available.
    [[nodiscard]] virtual auto is_available() const -> bool = 0;

    /// Get the diagram language name (e.g., "plantuml", "graphviz").
    [[nodiscard]] virtual auto language_id() const -> std::string = 0;

    /// Get a human-readable display name.
    [[nodiscard]] virtual auto display_name() const -> std::string = 0;
};

} // namespace markamp::core
