/// test_connector_data.cpp — Unit tests

#include "canvas/ConnectorData.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("AnchorPosition: enum values", "[connector_data]")
{
    REQUIRE(static_cast<uint8_t>(AnchorPosition::kTop) != static_cast<uint8_t>(AnchorPosition::kBottom));
}

TEST_CASE("ArrowheadStyle: enum values", "[connector_data]")
{
    REQUIRE(static_cast<uint8_t>(ArrowheadStyle::kNone) != static_cast<uint8_t>(ArrowheadStyle::kArrow));
}

TEST_CASE("ConnectorRouting: enum values", "[connector_data]")
{
    REQUIRE(static_cast<uint8_t>(ConnectorRouting::kStraight) != static_cast<uint8_t>(ConnectorRouting::kOrthogonal));
}
