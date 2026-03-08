/// test_canvas_app_manifest.cpp
#include "core/CanvasAppManifest.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("CanvasAppManifest: type compiles", "[canvas_app_manifest]")
{
    static_assert(sizeof(CanvasAppManifest) > 0);
}
