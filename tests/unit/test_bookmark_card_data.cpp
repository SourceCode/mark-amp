/// test_bookmark_card_data.cpp — Unit tests

#include "canvas/BookmarkCardObject.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("BookmarkMetadata: default values", "[bookmark_card]")
{
    BookmarkMetadata meta;
    REQUIRE(meta.title.empty());
    REQUIRE(meta.description.empty());
    REQUIRE(meta.site_name.empty());
    REQUIRE(meta.favicon_url.empty());
    REQUIRE(meta.image_url.empty());
}

TEST_CASE("BookmarkCardObject: default construction", "[bookmark_card]")
{
    BookmarkCardObject card;
    REQUIRE(card.url().empty());
}
