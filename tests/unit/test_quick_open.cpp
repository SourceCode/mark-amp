#include "../../src/core/FileIndex.h"
#include "../../src/core/FileSearchScorer.h"
#include "../../src/core/FrecencyTracker.h"
#include "../../src/core/RecentFilesManager.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

// Inline TabSwitcher models to avoid wxWidgets dependency
namespace markamp::ui
{

struct TabSwitcherEntry
{
    std::string file_path;
    std::string display_name;
    std::string relative_path;
    bool is_modified{false};
    bool is_preview{false};
};

class TabSwitcherModel
{
public:
    void set_tabs(std::vector<TabSwitcherEntry> tabs)
    {
        tabs_ = std::move(tabs);
        selected_index_ = tabs_.empty() ? 0 : 1;
        if (selected_index_ >= tabs_.size())
        {
            selected_index_ = 0;
        }
    }
    [[nodiscard]] auto tabs() const -> const std::vector<TabSwitcherEntry>&
    {
        return tabs_;
    }
    [[nodiscard]] auto selected_index() const -> std::size_t
    {
        return selected_index_;
    }
    void select_next()
    {
        if (tabs_.empty())
        {
            return;
        }
        selected_index_ = (selected_index_ + 1) % tabs_.size();
    }
    void select_previous()
    {
        if (tabs_.empty())
        {
            return;
        }
        selected_index_ = selected_index_ == 0 ? tabs_.size() - 1 : selected_index_ - 1;
    }
    void set_selected_index(std::size_t index)
    {
        if (index < tabs_.size())
        {
            selected_index_ = index;
        }
    }
    [[nodiscard]] auto selected_entry() const -> const TabSwitcherEntry&
    {
        static const TabSwitcherEntry empty_entry;
        if (tabs_.empty() || selected_index_ >= tabs_.size())
        {
            return empty_entry;
        }
        return tabs_[selected_index_];
    }
    [[nodiscard]] auto has_tabs() const -> bool
    {
        return tabs_.size() > 1;
    }
    [[nodiscard]] auto tab_count() const -> std::size_t
    {
        return tabs_.size();
    }

private:
    std::vector<TabSwitcherEntry> tabs_;
    std::size_t selected_index_{0};
};

} // namespace markamp::ui

using namespace markamp::core;
using namespace markamp::ui;

// ============================================================================
// RecentFilesManager Tests
// ============================================================================

TEST_CASE("RecentFilesManager empty state", "[quick_open][mru]")
{
    FrecencyTracker tracker;
    RecentFilesManager manager(tracker);

    auto files = manager.ranked_files(10);
    REQUIRE(files.empty());
    REQUIRE(manager.tracked_count() == 0);
    REQUIRE(manager.pinned_count() == 0);
}

TEST_CASE("RecentFilesManager record and rank", "[quick_open][mru]")
{
    FrecencyTracker tracker;
    RecentFilesManager manager(tracker);

    manager.record_open("/path/a.md");
    manager.record_open("/path/b.md");
    manager.record_open("/path/c.md");

    auto files = manager.ranked_files(10);
    REQUIRE(files.size() == 3);
}

TEST_CASE("RecentFilesManager pin/unpin", "[quick_open][mru]")
{
    FrecencyTracker tracker;
    RecentFilesManager manager(tracker);

    manager.record_open("/path/a.md");
    manager.record_open("/path/b.md");
    manager.pin_file("/path/b.md");

    REQUIRE(manager.is_pinned("/path/b.md"));
    REQUIRE_FALSE(manager.is_pinned("/path/a.md"));
    REQUIRE(manager.pinned_count() == 1);

    // Pinned files appear first
    auto files = manager.ranked_files(10);
    REQUIRE_FALSE(files.empty());
    REQUIRE(files[0].file_path == "/path/b.md");
    REQUIRE(files[0].is_pinned);

    manager.unpin_file("/path/b.md");
    REQUIRE_FALSE(manager.is_pinned("/path/b.md"));
    REQUIRE(manager.pinned_count() == 0);
}

TEST_CASE("RecentFilesManager max pinned files", "[quick_open][mru]")
{
    FrecencyTracker tracker;
    RecentFilesManager manager(tracker);

    for (int idx = 0; idx < 12; ++idx)
    {
        manager.pin_file("/path/file" + std::to_string(idx) + ".md");
    }

    // Should be capped at 10
    REQUIRE(manager.pinned_count() == 10);
}

