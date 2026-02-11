#pragma once

#include "core/SyntaxHighlighter.h"

#include <string>
#include <string_view>

namespace markamp::rendering
{

/// Renders code blocks with syntax highlighting and language labels.
/// Produces the PRD-specified HTML structure:
///   <div class="code-block-wrapper">
///     <div class="code-block-header">
///       <span class="language-label">{language}</span>
///     </div>
///     <pre class="code-block"><code>...highlighted tokens...</code></pre>
///   </div>
class CodeBlockRenderer
{
public:
    CodeBlockRenderer();

    /// Render a fenced code block with optional language.
    [[nodiscard]] auto render(std::string_view source, const std::string& language) const
        -> std::string;

    /// Render a code block without language (indented or bare fenced).
    [[nodiscard]] auto render_plain(std::string_view source) const -> std::string;

private:
    mutable core::SyntaxHighlighter highlighter_;

    [[nodiscard]] static auto escape_html(std::string_view text) -> std::string;
};

} // namespace markamp::rendering
