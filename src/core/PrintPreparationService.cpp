// ============================================================================
// File: src/core/PrintPreparationService.cpp
// Phase 24: Export & Publishing — Print preparation and preview
// ============================================================================

#include "PrintPreparationService.h"

#include <algorithm>
#include <regex>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Construction
// ============================================================================

PrintPreparationService::PrintPreparationService() = default;

// ============================================================================
// Core API
// ============================================================================

auto PrintPreparationService::prepare_for_print(const std::string& content_html,
                                                const PrintLayout& layout) const -> std::string
{
    // 1. Inject page breaks at heading boundaries.
    auto html = inject_page_breaks(content_html, layout.heading_break_level);

    // 2. Generate print CSS.
    auto css = generate_print_css(layout);

    // 3. Wrap in a complete HTML document.
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
        << "<meta charset=\"utf-8\">\n"
        << "<style>\n"
        << css << "\n</style>\n"
        << "</head>\n<body>\n"
        << html << "\n</body>\n</html>\n";

    return oss.str();
}

auto PrintPreparationService::generate_preview(const std::string& content_html,
                                               const std::string& title,
                                               const PrintLayout& layout) const -> PrintPreviewData
{
    PrintPreviewData data;
    data.content_length = static_cast<int64_t>(content_html.size());
    data.estimated_page_count = estimate_page_count(data.content_length, layout.page_size);
    data.print_css = generate_print_css(layout);
    data.sections = extract_sections(content_html, layout.heading_break_level);

    // Build the full print-ready HTML.
    auto html = inject_page_breaks(content_html, layout.heading_break_level);
    html = add_headers_footers(html, layout, title);

    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
        << "<meta charset=\"utf-8\">\n"
        << "<title>" << title << " — Print Preview</title>\n"
        << "<style>\n"
        << data.print_css << "\n</style>\n"
        << "</head>\n<body>\n"
        << html << "\n</body>\n</html>\n";

    data.content_html = oss.str();
    return data;
}

// ============================================================================
// Content transforms
// ============================================================================

auto PrintPreparationService::inject_page_breaks(const std::string& html, int heading_level) const
    -> std::string
{
    // Insert page-break-before style on <hN> tags at the specified level.
    // Match <h1>, <h2> etc. and prepend a page-break div before them.
    // We skip the very first heading to avoid a blank first page.

    const std::string tag = "h" + std::to_string(heading_level);
    const std::regex heading_re("<" + tag + R"((\s|>))", std::regex::icase);

    std::string result;
    result.reserve(html.size() + html.size() / 10);

    auto begin = std::sregex_iterator(html.begin(), html.end(), heading_re);
    auto end = std::sregex_iterator();

    size_t last_pos = 0;
    bool first = true;
    for (auto iter = begin; iter != end; ++iter)
    {
        const auto& match = *iter;
        auto match_start = static_cast<size_t>(match.position());

        result.append(html, last_pos, match_start - last_pos);

        if (!first)
        {
            result.append(R"(<div style="page-break-before:always"></div>)");
        }
        first = false;

        result.append(match.str());
        last_pos = match_start + static_cast<size_t>(match.length());
    }
    result.append(html, last_pos);

    return result;
}

auto PrintPreparationService::add_headers_footers(const std::string& html,
                                                  const PrintLayout& layout,
                                                  const std::string& title) const -> std::string
{
    if (layout.header_text.empty() && layout.footer_text.empty() && !layout.page_numbers)
    {
        return html;
    }

    std::ostringstream oss;

    // Header.
    if (!layout.header_text.empty())
    {
        std::string header = layout.header_text;
        // Replace {{title}} placeholder.
        std::string::size_type pos = 0;
        while ((pos = header.find("{{title}}", pos)) != std::string::npos)
        {
            header.replace(pos, 9, title);
            pos += title.size();
        }
        oss << "<div class=\"print-header\">" << header << "</div>\n";
    }

    oss << html;

    // Footer.
    if (!layout.footer_text.empty() || layout.page_numbers)
    {
        std::string footer = layout.footer_text;
        if (footer.empty() && layout.page_numbers)
        {
            footer = "Page {{page}}";
        }
        oss << "\n<div class=\"print-footer\">" << footer << "</div>\n";
    }

    return oss.str();
}

