#pragma once

#include "core/SyntaxHighlighter.h"

#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::rendering
{

/// Renders code blocks with syntax highlighting, language picker,
/// copy button, and line-highlight support.
///
/// HTML structure produced:
///   <div class="code-block-wrapper">
///     <div class="code-block-header">
///       <span class="language-label">{language}</span>
///       <a href="markamp://copy/{id}" class="copy-btn">📋 Copy</a>
///     </div>
///     <pre class="code-block"><code>...highlighted tokens...</code></pre>
///   </div>
class CodeBlockRenderer
{
public:
    CodeBlockRenderer();

    /// Render a fenced code block with optional language and highlight spec.
    /// @param source       Raw code text
    /// @param language     Language identifier (e.g. "cpp", "python")
    /// @param highlight_spec  Optional line highlight spec parsed from info_string (e.g. "{1,3-5}")
    [[nodiscard]] auto render(std::string_view source,
                              const std::string& language,
                              const std::string& highlight_spec = "") const -> std::string;

    /// Render a code block without language (indented or bare fenced).
    [[nodiscard]] auto render_plain(std::string_view source) const -> std::string;

    /// Reset the block counter (call once per full-document render).
    void reset_counter() const;

    /// Retrieve stored source for a block ID (for clipboard copy).
    [[nodiscard]] auto get_block_source(int block_id) const -> std::string;

    /// Parse "{1,3-5}" notation into a set of 1-based line numbers.
    [[nodiscard]] static auto parse_highlight_spec(const std::string& spec) -> std::set<int>;

    /// Extract highlight spec from full info_string (everything after language).
    [[nodiscard]] static auto extract_highlight_spec(const std::string& info_string,
                                                     const std::string& language) -> std::string;

private:
    mutable core::SyntaxHighlighter highlighter_;
    mutable int block_counter_{0};
    mutable std::vector<std::string> block_sources_;

    [[nodiscard]] static auto escape_html(std::string_view text) -> std::string;

    /// Wrap rendered code lines with highlight spans where requested.
    [[nodiscard]] static auto apply_line_highlights(const std::string& code_html,
                                                    const std::set<int>& highlight_lines)
        -> std::string;
};

} // namespace markamp::rendering