TEST_CASE("RecentFilesManager remove file", "[quick_open][mru]")
{
    FrecencyTracker tracker;
    RecentFilesManager manager(tracker);

    manager.record_open("/path/a.md");
    manager.record_open("/path/b.md");
    manager.pin_file("/path/a.md");

    manager.remove_file("/path/a.md");

    auto files = manager.ranked_files(10);
    REQUIRE(files.size() == 1);
    REQUIRE(files[0].file_path == "/path/b.md");
    REQUIRE_FALSE(manager.is_pinned("/path/a.md"));
}

TEST_CASE("RecentFilesManager clear", "[quick_open][mru]")
{
    FrecencyTracker tracker;
    RecentFilesManager manager(tracker);

    manager.record_open("/path/a.md");
    manager.pin_file("/path/a.md");
    manager.clear();

    REQUIRE(manager.tracked_count() == 0);
    REQUIRE(manager.pinned_count() == 0);
}

TEST_CASE("RecentFilesManager display name extraction", "[quick_open][mru]")
{
    FrecencyTracker tracker;
    RecentFilesManager manager(tracker);

    manager.record_open("/usr/local/src/main.cpp");
    auto files = manager.ranked_files(1);
    REQUIRE(files.size() == 1);
    REQUIRE(files[0].display_name == "main.cpp");
}

TEST_CASE("RecentFilesManager limit results", "[quick_open][mru]")
{
    FrecencyTracker tracker;
    RecentFilesManager manager(tracker);

    for (int idx = 0; idx < 30; ++idx)
    {
        manager.record_open("/path/file" + std::to_string(idx) + ".md");
    }

    auto files = manager.ranked_files(5);
    REQUIRE(files.size() == 5);
}

// ============================================================================
// FileSearchScorer Tests
// ============================================================================

TEST_CASE("FileSearchScorer scores filename match", "[quick_open][scorer]")
{
    FileSearchScorer scorer;

    auto result = scorer.score_file("src/ui/EditorPanel.cpp", "EdPan");
    REQUIRE(result.score > 0.0);
    REQUIRE(result.display_name == "EditorPanel.cpp");
}

TEST_CASE("FileSearchScorer path segment matching", "[quick_open][scorer]")
{
    FileSearchScorer scorer;

    auto result = scorer.score_file("src/ui/EditorPanel.cpp", "ui/Ed");
    REQUIRE(result.score > 0.0);
}

TEST_CASE("FileSearchScorer proximity bonus", "[quick_open][scorer]")
{
    FileSearchScorer scorer;

    auto result_near = scorer.score_file("src/ui/FileA.cpp", "File", "src/ui");
    auto result_far = scorer.score_file("src/core/FileA.cpp", "File", "src/ui");

    REQUIRE(result_near.score > result_far.score);
}

TEST_CASE("FileSearchScorer open tab bonus", "[quick_open][scorer]")
{
    FileSearchScorer scorer;

    auto result_open = scorer.score_file("src/ui/EditorPanel.cpp", "Ed", "", true);
    auto result_closed = scorer.score_file("src/ui/EditorPanel.cpp", "Ed", "", false);

    REQUIRE(result_open.score > result_closed.score);
}

TEST_CASE("FileSearchScorer empty query returns zero score", "[quick_open][scorer]")
{
    FileSearchScorer scorer;

    auto result = scorer.score_file("src/ui/EditorPanel.cpp", "");
    REQUIRE(result.score == 0.0);
}

TEST_CASE("FileSearchScorer score_files ranks results", "[quick_open][scorer]")
{
    FileSearchScorer scorer;

    std::vector<std::string> files = {
        "src/core/Config.cpp", "src/ui/EditorPanel.cpp", "src/core/EventBus.h", "CMakeLists.txt"};

    auto results = scorer.score_files(files, "Edit", "", {}, 10);
    // EditorPanel should score highest
    REQUIRE_FALSE(results.empty());
    REQUIRE(results[0].display_name == "EditorPanel.cpp");
}

TEST_CASE("FileSearchScorer score_files with open tabs", "[quick_open][scorer]")
{
    FileSearchScorer scorer;

    std::vector<std::string> files = {"src/core/Config.cpp", "src/ui/EditorPanel.cpp"};

    auto results = scorer.score_files(files, "c", "", {"src/core/Config.cpp"}, 10);
    // Config.cpp should get open-tab bonus
    REQUIRE(results.size() >= 1);
}

