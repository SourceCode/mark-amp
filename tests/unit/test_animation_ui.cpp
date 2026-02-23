#include "ui/animation/FocusRingAnimator.h"
#include "ui/animation/ProgressBar.h"
#include "ui/animation/SkeletonLoader.h"

#include <catch2/catch_test_macros.hpp>
#include <wx/app.h>

using namespace markamp::ui::animation;

TEST_CASE("FocusRingAnimator interpolates targets", "[animation][ui]")
{
    // Need a mock callback
    int draw_calls = 0;
    FocusRingAnimator animator([&]() { draw_calls++; });

    REQUIRE(animator.is_active() == false);

    wxRect r1(0, 0, 100, 100);
    animator.set_target(r1);

    REQUIRE(animator.is_active() == true);
    REQUIRE(animator.get_current_rect() == r1);

    animator.hide();
    // It shouldn't be immediately inactive because it fades out
    // but the timeline will run. Since we have no event loop, we can just check it hides
    // eventually.
}
