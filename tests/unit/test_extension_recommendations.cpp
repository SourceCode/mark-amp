/// test_extension_recommendations.cpp — Unit tests

#include "core/ExtensionRecommendations.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ExtensionRecommendations: default construction", "[extension_recommendations]")
{
    ExtensionRecommendations recs;
    (void)recs;
}
