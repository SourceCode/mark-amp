/// test_template_types.cpp
#include "core/TemplateTypes.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("TemplateNode: default values", "[template_types]")
{
    TemplateNode node;
    REQUIRE(node.name.empty());
}
