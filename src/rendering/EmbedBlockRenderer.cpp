// ============================================================================
// File: src/rendering/EmbedBlockRenderer.cpp
// Phase 29: Advanced Block Types — Embed renderer implementation
// ============================================================================
#include "EmbedBlockRenderer.h"

#include <regex>
#include <sstream>

namespace markamp::rendering
{

void EmbedBlockRenderer::set_block_resolver(BlockResolver resolver)
{
    resolver_ = std::move(resolver);
}

auto EmbedBlockRenderer::render(const std::string& block_id) const -> EmbedRenderResult
{
    EmbedRenderResult result;
    result.source_block_id = block_id;

    if (!resolver_)
    {
        result.html = R"(<div class="embed-block embed-unresolved" data-block-id=")" + block_id +
                      R"("><p class="embed-error">Embed resolver not configured</p></div>)";
        result.resolved = false;
        return result;
    }

    auto content_result = resolver_(block_id);
    if (!content_result.has_value())
    {
        result.html = R"(<div class="embed-block embed-unresolved" data-block-id=")" + block_id +
                      R"("><p class="embed-error">Block not found: )" + block_id + R"(</p></div>)";
        result.resolved = false;
        return result;
    }

    std::ostringstream oss;
    oss << R"(<div class="embed-block" data-block-id=")" << block_id << R"(">)"
        << "\n  <div class=\"embed-content\">\n    " << content_result.value() << "\n  </div>\n"
        << R"(  <a class="embed-source-link" href="#)" << block_id << R"(">↗ Source</a>)"
        << "\n"
        << "</div>\n";

    result.html = oss.str();
    result.resolved = true;
    return result;
}

auto EmbedBlockRenderer::detect_embed(std::string_view text) -> bool
{
    // Look for ((block-id)) pattern
    const auto open_pos = text.find("((");
    if (open_pos == std::string_view::npos)
    {
        return false;
    }

    const auto close_pos = text.find("))", open_pos + 2);
    if (close_pos == std::string_view::npos)
    {
        return false;
    }

    // Ensure there's content between the markers
    return (close_pos - open_pos) > 2;
}

auto EmbedBlockRenderer::extract_block_id(std::string_view embed_text) -> std::string
{
    const auto open_pos = embed_text.find("((");
    if (open_pos == std::string_view::npos)
    {
        return {};
    }

    const auto close_pos = embed_text.find("))", open_pos + 2);
    if (close_pos == std::string_view::npos)
    {
        return {};
    }

    return std::string(embed_text.substr(open_pos + 2, close_pos - open_pos - 2));
}

} // namespace markamp::rendering
