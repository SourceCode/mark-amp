/// test_notification_service.cpp
#include "core/NotificationService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("NotificationService: type compiles", "[notification_service]")
{
    static_assert(sizeof(NotificationService) > 0);
}
