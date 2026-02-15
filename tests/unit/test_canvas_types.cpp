#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cmath>
#include <numbers>

using namespace markamp::canvas;

// ============================================================================
// Concrete test object for testing CanvasObject base class
// ============================================================================

class TestCanvasObject : public CanvasObject
{
public:
    TestCanvasObject()
        : CanvasObject(CanvasObjectType::StickyNote)
    {
    }

    TestCanvasObject(double width, double height)
        : CanvasObject(CanvasObjectType::StickyNote)
        , width_(width)
        , height_(height)
    {
    }

    [[nodiscard]] auto local_bounds() const -> AABB override
    {
        return {0.0, 0.0, width_, height_};
    }

    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override
    {
        auto copy = std::make_unique<TestCanvasObject>(width_, height_);
        copy->set_name(name());
        copy->set_transform(transform());
        copy->set_z_index(z_index());
        return copy;
    }

private:
    double width_{100.0};
    double height_{80.0};
};

// ============================================================================
// Point2D tests
// ============================================================================

TEST_CASE("Point2D basic operations", "[canvas][types]")
{
    SECTION("default construction")
    {
        const Point2D point;
        REQUIRE(point.x == 0.0);
        REQUIRE(point.y == 0.0);
    }

    SECTION("addition")
    {
        const Point2D point_a{1.0, 2.0};
        const Point2D point_b{3.0, 4.0};
        const auto result = point_a + point_b;
        REQUIRE(result.x == 4.0);
        REQUIRE(result.y == 6.0);
    }

    SECTION("subtraction")
    {
        const Point2D point_a{5.0, 8.0};
        const Point2D point_b{3.0, 4.0};
        const auto result = point_a - point_b;
        REQUIRE(result.x == 2.0);
        REQUIRE(result.y == 4.0);
    }

    SECTION("scalar multiplication")
    {
        const Point2D point{2.0, 3.0};
        const auto result = point * 2.5;
        REQUIRE_THAT(result.x, Catch::Matchers::WithinAbs(5.0, 1e-9));
        REQUIRE_THAT(result.y, Catch::Matchers::WithinAbs(7.5, 1e-9));
    }

    SECTION("distance_to")
    {
        const Point2D point_a{0.0, 0.0};
        const Point2D point_b{3.0, 4.0};
        REQUIRE_THAT(point_a.distance_to(point_b), Catch::Matchers::WithinAbs(5.0, 1e-9));
    }

    SECTION("equality")
    {
        const Point2D point_a{1.0, 2.0};
        const Point2D point_b{1.0, 2.0};
        REQUIRE(point_a == point_b);
    }
}

// ============================================================================
// Transform2D tests
// ============================================================================

TEST_CASE("Transform2D apply and inverse", "[canvas][types]")
{
    SECTION("identity transform preserves point")
    {
        const auto identity = Transform2D::identity();
        const Point2D point{42.0, 17.0};
        const auto result = identity.apply(point);
        REQUIRE_THAT(result.x, Catch::Matchers::WithinAbs(42.0, 1e-9));
        REQUIRE_THAT(result.y, Catch::Matchers::WithinAbs(17.0, 1e-9));
    }

    SECTION("translation only")
    {
        Transform2D xform;
        xform.tx = 10.0;
        xform.ty = 20.0;
        const Point2D point{5.0, 5.0};
        const auto result = xform.apply(point);
        REQUIRE_THAT(result.x, Catch::Matchers::WithinAbs(15.0, 1e-9));
        REQUIRE_THAT(result.y, Catch::Matchers::WithinAbs(25.0, 1e-9));
    }

    SECTION("90 degree rotation")
    {
        Transform2D xform;
        xform.rotation = std::numbers::pi / 2.0;
        const Point2D point{1.0, 0.0};
        const auto result = xform.apply(point);
        REQUIRE_THAT(result.x, Catch::Matchers::WithinAbs(0.0, 1e-9));
        REQUIRE_THAT(result.y, Catch::Matchers::WithinAbs(1.0, 1e-9));
    }

    SECTION("apply then inverse roundtrips")
    {
        Transform2D xform;
        xform.tx = 100.0;
        xform.ty = -50.0;
        xform.rotation = std::numbers::pi / 4.0;
        xform.scale_x = 2.0;
        xform.scale_y = 1.5;

        const Point2D original{42.0, 17.0};
        const auto transformed = xform.apply(original);
        const auto recovered = xform.inverse(transformed);

        REQUIRE_THAT(recovered.x, Catch::Matchers::WithinAbs(original.x, 1e-6));
        REQUIRE_THAT(recovered.y, Catch::Matchers::WithinAbs(original.y, 1e-6));
    }

    SECTION("compose produces same result as sequential apply")
    {
        Transform2D xform_a;
        xform_a.tx = 10.0;
        xform_a.ty = 5.0;
        xform_a.rotation = 0.3;

        Transform2D xform_b;
        xform_b.tx = -3.0;
        xform_b.ty = 7.0;
        xform_b.scale_x = 2.0;
        xform_b.scale_y = 2.0;

        const auto composed = xform_a.compose(xform_b);
        const Point2D point{1.0, 1.0};

        const auto sequential = xform_a.apply(xform_b.apply(point));
        const auto composed_result = composed.apply(point);

        REQUIRE_THAT(composed_result.x, Catch::Matchers::WithinAbs(sequential.x, 1e-6));
        REQUIRE_THAT(composed_result.y, Catch::Matchers::WithinAbs(sequential.y, 1e-6));
    }
}

