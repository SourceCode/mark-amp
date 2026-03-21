/// @file TokenThemeMapper.cpp
/// @brief V20 P07-T03: Token-to-theme mapper implementation.

#include "TokenThemeMapper.h"

namespace markamp::core
{

void TokenThemeMapper::register_scope(const ScopeMapping& mapping)
{
    scope_mappings_[mapping.scope] = mapping;
    class_styles_[static_cast<int>(mapping.token_class)] = mapping.style;
}

auto TokenThemeMapper::resolve_style(TokenClass token_class) const -> TokenStyle
{
    auto it = class_styles_.find(static_cast<int>(token_class));
    if (it != class_styles_.end())
    {
        return it->second;
    }
    ++unmapped_count_;
    return default_style_;
}

auto TokenThemeMapper::resolve_scope(const std::string& scope) const -> TokenStyle
{
    auto it = scope_mappings_.find(scope);
    if (it != scope_mappings_.end())
    {
        return it->second.style;
    }

    // Try prefix match (e.g. "keyword" matches "keyword.control")
    for (const auto& [s, mapping] : scope_mappings_)
    {
        if (scope.starts_with(s) || s.starts_with(scope))
        {
            return mapping.style;
        }
    }

    ++unmapped_count_;
    return default_style_;
}

void TokenThemeMapper::set_default_style(const TokenStyle& style)
{
    default_style_ = style;
}

auto TokenThemeMapper::all_scopes() const -> std::vector<ScopeMapping>
{
    std::vector<ScopeMapping> result;
    result.reserve(scope_mappings_.size());
    for (const auto& [scope, mapping] : scope_mappings_)
    {
        result.push_back(mapping);
    }
    return result;
}

} // namespace markamp::core
