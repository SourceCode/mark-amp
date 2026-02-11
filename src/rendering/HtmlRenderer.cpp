#include "HtmlRenderer.h"

#include "CodeBlockRenderer.h"
#include "MermaidBlockRenderer.h"
#include "core/IMermaidRenderer.h"
#include "core/Profiler.h"
#include "core/StringUtils.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>

namespace markamp::rendering
{

// ═══════════════════════════════════════════════════════
// FootnotePreprocessor
// ═══════════════════════════════════════════════════════

auto FootnotePreprocessor::process(std::string_view markdown) -> FootnoteResult
{
    MARKAMP_PROFILE_SCOPE("FootnotePreprocessor::process");
    FootnoteResult result;

    // Extract definitions and get cleaned markdown
    auto [cleaned, definitions] = extract_definitions(markdown);

    if (definitions.empty())
    {
        result.processed_markdown = std::string(markdown);
        result.has_footnotes = false;
        return result;
    }

    // Replace [^N] references with superscript links
    result.processed_markdown = replace_references(cleaned, definitions);
    result.footnote_section_html = build_section(definitions);
    result.has_footnotes = true;
    return result;
}

auto FootnotePreprocessor::extract_definitions(std::string_view markdown)
    -> std::pair<std::string, std::vector<FootnoteDefinition>>
{
    std::vector<FootnoteDefinition> definitions;
    std::string cleaned;
    cleaned.reserve(markdown.size());

    // Parse line-by-line without regex: match lines like [^id]: content
    int number = 0;
    size_t pos = 0;
    while (pos < markdown.size())
    {
        // Find end of current line
        auto line_end = markdown.find('\n', pos);
        auto line = (line_end != std::string_view::npos) ? markdown.substr(pos, line_end - pos)
                                                         : markdown.substr(pos);

        // Check if line matches [^id]: content
        bool is_def = false;
        if (line.size() >= 5 && line[0] == '[' && line[1] == '^')
        {
            auto bracket_close = line.find("]:", 2);
            if (bracket_close != std::string_view::npos && bracket_close > 2)
            {
                auto id = line.substr(2, bracket_close - 2);
                auto content_start = bracket_close + 2;
                // Skip leading whitespace after "]: "
                while (content_start < line.size() &&
                       (line[content_start] == ' ' || line[content_start] == '\t'))
                {
                    ++content_start;
                }
                if (content_start < line.size())
                {
                    ++number;
                    FootnoteDefinition def;
                    def.id = std::string(id);
                    def.content = std::string(line.substr(content_start));
                    def.number = number;
                    definitions.push_back(std::move(def));
                    is_def = true;
                }
            }
        }

        if (!is_def)
        {
            cleaned.append(line);
            cleaned += '\n';
        }

        pos = (line_end != std::string_view::npos) ? line_end + 1 : markdown.size();
    }

    return {cleaned, definitions};
}

auto FootnotePreprocessor::replace_references(const std::string& markdown,
                                              const std::vector<FootnoteDefinition>& definitions)
    -> std::string
{
    std::string result = markdown;

    for (const auto& def : definitions)
    {
        // Replace [^id] with superscript link HTML
        std::string pattern = "[^" + def.id + "]";
        std::string replacement = fmt::format(
            R"(<sup class="footnote-ref"><a href="#fn-{}" id="fnref-{}">[{}]</a></sup>)",
            def.id,
            def.id,
            def.number);

        std::string::size_type pos = 0;
        while ((pos = result.find(pattern, pos)) != std::string::npos)
        {
            // Don't replace if it's the start of a definition line (already removed)
            // or inside a code block fence
            result.replace(pos, pattern.size(), replacement);
            pos += replacement.size();
        }
    }

    return result;
}

auto FootnotePreprocessor::build_section(const std::vector<FootnoteDefinition>& definitions)
    -> std::string
{
    std::string section;
    section += R"(<section class="footnotes">)";
    section += "\n<hr>\n<ol>\n";

    for (const auto& def : definitions)
    {
        section += fmt::format(
            R"(<li id="fn-{}">{} <a href="#fnref-{}" class="footnote-backref">↩</a></li>)",
            def.id,
            def.content,
            def.id);
        section += '\n';
    }

    section += "</ol>\n</section>\n";
    return section;
}

// ═══════════════════════════════════════════════════════
// HtmlRenderer — Public
// ═══════════════════════════════════════════════════════

auto HtmlRenderer::render(const core::MarkdownDocument& doc) -> std::string
{
    MARKAMP_PROFILE_SCOPE("HtmlRenderer::render");
    std::string output;
    // Pre-allocate: ~8 bytes of HTML per character of source (rough estimate)
    output.reserve(doc.root.children.size() * 256);
    render_children(doc.root, output);
    return output;
}

auto HtmlRenderer::render_with_footnotes(const core::MarkdownDocument& doc,
                                         const std::string& footnote_section) -> std::string
{
    std::string output;
    output.reserve(doc.root.children.size() * 256 + footnote_section.size());
    render_children(doc.root, output);
    if (!footnote_section.empty())
    {
        output += footnote_section;
    }
    return output;
}

// ═══════════════════════════════════════════════════════
// Recursive rendering
// ═══════════════════════════════════════════════════════

void HtmlRenderer::render_node(const core::MdNode& node, std::string& output)
{
    using core::MdNodeType;

    switch (node.type)
    {
        case MdNodeType::Document:
            render_children(node, output);
            break;

        case MdNodeType::Paragraph:
            output += "<p>";
            render_children(node, output);
            output += "</p>\n";
            break;

        case MdNodeType::Heading:
        {
            // Phase 4 Item 32: Generate heading anchor ID from text content
            std::string heading_text;
            for (const auto& child : node.children)
            {
                heading_text += child.plain_text();
            }

            // Slugify: lowercase, spaces→dashes, strip non-alnum
            std::string slug;
            slug.reserve(heading_text.size());
            for (char ch : heading_text)
            {
                if (std::isalnum(static_cast<unsigned char>(ch)))
                {
                    slug += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
                }
                else if (ch == ' ' || ch == '-')
                {
                    if (!slug.empty() && slug.back() != '-')
                    {
                        slug += '-';
                    }
                }
            }
            // Trim trailing dash
            if (!slug.empty() && slug.back() == '-')
            {
                slug.pop_back();
            }

            output += fmt::format("<h{} id=\"{}\">", node.heading_level, slug);
            render_children(node, output);
            output += fmt::format("</h{}>\n", node.heading_level);
            break;
        }

        case MdNodeType::BlockQuote:
            output += "<blockquote>\n";
            render_children(node, output);
            output += "</blockquote>\n";
            break;

        case MdNodeType::UnorderedList:
            output += "<ul>\n";
            render_children(node, output);
            output += "</ul>\n";
            break;

        case MdNodeType::OrderedList:
            if (node.start_number != 1)
            {
                output += fmt::format("<ol start=\"{}\">\n", node.start_number);
            }
            else
            {
                output += "<ol>\n";
            }
            render_children(node, output);
            output += "</ol>\n";
            break;

        case MdNodeType::ListItem:
            output += "<li>";
            render_children(node, output);
            output += "</li>\n";
            break;

        case MdNodeType::CodeBlock:
        case MdNodeType::FencedCodeBlock:
        {
            CodeBlockRenderer code_renderer;
            output += code_renderer.render(node.text_content, node.language);
            break;
        }

        case MdNodeType::MermaidBlock:
        {
            MermaidBlockRenderer mermaid_block;
            if (mermaid_renderer_ && mermaid_renderer_->is_available())
            {
                output += mermaid_block.render(node.text_content, *mermaid_renderer_);
            }
            else if (mermaid_renderer_ && !mermaid_renderer_->is_available())
            {
                output += mermaid_block.render_unavailable();
            }
            else
            {
                output += mermaid_block.render_placeholder(node.text_content);
            }
            break;
        }

        case MdNodeType::HorizontalRule:
            output += "<hr>\n";
            break;

        case MdNodeType::Table:
            output += "<div class=\"table-wrapper\">\n<table>\n";
            render_children(node, output);
            output += "</table>\n</div>\n";
            break;

        case MdNodeType::TableHead:
            output += "<thead>\n";
            render_children(node, output);
            output += "</thead>\n";
            break;

        case MdNodeType::TableBody:
            output += "<tbody>\n";
            render_children(node, output);
            output += "</tbody>\n";
            break;

        case MdNodeType::TableRow:
            output += "<tr>";
            render_children(node, output);
            output += "</tr>\n";
            break;

        case MdNodeType::TableCell:
        {
            auto tag = node.is_header ? "th" : "td";
            auto style = alignment_style(node.alignment);
            if (!style.empty())
            {
                output += fmt::format("<{} style=\"{}\">", tag, style);
            }
            else
            {
                output += fmt::format("<{}>", tag);
            }
            render_children(node, output);
            output += fmt::format("</{}>", tag);
            break;
        }

        case MdNodeType::HtmlBlock:
        case MdNodeType::HtmlInline:
            output += node.text_content;
            break;

        // --- Inline nodes ---
        case MdNodeType::Text:
            output += escape_html(node.text_content);
            break;

        case MdNodeType::Emphasis:
            output += "<em>";
            render_children(node, output);
            output += "</em>";
            break;

        case MdNodeType::Strong:
            output += "<strong>";
            render_children(node, output);
            output += "</strong>";
            break;

        case MdNodeType::StrongEmphasis:
            output += "<strong><em>";
            render_children(node, output);
            output += "</em></strong>";
            break;

        case MdNodeType::Code:
            output += "<code>";
            render_children(node, output);
            output += "</code>";
            break;

        case MdNodeType::Link:
            output += fmt::format("<a href=\"{}\"", escape_html(node.url));
            if (!node.title.empty())
            {
                output += fmt::format(" title=\"{}\"", escape_html(node.title));
            }
            output += ">";
            render_children(node, output);
            output += "</a>";
            break;

        case MdNodeType::Image:
        {
            // Collect alt text from children
            std::string alt_text;
            for (const auto& child : node.children)
            {
                alt_text += child.plain_text();
            }

            // Resolve and validate the image path
            auto resolved = resolve_image_path(node.url);
            if (resolved.empty())
            {
                // Missing or blocked image
                output += render_missing_image(node.url, alt_text);
            }
            else
            {
                auto data_uri = encode_image_as_data_uri(resolved);
                if (data_uri.empty())
                {
                    output += render_missing_image(node.url, alt_text);
                }
                else
                {
                    output +=
                        fmt::format("<img src=\"{}\" alt=\"{}\"", data_uri, escape_html(alt_text));
                    if (!node.title.empty())
                    {
                        output += fmt::format(" title=\"{}\"", escape_html(node.title));
                    }
                    output += ">";
                }
            }
            break;
        }

        case MdNodeType::LineBreak:
            output += "<br>\n";
            break;

        case MdNodeType::SoftBreak:
            output += "\n";
            break;

        case MdNodeType::TaskListMarker:
            if (node.is_checked)
            {
                output += "<input type=\"checkbox\" checked disabled> ";
            }
            else
            {
                output += "<input type=\"checkbox\" disabled> ";
            }
            break;

        case MdNodeType::Strikethrough:
            output += "<del>";
            render_children(node, output);
            output += "</del>";
            break;
    }
}

void HtmlRenderer::render_children(const core::MdNode& node, std::string& output)
{
    for (const auto& child : node.children)
    {
        render_node(child, output);
    }
}

// ═══════════════════════════════════════════════════════
// Helpers
// ═══════════════════════════════════════════════════════

auto HtmlRenderer::escape_html(std::string_view text) -> std::string
{
    return core::escape_html(text);
}

auto HtmlRenderer::alignment_style(core::MdAlignment align) -> std::string
{
    switch (align)
    {
        case core::MdAlignment::Left:
            return "text-align: left";
        case core::MdAlignment::Center:
            return "text-align: center";
        case core::MdAlignment::Right:
            return "text-align: right";
        default:
            return {};
    }
}

void HtmlRenderer::set_mermaid_renderer(core::IMermaidRenderer* renderer)
{
    mermaid_renderer_ = renderer;
}

void HtmlRenderer::set_base_path(const std::filesystem::path& base_path)
{
    base_path_ = base_path;
}

auto HtmlRenderer::resolve_image_path(std::string_view url) const -> std::filesystem::path
{
    auto url_str = std::string(url);

    // Block remote URLs
    if (url_str.starts_with("http://") || url_str.starts_with("https://") ||
        url_str.starts_with("ftp://") || url_str.starts_with("data:"))
    {
        return {};
    }

    // Resolve path
    std::filesystem::path image_path;
    if (url_str.starts_with("/"))
    {
        image_path = url_str;
    }
    else if (!base_path_.empty())
    {
        image_path = base_path_ / url_str;
    }
    else
    {
        image_path = url_str;
    }

    // Normalize
    std::error_code ec;
    image_path = std::filesystem::weakly_canonical(image_path, ec);
    if (ec)
    {
        return {};
    }

    // Path traversal prevention: ensure resolved path is within base directory
    if (!base_path_.empty())
    {
        auto canonical_base = std::filesystem::weakly_canonical(base_path_, ec);
        if (!ec)
        {
            auto path_str = image_path.string();
            auto base_str = canonical_base.string();
            if (!path_str.starts_with(base_str))
            {
                return {}; // Path traversal attempt detected
            }
        }
    }

    // Check existence
    if (!std::filesystem::exists(image_path, ec) || ec)
    {
        return {};
    }

    // Validate extension
    auto ext = image_path.extension().string();
    std::transform(
        ext.begin(), ext.end(), ext.begin(), [](unsigned char ch) { return std::tolower(ch); });

    static const std::vector<std::string> kAllowedExtensions = {
        ".png", ".jpg", ".jpeg", ".gif", ".bmp", ".svg", ".webp", ".ico"};

    bool valid_ext = false;
    for (const auto& allowed : kAllowedExtensions)
    {
        if (ext == allowed)
        {
            valid_ext = true;
            break;
        }
    }
    if (!valid_ext)
    {
        return {};
    }

    // Check file size
    auto file_size = std::filesystem::file_size(image_path, ec);
    if (ec || file_size > kMaxImageFileSize)
    {
        return {};
    }

    return image_path;
}

auto HtmlRenderer::encode_image_as_data_uri(const std::filesystem::path& image_path) -> std::string
{
    // Determine MIME type from extension
    auto ext = image_path.extension().string();
    std::transform(
        ext.begin(), ext.end(), ext.begin(), [](unsigned char ch) { return std::tolower(ch); });

    std::string mime;
    if (ext == ".png")
        mime = "image/png";
    else if (ext == ".jpg" || ext == ".jpeg")
        mime = "image/jpeg";
    else if (ext == ".gif")
        mime = "image/gif";
    else if (ext == ".bmp")
        mime = "image/bmp";
    else if (ext == ".svg")
        mime = "image/svg+xml";
    else if (ext == ".webp")
        mime = "image/webp";
    else if (ext == ".ico")
        mime = "image/x-icon";
    else
        return {};

    // Read file directly into string (avoids ostringstream copy)
    std::ifstream file(image_path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return {};
    }
    auto size = file.tellg();
    if (size <= 0 || static_cast<size_t>(size) > kMaxImageFileSize)
    {
        return {};
    }
    file.seekg(0, std::ios::beg);
    std::string data(static_cast<size_t>(size), '\0');
    if (!file.read(data.data(), size))
    {
        return {};
    }

    // Base64 encode using MermaidBlockRenderer's encoder
    auto b64 = MermaidBlockRenderer::base64_encode(data);

    return fmt::format("data:{};base64,{}", mime, b64);
}

auto HtmlRenderer::render_missing_image(std::string_view url, std::string_view alt_text)
    -> std::string
{
    std::string html;
    html += "<div class=\"image-missing\">";
    html += "<em>Image not found: ";
    html += escape_html(url);
    html += "</em>";
    if (!alt_text.empty())
    {
        html += "<br>";
        html += escape_html(alt_text);
    }
    html += "</div>";
    return html;
}

} // namespace markamp::rendering
