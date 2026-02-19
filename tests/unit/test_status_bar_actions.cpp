// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/StatusBarModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_statusbar() -> StatusBarModel
{
    StatusBarModel model;
    model.add_item({"errors",
                    "0 Errors",
                    "Error count",
                    StatusItemPriority::kHigh,
                    StatusItemKind::kClickable,
                    true,
                    0.0F,
                    false});
    model.add_item({"encoding",
                    "UTF-8",
                    "File encoding",
                    StatusItemPriority::kNormal,
                    StatusItemKind::kClickable,
                    true,
                    0.0F,
                    false});
    model.add_item({"zoom",
                    "100%",
                    "Zoom level",
                    StatusItemPriority::kLow,
                    StatusItemKind::kClickable,
                    true,
                    0.0F,
                    false});
    model.add_item({"notifications",
                    "🔔",
                    "Notifications",
                    StatusItemPriority::kLow,
                    StatusItemKind::kPassive,
                    true,
                    0.0F,
                    false});
    return model;
}

// ── Phase 13 Task 1: Priority ordering ──────────────────────────────

TEST_CASE("Items sort by priority", "[statusbar][ordering]")
{
    auto model = make_test_statusbar();
    const auto sorted = model.sorted_items();
    REQUIRE(sorted[0].item_id == "errors");   // kHigh
    REQUIRE(sorted[1].item_id == "encoding"); // kNormal
    // kLow items last (zoom, notifications)
}

// ── Phase 13 Task 2: Clickable item distinction ─────────────────────

TEST_CASE("Clickable items filtered correctly", "[statusbar][clickable]")
{
    auto model = make_test_statusbar();
    const auto clickable = model.clickable_items();
    REQUIRE(clickable.size() == 3); // errors, encoding, zoom
}

TEST_CASE("Passive items excluded from clickable", "[statusbar][clickable]")
{
    auto model = make_test_statusbar();
    const auto clickable = model.clickable_items();
    for (const auto& item : clickable)
    {
        REQUIRE(item.kind == StatusItemKind::kClickable);
    }
}

// ── Phase 13 Task 3: Label updates ──────────────────────────────────

TEST_CASE("Update label changes display text", "[statusbar][update]")
{
    auto model = make_test_statusbar();
    model.update_label("errors", "3 Errors");
    const auto sorted = model.sorted_items();
    REQUIRE(sorted[0].label == "3 Errors");
}

// ── Phase 13 Task 4: Progress items ─────────────────────────────────

TEST_CASE("Progress item tracks progress", "[statusbar][progress]")
{
    StatusBarModel model;
    model.add_item({"indexing",
                    "Indexing...",
                    "Indexing files",
                    StatusItemPriority::kNormal,
                    StatusItemKind::kProgress,
                    true,
                    0.0F,
                    true});

    model.update_progress("indexing", 0.5F);
    const auto progress = model.progress_items();
    REQUIRE(progress.size() == 1);
    REQUIRE(progress[0].progress > 0.4F);
    REQUIRE(progress[0].is_cancellable);
}

TEST_CASE("Complete progress removes item", "[statusbar][progress]")
{
    StatusBarModel model;
    model.add_item({"indexing",
                    "Indexing...",
                    "Indexing files",
                    StatusItemPriority::kNormal,
                    StatusItemKind::kProgress,
                    true,
                    0.0F,
                    false});

    model.complete_progress("indexing");
    REQUIRE(model.item_count() == 0);
}

// ── Overflow ────────────────────────────────────────────────────────

TEST_CASE("Truncation detects overflow", "[statusbar][overflow]")
{
    auto model = make_test_statusbar();
    const auto truncated = model.truncated_indices(200, 80);
    // 4 items × 80px = 320px > 200px → 2 truncated
    REQUIRE(truncated.size() == 2);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
