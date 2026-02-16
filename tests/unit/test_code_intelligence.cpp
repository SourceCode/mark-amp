/// @file test_code_intelligence.cpp
/// V8 Phase 10: Code Intelligence UX quality bar tests.
/// Covers diagnostic indicators, productivity modes, reading profiles,
/// preview block actions, new event structs, and DiagnosticsService integration.

#include "core/DiagnosticsService.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"
#include "ui/EditorPanel.h"
#include "ui/SplitView.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;
using namespace markamp::ui;

// ============================================================================
//  Test 1: DiagnosticIndicator struct fields and defaults
// ============================================================================

TEST_CASE("DiagnosticIndicator struct fields", "[diagnostic][phase10]")
{
    DiagnosticIndicator indicator;

    SECTION("Default values")
    {
        CHECK(indicator.line == 0);
        CHECK(indicator.severity == DiagnosticSeverity::kError);
        CHECK(indicator.message.empty());
        CHECK(indicator.quick_fix_available == false);
    }

    SECTION("Custom initialization")
    {
        DiagnosticIndicator custom{.line = 42,
                                   .severity = DiagnosticSeverity::kWarning,
                                   .message = "Unused variable",
                                   .quick_fix_available = true};

        CHECK(custom.line == 42);
        CHECK(custom.severity == DiagnosticSeverity::kWarning);
        CHECK(custom.message == "Unused variable");
        CHECK(custom.quick_fix_available == true);
    }
}

// ============================================================================
//  Test 2: ProductivityMode enum completeness and distinctness
// ============================================================================

TEST_CASE("ProductivityMode enum", "[editor][phase10]")
{
    auto writing = static_cast<int>(ProductivityMode::kWriting);
    auto review = static_cast<int>(ProductivityMode::kReview);
    auto refactor = static_cast<int>(ProductivityMode::kRefactor);

    SECTION("All values are distinct")
    {
        CHECK(writing != review);
        CHECK(review != refactor);
        CHECK(writing != refactor);
    }

    SECTION("All values >= 0")
    {
        CHECK(writing >= 0);
        CHECK(review >= 0);
        CHECK(refactor >= 0);
    }
}

// ============================================================================
//  Test 3: PreviewBlockAction enum completeness
// ============================================================================

TEST_CASE("PreviewBlockAction enum", "[preview][phase10]")
{
    auto copy = static_cast<int>(PreviewBlockAction::kCopyCode);
    auto collapse = static_cast<int>(PreviewBlockAction::kCollapseToggle);
    auto open = static_cast<int>(PreviewBlockAction::kOpenSource);
    auto edit = static_cast<int>(PreviewBlockAction::kEditSection);

    CHECK(copy != collapse);
    CHECK(collapse != open);
    CHECK(open != edit);
    CHECK(copy != edit);
}

// ============================================================================
//  Test 4: ReadingProfile enum completeness
// ============================================================================

TEST_CASE("ReadingProfile enum", "[preview][phase10]")
{
    auto doc = static_cast<int>(ReadingProfile::kDocumentation);
    auto pres = static_cast<int>(ReadingProfile::kPresentation);
    auto print = static_cast<int>(ReadingProfile::kPrintReady);

    CHECK(doc != pres);
    CHECK(pres != print);
    CHECK(doc != print);
}

// ============================================================================
//  Test 5: Phase 10 event structs compile and carry payloads
// ============================================================================

