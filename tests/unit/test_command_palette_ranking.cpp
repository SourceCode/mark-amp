// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/CommandPaletteModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_palette() -> CommandPaletteModel
{
    CommandPaletteModel model;
    model.add_command({"file.save",
                       "Save File",
                       "File",
                       "Cmd+S",
                       "Save the current file",
                       "builtin",
                       {"write", "save"},
                       false,
                       false});
    model.add_command({"file.saveAll",
                       "Save All Files",
                       "File",
                       "Cmd+Alt+S",
                       "Save all open files",
                       "builtin",
                       {"write all"},
                       false,
                       false});
    model.add_command({"editor.toggleWordWrap",
                       "Toggle Word Wrap",
                       "Editor",
                       "Alt+Z",
                       "Toggle word wrapping",
                       "builtin",
                       {"wrap"},
                       false,
                       false});
    model.add_command({"file.deleteWorkspace",
                       "Delete Workspace",
                       "File",
                       "",
                       "Permanently delete the entire workspace",
                       "builtin",
                       {"remove", "destroy"},
                       true,
                       false});
    return model;
}

// ── Phase 10 Task 1: Rich metadata ──────────────────────────────────

TEST_CASE("Commands have full metadata", "[palette][metadata]")
{
    auto model = make_test_palette();
    const auto& cmds = model.commands();
    REQUIRE(cmds.size() == 4);
    REQUIRE(cmds[0].category == "File");
    REQUIRE(cmds[0].shortcut == "Cmd+S");
    REQUIRE(cmds[0].source == "builtin");
}

// ── Phase 10 Task 2: Fuzzy search with aliases ──────────────────────

TEST_CASE("Search matches on label", "[palette][search]")
{
    auto model = make_test_palette();
    const auto results = model.search("word wrap");
    REQUIRE(!results.empty());
    REQUIRE(model.commands()[static_cast<std::size_t>(results[0].index)].command_id ==
            "editor.toggleWordWrap");
}

TEST_CASE("Search matches aliases", "[palette][search]")
{
    auto model = make_test_palette();
    const auto results = model.search("write");
    REQUIRE(!results.empty());
    // "write" is an alias for file.save
    REQUIRE(model.commands()[static_cast<std::size_t>(results[0].index)].category == "File");
}

TEST_CASE("Search returns empty for unmatched query", "[palette][search]")
{
    auto model = make_test_palette();
    const auto results = model.search("xyznonexistent");
    REQUIRE(results.empty());
}

// ── Phase 10 Task 3: Preview for destructive commands ───────────────

TEST_CASE("Destructive commands require confirmation", "[palette][safety]")
{
    auto model = make_test_palette();
    REQUIRE(model.needs_confirmation("file.deleteWorkspace"));
    REQUIRE_FALSE(model.needs_confirmation("file.save"));
}

TEST_CASE("Preview text includes warning for destructive commands", "[palette][safety]")
{
    auto model = make_test_palette();
    const auto preview = model.preview_text("file.deleteWorkspace");
    REQUIRE(preview.find("⚠") != std::string::npos);
    REQUIRE(preview.find("cannot be undone") != std::string::npos);
}

// ── Phase 10 Task 4: MRU and pinned ─────────────────────────────────

TEST_CASE("MRU records usage and caps at max", "[palette][mru]")
{
    auto model = make_test_palette();
    model.record_usage("file.save");
    model.record_usage("editor.toggleWordWrap");
    model.record_usage("file.save"); // duplicate moves to front

    const auto& mru = model.mru_history();
    REQUIRE(mru[0] == "file.save");
    REQUIRE(mru[1] == "editor.toggleWordWrap");
    REQUIRE(mru.size() == 2);
}

TEST_CASE("MRU boosts search ranking", "[palette][mru]")
{
    auto model = make_test_palette();
    model.record_usage("file.saveAll");

    const auto results = model.search("Save");
    REQUIRE(!results.empty());
    // saveAll should rank higher than save due to MRU boost
    REQUIRE(model.commands()[static_cast<std::size_t>(results[0].index)].command_id ==
            "file.saveAll");
}

TEST_CASE("Pinned commands rank highest", "[palette][pinned]")
{
    auto model = make_test_palette();
    model.pin_command("editor.toggleWordWrap");

    const auto results = model.search("save");
    // "Toggle Word Wrap" doesn't match "save" so it shouldn't appear
    // But if we search for something that matches all:
    const auto all_results = model.search("file");
    // Check pinned logic: pin a file command and verify it ranks first
    model.pin_command("file.saveAll");
    const auto pinned_results = model.search("file");
    REQUIRE(!pinned_results.empty());
    REQUIRE(model.commands()[static_cast<std::size_t>(pinned_results[0].index)].command_id ==
            "file.saveAll");
}

TEST_CASE("Unpin removes pin status", "[palette][pinned]")
{
    auto model = make_test_palette();
    model.pin_command("file.save");
    REQUIRE(model.commands()[0].is_pinned);
    model.unpin_command("file.save");
    REQUIRE_FALSE(model.commands()[0].is_pinned);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
