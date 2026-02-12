#include "MermaidBlockRenderer.h"

#include "core/StringUtils.h"

#include <array>
#include <fmt/format.h>

namespace markamp::rendering
{

namespace
{

constexpr std::string_view kBase64Chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

} // anonymous namespace

auto MermaidBlockRenderer::render(std::string_view mermaid_source, core::IMermaidRenderer& renderer)
    -> std::string
{
    auto result = renderer.render(mermaid_source);

    if (!result)
    {
        return render_error(result.error());
    }

    auto& svg = *result;

    // Encode SVG as base64 for wxHtmlWindow <img> data URI
    auto b64 = base64_encode(svg);

    std::string html;
    html += "<div class=\"mermaid-container\">\n";
    html +=
        fmt::format("<img src=\"data:image/svg+xml;base64,{}\" alt=\"Mermaid diagram\" />\n", b64);
    html += "</div>\n";
    return html;
}

auto MermaidBlockRenderer::render_error(const std::string& error_message) -> std::string
{
    std::string html;
    html += "<div class=\"mermaid-error\">\n";
    html += "<strong>Mermaid Error:</strong> ";
    html += escape_html(error_message);
    html += "\n</div>\n";
    return html;
}

auto MermaidBlockRenderer::render_unavailable() -> std::string
{
    std::string html;
    html += "<div class=\"mermaid-unavailable\">\n";
    html += "<em>Mermaid rendering is not available.</em><br/>\n";
    html += "Install with: <code>npm install -g @mermaid-js/mermaid-cli</code>\n";
    html += "</div>\n";
    return html;
}

auto MermaidBlockRenderer::render_placeholder(std::string_view mermaid_source) -> std::string
{
    std::string html;
    html += "<div class=\"mermaid-block\">\n";
    html += "<em>Mermaid diagram</em><br/>\n";
    html += "<pre><code>";
    html += escape_html(mermaid_source);
    html += "</code></pre>\n";
    html += "</div>\n";
    return html;
}

auto MermaidBlockRenderer::base64_encode(std::string_view input) -> std::string
{
    std::string output;
    output.reserve(((input.size() + 2) / 3) * 4);

    size_t i = 0;
    while (i < input.size())
    {
        auto a = static_cast<uint8_t>(input[i++]);

        // Track how many bytes we actually have for padding
        size_t bytes_in_group = 1;
        uint8_t b = 0;
        uint8_t c = 0;

        if (i < input.size())
        {
            b = static_cast<uint8_t>(input[i++]);
            bytes_in_group = 2;
        }
        if (i < input.size())
        {
            c = static_cast<uint8_t>(input[i++]);
            bytes_in_group = 3;
        }

        auto triple = (static_cast<uint32_t>(a) << 16) | (static_cast<uint32_t>(b) << 8) |
                      static_cast<uint32_t>(c);

        // Improvement 27: batch 4 chars via array for fewer append calls
        const std::array<char, 4> buf = {kBase64Chars[(triple >> 18) & 0x3F],
                                         kBase64Chars[(triple >> 12) & 0x3F],
                                         (bytes_in_group >= 2) ? kBase64Chars[(triple >> 6) & 0x3F]
                                                               : '=',
                                         (bytes_in_group >= 3) ? kBase64Chars[triple & 0x3F] : '='};
        output.append(buf.data(), 4);
    }

    return output;
}

auto MermaidBlockRenderer::escape_html(std::string_view text) -> std::string
{
    return core::escape_html(text);
}

// ---------------------------------------------------------------------------
// Phase 3: Enhanced container with controls
// ---------------------------------------------------------------------------

auto MermaidBlockRenderer::container_styles() -> std::string
{
    return R"(<style>
.mermaid-enhanced {
    position: relative;
    border: 1px solid #333;
    border-radius: 8px;
    overflow: hidden;
    margin: 12px 0;
    background: #1A1A2E;
}
.mermaid-viewport {
    overflow: auto;
    cursor: grab;
    padding: 16px;
    max-height: 600px;
}
.mermaid-viewport:active { cursor: grabbing; }
.mermaid-viewport img {
    display: block;
    margin: 0 auto;
    transition: transform 0.2s ease;
}
.mermaid-controls {
    position: absolute;
    top: 8px;
    right: 8px;
    display: flex;
    gap: 4px;
    z-index: 10;
}
.mermaid-btn {
    width: 28px;
    height: 28px;
    border: none;
    border-radius: 4px;
    background: rgba(40,40,60,0.85);
    color: #E0E0E0;
    font-size: 14px;
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
}
.mermaid-btn:hover { background: rgba(80,80,120,0.9); }
.mermaid-diag {
    padding: 8px 12px;
    border-top: 1px solid #333;
    background: #1E1020;
    font-family: monospace;
    font-size: 12px;
}
.mermaid-diag-error { color: #FF6B6B; }
.mermaid-diag-warning { color: #FFD93D; }
.mermaid-diag-info { color: #6BCBFF; }
</style>)";
}

auto MermaidBlockRenderer::render_with_controls(std::string_view mermaid_source,
                                                core::IMermaidRenderer& renderer) -> std::string
{
    auto result = renderer.render(mermaid_source);

    std::string html;

    // Include styles once per render call
    html += container_styles();

    html += "<div class=\"mermaid-enhanced\">\n";

    // Control buttons: zoom in, zoom out, fullscreen, export
    html += R"(<div class="mermaid-controls">
    <button class="mermaid-btn" title="Zoom In">+</button>
    <button class="mermaid-btn" title="Zoom Out">&minus;</button>
    <button class="mermaid-btn" title="Fullscreen">&#x2922;</button>
    <button class="mermaid-btn" title="Export SVG">&#x2B07;</button>
</div>
)";

    html += "<div class=\"mermaid-viewport\">\n";

    if (result.has_value())
    {
        auto b64 = base64_encode(*result);
        html += fmt::format(
            "<img src=\"data:image/svg+xml;base64,{}\" alt=\"Mermaid diagram\" />\n", b64);
    }
    else
    {
        html += "<div class=\"mermaid-error\"><strong>Mermaid Error:</strong> ";
        html += escape_html(result.error());
        html += "</div>\n";
    }

    html += "</div>\n"; // close viewport
    html += "</div>\n"; // close enhanced container
    return html;
}

auto MermaidBlockRenderer::render_diagnostics(const std::vector<core::DiagnosticInfo>& diagnostics)
    -> std::string
{
    if (diagnostics.empty())
    {
        return {};
    }

    std::string html;
    html += "<div class=\"mermaid-diag\">\n";

    for (const auto& diag : diagnostics)
    {
        std::string css_class;
        std::string label;
        switch (diag.severity)
        {
            case core::DiagnosticSeverity::Error:
                css_class = "mermaid-diag-error";
                label = "ERROR";
                break;
            case core::DiagnosticSeverity::Warning:
                css_class = "mermaid-diag-warning";
                label = "WARN";
                break;
            case core::DiagnosticSeverity::Info:
                css_class = "mermaid-diag-info";
                label = "INFO";
                break;
        }

        html += fmt::format("<div class=\"{}\"><strong>[{}]</strong> ", css_class, label);

        if (diag.line > 0)
        {
            html += fmt::format("Line {}: ", diag.line);
        }

        html += escape_html(diag.message);
        html += "</div>\n";
    }

    html += "</div>\n";
    return html;
}

} // namespace markamp::rendering
