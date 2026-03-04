/**
 * @file test_control_motion.cpp
 * @brief Phase 31: Unit tests for ControlMotionTokens and ControlAnimator.
 */

#include "ui/ControlAnimator.h"
#include "ui/ControlMotionTokens.h"

#include <catch2/catch_test_macros.hpp>

#include <set>
#include <string>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// MotionToken — basics
// ═══════════════════════════════════════════════════════

TEST_CASE("MotionToken - easing names", "[motion][tokens]")
{
    MotionToken token;
    token.easing = EasingFunction::kEaseOut;
    CHECK(token.easing_name() == "ease-out");

    token.easing = EasingFunction::kSpring;
    CHECK(token.easing_name() == "spring");

    token.easing = EasingFunction::kBounce;
    CHECK(token.easing_name() == "bounce");
}

TEST_CASE("MotionToken - is instant", "[motion][tokens]")
{
    MotionToken fast;
    fast.duration_ms = 16;
    CHECK(fast.is_instant());

    MotionToken slow;
    slow.duration_ms = 200;
    CHECK_FALSE(slow.is_instant());
}

TEST_CASE("MotionToken - reduced", "[motion][tokens]")
{
    MotionToken token;
    token.duration_ms = 200;
    token.easing = EasingFunction::kEaseOut;
    token.delay_ms = 50;

    auto reduced = token.reduced();
    CHECK(reduced.duration_ms == 0);
    CHECK(reduced.easing == EasingFunction::kLinear);
    CHECK(reduced.delay_ms == 0);
}

// ═══════════════════════════════════════════════════════
// ControlMotionTokens — registry
// ═══════════════════════════════════════════════════════

TEST_CASE("ControlMotionTokens - count", "[motion][tokens]")
{
    CHECK(ControlMotionTokens::count() == 12);
    auto all_tokens = ControlMotionTokens::all();
    CHECK(static_cast<int>(all_tokens.size()) == ControlMotionTokens::count());
}

TEST_CASE("ControlMotionTokens - all have names", "[motion][tokens]")
{
    auto tokens = ControlMotionTokens::all();
    std::set<std::string> names;
    for (const auto& token : tokens)
    {
        CHECK_FALSE(token.name.empty());
        CHECK(names.find(token.name) == names.end());
        names.insert(token.name);
    }
}

TEST_CASE("ControlMotionTokens - all within budget", "[motion][tokens][budget]")
{
    auto tokens = ControlMotionTokens::all();
    for (const auto& token : tokens)
    {
        INFO("Token: " << token.name);
        CHECK(ControlMotionTokens::within_budget(token.duration_ms));
    }
}

TEST_CASE("ControlMotionTokens - over budget detection", "[motion][tokens][budget]")
{
    CHECK_FALSE(ControlMotionTokens::within_budget(600));
    CHECK(ControlMotionTokens::within_budget(500));
    CHECK(ControlMotionTokens::within_budget(100));
}

TEST_CASE("ControlMotionTokens - max budget", "[motion][tokens][budget]")
{
    CHECK(ControlMotionTokens::max_budget_ms() == 500);
}

TEST_CASE("ControlMotionTokens - scaled", "[motion][tokens]")
{
    auto token = ControlMotionTokens::get(MotionTokenId::kFadeIn);
    auto original_ms = token.duration_ms;

    auto half = ControlMotionTokens::scaled(MotionTokenId::kFadeIn, 0.5F);
    CHECK(half.duration_ms == original_ms / 2);

    auto doubled = ControlMotionTokens::scaled(MotionTokenId::kFadeIn, 2.0F);
    CHECK(doubled.duration_ms == original_ms * 2);
}

TEST_CASE("ControlMotionTokens - token names", "[motion][tokens]")
{
    CHECK(ControlMotionTokens::token_name(MotionTokenId::kHoverReveal) == "hover_reveal");
    CHECK(ControlMotionTokens::token_name(MotionTokenId::kSlideIn) == "slide_in");
    CHECK(ControlMotionTokens::token_name(MotionTokenId::kBounceIn) == "bounce_in");
}

TEST_CASE("ControlMotionTokens - reduced motion variants", "[motion][tokens][reduced]")
{
    auto reduced = ControlMotionTokens::reduced_motion_variants();
    CHECK(reduced.size() == 12);
    for (const auto& token : reduced)
    {
        CHECK(token.is_instant());
        CHECK(token.easing == EasingFunction::kLinear);
    }
}

// ═══════════════════════════════════════════════════════
// ControlAnimator — lifecycle
// ═══════════════════════════════════════════════════════

TEST_CASE("ControlAnimator - empty state", "[motion][animator]")
{
    ControlAnimator animator;
    CHECK(animator.active_count() == 0);
    CHECK_FALSE(animator.is_animating("ctrl1"));
    CHECK_FALSE(animator.is_reduced_motion());
    CHECK(animator.motion_scale() == 1.0F);
}

