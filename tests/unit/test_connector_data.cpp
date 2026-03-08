/// test_connector_data.cpp
#include "canvas/ConnectorData.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;

TEST_CASE("AnchorPosition: enum values exist", "[connector_data]")
{
    REQUIRE(AnchorPosition::kTop != AnchorPosition::kBottom);
}
