#pragma once

#include "IMarkdownParser.h"
#include "Md4cWrapper.h"

namespace markamp::core
{

/// Concrete IMarkdownParser using md4c + HtmlRenderer.
class MarkdownParser : public IMarkdownParser
{
public:
    MarkdownParser();

    [[nodiscard]] auto parse(std::string_view markdown)
        -> std::expected<MarkdownDocument, std::string> override;
    [[nodiscard]] auto render_html(const MarkdownDocument& doc) -> std::string override;

private:
    Md4cParser parser_;
};

} // namespace markamp::core
