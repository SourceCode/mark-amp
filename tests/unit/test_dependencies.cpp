#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include <fmt/format.h>
#include <md4c.h>
#include <spdlog/spdlog.h>

// --- nlohmann/json ---
TEST_CASE("nlohmann/json: parse a JSON string", "[dependencies][json]")
{
    auto j = nlohmann::json::parse(R"({"key": "value", "number": 42})");

    REQUIRE(j["key"] == "value");
    REQUIRE(j["number"] == 42);
}

// --- fmt ---
TEST_CASE("fmt: format a string", "[dependencies][fmt]")
{
    auto result = fmt::format("Hello, {}! The answer is {}.", "world", 42);

    REQUIRE(result == "Hello, world! The answer is 42.");
}

// --- spdlog ---
TEST_CASE("spdlog: create a logger", "[dependencies][spdlog]")
{
    auto logger = spdlog::default_logger();

    REQUIRE(logger != nullptr);
    // Just verify we can log without crashing
    spdlog::info("Dependency test: spdlog is functional");
}

// --- md4c ---
// md4c no-op callbacks: md4c dereferences all callback pointers, they must be non-null
static int
md4cBlockCallback([[maybe_unused]] MD_BLOCKTYPE type, [[maybe_unused]] void* detail, void* userdata)
{
    auto* count = static_cast<int*>(userdata);
    (*count)++;
    return 0;
}

static int md4cSpanCallback([[maybe_unused]] MD_SPANTYPE type,
                            [[maybe_unused]] void* detail,
                            [[maybe_unused]] void* userdata)
{
    return 0;
}

static int md4cTextCallback([[maybe_unused]] MD_TEXTTYPE type,
                            [[maybe_unused]] const MD_CHAR* text,
                            [[maybe_unused]] MD_SIZE size,
                            [[maybe_unused]] void* userdata)
{
    return 0;
}

TEST_CASE("md4c: parse trivial markdown", "[dependencies][md4c]")
{
    const char* markdown = "# Hello\n\nThis is a paragraph.\n";
    int blockCount = 0;

    MD_PARSER parser = {};
    parser.abi_version = 0;
    parser.enter_block = md4cBlockCallback;
    parser.leave_block = md4cBlockCallback;
    parser.enter_span = md4cSpanCallback;
    parser.leave_span = md4cSpanCallback;
    parser.text = md4cTextCallback;

    int result = md_parse(markdown, static_cast<MD_SIZE>(strlen(markdown)), &parser, &blockCount);

    REQUIRE(result == 0);
    REQUIRE(blockCount > 0); // At least the heading and paragraph blocks
}
