#pragma once

#include "ControlState.h"
#include "core/Theme.h"

#include <cstdint>

namespace markamp::ui
{

/// Phase 41 Task 01: Control variant enum.
/// Every themed control supports these visual variants for consistent styling across
/// buttons, inputs, dropdowns, and other interactive surfaces.
enum class ControlVariant : uint8_t
{
    kPrimary,   ///< Filled with accent color (main CTA)
    kSecondary, ///< Outlined / subtle fill
    kGhost,     ///< Transparent background, text-only
    kDanger,    ///< Destructive action (red accent)
    kLink,      ///< Looks like a hyperlink (underlined text)
};

/// Phase 41 Task 01: Resolved visual tokens for a control in a given state.
struct ControlVisualTokens
{
    core::ThemeColorToken background{core::ThemeColorToken::BgPanel};
    core::ThemeColorToken foreground{core::ThemeColorToken::TextMain};
    core::ThemeColorToken border{core::ThemeColorToken::BorderLight};
    float opacity{1.0F};
};

/// Phase 41 Task 01: Maps (ControlVariant, ControlStateFlags) to theme color tokens.
///
/// Centralizes the styling logic so every control queries the same resolver instead
/// of duplicating variant×state color lookup tables.
class VariantTokenResolver
{
public:
    /// Resolve visual tokens for a control variant in a given state.
    [[nodiscard]] static auto resolve(ControlVariant variant, ControlStateFlags state)
        -> ControlVisualTokens;

    /// Get the foreground token for a variant (convenience for text rendering).
    [[nodiscard]] static auto foreground_for(ControlVariant variant, ControlStateFlags state)
        -> core::ThemeColorToken;

    /// Get the background token for a variant.
    [[nodiscard]] static auto background_for(ControlVariant variant, ControlStateFlags state)
        -> core::ThemeColorToken;

    /// Get the border token for a variant.
    [[nodiscard]] static auto border_for(ControlVariant variant, ControlStateFlags state)
        -> core::ThemeColorToken;

    /// Get opacity for disabled state.
    [[nodiscard]] static auto opacity_for(ControlStateFlags state) -> float;
};

} // namespace markamp::ui
