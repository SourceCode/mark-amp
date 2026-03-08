/// test_template_engine.cpp
#include "core/TemplateEngine.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("TemplateEngine: type compiles", "[template_engine]")
{
    static_assert(sizeof(TemplateEngine) > 0);
}
