/// test_a_i_panel.cpp — Unit tests for AIPanel
#include "core/AIPanel.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("AIPanel: type compiles", "[ai_panel]")
{
    static_assert(sizeof(AIPanel) > 0);
}
