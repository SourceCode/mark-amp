#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "canvas/SelectionManager.h"
#include "core/EventBus.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <memory>
#include <vector>

using namespace markamp::canvas;

/// Minimal concrete CanvasObject for testing.
class TestCanvasObj : public CanvasObject
{
public:
    explicit TestCanvasObj(double width = 100.0, double height = 50.0)
        : CanvasObject(CanvasObjectType::Shape)
        , w_(width)
        , h_(height)
    {
    }

    [[nodiscard]] auto local_bounds() const -> AABB override
    {
        return AABB{0.0, 0.0, w_, h_};
    }

    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override
    {
        auto copy = std::make_unique<TestCanvasObj>(w_, h_);
        copy->set_transform(transform());
        copy->set_z_index(z_index());
        return copy;
    }

private:
    double w_;
    double h_;
};

static auto make_event_bus() -> std::shared_ptr<markamp::core::EventBus>
{
    return std::make_shared<markamp::core::EventBus>();
}

// ============================================================================
// Selection State Tests
// ============================================================================

TEST_CASE("SelectionManager single select", "[canvas][selection]")
{
    SelectionManager mgr(make_event_bus());

    mgr.select(1);
    REQUIRE(mgr.is_selected(1));
    REQUIRE(mgr.selection_count() == 1);

    mgr.select(2);
    REQUIRE_FALSE(mgr.is_selected(1));
    REQUIRE(mgr.is_selected(2));
    REQUIRE(mgr.selection_count() == 1);
}

TEST_CASE("SelectionManager add/remove/toggle", "[canvas][selection]")
{
    SelectionManager mgr(make_event_bus());

    mgr.select(1);
    mgr.add_to_selection(2);
    mgr.add_to_selection(3);
    REQUIRE(mgr.selection_count() == 3);
    REQUIRE(mgr.is_selected(1));
    REQUIRE(mgr.is_selected(2));
    REQUIRE(mgr.is_selected(3));

    mgr.remove_from_selection(2);
    REQUIRE(mgr.selection_count() == 2);
    REQUIRE_FALSE(mgr.is_selected(2));

    mgr.toggle_selection(1);
    REQUIRE_FALSE(mgr.is_selected(1));

    mgr.toggle_selection(5);
    REQUIRE(mgr.is_selected(5));
}

TEST_CASE("SelectionManager clear", "[canvas][selection]")
{
    SelectionManager mgr(make_event_bus());

    mgr.select(1);
    mgr.add_to_selection(2);
    REQUIRE(mgr.selection_count() == 2);

    mgr.clear_selection();
    REQUIRE(mgr.selection_count() == 0);
    REQUIRE_FALSE(mgr.is_selected(1));
}

TEST_CASE("SelectionManager select_all", "[canvas][selection]")
{
    SelectionManager mgr(make_event_bus());

    mgr.select_all({10, 20, 30, 40});
    REQUIRE(mgr.selection_count() == 4);
    REQUIRE(mgr.is_selected(10));
    REQUIRE(mgr.is_selected(40));
}

TEST_CASE("SelectionManager selected_ids", "[canvas][selection]")
{
    SelectionManager mgr(make_event_bus());

    mgr.select(5);
    mgr.add_to_selection(10);

    const auto ids = mgr.selected_ids();
    REQUIRE(ids.size() == 2);
}

// ============================================================================
// Selection Bounds
// ============================================================================

TEST_CASE("SelectionManager selection_bounds", "[canvas][selection]")
{
    SelectionManager mgr(make_event_bus());
    std::vector<std::unique_ptr<CanvasObject>> objects;

    auto obj1 = std::make_unique<TestCanvasObj>(100.0, 50.0);
    auto obj2 = std::make_unique<TestCanvasObj>(80.0, 60.0);

    Transform2D transform2;
    transform2.tx = 200.0;
    transform2.ty = 100.0;
    obj2->set_transform(transform2);

    const auto id1 = obj1->id();
    const auto id2 = obj2->id();
    objects.push_back(std::move(obj1));
    objects.push_back(std::move(obj2));

    SECTION("empty selection has no bounds")
    {
        REQUIRE_FALSE(mgr.selection_bounds(objects).has_value());
    }

    SECTION("single-object bounds")
    {
        mgr.select(id1);
        const auto bounds = mgr.selection_bounds(objects);
        REQUIRE(bounds.has_value());
        REQUIRE(bounds->width() > 0.0);
    }

    SECTION("multi-object bounds merges")
    {
        mgr.select(id1);
        mgr.add_to_selection(id2);
        const auto bounds = mgr.selection_bounds(objects);
        REQUIRE(bounds.has_value());
        // Merged should be larger than either individual.
        REQUIRE(bounds->width() > 100.0);
    }
}

// ============================================================================
// Move Transform
// ============================================================================

