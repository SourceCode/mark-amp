#include "CodeBlockRenderer.h"

#include "core/StringUtils.h"

#include <fmt/format.h>

namespace markamp::rendering
{

CodeBlockRenderer::CodeBlockRenderer() = default;

auto CodeBlockRenderer::render(std::string_view source, const std::string& language) const
    -> std::string
{
    std::string html;
    html.reserve(source.size() * 2 + 256);

    if (language.empty())
    {
        return render_plain(source);
    }

    // Wrapper div
    html += "<div class=\"code-block-wrapper\">\n";

    // Header with language label
    html += fmt::format("<div class=\"code-block-header\">"
                        "<span class=\"language-label\">{}</span>"
                        "</div>\n",
                        core::escape_html(language));

    // Pre + code with highlighted tokens
    html += fmt::format("<pre class=\"code-block\"><code class=\"language-{}\">",
                        core::escape_html(language));

    if (highlighter_.is_supported(language))
    {
        html += highlighter_.render_html(source, language);
    }
    else
    {
        html += core::escape_html(source);
    }

    html += "</code></pre>\n</div>\n";
    return html;
}

auto CodeBlockRenderer::render_plain(std::string_view source) const -> std::string
{
    std::string html;
    html.reserve(source.size() + 128);
    html += "<div class=\"code-block-wrapper\">\n";
    html += "<pre class=\"code-block\"><code>";
    html += core::escape_html(source);
    html += "</code></pre>\n</div>\n";
    return html;
}

auto CodeBlockRenderer::escape_html(std::string_view text) -> std::string
{
    return core::escape_html(text);
}

} // namespace markamp::rendering
