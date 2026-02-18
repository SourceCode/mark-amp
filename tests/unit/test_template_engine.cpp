/// test_template_engine.cpp — Unit tests

#include "core/TemplateEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TemplateEngine: compiles", "[template_engine]")
{
    static_assert(sizeof(TemplateEngine) > 0);
}
