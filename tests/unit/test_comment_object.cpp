/// test_comment_object.cpp — Unit tests
#include "canvas/CommentObject.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("CommentPriority: enum values", "[comment_object]")
{
    REQUIRE(static_cast<int>(CommentPriority::kNormal) != static_cast<int>(CommentPriority::kHigh));
}

TEST_CASE("CommentPriority: compiles", "[comment_object]")
{
    static_assert(sizeof(CommentPriority) > 0);
}

TEST_CASE("CommentObject: compiles", "[comment_object]")
{
    static_assert(sizeof(CommentObject) > 0);
}
