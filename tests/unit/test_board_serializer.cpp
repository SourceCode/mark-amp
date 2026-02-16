#include "canvas/Board.h"
#include "canvas/BoardSerializer.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>

using namespace markamp::canvas;

/// Minimal CanvasObject for serializer tests.
class SerTestObj : public CanvasObject
{
public:
    SerTestObj()
        : CanvasObject(CanvasObjectType::Shape)
    {
    }

    [[nodiscard]] auto local_bounds() const -> AABB override
    {
        return AABB{0.0, 0.0, 100.0, 50.0};
    }

    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override
    {
        return std::make_unique<SerTestObj>();
    }
};

TEST_CASE("BoardSerializer format version", "[canvas][serializer]")
{
    REQUIRE(BoardSerializer::kFormatVersion == 2);
}

TEST_CASE("BoardSerializer register factory", "[canvas][serializer]")
{
    BoardSerializer serializer;

    REQUIRE_FALSE(serializer.has_factory(CanvasObjectType::Shape));

    serializer.register_factory(CanvasObjectType::Shape,
                                [](const std::string& /*json*/) -> std::unique_ptr<CanvasObject>
                                { return std::make_unique<SerTestObj>(); });

    REQUIRE(serializer.has_factory(CanvasObjectType::Shape));
    REQUIRE_FALSE(serializer.has_factory(CanvasObjectType::TextBox));
}

TEST_CASE("BoardSerializer serialize empty board", "[canvas][serializer]")
{
    BoardSerializer serializer;
    Board board("Test Board");

    const auto json = serializer.serialize(board);
    REQUIRE_FALSE(json.empty());
    REQUIRE(json.find("format_version") != std::string::npos);
    REQUIRE(json.find("Test Board") != std::string::npos);
    REQUIRE(json.find("\"object_count\": 0") != std::string::npos);
}

TEST_CASE("BoardSerializer serialize board with objects", "[canvas][serializer]")
{
    BoardSerializer serializer;
    Board board("Obj Board");

    board.add_object(std::make_unique<SerTestObj>());
    board.add_object(std::make_unique<SerTestObj>());

    const auto json = serializer.serialize(board);
    REQUIRE(json.find("\"object_count\": 2") != std::string::npos);
    REQUIRE(json.find("Obj Board") != std::string::npos);
}

TEST_CASE("BoardSerializer deserialize returns board", "[canvas][serializer]")
{
    BoardSerializer serializer;

    // Stub implementation produces an empty board.
    const auto board = serializer.deserialize("{}");
    REQUIRE(board.object_count() == 0);
    REQUIRE(board.metadata().name == "Untitled Board");
}
