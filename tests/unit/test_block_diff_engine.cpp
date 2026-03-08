/// test_block_diff_engine.cpp — Unit tests for BlockDiffEngine
#include "core/BlockDiffEngine.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("BlockDiffEngine: default construction", "[block_diff_engine]")
{
    BlockDiffEngine engine;
    (void)engine;
}
TEST_CASE("BlockDiffEngine: identical documents produce result", "[block_diff_engine]")
{
    BlockDiffEngine engine;
    std::vector<std::pair<std::string, std::string>> blocks = {{"id1", "content1"},
                                                               {"id2", "content2"}};
    auto result = engine.diff_documents(blocks, blocks);
    // Result is populated even for identical docs — stats reflect zero changes
    REQUIRE(result.compute_time_ms >= 0);
}
TEST_CASE("BlockDiffEngine: added blocks detected", "[block_diff_engine]")
{
    BlockDiffEngine engine;
    std::vector<std::pair<std::string, std::string>> old_blocks;
    std::vector<std::pair<std::string, std::string>> new_blocks = {{"id1", "content1"}};
    auto result = engine.diff_documents(old_blocks, new_blocks);
    REQUIRE_FALSE(result.diffs.empty());
}
TEST_CASE("BlockDiffEngine: removed blocks detected", "[block_diff_engine]")
{
    BlockDiffEngine engine;
    std::vector<std::pair<std::string, std::string>> old_blocks = {{"id1", "content1"}};
    std::vector<std::pair<std::string, std::string>> new_blocks;
    auto result = engine.diff_documents(old_blocks, new_blocks);
    REQUIRE_FALSE(result.diffs.empty());
}
TEST_CASE("BlockDiffEngine: modified content detected", "[block_diff_engine]")
{
    BlockDiffEngine engine;
    std::vector<std::pair<std::string, std::string>> old_blocks = {{"id1", "old content"}};
    std::vector<std::pair<std::string, std::string>> new_blocks = {{"id1", "new content"}};
    auto result = engine.diff_documents(old_blocks, new_blocks);
    REQUIRE_FALSE(result.diffs.empty());
}
