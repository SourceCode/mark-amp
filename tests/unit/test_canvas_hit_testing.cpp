// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/HitTestModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Priority resolution handle over object", "[hittest][priority]")
{
    HitTestModel model;
    model.set_candidates({
        {"obj-1", HitTarget::kObject, 5.0},
        {"handle-1", HitTarget::kHandle, 8.0},
    });
    const auto winner = model.resolve();
    REQUIRE(winner.object_id == "handle-1"); // handle wins despite further distance
}

TEST_CASE("Distance tiebreak within same priority", "[hittest][distance]")
{
    HitTestModel model;
    model.set_candidates({
        {"obj-1", HitTarget::kObject, 10.0},
        {"obj-2", HitTarget::kObject, 3.0},
    });
    const auto winner = model.resolve();
    REQUIRE(winner.object_id == "obj-2"); // closer wins
}

TEST_CASE("Empty candidates return canvas", "[hittest][empty]")
{
    HitTestModel model;
    const auto winner = model.resolve();
    REQUIRE(winner.target_type == HitTarget::kCanvas);
}

TEST_CASE("Tolerance scales inversely with zoom", "[hittest][tolerance]")
{
    HitTestModel model;
    model.set_base_tolerance(4.0);
    REQUIRE(model.tolerance_at_zoom(1.0) == 4.0);
    REQUIRE(model.tolerance_at_zoom(0.5) == 8.0);
    REQUIRE(model.tolerance_at_zoom(2.0) == 2.0);
}

TEST_CASE("Prefetch candidates", "[hittest][prefetch]")
{
    HitTestModel model;
    model.set_prefetch_candidates({"obj-1", "obj-2"});
    REQUIRE(model.prefetch_candidates().size() == 2);
}

TEST_CASE("Latency budget check", "[hittest][latency]")
{
    HitTestModel model;
    model.set_latency_ms(12.0);
    REQUIRE(model.is_within_budget(16.0));
    REQUIRE_FALSE(model.is_within_budget(8.0));
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
