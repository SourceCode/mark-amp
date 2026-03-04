#include "ControlMotionTokens.h"

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

// ── MotionToken ─────────────────────────────────────────────────────

auto MotionToken::reduced() const -> MotionToken
{
    MotionToken token = *this;
    // Reduced motion: instant or very short, linear easing
    if (duration_ms > 16)
    {
        token.duration_ms = 0;
    }
    token.easing = EasingFunction::kLinear;
    token.delay_ms = 0;
    return token;
}

auto MotionToken::easing_name() const -> std::string
{
    switch (easing)
    {
        case EasingFunction::kLinear:
            return "linear";
        case EasingFunction::kEaseIn:
            return "ease-in";
        case EasingFunction::kEaseOut:
            return "ease-out";
        case EasingFunction::kEaseInOut:
            return "ease-in-out";
        case EasingFunction::kSpring:
            return "spring";
        case EasingFunction::kBounce:
            return "bounce";
    }
    return "unknown";
}

auto MotionToken::is_instant() const -> bool
{
    return duration_ms <= 16;
}

// ── ControlMotionTokens ─────────────────────────────────────────────

auto ControlMotionTokens::get(MotionTokenId token_id) -> MotionToken
{
    switch (token_id)
    {
        case MotionTokenId::kHoverReveal:
            return {"hover_reveal", 120, EasingFunction::kEaseOut, 0};
        case MotionTokenId::kHoverDismiss:
            return {"hover_dismiss", 80, EasingFunction::kEaseIn, 0};
        case MotionTokenId::kPressResponse:
            return {"press_response", 50, EasingFunction::kEaseOut, 0};
        case MotionTokenId::kPressRelease:
            return {"press_release", 100, EasingFunction::kEaseOut, 0};
        case MotionTokenId::kFadeIn:
            return {"fade_in", 200, EasingFunction::kEaseOut, 0};
        case MotionTokenId::kFadeOut:
            return {"fade_out", 150, EasingFunction::kEaseIn, 0};
        case MotionTokenId::kSlideIn:
            return {"slide_in", 250, EasingFunction::kEaseInOut, 0};
        case MotionTokenId::kSlideOut:
            return {"slide_out", 200, EasingFunction::kEaseIn, 0};
        case MotionTokenId::kBounceIn:
            return {"bounce_in", 400, EasingFunction::kBounce, 0};
        case MotionTokenId::kPopIn:
            return {"pop_in", 180, EasingFunction::kSpring, 0};
        case MotionTokenId::kDismiss:
            return {"dismiss", 100, EasingFunction::kEaseIn, 0};
        case MotionTokenId::kFocusRing:
            return {"focus_ring", 80, EasingFunction::kEaseOut, 0};
    }
    return {"unknown", 200, EasingFunction::kEaseOut, 0};
}

auto ControlMotionTokens::all() -> std::vector<MotionToken>
{
    return {get(MotionTokenId::kHoverReveal),
            get(MotionTokenId::kHoverDismiss),
            get(MotionTokenId::kPressResponse),
            get(MotionTokenId::kPressRelease),
            get(MotionTokenId::kFadeIn),
            get(MotionTokenId::kFadeOut),
            get(MotionTokenId::kSlideIn),
            get(MotionTokenId::kSlideOut),
            get(MotionTokenId::kBounceIn),
            get(MotionTokenId::kPopIn),
            get(MotionTokenId::kDismiss),
            get(MotionTokenId::kFocusRing)};
}

auto ControlMotionTokens::count() -> int
{
    return 12;
}

auto ControlMotionTokens::reduced_motion_variants() -> std::vector<MotionToken>
{
    auto tokens = all();
    std::vector<MotionToken> reduced;
    reduced.reserve(tokens.size());
    for (const auto& token : tokens)
    {
        reduced.push_back(token.reduced());
    }
    return reduced;
}

auto ControlMotionTokens::scaled(MotionTokenId token_id, float scale) -> MotionToken
{
    auto token = get(token_id);
    token.duration_ms = static_cast<int>(std::round(static_cast<float>(token.duration_ms) * scale));
    token.delay_ms = static_cast<int>(std::round(static_cast<float>(token.delay_ms) * scale));
    return token;
}

auto ControlMotionTokens::token_name(MotionTokenId token_id) -> std::string
{
    return get(token_id).name;
}

auto ControlMotionTokens::within_budget(int duration_ms) -> bool
{
    return duration_ms <= max_budget_ms();
}

auto ControlMotionTokens::max_budget_ms() -> int
{
    return 500; // 500ms maximum for any control motion
}

} // namespace markamp::ui
