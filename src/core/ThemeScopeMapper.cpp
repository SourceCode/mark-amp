/// @file ThemeScopeMapper.cpp
/// @brief V8 Phase 12 (Phase 42) — TextMate Scope Matching implementation.

#include "core/ThemeScopeMapper.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

void ThemeScopeMapper::add_rule(const ScopeRule& rule)
{
    textmate_rules_.push_back(rule);
}

void ThemeScopeMapper::add_semantic_override(const ScopeRule& rule)
{
    ScopeRule override_rule = rule;
    override_rule.is_semantic = true;
    semantic_overrides_.push_back(override_rule);
}

auto ThemeScopeMapper::match_scope(const std::string& token_scope) const -> ScopeMatch
{
    ScopeMatch best_match;
    best_match.specificity = -1;

    // First check semantic overrides (higher priority)
    for (const auto& rule : semantic_overrides_)
    {
        const int specificity = calculate_specificity(rule.selector, token_scope);
        if (specificity > best_match.specificity)
        {
            best_match.scope = rule.selector;
            best_match.specificity = specificity;
            best_match.priority = 1000; // Semantic overrides always high priority
            best_match.foreground = rule.foreground;
            best_match.font_style = rule.font_style;
        }
    }

    // If no semantic match, check TextMate rules
    if (best_match.specificity <= 0)
    {
        for (const auto& rule : textmate_rules_)
        {
            const int specificity = calculate_specificity(rule.selector, token_scope);
            if (specificity > best_match.specificity)
            {
                best_match.scope = rule.selector;
                best_match.specificity = specificity;
                best_match.priority = specificity;
                best_match.foreground = rule.foreground;
                best_match.font_style = rule.font_style;
            }
        }
    }

    return best_match;
}

auto ThemeScopeMapper::apply_overrides(const ScopeMatch& textmate_match,
                                       const std::string& semantic_type) const -> ScopeMatch
{
    // Look for a semantic override matching the semantic type
    for (const auto& rule : semantic_overrides_)
    {
        if (rule.selector == semantic_type)
        {
            ScopeMatch result = textmate_match;
            result.foreground = rule.foreground;
            result.font_style = rule.font_style;
            result.priority = 1000;
            return result;
        }
    }
    return textmate_match;
}

auto ThemeScopeMapper::parse_font_style(const std::string& style_str) -> FontStyleFlag
{
    auto flags = FontStyleFlag::kNone;

    if (style_str.find("italic") != std::string::npos)
    {
        flags = flags | FontStyleFlag::kItalic;
    }
    if (style_str.find("bold") != std::string::npos)
    {
        flags = flags | FontStyleFlag::kBold;
    }
    if (style_str.find("underline") != std::string::npos)
    {
        flags = flags | FontStyleFlag::kUnderline;
    }

    return flags;
}

auto ThemeScopeMapper::rule_count() const -> int
{
    return static_cast<int>(textmate_rules_.size());
}

auto ThemeScopeMapper::semantic_override_count() const -> int
{
    return static_cast<int>(semantic_overrides_.size());
}

void ThemeScopeMapper::clear()
{
    textmate_rules_.clear();
    semantic_overrides_.clear();
}

auto ThemeScopeMapper::calculate_specificity(const std::string& selector, const std::string& scope)
    -> int
{
    // TextMate scope matching: more specific selectors score higher.
    // A selector "keyword.control" matches "keyword.control.flow" with
    // specificity proportional to the selector depth.

    if (scope.empty() || selector.empty())
    {
        return 0;
    }

    // Exact match is highest specificity
    if (scope == selector)
    {
        return 1000;
    }

    // Check if scope starts with selector (prefix match)
    if (scope.length() > selector.length() && scope.substr(0, selector.length()) == selector &&
        scope[selector.length()] == '.')
    {
        // Specificity = number of segments in selector * 100
        const int segments =
            static_cast<int>(std::count(selector.begin(), selector.end(), '.')) + 1;
        return segments * 100;
    }

    // Check if selector starts with scope (reverse prefix — less specific)
    if (selector.length() > scope.length() && selector.substr(0, scope.length()) == scope &&
        selector[scope.length()] == '.')
    {
        return 50; // Partial match, low specificity
    }

    // No match
    return 0;
}

// ── Batch 23-25 (#147-148) ──

auto ThemeScopeMapper::has_rule(const std::string& selector) const -> bool
{
    for (const auto& rule : textmate_rules_)
    {
        if (rule.selector == selector)
        {
            return true;
        }
    }
    for (const auto& rule : semantic_overrides_)
    {
        if (rule.selector == selector)
        {
            return true;
        }
    }
    return false;
}

auto ThemeScopeMapper::total_rule_count() const -> int
{
    return rule_count() + semantic_override_count();
}

} // namespace markamp::core
