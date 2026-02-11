#pragma once

#include "Types.h"

#include <expected>
#include <string>
#include <string_view>

namespace markamp::core
{

/// Interface for the Markdown parsing subsystem.
class IMarkdownParser
{
public:
    virtual ~IMarkdownParser() = default;
    [[nodiscard]] virtual auto parse(std::string_view markdown)
        -> std::expected<MarkdownDocument, std::string> = 0;
    [[nodiscard]] virtual auto render_html(const MarkdownDocument& doc) -> std::string = 0;
};

} // namespace markamp::core
