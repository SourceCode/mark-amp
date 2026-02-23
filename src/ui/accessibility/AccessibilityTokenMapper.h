#pragma once

#include "core/ThemeTokens.h"

#include <utility>
#include <vector>

namespace markamp::ui::accessibility
{

/// Maps foreground theme color tokens to their expected background theme color tokens
/// for the purpose of automated contrast validation.
class AccessibilityTokenMapper
{
public:
    /// Returns a list of foreground-background token pairs to run contrast validation against.
    [[nodiscard]] static auto get_token_pairs()
        -> std::vector<std::pair<core::ThemeColorToken, core::ThemeColorToken>>;
};

} // namespace markamp::ui::accessibility