auto PrintPreparationService::estimate_page_count(int64_t content_length, PageSize page_size) -> int
{
    // Rough heuristic: characters per page varies by page size.
    int chars_per_page = 3000; // Default for A4.
    switch (page_size)
    {
        case PageSize::kA4:
            chars_per_page = 3000;
            break;
        case PageSize::kA5:
            chars_per_page = 1800;
            break;
        case PageSize::kLetter:
            chars_per_page = 3200;
            break;
        case PageSize::kLegal:
            chars_per_page = 4000;
            break;
        case PageSize::kCustom:
            chars_per_page = 3000;
            break;
    }

    if (content_length <= 0)
    {
        return 0;
    }
    return std::max(1, static_cast<int>((content_length + chars_per_page - 1) / chars_per_page));
}

// ============================================================================
// CSS generation
// ============================================================================

auto PrintPreparationService::generate_print_css(const PrintLayout& layout) -> std::string
{
    std::ostringstream oss;

    oss << "@page {\n"
        << "  size: " << page_size_css(layout.page_size);
    if (layout.landscape)
    {
        oss << " landscape";
    }
    oss << ";\n"
        << "  margin-top: " << layout.margin_top << ";\n"
        << "  margin-bottom: " << layout.margin_bottom << ";\n"
        << "  margin-left: " << layout.margin_left << ";\n"
        << "  margin-right: " << layout.margin_right << ";\n"
        << "}\n\n";

    oss << "@media print {\n"
        << "  body { font-family: Georgia, serif; line-height: 1.6; }\n"
        << "  .print-header { position: running(header); font-size: 0.8em; color: #666; }\n"
        << "  .print-footer { position: running(footer); font-size: 0.8em; color: #666; "
           "text-align: center; }\n"
        << "  pre { page-break-inside: avoid; }\n"
        << "  table { page-break-inside: avoid; }\n"
        << "  img { max-width: 100%; page-break-inside: avoid; }\n"
        << "}\n\n";

    // Screen preview styles.
    oss << "body { font-family: Georgia, serif; max-width: 800px; margin: 2em auto; "
           "line-height: 1.6; }\n"
        << "code { background: #f5f5f5; padding: 2px 6px; border-radius: 3px; }\n"
        << "pre { background: #f5f5f5; padding: 1em; overflow-x: auto; }\n";

    return oss.str();
}

auto PrintPreparationService::page_size_css(PageSize ps) -> std::string
{
    switch (ps)
    {
        case PageSize::kA4:
            return "A4";
        case PageSize::kA5:
            return "A5";
        case PageSize::kLetter:
            return "letter";
        case PageSize::kLegal:
            return "legal";
        case PageSize::kCustom:
            return "A4"; // Default fallback.
    }
    return "A4";
}

// ============================================================================
// Section extraction
// ============================================================================

auto PrintPreparationService::extract_sections(const std::string& html, int heading_level)
    -> std::vector<std::string>
{
    const std::string tag = "h" + std::to_string(heading_level);
    // Match <hN>...</hN> and capture the inner text.
    const std::regex heading_re("<" + tag + R"([^>]*>(.*?)</)" + tag + ">", std::regex::icase);

    std::vector<std::string> sections;
    auto begin = std::sregex_iterator(html.begin(), html.end(), heading_re);
    auto end = std::sregex_iterator();

    for (auto iter = begin; iter != end; ++iter)
    {
        // Strip inner HTML tags from the heading text.
        std::string text = (*iter)[1].str();
        static const std::regex kHtmlTag(R"(<[^>]+>)");
        text = std::regex_replace(text, kHtmlTag, "");
        sections.push_back(text);
    }

    return sections;
}

} // namespace markamp::core
