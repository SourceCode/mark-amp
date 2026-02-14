#include "HtmlRenderer.h"

#include "CodeBlockRenderer.h"
#include "MermaidBlockRenderer.h"
#include "core/IMathRenderer.h"
#include "core/IMermaidRenderer.h"
#include "core/Profiler.h"
#include "core/StringUtils.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

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
    // Stability #34: cap footnote definitions to prevent memory exhaustion
    static constexpr int kMaxFootnoteDefinitions = 1000;
    size_t pos = 0;
    while (pos < markdown.size())
    {
        // Stability #34: stop collecting if we've hit the cap
        if (number >= kMaxFootnoteDefinitions)
        {
            // Flush remaining lines into cleaned output without scanning for defs
            cleaned.append(markdown.substr(pos));
            break;
        }

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

    // Improvement #3: track code fence regions to skip footnote replacement inside them
    // Build a set of code fence ranges [start, end)
    std::vector<std::pair<size_t, size_t>> fence_ranges;
    {
        size_t scan = 0;
        while (scan < result.size())
        {
            auto fence_start = result.find("```", scan);
            if (fence_start == std::string::npos)
                break;
            auto fence_end = result.find("```", fence_start + 3);
            if (fence_end == std::string::npos)
                break;
            fence_ranges.emplace_back(fence_start, fence_end + 3);
            scan = fence_end + 3;
        }
    }

    auto is_inside_fence = [&fence_ranges](size_t pos) -> bool
    {
        for (const auto& [start, end] : fence_ranges)
        {
            if (pos >= start && pos < end)
                return true;
        }
        return false;
    };

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
            // Improvement #3: skip replacement inside code fences
            if (is_inside_fence(pos))
            {
                pos += pattern.size();
                continue;
            }
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
    // New stability #34: wrap entire render pipeline in try-catch
    try
    {
        code_renderer_.reset_counter();
        heading_slug_counts_.clear(); // Improvement #6: reset per render
        std::string output;
        // Improvement #13: pre-allocate based on total text length estimate
        size_t estimate = 0;
        for (const auto& child : doc.root.children)
        {
            estimate += child.text_content.size();
        }
        output.reserve(std::max(estimate * 4, static_cast<size_t>(512)));
        render_children(doc.root, output);
        return output;
    }
    catch (const std::exception& ex)
    {
        return std::string("<!-- render error: ") + ex.what() + " -->";
    }
}

auto HtmlRenderer::render_with_footnotes(const core::MarkdownDocument& doc,
                                         const std::string& footnote_section) -> std::string
{
    // Improvement #1: wrap in try-catch matching render()
    try
    {
        code_renderer_.reset_counter();
        heading_slug_counts_.clear(); // Improvement #6: reset per render
        std::string output;
        output.reserve(doc.root.children.size() * 256 + footnote_section.size());
        render_children(doc.root, output);
        if (!footnote_section.empty())
        {
            output += footnote_section;
        }
        return output;
    }
    catch (const std::exception& ex)
    {
        return std::string("<!-- render error: ") + ex.what() + " -->";
    }
}

// ═══════════════════════════════════════════════════════
// Recursive rendering
// ═══════════════════════════════════════════════════════