// ============================================================================
// AABB tests
// ============================================================================

TEST_CASE("AABB geometry operations", "[canvas][types]")
{
    SECTION("contains point")
    {
        const AABB box{0.0, 0.0, 10.0, 10.0};
        REQUIRE(box.contains({5.0, 5.0}));
        REQUIRE(box.contains({0.0, 0.0}));
        REQUIRE(box.contains({10.0, 10.0}));
        REQUIRE_FALSE(box.contains({-1.0, 5.0}));
        REQUIRE_FALSE(box.contains({5.0, 11.0}));
    }

    SECTION("intersects")
    {
        const AABB box_a{0.0, 0.0, 10.0, 10.0};
        const AABB box_b{5.0, 5.0, 15.0, 15.0};
        const AABB box_c{20.0, 20.0, 30.0, 30.0};

        REQUIRE(box_a.intersects(box_b));
        REQUIRE(box_b.intersects(box_a));
        REQUIRE_FALSE(box_a.intersects(box_c));
    }

    SECTION("merged")
    {
        const AABB box_a{0.0, 0.0, 5.0, 5.0};
        const AABB box_b{3.0, 3.0, 10.0, 10.0};
        const auto result = box_a.merged(box_b);

        REQUIRE(result.min_x == 0.0);
        REQUIRE(result.min_y == 0.0);
        REQUIRE(result.max_x == 10.0);
        REQUIRE(result.max_y == 10.0);
    }

    SECTION("expanded")
    {
        const AABB box{5.0, 5.0, 15.0, 15.0};
        const auto result = box.expanded(2.0);

        REQUIRE(result.min_x == 3.0);
        REQUIRE(result.min_y == 3.0);
        REQUIRE(result.max_x == 17.0);
        REQUIRE(result.max_y == 17.0);
    }

    SECTION("from_center_size")
    {
        const auto box = AABB::from_center_size({10.0, 10.0}, {20.0, 10.0});
        REQUIRE(box.min_x == 0.0);
        REQUIRE(box.min_y == 5.0);
        REQUIRE(box.max_x == 20.0);
        REQUIRE(box.max_y == 15.0);
    }

    SECTION("center and dimensions")
    {
        const AABB box{0.0, 0.0, 10.0, 20.0};
        const auto center = box.center();
        REQUIRE(center.x == 5.0);
        REQUIRE(center.y == 10.0);
        REQUIRE(box.width() == 10.0);
        REQUIRE(box.height() == 20.0);
    }

    SECTION("is_valid")
    {
        REQUIRE(AABB{0.0, 0.0, 10.0, 10.0}.is_valid());
        REQUIRE(AABB{5.0, 5.0, 5.0, 5.0}.is_valid()); // zero-area is valid
        REQUIRE_FALSE(AABB{}.is_valid());             // default-constructed is invalid
    }
}

// ============================================================================
// CanvasColor tests
// ============================================================================

TEST_CASE("CanvasColor hex parsing and formatting", "[canvas][types]")
{
    SECTION("from_hex 6 chars")
    {
        const auto color = CanvasColor::from_hex("#ff8040");
        REQUIRE(color.r == 0xff);
        REQUIRE(color.g == 0x80);
        REQUIRE(color.b == 0x40);
        REQUIRE(color.a == 255);
    }

    SECTION("from_hex 8 chars with alpha")
    {
        const auto color = CanvasColor::from_hex("#ff804080");
        REQUIRE(color.r == 0xff);
        REQUIRE(color.g == 0x80);
        REQUIRE(color.b == 0x40);
        REQUIRE(color.a == 0x80);
    }

    SECTION("to_hex roundtrip")
    {
        const auto original = CanvasColor::from_hex("#abcdef");
        const auto hex = original.to_hex();
        REQUIRE(hex == "#abcdef");
    }

    SECTION("invalid hex returns black")
    {
        const auto color = CanvasColor::from_hex("xyz");
        REQUIRE(color.r == 0);
        REQUIRE(color.g == 0);
        REQUIRE(color.b == 0);
        REQUIRE(color.a == 255);
    }
}

