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

} // namespace markamp::rendering
