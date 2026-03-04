/// @file test_command_palette_v2.cpp
/// @brief V13 Phase 31 — Unit tests for Command Palette V2 components.

#include "core/Events.h"
#include "core/FrecencyTracker.h"
#include "core/FuzzyScorer.h"
#include "core/ISymbolProvider.h"
#include "core/MarkdownSymbolProvider.h"
#include "ui/FileIconResolver.h"
#include "ui/IconProvider.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

// ============================================================================
// FuzzyScorer Tests
// ============================================================================

TEST_CASE("FuzzyScorer - exact match scores highest", "[phase31][fuzzy]")
{
    auto result = FuzzyScorer::score("save", "save");
    REQUIRE(result.score > 0);
    REQUIRE(result.is_exact_match);
    REQUIRE(result.match_positions.size() == 4);
}

TEST_CASE("FuzzyScorer - case-insensitive exact match", "[phase31][fuzzy]")
{
    auto result = FuzzyScorer::score("Save", "save");
    REQUIRE(result.score > 0);
    REQUIRE(result.is_exact_match);
}

TEST_CASE("FuzzyScorer - prefix match scores well", "[phase31][fuzzy]")
{
    auto result = FuzzyScorer::score("for", "Format Document");
    REQUIRE(result.score > 0);
    REQUIRE(result.match_positions.size() == 3);
    REQUIRE(result.match_positions[0] == 0);
    REQUIRE(result.match_positions[1] == 1);
    REQUIRE(result.match_positions[2] == 2);
}

TEST_CASE("FuzzyScorer - word boundary bonus", "[phase31][fuzzy]")
{
    auto word_boundary = FuzzyScorer::score("fd", "Format Document");
    auto consecutive = FuzzyScorer::score("fo", "Format Document");

    // Both should match
    REQUIRE(word_boundary.score > 0);
    REQUIRE(consecutive.score > 0);
    // Word boundary match "f" + "D" should score well
    REQUIRE(word_boundary.match_positions.size() == 2);
}

TEST_CASE("FuzzyScorer - no match returns score 0", "[phase31][fuzzy]")
{
    auto result = FuzzyScorer::score("xyz", "Format Document");
    REQUIRE(result.score == 0);
    REQUIRE(result.match_positions.empty());
    REQUIRE_FALSE(result.is_exact_match);
}

TEST_CASE("FuzzyScorer - empty query returns 0", "[phase31][fuzzy]")
{
    auto result = FuzzyScorer::score("", "anything");
    REQUIRE(result.score == 0);
}

TEST_CASE("FuzzyScorer - empty candidate returns 0", "[phase31][fuzzy]")
{
    auto result = FuzzyScorer::score("abc", "");
    REQUIRE(result.score == 0);
}

TEST_CASE("FuzzyScorer - camelCase scoring", "[phase31][fuzzy]")
{
    auto result = FuzzyScorer::score("gS", "goToSymbol");
    REQUIRE(result.score > 0);
    REQUIRE(result.match_positions.size() == 2);
}

TEST_CASE("FuzzyScorer - consecutive bonus", "[phase31][fuzzy]")
{
    auto consec = FuzzyScorer::score("save", "Save File");
    auto spread = FuzzyScorer::score("safi", "Save File");

    REQUIRE(consec.score > 0);
    REQUIRE(spread.score > 0);
    // Consecutive matches should score higher
    REQUIRE(consec.score > spread.score);
}

TEST_CASE("FuzzyScorer - scoring constants are positive", "[phase31][fuzzy]")
{
    REQUIRE(FuzzyScorer::kConsecutiveBonus > 0);
    REQUIRE(FuzzyScorer::kWordBoundaryBonus > 0);
    REQUIRE(FuzzyScorer::kCamelCaseBonus > 0);
    REQUIRE(FuzzyScorer::kExactPrefixBonus > 0);
    REQUIRE(FuzzyScorer::kGapPenalty < 0);
}

