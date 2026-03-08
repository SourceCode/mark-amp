/// test_frame_scheduler.cpp — Unit tests for FrameScheduler
#include "core/FrameScheduler.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("FrameScheduler: type compiles", "[frame_scheduler]")
{
    static_assert(sizeof(FrameScheduler) > 0);
}
