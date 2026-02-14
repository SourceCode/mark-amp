#pragma once

#include "IMathRenderer.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Concrete math renderer that converts LaTeX expressions to HTML
/// using Unicode symbol approximations.
///
/// Supports:
/// - Greek letters (\alpha, \beta, etc.)
/// - Mathematical operators (\sum, \prod, \int, etc.)
/// - Relations (\leq, \geq, \neq, etc.)
/// - Arrows (\rightarrow, \leftarrow, etc.)
/// - Common functions (\sin, \cos, \log, etc.)
/// - Superscripts and subscripts (^, _)
/// - Fractions (\frac{a}{b} → a/b)
class MathRenderer : public IMathRenderer
{
public:
    MathRenderer();

    [[nodiscard]] auto render(std::string_view latex, bool is_display) -> std::string override;
    [[nodiscard]] auto is_available() const -> bool override;

private:
    /// Tokenize LaTeX into segments (commands, braces, text)
    struct Token
    {
        enum class Kind
        {
            kCommand,     // \alpha, \frac, etc.
            kText,        // plain text/numbers
            kSuperscript, // ^
            kSubscript,   // _
            kOpenBrace,   // {
            kCloseBrace,  // }
            kCaret,       // ^ without braces
            kUnderscore,  // _ without braces
        };
        Kind kind;
        std::string value;
    };

    [[nodiscard]] auto tokenize(std::string_view latex) const -> std::vector<Token>;
    [[nodiscard]] auto render_tokens(const std::vector<Token>& tokens) const -> std::string;

    /// Convert a single character or short string to Unicode superscript
    [[nodiscard]] static auto to_superscript(std::string_view text) -> std::string;
    /// Convert a single character or short string to Unicode subscript
    [[nodiscard]] static auto to_subscript(std::string_view text) -> std::string;

    /// Symbol lookup table: LaTeX command → Unicode string
    std::unordered_map<std::string, std::string> symbols_;
};

} // namespace markamp::core
