/// @file TokenThemeMapper.h
/// @brief V20 P07-T03: Token-to-theme scope mapping.
///
/// Connects tokenization output to the theme scope machinery. Maps syntax
/// token classes to theme-driven visual styles, ensuring consistent
/// presentation across all surfaces.
#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Token class for syntax elements.
enum class TokenClass
{
    kKeyword,
    kString,
    kComment,
    kNumber,
    kOperator,
    kPunctuation,
    kType,
    kFunction,
    kVariable,
    kConstant,
    kPreprocessor,
    kDecorator,
    kAttribute,
    kNamespace,
    kLabel,
    kRegex,
    kEscape,
    kInvalid,
    kPlainText
};

/// Visual style for a token.
struct TokenStyle
{
    std::string foreground_color;            ///< Hex color
    std::string background_color;            ///< Optional background
    bool is_bold{false};
    bool is_italic{false};
    bool is_underline{false};

    [[nodiscard]] auto has_foreground() const noexcept -> bool
    {
        return !foreground_color.empty();
    }
};

/// Mapping from token scope to visual style.
struct ScopeMapping
{
    std::string scope;                       ///< TextMate-style scope (e.g. "keyword.control")
    TokenClass token_class{TokenClass::kPlainText};
    TokenStyle style;
};

/// Maps syntax tokens to theme-driven styles.
class TokenThemeMapper
{
public:
    /// Register a scope mapping.
    void register_scope(const ScopeMapping& mapping);

    /// Resolve style for a token class.
    [[nodiscard]] auto resolve_style(TokenClass token_class) const -> TokenStyle;

    /// Resolve style for a scope string.
    [[nodiscard]] auto resolve_scope(const std::string& scope) const -> TokenStyle;

    /// Set the default style.
    void set_default_style(const TokenStyle& style);

    /// Get the default style.
    [[nodiscard]] auto default_style() const -> const TokenStyle& { return default_style_; }

    /// List all scope mappings.
    [[nodiscard]] auto all_scopes() const -> std::vector<ScopeMapping>;

    /// Count of unmapped token requests (for diagnostics).
    [[nodiscard]] auto unmapped_count() const noexcept -> int { return unmapped_count_; }

    /// Total mappings.
    [[nodiscard]] auto mapping_count() const noexcept -> int
    {
        return static_cast<int>(scope_mappings_.size());
    }

private:
    std::unordered_map<std::string, ScopeMapping> scope_mappings_;
    std::unordered_map<int, TokenStyle> class_styles_;
    TokenStyle default_style_;
    mutable int unmapped_count_{0};
};

} // namespace markamp::core
