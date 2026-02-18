/// @file fuzz_board_serializer.cpp
/// Fuzz testing stub for BoardSerializer (Phase 34D, PRD Task 17).
///
/// Ensures the board serializer never crashes on malformed JSON input.
/// In a CI environment with libFuzzer, this file would be compiled
/// with -fsanitize=fuzzer. For now, it runs as a Catch2 test with
/// pre-defined edge-case inputs (seed corpus).

#include "canvas/Board.h"
#include "canvas/BoardSerializer.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::canvas;

namespace
{

/// Seed corpus of malformed/edge-case JSON inputs.
auto seed_corpus() -> const std::vector<std::string>&
{
    static const std::vector<std::string> corpus = {
        "",
        "{}",
        "null",
        "[]",
        "\"string\"",
        "42",
        "true",
        "false",
        "{\"objects\":[]}",
        "{\"objects\":null}",
        "{\"objects\":\"not_array\"}",
        "{\"metadata\":{},\"objects\":[]}",
        "{\"metadata\":{\"name\":\"test\"},\"objects\":[]}",
        "{\"objects\":[{\"type\":999}]}",
        "{\"objects\":[{\"type\":0,\"x\":0,\"y\":0}]}",
        "not json at all",
        "{{{",
        "[[[",
        "}}}",
        "]]]",
        std::string(10000, '{'),
        std::string(10000, '['),
        "{\"objects\":[" + std::string(100, ',') + "]}",
        "{\"objects\":[{\"type\":1,\"content\":\"" + std::string(10000, 'x') + "\"}]}",
        "{\"version\":-1}",
        "{\"version\":999999}",
        "{\"objects\":[{\"type\":1,\"x\":1e308,\"y\":-1e308}]}",
        "{\"objects\":[{\"type\":1,\"x\":\"NaN\",\"y\":\"Infinity\"}]}",
    };
    return corpus;
}

} // namespace

TEST_CASE("Fuzz: BoardSerializer does not crash on malformed JSON", "[fuzz][board]")
{
    BoardSerializer serializer;

    for (const auto& input : seed_corpus())
    {
        try
        {
            auto board = serializer.deserialize(input);
            (void)board;
        }
        catch (...)
        {
            // Exceptions are acceptable — crashes are not
        }
    }
    REQUIRE(true); // Reached — no crash
}

TEST_CASE("Fuzz: BoardSerializer handles binary-like JSON", "[fuzz][board]")
{
    BoardSerializer serializer;

    std::string binary_input = "{\"objects\":[";
    for (int idx = 0; idx < 256; ++idx)
    {
        binary_input.push_back(static_cast<char>(idx));
    }
    binary_input += "]}";

    try
    {
        auto board = serializer.deserialize(binary_input);
        (void)board;
    }
    catch (...)
    {
        // Acceptable
    }
    REQUIRE(true);
}

TEST_CASE("Fuzz: BoardSerializer round-trip is safe", "[fuzz][board]")
{
    Board board;
    BoardSerializer serializer;

    // Serialize valid board — guaranteed to be valid JSON
    auto json = serializer.serialize(board);
    REQUIRE(!json.empty());

    // Deserialize should not crash
    auto restored = serializer.deserialize(json);
    REQUIRE(restored.object_count() == 0);
}
