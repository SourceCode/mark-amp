// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/FileTreeModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_tree() -> FileTreeModel
{
    FileTreeModel model;
    std::vector<TreeItemModel> items = {
        {"src", "src", true, true, false, 0},
        {"src/main.cpp", "main.cpp", false, false, false, 1},
        {"src/utils.cpp", "utils.cpp", false, false, false, 1},
        {"tests", "tests", true, false, false, 0},
        {"tests/test_main.cpp", "test_main.cpp", false, false, false, 1},
        {"README.md", "README.md", false, false, false, 0},
    };
    model.set_items(std::move(items));
    return model;
}

// ── Phase 07 Task 1: Selection ──────────────────────────────────────

TEST_CASE("FileTree single select clears previous", "[filetree][select]")
{
    auto model = make_test_tree();
    model.select(1);
    model.select(3);
    REQUIRE(model.selection_count() == 1);
    REQUIRE(model.selected_indices()[0] == 3);
}

TEST_CASE("FileTree toggle select adds items", "[filetree][select]")
{
    auto model = make_test_tree();
    model.select(1);
    model.toggle_select(3);
    REQUIRE(model.selection_count() == 2);
}

TEST_CASE("FileTree range select covers span", "[filetree][select]")
{
    auto model = make_test_tree();
    model.select(1);
    model.range_select(4);
    REQUIRE(model.selection_count() == 4); // indices 1,2,3,4
}

// ── Phase 07 Task 2: Inline rename validation ───────────────────────

TEST_CASE("Rename validates empty name", "[filetree][rename]")
{
    REQUIRE(FileTreeModel::validate_name("", {}) == RenameValidation::kEmpty);
}

TEST_CASE("Rename detects conflicts", "[filetree][rename]")
{
    REQUIRE(FileTreeModel::validate_name("existing.cpp", {"existing.cpp", "other.h"}) ==
            RenameValidation::kConflict);
}

TEST_CASE("Rename detects invalid characters", "[filetree][rename]")
{
    REQUIRE(FileTreeModel::validate_name("file/name.cpp", {}) == RenameValidation::kInvalidChars);
    REQUIRE(FileTreeModel::validate_name("file:name", {}) == RenameValidation::kInvalidChars);
}

TEST_CASE("Rename accepts valid names", "[filetree][rename]")
{
    REQUIRE(FileTreeModel::validate_name("new_file.cpp", {"other.h"}) == RenameValidation::kValid);
}

// ── Phase 07 Task 3: Bulk operations ────────────────────────────────

TEST_CASE("Selected IDs returns correct items", "[filetree][bulk]")
{
    auto model = make_test_tree();
    model.select(1);
    model.toggle_select(2);
    const auto ids = model.selected_ids();
    REQUIRE(ids.size() == 2);
    REQUIRE(ids[0] == "src/main.cpp");
    REQUIRE(ids[1] == "src/utils.cpp");
}

// ── Phase 07 Task 4: Type-ahead filter ──────────────────────────────

TEST_CASE("Type-ahead filter matches substring", "[filetree][filter]")
{
    auto model = make_test_tree();
    const auto matches = model.filter("main");
    REQUIRE(matches.size() == 2); // main.cpp and test_main.cpp
}

TEST_CASE("Type-ahead filter is case-insensitive", "[filetree][filter]")
{
    auto model = make_test_tree();
    const auto matches = model.filter("README");
    REQUIRE(matches.size() == 1);
}

// ── Phase 07 Task 5: Keyboard navigation ────────────────────────────

TEST_CASE("Focus clamps at boundaries", "[filetree][keyboard]")
{
    auto model = make_test_tree();
    model.set_items({{"a", "a.txt", false, false, false, 0}});
    model.focus_next();
    REQUIRE(model.focus_index() == 0);
    model.focus_previous();
    REQUIRE(model.focus_index() == 0);
}

TEST_CASE("Expand/collapse works on directories", "[filetree][keyboard]")
{
    auto model = make_test_tree();
    // Focus on "tests" (index 3), which is collapsed
    model.select(3);
    model.expand_focused();
    REQUIRE(model.items()[3].is_expanded);
    model.collapse_focused();
    REQUIRE_FALSE(model.items()[3].is_expanded);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
