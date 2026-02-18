/// test_block_type_detector.cpp — Unit tests
#include "core/BlockTypeDetector.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("AdvancedBlockType: enum values exist", "[block_type_detector]")
{
    REQUIRE(static_cast<uint8_t>(AdvancedBlockType::kCallout) != static_cast<uint8_t>(AdvancedBlockType::kEmbed));
}

TEST_CASE("BlockDetection: default type is kNone", "[block_type_detector]")
{
    BlockDetection det;
    REQUIRE(det.type == AdvancedBlockType::kNone);
    REQUIRE(det.content.empty());
    REQUIRE(det.metadata.empty());
}