// ============================================================================
// FrecencyTracker Tests
// ============================================================================

TEST_CASE("FrecencyTracker - record access and score", "[phase31][frecency]")
{
    FrecencyTracker tracker;
    const int64_t now = 1000000;

    tracker.record_access("file.md", now);
    REQUIRE(tracker.has_key("file.md"));
    REQUIRE(tracker.entry_count() == 1);

    auto score_val = tracker.score("file.md", now);
    REQUIRE(score_val > 0.0);
}

TEST_CASE("FrecencyTracker - recent access scores higher", "[phase31][frecency]")
{
    FrecencyTracker tracker;
    const int64_t now = 1000000;

    // Access "recent" just now
    tracker.record_access("recent.md", now);
    // Access "old" 2 days ago
    tracker.record_access("old.md", now - 2 * 24 * 3600);

    auto recent_score = tracker.score("recent.md", now);
    auto old_score = tracker.score("old.md", now);

    REQUIRE(recent_score > old_score);
}

TEST_CASE("FrecencyTracker - multiple accesses boost score", "[phase31][frecency]")
{
    FrecencyTracker tracker;
    const int64_t now = 1000000;

    tracker.record_access("frequent.md", now - 100);
    tracker.record_access("frequent.md", now - 50);
    tracker.record_access("frequent.md", now);

    tracker.record_access("once.md", now);

    auto frequent_score = tracker.score("frequent.md", now);
    auto once_score = tracker.score("once.md", now);

    REQUIRE(frequent_score > once_score);
}

TEST_CASE("FrecencyTracker - ranked_keys returns sorted", "[phase31][frecency]")
{
    FrecencyTracker tracker;
    const int64_t now = 1000000;

    tracker.record_access("a.md", now - 100000);
    tracker.record_access("b.md", now);
    tracker.record_access("c.md", now - 50000);

    auto ranked = tracker.ranked_keys(3, now);
    REQUIRE(ranked.size() == 3);
    REQUIRE(ranked[0] == "b.md"); // Most recent
}

TEST_CASE("FrecencyTracker - clear removes all entries", "[phase31][frecency]")
{
    FrecencyTracker tracker;
    tracker.record_access("file.md", 1000);
    REQUIRE(tracker.entry_count() == 1);

    tracker.clear();
    REQUIRE(tracker.entry_count() == 0);
}

TEST_CASE("FrecencyTracker - unknown key returns 0 score", "[phase31][frecency]")
{
    FrecencyTracker tracker;
    REQUIRE(tracker.score("nonexistent.md") == 0.0);
}

TEST_CASE("FrecencyTracker - decay constants are ordered", "[phase31][frecency]")
{
    REQUIRE(FrecencyTracker::kWeight4Hours > FrecencyTracker::kWeight1Day);
    REQUIRE(FrecencyTracker::kWeight1Day > FrecencyTracker::kWeight3Days);
    REQUIRE(FrecencyTracker::kWeight3Days > FrecencyTracker::kWeight7Days);
    REQUIRE(FrecencyTracker::kWeight7Days > FrecencyTracker::kWeight30Days);
    REQUIRE(FrecencyTracker::kWeight30Days > FrecencyTracker::kWeightOlder);
}

TEST_CASE("FrecencyTracker - max timestamps enforced", "[phase31][frecency]")
{
    FrecencyTracker tracker;
    const int64_t base = 1000000;

    // Record more than kMaxTimestamps accesses
    for (int i = 0; i < 15; ++i)
    {
        tracker.record_access("file.md", base + i);
    }

    const auto* entry = tracker.get_entry("file.md");
    REQUIRE(entry != nullptr);
    REQUIRE(entry->access_count == 15);
    REQUIRE(static_cast<int>(entry->access_timestamps.size()) <= FrecencyEntry::kMaxTimestamps);
}

// ============================================================================
// MarkdownSymbolProvider Tests
// ============================================================================