TEST_CASE("ControlAnimator - start and tick", "[motion][animator]")
{
    ControlAnimator animator;
    animator.start("ctrl1", MotionTokenId::kFadeIn);
    CHECK(animator.active_count() == 1);
    CHECK(animator.is_animating("ctrl1"));

    const auto* anim = animator.animation_for("ctrl1");
    REQUIRE(anim != nullptr);
    CHECK(anim->progress == 0.0F);
    CHECK(anim->state == AnimationState::kRunning);

    // Tick halfway
    int half_ms = anim->total_ms / 2;
    animator.tick(half_ms);
    anim = animator.animation_for("ctrl1");
    REQUIRE(anim != nullptr);
    CHECK(anim->progress > 0.4F);
    CHECK(anim->progress < 0.6F);
}

TEST_CASE("ControlAnimator - animation completes", "[motion][animator]")
{
    ControlAnimator animator;
    animator.start("ctrl1", MotionTokenId::kPressResponse);

    // Tick past total duration
    animator.tick(1000);
    CHECK(animator.active_count() == 0);
    CHECK_FALSE(animator.is_animating("ctrl1"));
}

TEST_CASE("ControlAnimator - cancel animation", "[motion][animator]")
{
    ControlAnimator animator;
    animator.start("ctrl1", MotionTokenId::kFadeIn);
    animator.start("ctrl2", MotionTokenId::kSlideIn);
    CHECK(animator.active_count() == 2);

    animator.cancel("ctrl1");
    CHECK(animator.active_count() == 1);
    CHECK_FALSE(animator.is_animating("ctrl1"));
    CHECK(animator.is_animating("ctrl2"));
}

TEST_CASE("ControlAnimator - cancel all", "[motion][animator]")
{
    ControlAnimator animator;
    animator.start("ctrl1", MotionTokenId::kFadeIn);
    animator.start("ctrl2", MotionTokenId::kSlideIn);
    animator.start("ctrl3", MotionTokenId::kBounceIn);

    animator.cancel_all();
    CHECK(animator.active_count() == 0);
}

TEST_CASE("ControlAnimator - callback invoked", "[motion][animator]")
{
    ControlAnimator animator;
    float last_progress = -1.0F;
    animator.start("ctrl1",
                   MotionTokenId::kFadeIn,
                   [&last_progress](const std::string& /*id*/, float progress)
                   { last_progress = progress; });

    animator.tick(50);
    CHECK(last_progress > 0.0F);
}

TEST_CASE("ControlAnimator - reduced motion", "[motion][animator][reduced]")
{
    ControlAnimator animator;
    animator.set_reduced_motion(true);
    CHECK(animator.is_reduced_motion());

    // Animations should complete instantly
    bool callback_called = false;
    animator.start("ctrl1",
                   MotionTokenId::kFadeIn,
                   [&callback_called](const std::string& /*id*/, float progress)
                   {
                       CHECK(progress == 1.0F);
                       callback_called = true;
                   });
    CHECK(callback_called);
    CHECK(animator.active_count() == 0);
}

TEST_CASE("ControlAnimator - reduced motion clears active", "[motion][animator][reduced]")
{
    ControlAnimator animator;
    animator.start("ctrl1", MotionTokenId::kSlideIn);
    animator.start("ctrl2", MotionTokenId::kFadeIn);
    CHECK(animator.active_count() == 2);

    animator.set_reduced_motion(true);
    CHECK(animator.active_count() == 0);
}

TEST_CASE("ControlAnimator - motion scale", "[motion][animator]")
{
    ControlAnimator animator;
    animator.set_motion_scale(2.0F);
    CHECK(animator.motion_scale() == 2.0F);

    animator.start("ctrl1", MotionTokenId::kHoverReveal);
    const auto* anim = animator.animation_for("ctrl1");
    REQUIRE(anim != nullptr);
    // Hover reveal base is 120ms, scaled 2x = 240ms
    CHECK(anim->total_ms == 240);
}

TEST_CASE("ControlAnimator - replace animation on same control", "[motion][animator]")
{
    ControlAnimator animator;
    animator.start("ctrl1", MotionTokenId::kFadeIn);
    CHECK(animator.active_count() == 1);

    animator.start("ctrl1", MotionTokenId::kSlideIn);
    CHECK(animator.active_count() == 1);
    CHECK(animator.is_animating("ctrl1"));
}

// ═══════════════════════════════════════════════════════
// ActiveAnimation — queries
// ═══════════════════════════════════════════════════════

TEST_CASE("ActiveAnimation - remaining ms", "[motion][animator]")
{
    ActiveAnimation anim;
    anim.total_ms = 200;
    anim.elapsed_ms = 50;
    CHECK(anim.remaining_ms() == 150);
}

TEST_CASE("ActiveAnimation - is complete", "[motion][animator]")
{
    ActiveAnimation anim;
    anim.state = AnimationState::kRunning;
    anim.progress = 0.5F;
    CHECK_FALSE(anim.is_complete());

    anim.progress = 1.0F;
    CHECK(anim.is_complete());

    anim.state = AnimationState::kComplete;
    anim.progress = 0.0F;
    CHECK(anim.is_complete());
}
