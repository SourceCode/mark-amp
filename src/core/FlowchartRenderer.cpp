// ============================================================================
// File: src/core/FlowchartRenderer.cpp
// Phase 30: Diagram Rendering Extended — Flowchart.js renderer implementation
// ============================================================================
#include "FlowchartRenderer.h"

#include <sstream>

namespace markamp::core
{

auto FlowchartRenderer::render(const std::string& source) const
    -> std::expected<DiagramResult, std::string>
{
    // Generate an HTML container that renders via Flowchart.js in the webview
    DiagramResult result;

    std::ostringstream oss;
    oss << R"(<div class="diagram-flowchart" id="flowchart-diagram">)"
        << "\n"
        << R"(  <script src="https://cdnjs.cloudflare.com/ajax/libs/flowchart/1.17.1/flowchart.min.js"></script>)"
        << "\n"
        << "  <script>\n"
        << "    var chart = flowchart.parse("
        << "'" << source << "'"
        << ");\n"
        << "    chart.drawSVG('flowchart-diagram');\n"
        << "  </script>\n"
        << "</div>\n";

    result.svg = oss.str();
    result.diagnostics.push_back({DiagramDiagnosticLevel::kInfo,
                                  "Flowchart.js rendering requires webview with JS support",
                                  -1});
    return result;
}

auto FlowchartRenderer::is_available() const -> bool
{
    return true; // Always available; actual rendering happens in webview
}

auto FlowchartRenderer::language_id() const -> std::string
{
    return "flowchart";
}

auto FlowchartRenderer::display_name() const -> std::string
{
    return "Flowchart.js";
}

} // namespace markamp::core
