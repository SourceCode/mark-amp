/// test_board_template_data.cpp — Unit tests

#include "canvas/BoardTemplate.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("TemplateInfo: default values", "[board_template]")
{
    TemplateInfo info;
    REQUIRE(info.name.empty());
    REQUIRE(info.description.empty());
}

TEST_CASE("BoardTemplateLibrary: default construction", "[board_template]")
{
    BoardTemplateLibrary lib;
    REQUIRE(lib.all_templates().empty());
}
