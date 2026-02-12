#pragma once

#include "core/IMermaidRenderer.h"
#include "core/MermaidRenderer.h"

#include <string>
#include <string_view>

namespace markamp::rendering
{

/// Renders Mermaid diagram blocks into HTML for the preview pane.
/// Produces container HTML with inline SVG (base64 data URI for wxHtmlWindow),
/// error overlays for invalid syntax, and unavailability placeholders.
class MermaidBlockRenderer
{
public:
    /// Render a Mermaid diagram source using the given renderer.
    /// Returns container HTML with the SVG image or error overlay.
    [[nodiscard]] auto render(std::string_view mermaid_source, core::IMermaidRenderer& renderer)
        -> std::string;

    /// Render an error overlay with the given message.
    [[nodiscard]] static auto render_error(const std::string& error_message) -> std::string;

    /// Render a placeholder when no renderer is available.
    [[nodiscard]] static auto render_unavailable() -> std::string;

    /// Render a placeholder div with the source stored as data attribute (fallback).
    [[nodiscard]] static auto render_placeholder(std::string_view mermaid_source) -> std::string;

    /// Encode a string as base64 (for SVG data URIs).
    [[nodiscard]] static auto base64_encode(std::string_view input) -> std::string;

    // --- Phase 3: Enhanced container with controls ---

    /// Render SVG with zoom/pan/fullscreen/export overlay controls.
    [[nodiscard]] auto render_with_controls(std::string_view mermaid_source,
                                            core::IMermaidRenderer& renderer) -> std::string;

    /// Render inline diagnostics below a diagram.
    [[nodiscard]] static auto
    render_diagnostics(const std::vector<core::DiagnosticInfo>& diagnostics) -> std::string;

private:
    /// Escape HTML special characters in text.
    [[nodiscard]] static auto escape_html(std::string_view text) -> std::string;

    /// Generate CSS styles for the Mermaid container controls.
    [[nodiscard]] static auto container_styles() -> std::string;
};

} // namespace markamp::rendering