TEST_CASE("SelectionManager move transform", "[canvas][selection]")
{
    SelectionManager mgr(make_event_bus());
    std::vector<std::unique_ptr<CanvasObject>> objects;

    auto obj = std::make_unique<TestCanvasObj>(100.0, 50.0);
    const auto obj_id = obj->id();
    objects.push_back(std::move(obj));

    mgr.select(obj_id);

    SECTION("move updates position")
    {
        mgr.begin_move(objects);
        REQUIRE(mgr.is_moving());

        mgr.update_move(50.0, 30.0, objects);

        const auto& moved = objects[0];
        REQUIRE(moved->transform().tx == Catch::Approx(50.0));
        REQUIRE(moved->transform().ty == Catch::Approx(30.0));

        mgr.end_move();
        REQUIRE_FALSE(mgr.is_moving());
    }

    SECTION("cancel move restores original")
    {
        const double original_tx = objects[0]->transform().tx;

        mgr.begin_move(objects);
        mgr.update_move(100.0, 200.0, objects);

        REQUIRE(objects[0]->transform().tx == Catch::Approx(100.0));

        mgr.cancel_move(objects);
        REQUIRE(objects[0]->transform().tx == Catch::Approx(original_tx));
        REQUIRE_FALSE(mgr.is_moving());
    }
}

// ============================================================================
// Resize Transform
// ============================================================================

TEST_CASE("SelectionManager resize transform", "[canvas][selection]")
{
    SelectionManager mgr(make_event_bus());
    std::vector<std::unique_ptr<CanvasObject>> objects;

    auto obj = std::make_unique<TestCanvasObj>(100.0, 50.0);
    const auto obj_id = obj->id();
    objects.push_back(std::move(obj));

    mgr.select(obj_id);

    mgr.begin_resize(objects, HandleType::BottomRight);
    REQUIRE(mgr.is_resizing());
    REQUIRE(mgr.active_handle() == HandleType::BottomRight);

    mgr.update_resize(50.0, 0.0, objects);

    mgr.end_resize();
    REQUIRE_FALSE(mgr.is_resizing());
    REQUIRE(mgr.active_handle() == HandleType::None);
}

// ============================================================================
// Rotate Transform
// ============================================================================

TEST_CASE("SelectionManager rotate transform", "[canvas][selection]")
{
    SelectionManager mgr(make_event_bus());
    std::vector<std::unique_ptr<CanvasObject>> objects;

    auto obj = std::make_unique<TestCanvasObj>(100.0, 50.0);
    const auto obj_id = obj->id();
    objects.push_back(std::move(obj));

    mgr.select(obj_id);

    mgr.begin_rotate(objects);
    REQUIRE(mgr.is_rotating());

    const double pi_half = M_PI / 2.0;
    mgr.update_rotate(pi_half, objects);

    REQUIRE(objects[0]->transform().rotation == Catch::Approx(pi_half));

    mgr.end_rotate();
    REQUIRE_FALSE(mgr.is_rotating());
}

// ============================================================================
// Cancel Rotate
// ============================================================================

TEST_CASE("SelectionManager cancel rotate restores original", "[canvas][selection]")
{
    SelectionManager mgr(make_event_bus());
    std::vector<std::unique_ptr<CanvasObject>> objects;

    auto obj = std::make_unique<TestCanvasObj>(100.0, 50.0);
    const auto obj_id = obj->id();
    objects.push_back(std::move(obj));

    mgr.select(obj_id);

    const double original_rot = objects[0]->transform().rotation;

    mgr.begin_rotate(objects);
    mgr.update_rotate(1.5, objects);
    REQUIRE(objects[0]->transform().rotation != original_rot);

    mgr.cancel_rotate(objects);
    REQUIRE(objects[0]->transform().rotation == Catch::Approx(original_rot));
    REQUIRE_FALSE(mgr.is_rotating());
}

// ============================================================================
// HandleType enum
// ============================================================================

TEST_CASE("HandleType enum values", "[canvas][selection]")
{
    REQUIRE(static_cast<uint8_t>(HandleType::TopLeft) == 0);
    REQUIRE(static_cast<uint8_t>(HandleType::BottomRight) == 7);
    REQUIRE(static_cast<uint8_t>(HandleType::Rotation) == 8);
    REQUIRE(static_cast<uint8_t>(HandleType::None) == 9);
}

// ============================================================================
// TransformSnapshot struct
// ============================================================================

TEST_CASE("TransformSnapshot default values", "[canvas][selection]")
{
    const TransformSnapshot snap;
    REQUIRE(snap.id == kInvalidObjectId);
    REQUIRE(snap.transform.tx == 0.0);
    REQUIRE(snap.transform.ty == 0.0);
    REQUIRE(snap.transform.rotation == 0.0);
    REQUIRE(snap.transform.scale_x == 1.0);
}