// ============================================================================
// FileIndex Tests
// ============================================================================

TEST_CASE("FileIndex empty state", "[quick_open][index]")
{
    FileIndex index;
    REQUIRE(index.size() == 0);
    REQUIRE(index.all_files().empty());
}

TEST_CASE("FileIndex build from file list", "[quick_open][index]")
{
    FileIndex index;
    index.build({"src/main.cpp", "src/util.h", "CMakeLists.txt"});
    REQUIRE(index.size() == 3);
}

TEST_CASE("FileIndex add and remove", "[quick_open][index]")
{
    FileIndex index;
    index.add("src/new_file.cpp");
    REQUIRE(index.size() == 1);

    index.remove("src/new_file.cpp");
    REQUIRE(index.size() == 0);
}

TEST_CASE("FileIndex trigram search", "[quick_open][index]")
{
    FileIndex index;
    index.build(
        {"src/ui/EditorPanel.cpp", "src/core/Config.h", "src/ui/TabBar.cpp", "CMakeLists.txt"});

    // "editor" has trigrams: edi, dit, ito, tor
    auto results = index.search("editor", 10);
    REQUIRE(results.size() == 1);
    REQUIRE(results[0] == "src/ui/EditorPanel.cpp");
}

TEST_CASE("FileIndex short query fallback to substring", "[quick_open][index]")
{
    FileIndex index;
    index.build({"src/ui/EditorPanel.cpp", "src/core/Config.h"});

    // "ui" is too short for trigrams, falls back to substring
    auto results = index.search("ui", 10);
    REQUIRE(results.size() == 1);
}

TEST_CASE("FileIndex empty query returns all files", "[quick_open][index]")
{
    FileIndex index;
    index.build({"a.cpp", "b.cpp", "c.cpp"});

    auto results = index.search("", 100);
    REQUIRE(results.size() == 3);
}

TEST_CASE("FileIndex clear", "[quick_open][index]")
{
    FileIndex index;
    index.build({"a.cpp", "b.cpp"});
    index.clear();
    REQUIRE(index.size() == 0);
}

TEST_CASE("FileIndex search with no matches", "[quick_open][index]")
{
    FileIndex index;
    index.build({"src/main.cpp"});

    auto results = index.search("xyz_nonexistent", 10);
    REQUIRE(results.empty());
}

// ============================================================================
// TabSwitcherModel Tests
// ============================================================================

TEST_CASE("TabSwitcherModel empty state", "[quick_open][switcher]")
{
    TabSwitcherModel model;
    REQUIRE_FALSE(model.has_tabs());
    REQUIRE(model.tab_count() == 0);
}

TEST_CASE("TabSwitcherModel set_tabs and selection", "[quick_open][switcher]")
{
    TabSwitcherModel model;
    std::vector<TabSwitcherEntry> tabs = {{"a.md", "a.md", "src/a.md", false, false},
                                          {"b.md", "b.md", "src/b.md", true, false},
                                          {"c.md", "c.md", "src/c.md", false, false}};
    model.set_tabs(tabs);

    REQUIRE(model.has_tabs());
    REQUIRE(model.tab_count() == 3);
    REQUIRE(model.selected_index() == 1); // Starts at second (previous file)
}

TEST_CASE("TabSwitcherModel select_next wraps", "[quick_open][switcher]")
{
    TabSwitcherModel model;
    model.set_tabs(
        {{"a", "a", "", false, false}, {"b", "b", "", false, false}, {"c", "c", "", false, false}});

    model.set_selected_index(2);
    model.select_next();
    REQUIRE(model.selected_index() == 0); // Wrapped
}

TEST_CASE("TabSwitcherModel select_previous wraps", "[quick_open][switcher]")
{
    TabSwitcherModel model;
    model.set_tabs({{"a", "a", "", false, false}, {"b", "b", "", false, false}});

    model.set_selected_index(0);
    model.select_previous();
    REQUIRE(model.selected_index() == 1); // Wrapped
}

TEST_CASE("TabSwitcherModel selected_entry", "[quick_open][switcher]")
{
    TabSwitcherModel model;
    model.set_tabs({{"path/a.md", "a.md", "path/", false, false},
                    {"path/b.md", "b.md", "path/", true, false}});

    model.set_selected_index(1);
    REQUIRE(model.selected_entry().display_name == "b.md");
    REQUIRE(model.selected_entry().is_modified);
}
