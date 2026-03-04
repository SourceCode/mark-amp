#include "ComponentVariants.h"

namespace markamp::ui
{

auto VariantTokenResolver::resolve(ControlVariant variant, ControlStateFlags state)
    -> ControlVisualTokens
{
    return {
        .background = background_for(variant, state),
        .foreground = foreground_for(variant, state),
        .border = border_for(variant, state),
        .opacity = opacity_for(state),
    };
}

auto VariantTokenResolver::foreground_for(ControlVariant variant, ControlStateFlags state)
    -> core::ThemeColorToken
{
    if (has_flag(state, ControlStateFlag::kDisabled))
    {
        return core::ThemeColorToken::ControlFgDisabled;
    }

    switch (variant)
    {
        case ControlVariant::kPrimary:
            return core::ThemeColorToken::AccentSecondary;
        case ControlVariant::kSecondary:
            return core::ThemeColorToken::TextMain;
        case ControlVariant::kGhost:
            return core::ThemeColorToken::TextMain;
        case ControlVariant::kDanger:
            return core::ThemeColorToken::AccentSecondary;
        case ControlVariant::kLink:
            return core::ThemeColorToken::AccentPrimary;
    }
    return core::ThemeColorToken::TextMain;
}

auto VariantTokenResolver::background_for(ControlVariant variant, ControlStateFlags state)
    -> core::ThemeColorToken
{
    if (has_flag(state, ControlStateFlag::kDisabled))
    {
        return core::ThemeColorToken::ControlBgDisabled;
    }

    const bool pressed = has_flag(state, ControlStateFlag::kPressed);
    const bool hover = has_flag(state, ControlStateFlag::kHover);

    switch (variant)
    {
        case ControlVariant::kPrimary:
            if (pressed)
                return core::ThemeColorToken::AccentSecondary;
            if (hover)
                return core::ThemeColorToken::HoverBg;
            return core::ThemeColorToken::AccentPrimary;

        case ControlVariant::kSecondary:
            if (pressed)
                return core::ThemeColorToken::ControlBgPressed;
            if (hover)
                return core::ThemeColorToken::HoverBg;
            return core::ThemeColorToken::BgInput;

        case ControlVariant::kGhost:
            if (pressed)
                return core::ThemeColorToken::ControlBgPressed;
            if (hover)
                return core::ThemeColorToken::HoverBg;
            return core::ThemeColorToken::BgPanel; // transparent-like

        case ControlVariant::kDanger:
            if (pressed)
                return core::ThemeColorToken::ErrorColor;
            if (hover)
                return core::ThemeColorToken::ErrorColor;
            return core::ThemeColorToken::ErrorColor;

        case ControlVariant::kLink:
            return core::ThemeColorToken::BgPanel; // transparent
    }
    return core::ThemeColorToken::BgPanel;
}

auto VariantTokenResolver::border_for(ControlVariant variant, ControlStateFlags state)
    -> core::ThemeColorToken
{
    if (has_flag(state, ControlStateFlag::kFocused))
    {
        return core::ThemeColorToken::FocusRingColor;
    }

    switch (variant)
    {
        case ControlVariant::kPrimary:
            return core::ThemeColorToken::AccentPrimary;
        case ControlVariant::kSecondary:
            return core::ThemeColorToken::BorderLight;
        case ControlVariant::kGhost:
            return core::ThemeColorToken::BgPanel; // no visible border
        case ControlVariant::kDanger:
            return core::ThemeColorToken::ErrorColor;
        case ControlVariant::kLink:
            return core::ThemeColorToken::BgPanel;
    }
    return core::ThemeColorToken::BorderLight;
}

auto VariantTokenResolver::opacity_for(ControlStateFlags state) -> float
{
    if (has_flag(state, ControlStateFlag::kDisabled))
    {
        return 0.5F;
    }
    return 1.0F;
}

} // namespace markamp::ui