// ============================================================================
// CanvasObject base class tests
// ============================================================================

TEST_CASE("CanvasObject base class", "[canvas][object]")
{
    SECTION("unique monotonic IDs")
    {
        TestCanvasObject obj_a;
        TestCanvasObject obj_b;
        REQUIRE(obj_a.id() != obj_b.id());
        REQUIRE(obj_b.id() > obj_a.id());
    }

    SECTION("default properties")
    {
        TestCanvasObject obj;
        REQUIRE(obj.type() == CanvasObjectType::StickyNote);
        REQUIRE(obj.is_visible());
        REQUIRE_FALSE(obj.is_locked());
        REQUIRE(obj.opacity() == 1.0);
        REQUIRE(obj.z_index() == 0);
        REQUIRE(obj.parent_id() == kInvalidObjectId);
        REQUIRE(obj.name().empty());
    }

    SECTION("set / get properties")
    {
        TestCanvasObject obj;
        obj.set_name("Test Note");
        obj.set_z_index(5);
        obj.set_locked(true);
        obj.set_visible(false);
        obj.set_opacity(0.5);
        obj.set_parent_id(42);

        REQUIRE(obj.name() == "Test Note");
        REQUIRE(obj.z_index() == 5);
        REQUIRE(obj.is_locked());
        REQUIRE_FALSE(obj.is_visible());
        REQUIRE_THAT(obj.opacity(), Catch::Matchers::WithinAbs(0.5, 1e-9));
        REQUIRE(obj.parent_id() == 42);
    }

    SECTION("transform and position")
    {
        TestCanvasObject obj;
        obj.set_position(100.0, 200.0);
        REQUIRE(obj.position().x == 100.0);
        REQUIRE(obj.position().y == 200.0);
    }

    SECTION("world_bounds with translation")
    {
        TestCanvasObject obj(100.0, 80.0);
        obj.set_position(50.0, 50.0);
        const auto world_box = obj.world_bounds();

        REQUIRE_THAT(world_box.min_x, Catch::Matchers::WithinAbs(50.0, 1e-6));
        REQUIRE_THAT(world_box.min_y, Catch::Matchers::WithinAbs(50.0, 1e-6));
        REQUIRE_THAT(world_box.max_x, Catch::Matchers::WithinAbs(150.0, 1e-6));
        REQUIRE_THAT(world_box.max_y, Catch::Matchers::WithinAbs(130.0, 1e-6));
    }

    SECTION("metadata")
    {
        TestCanvasObject obj;
        obj.set_metadata("color", "blue");
        obj.set_metadata("priority", "high");

        REQUIRE(obj.get_metadata("color").value() == "blue");
        REQUIRE(obj.get_metadata("priority").value() == "high");
        REQUIRE_FALSE(obj.get_metadata("missing").has_value());

        obj.remove_metadata("color");
        REQUIRE_FALSE(obj.get_metadata("color").has_value());
    }

    SECTION("tags")
    {
        TestCanvasObject obj;
        obj.add_tag("important");
        obj.add_tag("urgent");
        obj.add_tag("important"); // duplicate should be ignored

        REQUIRE(obj.tags().size() == 2);

        obj.remove_tag("urgent");
        REQUIRE(obj.tags().size() == 1);
        REQUIRE(obj.tags()[0] == "important");
    }

    SECTION("dirty tracking")
    {
        TestCanvasObject obj;
        REQUIRE(obj.is_dirty()); // new objects are dirty
        obj.mark_clean();
        REQUIRE_FALSE(obj.is_dirty());
        obj.set_name("changed");
        REQUIRE(obj.is_dirty()); // mutation marks dirty
    }

    SECTION("clone")
    {
        TestCanvasObject obj(200.0, 150.0);
        obj.set_name("Original");
        obj.set_position(10.0, 20.0);
        obj.set_z_index(3);

        const auto copy = obj.clone();
        REQUIRE(copy->id() != obj.id()); // new ID
        REQUIRE(copy->name() == "Original");
        REQUIRE(copy->z_index() == 3);
    }
}
