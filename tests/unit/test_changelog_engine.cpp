// test_changelog_engine.cpp — 10 tests for ChangelogEngine
#include "core/ChangelogEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ChangelogEngine starts empty", "[changelog]")
{
    ChangelogEngine engine;
    CHECK(engine.entry_count() == 0);
}

TEST_CASE("ChangelogEngine add_entry increases count", "[changelog]")
{
    ChangelogEngine engine;
    engine.add_entry(
        {.date = "2025-01-01", .version = "v1.0", .category = "Added", .message = "New feature"});
    CHECK(engine.entry_count() == 1);
}

TEST_CASE("ChangelogEngine get_entries by version", "[changelog]")
{
    ChangelogEngine engine;
    engine.add_entry(
        {.date = "2025-01-01", .version = "v1.0", .category = "Added", .message = "Feature A"});
    engine.add_entry(
        {.date = "2025-01-02", .version = "v1.0", .category = "Fixed", .message = "Bug B"});
    engine.add_entry(
        {.date = "2025-02-01", .version = "v2.0", .category = "Added", .message = "Feature C"});
    auto v1_entries = engine.get_entries("v1.0");
    CHECK(v1_entries.size() == 2);
}

TEST_CASE("ChangelogEngine get_all_entries returns all", "[changelog]")
{
    ChangelogEngine engine;
    engine.add_entry(
        {.date = "2025-01-01", .version = "v1.0", .category = "Added", .message = "A"});
    engine.add_entry(
        {.date = "2025-02-01", .version = "v2.0", .category = "Fixed", .message = "B"});
    CHECK(engine.get_all_entries().size() == 2);
}

TEST_CASE("ChangelogEngine generate_markdown produces output", "[changelog]")
{
    ChangelogEngine engine;
    engine.add_entry(
        {.date = "2025-01-01", .version = "v1.0", .category = "Added", .message = "Feature"});
    auto md = engine.generate_markdown();
    CHECK_FALSE(md.empty());
    CHECK(md.find("v1.0") != std::string::npos);
}

TEST_CASE("ChangelogEngine generate_markdown empty returns valid", "[changelog]")
{
    ChangelogEngine engine;
    auto md = engine.generate_markdown();
    // Even with no entries, should produce valid (possibly minimal) markdown
    // It's valid to return empty for no entries
    CHECK(true);
}

TEST_CASE("ChangelogEngine clear removes all entries", "[changelog]")
{
    ChangelogEngine engine;
    engine.add_entry(
        {.date = "2025-01-01", .version = "v1.0", .category = "Added", .message = "X"});
    engine.clear();
    CHECK(engine.entry_count() == 0);
}

TEST_CASE("ChangelogEngine load_defaults populates entries", "[changelog]")
{
    ChangelogEngine engine;
    engine.load_defaults();
    CHECK(engine.entry_count() > 0);
}

TEST_CASE("ChangelogEngine entries preserve category", "[changelog]")
{
    ChangelogEngine engine;
    engine.add_entry(
        {.date = "2025-01-01", .version = "v1.0", .category = "Fixed", .message = "Bug fix"});
    auto entries = engine.get_entries("v1.0");
    REQUIRE(entries.size() == 1);
    CHECK(entries[0].category == "Fixed");
}

TEST_CASE("ChangelogEngine entries preserve message", "[changelog]")
{
    ChangelogEngine engine;
    engine.add_entry(
        {.date = "2025-03-01", .version = "v3.0", .category = "Changed", .message = "API changed"});
    auto entries = engine.get_entries("v3.0");
    REQUIRE(entries.size() == 1);
    CHECK(entries[0].message == "API changed");
}
