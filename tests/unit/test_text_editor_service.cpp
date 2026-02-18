/// test_text_editor_service.cpp — Unit tests

#include "core/TextEditorService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TextPosition: default values", "[text_editor_service]")
{
    TextPosition pos;
    REQUIRE(pos.line == 0);
    REQUIRE(pos.character == 0);
}

TEST_CASE("TextRange: from positions", "[text_editor_service]")
{
    TextRange range;
    range.start = {0, 0};
    range.end = {0, 5};
    REQUIRE(range.end.character == 5);
}

TEST_CASE("TextSelection: from range", "[text_editor_service]")
{
    TextSelection sel;
    sel.anchor = {0, 0};
    sel.active = {1, 3};
    REQUIRE(sel.active.line == 1);
}

TEST_CASE("EditBuilder: default construction", "[text_editor_service]")
{
    EditBuilder builder;
    (void)builder;
}
