#include "MarkdownParser.h"

#include "rendering/HtmlRenderer.h"

namespace markamp::core
{

MarkdownParser::MarkdownParser() = default;

auto MarkdownParser::parse(std::string_view markdown)
    -> std::expected<MarkdownDocument, std::string>
{
    // Pre-process footnotes (md4c doesn't support them natively)
    rendering::FootnotePreprocessor footnote_processor;
    auto footnote_result = footnote_processor.process(markdown);

    // Parse the processed markdown (footnote refs are now HTML superscripts)
    auto doc_result = parser_.parse(footnote_result.processed_markdown);
    if (!doc_result.has_value())
    {
        return doc_result;
    }

    // Store footnote section for later use during rendering
    doc_result->footnote_section_html = std::move(footnote_result.footnote_section_html);
    doc_result->has_footnotes_ = footnote_result.has_footnotes;

    return doc_result;
}

auto MarkdownParser::render_html(const MarkdownDocument& doc) -> std::string
{
    rendering::HtmlRenderer renderer;
    return renderer.render_with_footnotes(doc, doc.footnote_section_html);
}

} // namespace markamp::core
