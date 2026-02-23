#pragma once

#include "Color.h"

#include <wx/colour.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Determines how scoped tokens fallback if not explicitly defined.
/// This acts like CSS inheritance (e.g., "tab.activeBackground" -> "editor.background").
class TokenInheritanceResolver
{
public:
    TokenInheritanceResolver();

    /// Registers a fallback relationship: if `token_name` is missing, try `fallback_token`.
    void register_fallback(const std::string& token_name, const std::string& fallback_token);

    /// Gets the fallback token for a given token, if one is registered.
    [[nodiscard]] auto get_fallback(const std::string& token_name) const
        -> std::optional<std::string>;

    /// Gets the full inheritance chain (including the starting token) to prevent infinite loops.
    [[nodiscard]] auto get_resolution_chain(const std::string& token_name) const
        -> std::vector<std::string>;

    /// Populates the standard V2 token fallback rules.
    void apply_default_rules();

private:
    std::unordered_map<std::string, std::string> fallback_rules_;
};

} // namespace markamp::core
