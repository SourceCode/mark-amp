#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>

using namespace markamp::canvas;

/// Minimal CanvasObject for Board tests.
class MockBoardObj : public CanvasObject
{
public:
    MockBoardObj()
        : CanvasObject(CanvasObjectType::Shape)
    {
    }

    [[nodiscard]] auto local_bounds() const -> AABB override
    {
        return AABB{0.0, 0.0, 100.0, 50.0};
    }

    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override
    {
        auto copy = std::make_unique<MockBoardObj>();
        copy->set_transform(transform());
        copy->set_z_index(z_index());
        return copy;
    }
};

TEST_CASE("Board default construction", "[canvas][board]")
{
    Board board;
    REQUIRE(board.metadata().name == "Untitled Board");
    REQUIRE(board.object_count() == 0);
    REQUIRE_FALSE(board.is_dirty());
}

TEST_CASE("Board named construction", "[canvas][board]")
{
    Board board("My Board");
    REQUIRE(board.metadata().name == "My Board");
}

TEST_CASE("Board add and remove objects", "[canvas][board]")
{
    Board board;

    auto obj1 = std::make_unique<MockBoardObj>();
    auto obj2 = std::make_unique<MockBoardObj>();
    const auto id1 = obj1->id();
    const auto id2 = obj2->id();

    board.add_object(std::move(obj1));
    board.add_object(std::move(obj2));

    REQUIRE(board.object_count() == 2);
    REQUIRE(board.get_object(id1) != nullptr);
    REQUIRE(board.get_object(id2) != nullptr);
    REQUIRE(board.is_dirty());

    // Remove one.
    auto removed = board.remove_object(id1);
    REQUIRE(removed != nullptr);
    REQUIRE(removed->id() == id1);
    REQUIRE(board.object_count() == 1);
    REQUIRE(board.get_object(id1) == nullptr);

    // Remove non-existent.
    auto nothing = board.remove_object(999999);
    REQUIRE(nothing == nullptr);
}

TEST_CASE("Board all_object_ids", "[canvas][board]")
{
    Board board;

    auto obj1 = std::make_unique<MockBoardObj>();
    auto obj2 = std::make_unique<MockBoardObj>();
    const auto id1 = obj1->id();
    const auto id2 = obj2->id();

    board.add_object(std::move(obj1));
    board.add_object(std::move(obj2));

    const auto ids = board.all_object_ids();
    REQUIRE(ids.size() == 2);
}

TEST_CASE("Board z-ordering", "[canvas][board]")
{
    Board board;

    auto obj1 = std::make_unique<MockBoardObj>();
    auto obj2 = std::make_unique<MockBoardObj>();
    auto obj3 = std::make_unique<MockBoardObj>();
    const auto id1 = obj1->id();
    const auto id2 = obj2->id();
    const auto id3 = obj3->id();

    board.add_object(std::move(obj1));
    board.add_object(std::move(obj2));
    board.add_object(std::move(obj3));

    board.bring_to_front(id1);
    REQUIRE(board.get_object(id1)->z_index() > board.get_object(id2)->z_index());
    REQUIRE(board.get_object(id1)->z_index() > board.get_object(id3)->z_index());

    board.send_to_back(id1);
    REQUIRE(board.get_object(id1)->z_index() < board.get_object(id2)->z_index());

    const auto z_ids = board.z_ordered_ids();
    REQUIRE(z_ids.size() == 3);
}

TEST_CASE("Board content_bounds", "[canvas][board]")
{
    Board board;

    SECTION("empty board has zero bounds")
    {
        const auto bounds = board.content_bounds();
        REQUIRE(bounds.width() == 0.0);
    }

    SECTION("bounds merge objects")
    {
        auto obj1 = std::make_unique<MockBoardObj>();
        auto obj2 = std::make_unique<MockBoardObj>();

        Transform2D xform;
        xform.tx = 500.0;
        obj2->set_transform(xform);

        board.add_object(std::move(obj1));
        board.add_object(std::move(obj2));

        const auto bounds = board.content_bounds();
        REQUIRE(bounds.width() > 500.0);
    }
}

TEST_CASE("Board dirty tracking", "[canvas][board]")
{
    Board board;

    REQUIRE_FALSE(board.is_dirty());

    board.add_object(std::make_unique<MockBoardObj>());
    REQUIRE(board.is_dirty());

    board.clear_dirty();
    REQUIRE_FALSE(board.is_dirty());

    board.mark_dirty();
    REQUIRE(board.is_dirty());
}

TEST_CASE("Board deep clone", "[canvas][board]")
{
    Board board("Original");
    board.add_object(std::make_unique<MockBoardObj>());
    board.add_object(std::make_unique<MockBoardObj>());

    auto cloned = board.deep_clone();
    REQUIRE(cloned.metadata().name == "Original");
    REQUIRE(cloned.object_count() == 2);
}
