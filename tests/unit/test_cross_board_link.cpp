#include "canvas/BoardNavigator.h"
#include "canvas/CrossBoardLink.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::canvas;

namespace
{

/// Create a temporary directory with .markboard files for testing.
auto create_temp_boards(const std::filesystem::path& dir, int count) -> void
{
    std::filesystem::create_directories(dir);
    for (int idx = 0; idx < count; ++idx)
    {
        const std::string filename = "board_" + std::to_string(idx) + ".markboard";
        std::ofstream file(dir / filename);
        file << "{}";
    }
}

} // anonymous namespace

TEST_CASE("CrossBoardLink: set target board_id", "[cross_board_link]")
{
    CrossBoardLinkObject link;
    BoardLinkTarget target;
    target.board_id = "abc-123";
    target.board_name = "My Board";
    link.set_target(target);

    REQUIRE(link.target().board_id == "abc-123");
    REQUIRE(link.target().board_name == "My Board");
}

TEST_CASE("CrossBoardLink: display text", "[cross_board_link]")
{
    CrossBoardLinkObject link;
    REQUIRE(link.display_text() == "Link to board");

    link.set_display_text("Go to Design Board");
    REQUIRE(link.display_text() == "Go to Design Board");
}

TEST_CASE("CrossBoardLink: JSON round-trip", "[cross_board_link]")
{
    CrossBoardLinkObject link;
    BoardLinkTarget target;
    target.board_id = "board-42";
    target.board_name = "Planning";
    link.set_target(target);
    link.set_display_text("Navigate");

    const auto json = link.to_json();
    REQUIRE(json.find("board-42") != std::string::npos);
    REQUIRE(json.find("Planning") != std::string::npos);
    REQUIRE(json.find("Navigate") != std::string::npos);
}

TEST_CASE("BoardNavigator: scan directory", "[cross_board_link]")
{
    const auto temp_dir = std::filesystem::temp_directory_path() / "markamp_test_boards";
    std::filesystem::remove_all(temp_dir);
    create_temp_boards(temp_dir, 3);

    BoardNavigator navigator;
    navigator.scan_boards(temp_dir);

    REQUIRE(navigator.all_boards().size() == 3);

    std::filesystem::remove_all(temp_dir);
}

TEST_CASE("BoardNavigator: navigate callback fires", "[cross_board_link]")
{
    BoardNavigator navigator;
    std::string navigated_id;
    navigator.set_on_board_open([&](const std::string& board_id) { navigated_id = board_id; });

    navigator.navigate_to("target-board");
    REQUIRE(navigated_id == "target-board");
}

TEST_CASE("CrossBoardLink: frame-specific link", "[cross_board_link]")
{
    CrossBoardLinkObject link;
    BoardLinkTarget target;
    target.board_id = "board-99";
    target.board_name = "Wireframes";
    target.frame_id = 42;
    link.set_target(target);

    REQUIRE(link.target().frame_id.has_value());
    REQUIRE(link.target().frame_id.value() == 42);
    REQUIRE_FALSE(link.target().object_id.has_value());

    const auto json = link.to_json();
    REQUIRE(json.find("\"frame_id\":42") != std::string::npos);
}
