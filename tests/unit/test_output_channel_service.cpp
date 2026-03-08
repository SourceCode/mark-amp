/// test_output_channel_service.cpp
#include "core/OutputChannelService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("OutputChannelService: type compiles", "[output_channel_service]")
{
    static_assert(sizeof(OutputChannelService) > 0);
}
