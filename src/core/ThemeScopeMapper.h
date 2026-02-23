/// @file ThemeScopeMapper.h
/// @brief V8 Phase 12 (Phase 42) — TextMate Scope Matching & Semantic Token Overrides.
/// Matches VSCode's visual fidelity for code coloring by implementing
/// TextMate scope priority logic and semantic token override support.

#pragma once

#include "ThemeTokens.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Font style flags matching VSCode theme conventions.
enum class FontStyleFlag : std::uint8_t
{
    kNone = 0,
    kItalic = 1 << 0,
    kBold = 1 << 1,
    kUnderline = 1 << 2
};

/// Allow bitwise OR for FontStyleFlag.
inline auto operator|(FontStyleFlag lhs, FontStyleFlag rhs) -> FontStyleFlag
{
    return static_cast<FontStyleFlag>(static_cast<std::uint8_t>(lhs) |
                                      static_cast<std::uint8_t>(rhs));
}

/// Allow bitwise AND for FontStyleFlag.
inline auto operator&(FontStyleFlag lhs, FontStyleFlag rhs) -> FontStyleFlag
{
    return static_cast<FontStyleFlag>(static_cast<std::uint8_t>(lhs) &
                                      static_cast<std::uint8_t>(rhs));
}

/// A scope match result with specificity/priority ranking.
struct ScopeMatch
{
    std::string scope;      ///< The matched scope selector
    int specificity{0};     ///< Higher = more specific match
    int priority{0};        ///< Higher = takes precedence
    std::string foreground; ///< Resolved foreground color
    FontStyleFlag font_style{FontStyleFlag::kNone};
};

/// Maps TextMate scopes to colors and font styles using VSCode-compatible
/// priority and specificity rules. Supports semantic token overrides.
class ThemeScopeMapper
{
public:
    /// A rule entry for scope matching.
    struct ScopeRule
    {
        std::string selector;   ///< e.g. "keyword.control.flow"
        std::string foreground; ///< Hex color
        FontStyleFlag font_style{FontStyleFlag::kNone};
        bool is_semantic{false}; ///< Semantic tokens override TextMate
    };

    /// Add a TextMate scope rule.
    void add_rule(const ScopeRule& rule);

    /// Add a semantic token override (higher priority than TextMate rules).
    void add_semantic_override(const ScopeRule& rule);

    /// Find the best matching scope for a given token scope string.
    /// Returns the most specific match, with semantic overrides taking precedence.
    [[nodiscard]] auto match_scope(const std::string& token_scope) const -> ScopeMatch;

    /// Apply semantic overrides on top of TextMate matches.
    [[nodiscard]] auto apply_overrides(const ScopeMatch& textmate_match,
                                       const std::string& semantic_type) const -> ScopeMatch;

    /// Parse a font style string ("italic bold underline") into flags.
    [[nodiscard]] static auto parse_font_style(const std::string& style_str) -> FontStyleFlag;

    /// Number of registered rules.
    [[nodiscard]] auto rule_count() const -> int;

    /// Number of semantic overrides.
    [[nodiscard]] auto semantic_override_count() const -> int;

    /// Clear all rules.
    void clear();

private:
    std::vector<ScopeRule> textmate_rules_;
    std::vector<ScopeRule> semantic_overrides_;

    /// Calculate specificity of a selector match against a scope.
    [[nodiscard]] static auto calculate_specificity(const std::string& selector,
                                                    const std::string& scope) -> int;
};

} // namespace markamp::core