void HtmlRenderer::render_node(const core::MdNode& node, std::string& output, int depth)
{
    // Stability #31: cap recursion depth to prevent stack overflow
    if (depth > kMaxRenderDepth)
    {
        output += "<!-- max render depth exceeded -->";
        return;
    }

    // New stability #35: cap children count to prevent excessive processing
    if (node.children.size() > 10000)
    {
        output += "<!-- node children limit exceeded -->";
        return;
    }

    using core::MdNodeType;

    switch (node.type)
    {
        case MdNodeType::Document:
            render_children(node, output, depth + 1);
            break;

        case MdNodeType::Paragraph:
            output += "<p>";
            render_children(node, output, depth + 1);
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

            // Improvement #40: slugify extracted as reusable logic
            auto slug = slugify(heading_text);

            // Improvement #6: ensure unique heading IDs by appending suffix
            if (!slug.empty())
            {
                auto it = heading_slug_counts_.find(slug);
                if (it != heading_slug_counts_.end())
                {
                    it->second++;
                    slug += "-" + std::to_string(it->second);
                }
                else
                {
                    heading_slug_counts_[slug] = 0;
                }
            }

            // Stability #32: clamp heading level to valid range [1, 6]
            int level = std::clamp(node.heading_level, 1, 6);

            output += fmt::format("<h{} id=\"{}\">", level, slug);
            render_children(node, output, depth + 1);
            output += fmt::format("</h{}>\n", level);
            break;
        }

        case MdNodeType::BlockQuote:
            output += "<blockquote>\n";
            render_children(node, output, depth + 1);
            output += "</blockquote>\n";
            break;

        case MdNodeType::UnorderedList:
            output += "<ul>\n";
            render_children(node, output, depth + 1);
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
            render_children(node, output, depth + 1);
            output += "</ol>\n";
            break;

        case MdNodeType::ListItem:
            output += "<li>";
            render_children(node, output, depth + 1);
            output += "</li>\n";
            break;

        case MdNodeType::CodeBlock:
        case MdNodeType::FencedCodeBlock:
        {
            // Improvement #30: normalize language aliases
            auto lang = node.language;
            if (lang == "js")
            {
                lang = "javascript";
            }
            else if (lang == "py")
            {
                lang = "python";
            }
            else if (lang == "ts")
            {
                lang = "typescript";
            }
            else if (lang == "rb")
            {
                lang = "ruby";
            }
            else if (lang == "sh")
            {
                lang = "bash";
            }
            else if (lang == "yml")
            {
                lang = "yaml";
            }
            else if (lang == "md")
            {
                lang = "markdown";
            }

            auto hl_spec = CodeBlockRenderer::extract_highlight_spec(node.info_string, lang);
            output += code_renderer_.render(node.text_content, lang, hl_spec);
            break;
        }

        case MdNodeType::MermaidBlock:
        {
            MermaidBlockRenderer mermaid_block;
            // Phase 4: Guard Mermaid rendering behind feature toggle
            if (!mermaid_enabled_)
            {
                output += mermaid_block.render_placeholder(node.text_content);
            }
            else if (mermaid_renderer_ && mermaid_renderer_->is_available())
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
            render_children(node, output, depth + 1);
            output += "</table>\n</div>\n";
            break;

        case MdNodeType::TableHead:
            output += "<thead>\n";
            render_children(node, output, depth + 1);
            output += "</thead>\n";
            break;

        case MdNodeType::TableBody:
            output += "<tbody>\n";
            render_children(node, output, depth + 1);
            output += "</tbody>\n";
            break;

        case MdNodeType::TableRow:
            output += "<tr>";
            render_children(node, output, depth + 1);
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
            render_children(node, output, depth + 1);
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
            render_children(node, output, depth + 1);
            output += "</em>";
            break;

        case MdNodeType::Strong:
            output += "<strong>";
            render_children(node, output, depth + 1);
            output += "</strong>";
            break;

        case MdNodeType::StrongEmphasis:
            output += "<strong><em>";
            render_children(node, output, depth + 1);
            output += "</em></strong>";
            break;

        case MdNodeType::Code:
            output += "<code>";
            render_children(node, output, depth + 1);
            output += "</code>";
            break;

        case MdNodeType::Link:
            output += fmt::format("<a href=\"{}\"", escape_html(node.url));
            if (!node.title.empty())
            {
                output += fmt::format(" title=\"{}\"", escape_html(node.title));
            }
            output += ">";
            render_children(node, output, depth + 1);
            output += "</a>";
            break;

        case MdNodeType::Image:
        {
            // Improvement #38: collect_plain_text helper
            auto alt_text = collect_plain_text(node);

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
                    // Improvement #35: lazy loading + #36: responsive sizing
                    output += fmt::format(
                        R"(<img src="{}" alt="{}" loading="lazy" style="max-width:100%;height:auto")",
                        data_uri,
                        escape_html(alt_text));
                    if (!node.title.empty())
                    {
                        output += fmt::format(R"( title="{}")", escape_html(node.title));
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
            render_children(node, output, depth + 1);
            output += "</del>";
            break;

        case MdNodeType::MathInline:
        case MdNodeType::MathDisplay:
        {
            // Collect math content from children
            std::string math_content;
            for (const auto& child : node.children)
            {
                math_content += child.text_content;
            }

            bool is_display = (node.type == MdNodeType::MathDisplay);

            if (math_enabled_ && math_renderer_ != nullptr && math_renderer_->is_available())
            {
                output += math_renderer_->render(math_content, is_display);
            }
            else
            {
                // Fallback: render raw LaTeX in a styled code element
                if (is_display)
                {
                    output += "<div class=\"math-fallback\"><code>";
                    output += escape_html(math_content);
                    output += "</code></div>\n";
                }
                else
                {
                    output += "<code class=\"math-fallback\">";
                    output += escape_html(math_content);
                    output += "</code>";
                }
            }
            break;
        }

        // Improvement #2: explicit default case for unhandled node types
        default:
            output += fmt::format("<!-- unhandled node type {} -->", static_cast<int>(node.type));
            render_children(node, output, depth + 1);
            break;
    }
}

void HtmlRenderer::render_children(const core::MdNode& node, std::string& output, int depth)
{
    for (const auto& child : node.children)
    {
        render_node(child, output, depth);
    }
}

// ═══════════════════════════════════════════════════════
// Helpers
// ═══════════════════════════════════════════════════════

auto HtmlRenderer::escape_html(std::string_view text) -> std::string
{
    return core::escape_html(text);
}

// Improvement #38: collect plain text from node children (replaces duplicate loops)
auto HtmlRenderer::collect_plain_text(const core::MdNode& node) -> std::string
{
    std::string result;
    for (const auto& child : node.children)
    {
        result += child.plain_text();
    }
    return result;
}

// Improvement #40: reusable slug generation
auto HtmlRenderer::slugify(std::string_view text) -> std::string
{
    std::string slug;
    slug.reserve(text.size());
    for (auto ch : text)
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
    return slug;
}

// Improvement #15: return string_view to avoid allocation per table cell
auto HtmlRenderer::alignment_style(core::MdAlignment align) -> std::string_view
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

// Improvement #16: shared MIME type lookup table
auto HtmlRenderer::mime_for_extension(std::string_view ext) -> std::string_view
{
    static const std::unordered_map<std::string, std::string_view> kMimeTypes = {
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},
        {".bmp", "image/bmp"},
        {".svg", "image/svg+xml"},
        {".webp", "image/webp"},
        {".ico", "image/x-icon"},
    };

    auto iter = kMimeTypes.find(std::string(ext));
    if (iter != kMimeTypes.end())
    {
        return iter->second;
    }
    return {};
}

void HtmlRenderer::set_math_renderer(core::IMathRenderer* renderer)
{
    math_renderer_ = renderer;
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

    // Improvement #4: URL-decode percent-encoded characters (e.g. %20 → space)
    std::string decoded;
    decoded.reserve(url_str.size());
    for (size_t i = 0; i < url_str.size(); ++i)
    {
        if (url_str[i] == '%' && i + 2 < url_str.size())
        {
            auto hi = url_str[i + 1];
            auto lo = url_str[i + 2];
            auto hex_digit = [](char c) -> int
            {
                if (c >= '0' && c <= '9')
                    return c - '0';
                if (c >= 'a' && c <= 'f')
                    return c - 'a' + 10;
                if (c >= 'A' && c <= 'F')
                    return c - 'A' + 10;
                return -1;
            };
            int h = hex_digit(hi);
            int l = hex_digit(lo);
            if (h >= 0 && l >= 0)
            {
                decoded += static_cast<char>((h << 4) | l);
                i += 2;
                continue;
            }
        }
        decoded += url_str[i];
    }
    url_str = std::move(decoded);

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

    // Improvement #18: use unordered_set for O(1) extension lookup
    static const std::unordered_set<std::string> kAllowedExtensions = {
        ".png", ".jpg", ".jpeg", ".gif", ".bmp", ".svg", ".webp", ".ico"};

    if (!kAllowedExtensions.contains(ext))
    {
        return {};
    }

    // Check file size
    auto file_size = std::filesystem::file_size(image_path, ec);
    if (ec || file_size > kMaxImageFileSize)
    {
        return {};
    }

    // Improvement #5: cap SVG files to 1MB to prevent script-laden SVG bombs
    static constexpr size_t kMaxSvgFileSize = 1024 * 1024;
    if (ext == ".svg" && file_size > kMaxSvgFileSize)
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

    // Improvement #16: shared MIME lookup table
    auto mime = mime_for_extension(ext);
    if (mime.empty())
    {
        return {};
    }

    // Stability #33: verify it's a regular file (not a symlink to device file)
    std::error_code fsec;
    if (!std::filesystem::is_regular_file(image_path, fsec) || fsec)
    {
        return {};
    }

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
    // Improvement #17: pre-allocate output string for base64 result
    auto b64 = MermaidBlockRenderer::base64_encode(data);

    std::string result;
    result.reserve(5 + mime.size() + 8 + b64.size()); // "data:" + mime + ";base64," + b64
    result += "data:";
    result += mime;
    result += ";base64,";
    result += b64;
    return result;
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
