#include "CodeBlockRenderer.h"

#include "core/StringUtils.h"

#include <fmt/format.h>
#include <sstream>

namespace markamp::rendering
{

CodeBlockRenderer::CodeBlockRenderer() = default;

auto CodeBlockRenderer::render(std::string_view source,
                               const std::string& language,
                               const std::string& highlight_spec) const -> std::string
{
    std::string html;
    html.reserve(source.size() * 2 + 512);

    if (language.empty() && highlight_spec.empty())
    {
        return render_plain(source);
    }

    // Assign block ID and store source for clipboard copy
    // New stability #33: cap block_sources_ to prevent unbounded memory growth
    if (block_sources_.size() >= 10000)
    {
        block_sources_.clear();
        block_counter_ = 0;
    }
    int block_id = block_counter_++;
    block_sources_.emplace_back(source);

    // Parse line highlights
    auto highlight_lines = parse_highlight_spec(highlight_spec);

    // Wrapper div
    html += fmt::format("<div class=\"code-block-wrapper\" id=\"codeblock-{}\">\n", block_id);

    // Header with language label + copy button
    html += "<div class=\"code-block-header\">";
    if (!language.empty())
    {
        html +=
            fmt::format("<span class=\"language-label\">{}</span>", core::escape_html(language));
    }
    // Copy button
    html +=
        fmt::format("<a href=\"markamp://copy/{}\" class=\"copy-btn\" title=\"Copy to clipboard\">"
                    "\xF0\x9F\x93\x8B</a>", // 📋
                    block_id);
    html += "</div>\n";

    // Pre + code with highlighted tokens
    html += fmt::format("<pre class=\"code-block\"><code class=\"language-{}\">",
                        core::escape_html(language.empty() ? "text" : language));

    std::string code_html;
    if (!language.empty() && highlighter_.is_supported(language))
    {
        code_html = highlighter_.render_html(source, language);
    }
    else
    {
        code_html = core::escape_html(source);
    }

    // Apply line highlights if requested
    if (!highlight_lines.empty())
    {
        code_html = apply_line_highlights(code_html, highlight_lines);
    }

    html += code_html;
    html += "</code></pre>\n</div>\n";
    return html;
}

auto CodeBlockRenderer::render_plain(std::string_view source) const -> std::string
{
    std::string html;
    html.reserve(source.size() + 256);

    // R20 Fix 33: cap block_sources_ to prevent unbounded memory growth (same as render)
    if (block_sources_.size() >= 10000)
    {
        block_sources_.clear();
        block_counter_ = 0;
    }
    int block_id = block_counter_++;
    block_sources_.emplace_back(source);

    html += fmt::format("<div class=\"code-block-wrapper\" id=\"codeblock-{}\">\n", block_id);

    // Header with copy button only
    html +=
        fmt::format("<div class=\"code-block-header\">"
                    "<a href=\"markamp://copy/{}\" class=\"copy-btn\" title=\"Copy to clipboard\">"
                    "\xF0\x9F\x93\x8B</a>"
                    "</div>\n",
                    block_id);

    html += "<pre class=\"code-block\"><code>";
    html += core::escape_html(source);
    html += "</code></pre>\n</div>\n";
    return html;
}

void CodeBlockRenderer::reset_counter() const
{
    block_counter_ = 0;
    block_sources_.clear();
}

auto CodeBlockRenderer::get_block_source(int block_id) const -> std::string
{
    if (block_id >= 0 && block_id < static_cast<int>(block_sources_.size()))
    {
        return block_sources_[static_cast<size_t>(block_id)];
    }
    return {};
}

auto CodeBlockRenderer::parse_highlight_spec(const std::string& spec) -> std::set<int>
{
    std::set<int> lines;
    if (spec.empty())
    {
        return lines;
    }

    // Strip outer braces if present
    std::string raw = spec;
    if (!raw.empty() && raw.front() == '{')
    {
        raw.erase(raw.begin());
    }
    if (!raw.empty() && raw.back() == '}')
    {
        raw.pop_back();
    }

    // Parse comma-separated items: "1,3-5,8"
    std::istringstream stream(raw);
    std::string item;
    while (std::getline(stream, item, ','))
    {
        // Trim whitespace
        while (!item.empty() && item.front() == ' ')
        {
            item.erase(item.begin());
        }
        while (!item.empty() && item.back() == ' ')
        {
            item.pop_back();
        }
        if (item.empty())
        {
            continue;
        }

        // Check for range: "3-5"
        // New stability #31: wrap stoi in try-catch for malformed specs
        try
        {
            auto dash = item.find('-');
            if (dash != std::string::npos && dash > 0 && dash < item.size() - 1)
            {
                const int start = std::stoi(item.substr(0, dash));
                const int end = std::stoi(item.substr(dash + 1));
                // New stability #32: cap range to prevent memory exhaustion
                if (end - start > 10000)
                    continue;
                for (int ln = start; ln <= end; ++ln)
                {
                    lines.insert(ln);
                }
            }
            else
            {
                lines.insert(std::stoi(item));
            }
        }
        catch (const std::exception&)
        {
            // Malformed highlight spec item — skip it
        }
    }

    return lines;
}

auto CodeBlockRenderer::extract_highlight_spec(const std::string& info_string,
                                               const std::string& language) -> std::string
{
    if (info_string.size() <= language.size())
    {
        return {};
    }

    // Everything after the language name
    auto remainder = info_string.substr(language.size());

    // Trim leading whitespace
    while (!remainder.empty() && remainder.front() == ' ')
    {
        remainder.erase(remainder.begin());
    }

    // Look for {…} pattern
    auto brace_start = remainder.find('{');
    auto brace_end = remainder.rfind('}');
    if (brace_start != std::string::npos && brace_end != std::string::npos &&
        brace_end > brace_start)
    {
        return remainder.substr(brace_start, brace_end - brace_start + 1);
    }

    return {};
}

auto CodeBlockRenderer::apply_line_highlights(const std::string& code_html,
                                              const std::set<int>& highlight_lines) -> std::string
{
    // Split by newlines, wrap highlighted lines
    std::string result;
    result.reserve(code_html.size() + highlight_lines.size() * 64);

    int line_num = 1;
    size_t pos = 0;

    while (pos < code_html.size())
    {
        auto newline_pos = code_html.find('\n', pos);
        std::string_view line_content;

        if (newline_pos != std::string::npos)
        {
            line_content = std::string_view(code_html).substr(pos, newline_pos - pos);
        }
        else
        {
            line_content = std::string_view(code_html).substr(pos);
        }

        if (highlight_lines.contains(line_num))
        {
            result += "<span class=\"line-highlight\">";
            result += line_content;
            result += "</span>";
        }
        else
        {
            result += line_content;
        }

        if (newline_pos != std::string::npos)
        {
            result += '\n';
            pos = newline_pos + 1;
        }
        else
        {
            break;
        }

        ++line_num;
    }

    return result;
}

auto CodeBlockRenderer::escape_html(std::string_view text) -> std::string
{
    return core::escape_html(text);
}

} // namespace markamp::rendering
