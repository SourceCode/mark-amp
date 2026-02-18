/// test_notification_service.cpp — Unit tests
#include "core/NotificationService.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("NotificationOptions: default values", "[notification_service]")
{
    NotificationOptions opts;
    REQUIRE(opts.message.empty());
}

TEST_CASE("NotificationService: compiles", "[notification_service]")
{
    static_assert(sizeof(NotificationService) > 0);
}
