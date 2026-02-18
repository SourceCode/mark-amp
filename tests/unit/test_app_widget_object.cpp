/// test_app_widget_object.cpp — Unit tests
#include "canvas/AppWidgetObject.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("WidgetSyncStatus: enum values", "[app_widget_object]")
{
    REQUIRE(static_cast<int>(WidgetSyncStatus::kIdle) != static_cast<int>(WidgetSyncStatus::kSyncing));
}

TEST_CASE("AppWidgetObject: compiles", "[app_widget_object]")
{
    static_assert(sizeof(AppWidgetObject) > 0);
}
