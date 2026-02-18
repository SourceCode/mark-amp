/// test_template_types.cpp — Unit tests

#include "core/TemplateTypes.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TemplateTypes: compiles and has types", "[template_types]")
{
    static_assert(sizeof(TemplateVariable) > 0);
}