TEST_CASE("Phase 10 event structs", "[events][phase10]")
{
    SECTION("DiagnosticIndicatorClickedEvent")
    {
        DiagnosticIndicatorClickedEvent evt;
        evt.line = 10;
        evt.severity = static_cast<std::uint8_t>(DiagnosticSeverity::kWarning);
        evt.message = "Missing semicolon";

        CHECK(evt.line == 10);
        CHECK(evt.severity == 1);
        CHECK(evt.message == "Missing semicolon");
        CHECK(evt.type_name() == "DiagnosticIndicatorClickedEvent");
    }

    SECTION("QuickFixRequestedEvent")
    {
        QuickFixRequestedEvent evt;
        evt.line = 5;
        evt.character = 12;
        evt.file_uri = "file:///test.md";

        CHECK(evt.line == 5);
        CHECK(evt.character == 12);
        CHECK(evt.file_uri == "file:///test.md");
        CHECK(evt.type_name() == "QuickFixRequestedEvent");
    }

    SECTION("PeekProblemRequestedEvent")
    {
        PeekProblemRequestedEvent evt;
        evt.start_line = 1;
        evt.start_char = 0;
        evt.end_line = 1;
        evt.end_char = 15;
        evt.file_uri = "file:///readme.md";

        CHECK(evt.start_line == 1);
        CHECK(evt.end_char == 15);
        CHECK(evt.type_name() == "PeekProblemRequestedEvent");
    }

    SECTION("PreviewRenderErrorEvent")
    {
        PreviewRenderErrorEvent evt;
        evt.source_line = 20;
        evt.error_message = "Invalid Mermaid syntax";
        evt.block_type = "mermaid";

        CHECK(evt.source_line == 20);
        CHECK(evt.error_message == "Invalid Mermaid syntax");
        CHECK(evt.block_type == "mermaid");
        CHECK(evt.type_name() == "PreviewRenderErrorEvent");
    }

    SECTION("JumpToSourceFromPreviewEvent")
    {
        JumpToSourceFromPreviewEvent evt;
        evt.target_line = 42;
        evt.target_character = 0;
        evt.file_uri = "file:///doc.md";

        CHECK(evt.target_line == 42);
        CHECK(evt.file_uri == "file:///doc.md");
        CHECK(evt.type_name() == "JumpToSourceFromPreviewEvent");
    }
}

// ============================================================================
//  Test 6: DiagnosticsService integration
// ============================================================================

TEST_CASE("DiagnosticsService set/get/count", "[diagnostic][phase10]")
{
    DiagnosticsService service;

    SECTION("Empty by default")
    {
        CHECK(service.total_count() == 0);
        CHECK(service.uris().empty());
    }

    SECTION("Set and get diagnostics")
    {
        std::vector<Diagnostic> diags;
        Diagnostic diag;
        diag.message = "Test error";
        diag.severity = DiagnosticSeverity::kError;
        diag.range.start = {0, 0};
        diag.range.end = {0, 10};
        diags.push_back(diag);

        service.set("file:///test.md", std::move(diags));

        CHECK(service.total_count() == 1);
        CHECK(service.count_by_severity(DiagnosticSeverity::kError) == 1);
        CHECK(service.count_by_severity(DiagnosticSeverity::kWarning) == 0);

        const auto& retrieved = service.get("file:///test.md");
        REQUIRE(retrieved.size() == 1);
        CHECK(retrieved[0].message == "Test error");
    }

    SECTION("Remove and clear")
    {
        Diagnostic diag;
        diag.message = "Warning";
        diag.severity = DiagnosticSeverity::kWarning;
        service.set("file:///a.md", {diag});
        service.set("file:///b.md", {diag});

        CHECK(service.total_count() == 2);
        CHECK(service.uris().size() == 2);

        service.remove("file:///a.md");
        CHECK(service.total_count() == 1);

        service.clear();
        CHECK(service.total_count() == 0);
    }
}

// ============================================================================
//  Test 7: DiagnosticSeverity completeness
// ============================================================================

TEST_CASE("DiagnosticSeverity enum values", "[diagnostic][phase10]")
{
    CHECK(static_cast<int>(DiagnosticSeverity::kError) == 0);
    CHECK(static_cast<int>(DiagnosticSeverity::kWarning) == 1);
    CHECK(static_cast<int>(DiagnosticSeverity::kInformation) == 2);
    CHECK(static_cast<int>(DiagnosticSeverity::kHint) == 3);
}

// ============================================================================
//  Test 8: ThemeColorToken count integrity after Phase 10
// ============================================================================

TEST_CASE("ThemeColorToken count integrity", "[theme][phase10]")
{
    // Phase 10 does not add new tokens, so count should remain at Phase 9 level:
    // EditorQuickFix is the last token added, kColorTokenCount should be stable.
    CHECK(kColorTokenCount > 0);
    CHECK(static_cast<std::size_t>(ThemeColorToken::EditorQuickFix) < kColorTokenCount);
}
