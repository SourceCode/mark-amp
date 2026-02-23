#pragma once

#include "ThemeTokens.h"
#include "TokenInheritanceResolver.h"

#include <wx/colour.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Maps string-based scoped tokens to exact color values, resolving fallbacks dynamically.
class ScopedTokenMap
{
public:
    ScopedTokenMap();

    /// Sets a parsed color value for a specific scoped token.
    void set(const std::string& token, const wxColour& color);

    /// Gets a color directly without falling back to inheritance.
    [[nodiscard]] auto get_explicit(const std::string& token) const -> std::optional<wxColour>;

    /// Resolves a color, tracing the inheritance chain if the token is not explicit.
    [[nodiscard]] auto resolve(const std::string& token) const -> std::optional<wxColour>;

    /// Bridges the legacy ThemeColorToken enum to the new scoped string paths.
    [[nodiscard]] auto resolve(ThemeColorToken enum_token) const -> std::optional<wxColour>;

    /// Check if a scoped token is explicitly defined in the map.
    [[nodiscard]] auto is_explicit(const std::string& token) const -> bool;

    /// Gets the full inheritance mapping engine (const).
    [[nodiscard]] auto resolver() const -> const TokenInheritanceResolver&;

    /// Gets the full inheritance mapping engine (mutable).
    [[nodiscard]] auto resolver() -> TokenInheritanceResolver&;

    /// Clears all explicit tokens.
    void clear();

    /// Gets a list of all tokens explicitly defined in the map.
    [[nodiscard]] auto get_explicit_tokens() const -> std::vector<std::string>;

private:
    std::unordered_map<std::string, wxColour> token_values_;
    TokenInheritanceResolver inheritance_resolver_;

    /// Returns the static mapping from legacy enum to modern string paths.
    [[nodiscard]] static auto get_enum_mapping()
        -> const std::unordered_map<ThemeColorToken, std::string>&;
};

} // namespace markamp::core
