/// test_accessibility_manager.cpp
#include "core/AccessibilityManager.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("AccessibilityManager: type compiles", "[accessibility_manager]")
{
    static_assert(sizeof(AccessibilityManager) > 0);
}