TEST_CASE("MarkdownSymbolProvider - extract headings", "[phase31][symbol]")
{
    std::string text = "# Title\n\nSome text.\n\n## Section 1\n\nContent.\n\n### Subsection\n";
    auto symbols = MarkdownSymbolProvider::extract_from_text(text);

    REQUIRE(symbols.size() == 3);

    REQUIRE(symbols[0].name == "Title");
    REQUIRE(symbols[0].kind == SymbolKind::kHeading);
    REQUIRE(symbols[0].level == 1);
    REQUIRE(symbols[0].line == 1);
    REQUIRE(symbols[0].container_name.empty());

    REQUIRE(symbols[1].name == "Section 1");
    REQUIRE(symbols[1].level == 2);
    REQUIRE(symbols[1].container_name == "Title");

    REQUIRE(symbols[2].name == "Subsection");
    REQUIRE(symbols[2].level == 3);
    REQUIRE(symbols[2].container_name == "Section 1");
}

TEST_CASE("MarkdownSymbolProvider - skip code blocks", "[phase31][symbol]")
{
    std::string text = "# Real\n\n```\n# Not a heading\n```\n\n## Also Real\n";
    auto symbols = MarkdownSymbolProvider::extract_from_text(text);

    REQUIRE(symbols.size() == 2);
    REQUIRE(symbols[0].name == "Real");
    REQUIRE(symbols[1].name == "Also Real");
}

TEST_CASE("MarkdownSymbolProvider - supports markdown files", "[phase31][symbol]")
{
    MarkdownSymbolProvider provider;
    REQUIRE(provider.supports("document.md"));
    REQUIRE(provider.supports("readme.markdown"));
    REQUIRE(provider.supports("page.mdx"));
    REQUIRE_FALSE(provider.supports("code.cpp"));
    REQUIRE_FALSE(provider.supports("data.json"));
}

TEST_CASE("MarkdownSymbolProvider - empty text returns no symbols", "[phase31][symbol]")
{
    auto symbols = MarkdownSymbolProvider::extract_from_text("");
    REQUIRE(symbols.empty());
}

// ============================================================================
// SymbolKind helpers Tests
// ============================================================================

TEST_CASE("SymbolKind labels and icons", "[phase31][symbol]")
{
    REQUIRE(std::string(symbol_kind_label(SymbolKind::kHeading)) == "heading");
    REQUIRE(std::string(symbol_kind_label(SymbolKind::kFunction)) == "function");
    REQUIRE(std::string(symbol_kind_label(SymbolKind::kClass)) == "class");

    REQUIRE(std::string(symbol_kind_icon(SymbolKind::kHeading)) == "H");
    REQUIRE(std::string(symbol_kind_icon(SymbolKind::kFunction)) == "ƒ");
}

// ============================================================================
// FileIconResolver Tests
// ============================================================================

TEST_CASE("FileIconResolver - code files", "[phase31][icon]")
{
    REQUIRE(resolve_file_icon("main.cpp") == FileIconType::kCode);
    REQUIRE(resolve_file_icon("header.h") == FileIconType::kCode);
    REQUIRE(resolve_file_icon("source.cc") == FileIconType::kCode);
}

TEST_CASE("FileIconResolver - markdown files", "[phase31][icon]")
{
    REQUIRE(resolve_file_icon("readme.md") == FileIconType::kMarkdown);
    REQUIRE(resolve_file_icon("page.mdx") == FileIconType::kMarkdown);
}

TEST_CASE("FileIconResolver - config files", "[phase31][icon]")
{
    REQUIRE(resolve_file_icon("config.json") == FileIconType::kConfig);
    REQUIRE(resolve_file_icon("settings.yaml") == FileIconType::kConfig);
}

TEST_CASE("FileIconResolver - image files", "[phase31][icon]")
{
    REQUIRE(resolve_file_icon("photo.png") == FileIconType::kImage);
    REQUIRE(resolve_file_icon("icon.svg") == FileIconType::kImage);
}

