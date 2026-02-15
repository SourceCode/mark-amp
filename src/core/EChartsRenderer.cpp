// ============================================================================
// File: src/core/EChartsRenderer.cpp
// Phase 30: Diagram Rendering Extended — ECharts renderer implementation
// ============================================================================
#include "EChartsRenderer.h"

#include <sstream>

namespace markamp::core
{

auto EChartsRenderer::render(const std::string& source) const
    -> std::expected<DiagramResult, std::string>
{
    DiagramResult result;

    std::ostringstream oss;
    oss << R"(<div class="diagram-echarts" id="echarts-diagram" style="width:600px;height:400px;">)"
        << "\n"
        << R"(  <script src="https://cdnjs.cloudflare.com/ajax/libs/echarts/5.4.3/echarts.min.js"></script>)"
        << "\n"
        << "  <script>\n"
        << "    var chart = echarts.init(document.getElementById('echarts-diagram'));\n"
        << "    var option = " << source << ";\n"
        << "    chart.setOption(option);\n"
        << "  </script>\n"
        << "</div>\n";

    result.svg = oss.str();
    result.diagnostics.push_back(
        {DiagramDiagnosticLevel::kInfo, "ECharts rendering requires webview with JS support", -1});
    return result;
}

auto EChartsRenderer::is_available() const -> bool
{
    return true; // Always available; actual rendering happens in webview
}

auto EChartsRenderer::language_id() const -> std::string
{
    return "echarts";
}

auto EChartsRenderer::display_name() const -> std::string
{
    return "Apache ECharts";
}

} // namespace markamp::core
