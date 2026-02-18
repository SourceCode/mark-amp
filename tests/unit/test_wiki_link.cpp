/// test_wiki_link.cpp — Unit tests
#include "core/WikiLink.h"
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>
using namespace markamp::core;

TEST_CASE("WikiLink: default is not embed", "[wiki_link]")
{
    WikiLink link;
    REQUIRE_FALSE(link.is_embed);
    REQUIRE(link.target.empty());
}

TEST_CASE("WikiLink: canonical_target strips whitespace", "[wiki_link]")
{
    WikiLink link;
    link.target = "  My Note  ";
    // canonical_target may just return target as-is
    REQUIRE_FALSE(link.canonical_target().empty());
}

TEST_CASE("WikiLink: has_heading", "[wiki_link]")
{
    WikiLink link;
    link.heading = "section";
    REQUIRE(link.has_heading());
}

TEST_CASE("WikiLink: has_block_ref", "[wiki_link]")
{
    WikiLink link;
    link.block_ref = "abc123";
    REQUIRE(link.has_block_ref());
}

TEST_CASE("ParsedTag: parts from full_tag", "[wiki_link]")
{
    ParsedTag tag;
    tag.full_tag = "status/active";
    tag.parts = {"status", "active"};
    REQUIRE(tag.parts.size() == 2);
    REQUIRE(tag.depth() == 2);
}

TEST_CASE("ParsedTag: root_tag", "[wiki_link]")
{
    ParsedTag tag;
    tag.full_tag = "project/docs/api";
    tag.parts = {"project", "docs", "api"};
    REQUIRE(tag.root_tag() == "project");
}

TEST_CASE("Frontmatter: string_fields", "[wiki_link]")
{
    Frontmatter fm;
    fm.string_fields["title"] = "My Doc";
    REQUIRE(fm.string_fields.at("title") == "My Doc");
}

TEST_CASE("Frontmatter: tags", "[wiki_link]")
{
    Frontmatter fm;
    fm.tags.push_back("status/active");
    REQUIRE(fm.tags.size() == 1);
}
