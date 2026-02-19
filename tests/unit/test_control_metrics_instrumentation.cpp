// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/UxMetricsModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("Record and count events", "[metrics][record]")
{
    UxMetricsModel model;
    model.record({"e1", MetricType::kCommandLatency, 50.0, "editor"});
    model.record({"e2", MetricType::kCommandLatency, 30.0, "editor"});
    model.record({"e3", MetricType::kMisclickRate, 0.05, "toolbar"});

    REQUIRE(model.events().size() == 3);
    REQUIRE(model.count_by_type(MetricType::kCommandLatency) == 2);
    REQUIRE(model.count_by_type(MetricType::kMisclickRate) == 1);
}

TEST_CASE("Average by type", "[metrics][aggregate]")
{
    UxMetricsModel model;
    model.record({"e1", MetricType::kCommandLatency, 40.0, "editor"});
    model.record({"e2", MetricType::kCommandLatency, 60.0, "editor"});

    REQUIRE(model.average_by_type(MetricType::kCommandLatency) == 50.0);
    REQUIRE(model.average_by_type(MetricType::kMisclickRate) == 0.0);
}

TEST_CASE("Telemetry off blocks emission", "[metrics][telemetry]")
{
    UxMetricsModel model;
    REQUIRE_FALSE(model.telemetry_enabled());
    REQUIRE_FALSE(model.should_emit(MetricType::kCommandLatency));
}

TEST_CASE("Telemetry on allows emission", "[metrics][telemetry]")
{
    UxMetricsModel model;
    model.set_telemetry_enabled(true);
    REQUIRE(model.should_emit(MetricType::kCommandLatency));
}

TEST_CASE("Per-metric exclusion", "[metrics][telemetry]")
{
    UxMetricsModel model;
    model.set_telemetry_enabled(true);
    model.exclude_metric(MetricType::kMisclickRate);
    REQUIRE(model.should_emit(MetricType::kCommandLatency));
    REQUIRE_FALSE(model.should_emit(MetricType::kMisclickRate));
    model.include_metric(MetricType::kMisclickRate);
    REQUIRE(model.should_emit(MetricType::kMisclickRate));
}

TEST_CASE("Clear events", "[metrics][record]")
{
    UxMetricsModel model;
    model.record({"e1", MetricType::kCommandLatency, 50.0, "editor"});
    model.clear_events();
    REQUIRE(model.events().empty());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
