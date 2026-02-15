// ============================================================================
// File: src/rendering/SuperBlockRenderer.cpp
// Phase 29: Advanced Block Types — Super block renderer implementation
// ============================================================================
#include "SuperBlockRenderer.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::rendering
{

auto SuperBlockRenderer::render(const std::vector<std::string>& child_html_fragments,
                                SuperBlockLayout layout) const -> std::string
{
    const auto direction = (layout == SuperBlockLayout::kHorizontal) ? "row" : "column";
    const auto layout_class = (layout == SuperBlockLayout::kHorizontal) ? "super-block-horizontal"
                                                                        : "super-block-vertical";

    std::ostringstream oss;
    oss << R"(<div class="super-block )" << layout_class
        << R"(" style="display:flex;flex-direction:)" << direction << R"(;gap:8px;">)"
        << "\n";

    for (const auto& child_html : child_html_fragments)
    {
        oss << R"(  <div class="super-block-child")"
            << (layout == SuperBlockLayout::kHorizontal ? R"( style="flex:1;")" : "") << ">\n"
            << "    " << child_html << "\n"
            << "  </div>\n";
    }

    oss << "</div>\n";
    return oss.str();
}

auto SuperBlockRenderer::detect_super_block(std::string_view content) -> bool
{
    const auto trimmed = content;
    return trimmed.find("{{{row") != std::string_view::npos ||
           trimmed.find("{{{col") != std::string_view::npos;
}

auto SuperBlockRenderer::parse_layout(std::string_view marker) -> SuperBlockLayout
{
    // Normalize to lowercase
    auto lower = std::string(marker);
    std::transform(lower.begin(),
                   lower.end(),
                   lower.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });

    if (lower.find("col") != std::string::npos)
    {
        return SuperBlockLayout::kVertical;
    }

    return SuperBlockLayout::kHorizontal;
}

auto SuperBlockRenderer::css() -> std::string
{
    return R"(
.super-block {
    display: flex;
    gap: 8px;
    padding: 4px;
    margin: 8px 0;
}
.super-block-horizontal {
    flex-direction: row;
}
.super-block-vertical {
    flex-direction: column;
}
.super-block-child {
    min-width: 0;
}
.super-block-horizontal > .super-block-child {
    flex: 1;
}
)";
}

} // namespace markamp::rendering