TEST_CASE("FileIconResolver - build files", "[phase31][icon]")
{
    REQUIRE(resolve_file_icon("CMakeLists.txt") == FileIconType::kBuild);
    REQUIRE(resolve_file_icon("Makefile") == FileIconType::kBuild);
    REQUIRE(resolve_file_icon("build.cmake") == FileIconType::kBuild);
}

TEST_CASE("FileIconResolver - git files", "[phase31][icon]")
{
    REQUIRE(resolve_file_icon(".gitignore") == FileIconType::kGit);
}

TEST_CASE("FileIconResolver - unknown extension", "[phase31][icon]")
{
    REQUIRE(resolve_file_icon("unknown.xyz") == FileIconType::kUnknown);
    REQUIRE(resolve_file_icon("") == FileIconType::kUnknown);
}

TEST_CASE("FileIconResolver - with path", "[phase31][icon]")
{
    REQUIRE(resolve_file_icon("/some/path/main.cpp") == FileIconType::kCode);
    REQUIRE(resolve_file_icon("/project/CMakeLists.txt") == FileIconType::kBuild);
}

TEST_CASE("FileIconResolver - label and char helpers", "[phase31][icon]")
{
    REQUIRE(std::string(file_icon_label(FileIconType::kCode)) == "code");
    REQUIRE(std::string(file_icon_label(FileIconType::kMarkdown)) == "markdown");
    REQUIRE(std::string(file_icon_char(FileIconType::kCode)) == "{ }");
}

// ============================================================================
// IconProvider Tests
// ============================================================================

TEST_CASE("IconProvider - category defaults", "[phase31][icon]")
{
    IconProvider provider;

    REQUIRE(provider.icon_for_category("File") == IconProvider::kFileIcon);
    REQUIRE(provider.icon_for_category("Edit") == IconProvider::kEditIcon);
    REQUIRE(provider.icon_for_category("View") == IconProvider::kViewIcon);
    REQUIRE(provider.icon_for_category("Navigation") == IconProvider::kNavigationIcon);
    REQUIRE(provider.icon_for_category("Unknown") == IconProvider::kDefaultIcon);
}

TEST_CASE("IconProvider - specific icon override", "[phase31][icon]")
{
    IconProvider provider;
    provider.register_icon("bold", "𝐁");

    REQUIRE(provider.has_icon("bold"));
    REQUIRE(provider.icon_for_command("bold", "Editor") == "𝐁");
}

TEST_CASE("IconProvider - falls back to category", "[phase31][icon]")
{
    IconProvider provider;
    // No specific icon for "save", should use File category default
    REQUIRE(provider.icon_for_command("save", "File") == IconProvider::kFileIcon);
}

TEST_CASE("IconProvider - counts", "[phase31][icon]")
{
    IconProvider provider;
    REQUIRE(provider.category_count() > 0);
    REQUIRE(provider.specific_icon_count() == 0);

    provider.register_icon("test", "T");
    REQUIRE(provider.specific_icon_count() == 1);
}

// ============================================================================
// Phase 31 Events Tests
// ============================================================================

TEST_CASE("PaletteOpenedEvent construction", "[phase31][events]")
{
    events::PaletteOpenedEvent evt;
    evt.mode = 1;
    REQUIRE(evt.mode == 1);
}

TEST_CASE("PaletteClosedEvent construction", "[phase31][events]")
{
    events::PaletteClosedEvent evt;
    evt.selected_command = "editor.action.save";
    evt.cancelled = false;
    REQUIRE(evt.selected_command == "editor.action.save");
    REQUIRE_FALSE(evt.cancelled);
}

TEST_CASE("PaletteClosedEvent cancelled", "[phase31][events]")
{
    events::PaletteClosedEvent evt;
    evt.cancelled = true;
    REQUIRE(evt.cancelled);
    REQUIRE(evt.selected_command.empty());
}
