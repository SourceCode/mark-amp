/// @file test_v23_severity_policy.cpp
/// @brief V23 Phase 01 — Tests for CompletionSeverityPolicy.
///
/// Tests cover:
///   - Waiver management: add, lookup, clear
///   - Effective severity with and without waivers
///   - Release gate pass/fail behavior
///   - Integration with CompletionInventory
///   - JSON and Markdown gate export

#include "core/CompletionSeverityPolicy.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;

// ============================================================================
// Waiver management
// ============================================================================

TEST_CASE("CompletionSeverityPolicy — waiver management", "[v23][p01][waiver]")
{
    CompletionSeverityPolicy policy;

    SECTION("No waivers initially")
    {
        CHECK(policy.waiver_count() == 0);
        CHECK(!policy.has_waiver("file.cpp:10"));
        CHECK(policy.get_waiver("file.cpp:10") == nullptr);
    }

    SECTION("Add and find waiver")
    {
        SeverityWaiver waiver;
        waiver.item_id = "src/core/GitService.cpp:42";
        waiver.reason = "Experimental feature";
        waiver.owner = "team-lead";
        waiver.original_severity = CompletionSeverity::kCritical;
        waiver.waived_severity = CompletionSeverity::kLow;
        policy.add_waiver(std::move(waiver));

        CHECK(policy.waiver_count() == 1);
        CHECK(policy.has_waiver("src/core/GitService.cpp:42"));

        auto* found = policy.get_waiver("src/core/GitService.cpp:42");
        REQUIRE(found != nullptr);
        CHECK(found->reason == "Experimental feature");
        CHECK(found->owner == "team-lead");
    }

    SECTION("all_waivers returns all")
    {
        SeverityWaiver w1;
        w1.item_id = "a.cpp:1";
        policy.add_waiver(std::move(w1));

        SeverityWaiver w2;
        w2.item_id = "b.cpp:2";
        policy.add_waiver(std::move(w2));

        CHECK(policy.all_waivers().size() == 2);
    }

    SECTION("clear_waivers removes all")
    {
        SeverityWaiver w;
        w.item_id = "c.cpp:3";
        policy.add_waiver(std::move(w));
        CHECK(policy.waiver_count() == 1);

        policy.clear_waivers();
        CHECK(policy.waiver_count() == 0);
    }
}

// ============================================================================
// Effective severity
// ============================================================================

TEST_CASE("CompletionSeverityPolicy — effective severity", "[v23][p01][severity]")
{
    CompletionSeverityPolicy policy;

    InventoryItem item;
    item.file_path = "src/core/GitService.cpp";
    item.line_number = 42;
    item.severity = CompletionSeverity::kCritical;
    item.is_production_path = true;

    SECTION("Without waiver returns original severity")
    {
        CHECK(policy.effective_severity(item) == CompletionSeverity::kCritical);
    }

    SECTION("With waiver returns waived severity")
    {
        SeverityWaiver waiver;
        waiver.item_id = "src/core/GitService.cpp:42";
        waiver.waived_severity = CompletionSeverity::kLow;
        policy.add_waiver(std::move(waiver));

        CHECK(policy.effective_severity(item) == CompletionSeverity::kLow);
    }
}

// ============================================================================
// Effective blocker
// ============================================================================

TEST_CASE("CompletionSeverityPolicy — is_effective_blocker", "[v23][p01][blocker]")
{
    CompletionSeverityPolicy policy;

    InventoryItem blocker;
    blocker.file_path = "src/core/GitService.cpp";
    blocker.line_number = 42;
    blocker.severity = CompletionSeverity::kCritical;
    blocker.is_production_path = true;

    SECTION("Blocker without waiver is effective blocker")
    {
        CHECK(policy.is_effective_blocker(blocker));
    }

    SECTION("Blocker with waiver is not effective blocker")
    {
        SeverityWaiver waiver;
        waiver.item_id = "src/core/GitService.cpp:42";
        waiver.waived_severity = CompletionSeverity::kLow;
        policy.add_waiver(std::move(waiver));

        CHECK(!policy.is_effective_blocker(blocker));
    }

    SECTION("Non-production critical is not effective blocker")
    {
        InventoryItem test_item;
        test_item.file_path = "test.cpp";
        test_item.line_number = 1;
        test_item.severity = CompletionSeverity::kCritical;
        test_item.is_production_path = false;
        CHECK(!policy.is_effective_blocker(test_item));
    }
}

// ============================================================================
// Release gate
// ============================================================================

TEST_CASE("CompletionSeverityPolicy — release gate passes with no items", "[v23][p01][gate]")
{
    CompletionSeverityPolicy policy;
    CompletionInventory inv;

    auto result = policy.check_release_gate(inv);
    CHECK(result.passes);
    CHECK(result.is_clear());
    CHECK(result.total_items == 0);
    CHECK(result.effective_blockers == 0);
}

TEST_CASE("CompletionSeverityPolicy — release gate fails with blockers", "[v23][p01][gate]")
{
    CompletionSeverityPolicy policy;
    CompletionInventory inv;

    InventoryItem blocker;
    blocker.file_path = "src/core/GitService.cpp";
    blocker.line_number = 42;
    blocker.marker_type = MarkerType::kStub;
    blocker.raw_text = "stubbed for testability";
    blocker.severity = CompletionSeverity::kCritical;
    blocker.is_production_path = true;
    inv.add_item(std::move(blocker));

    auto result = policy.check_release_gate(inv);
    CHECK(!result.passes);
    CHECK(!result.is_clear());
    CHECK(result.total_items == 1);
    CHECK(result.blockers == 1);
    CHECK(result.effective_blockers == 1);
    CHECK(result.blocking_reasons.size() == 1);
}

TEST_CASE("CompletionSeverityPolicy — release gate passes when blocker is waived", "[v23][p01][gate]")
{
    CompletionSeverityPolicy policy;
    CompletionInventory inv;

    InventoryItem blocker;
    blocker.file_path = "src/core/GitService.cpp";
    blocker.line_number = 42;
    blocker.marker_type = MarkerType::kStub;
    blocker.raw_text = "stubbed for testability";
    blocker.severity = CompletionSeverity::kCritical;
    blocker.is_production_path = true;
    inv.add_item(std::move(blocker));

    SeverityWaiver waiver;
    waiver.item_id = "src/core/GitService.cpp:42";
    waiver.reason = "Not shipping in V23";
    waiver.waived_severity = CompletionSeverity::kLow;
    policy.add_waiver(std::move(waiver));

    auto result = policy.check_release_gate(inv);
    CHECK(result.passes);
    CHECK(result.waived == 1);
    CHECK(result.effective_blockers == 0);
}

TEST_CASE("CompletionSeverityPolicy — release gate blocks on High production items", "[v23][p01][gate]")
{
    CompletionSeverityPolicy policy;
    CompletionInventory inv;

    InventoryItem high;
    high.file_path = "src/ui/PDFViewerPanel.cpp";
    high.line_number = 15;
    high.marker_type = MarkerType::kPlaceholder;
    high.raw_text = "placeholder rendering";
    high.severity = CompletionSeverity::kHigh;
    high.is_production_path = true;
    inv.add_item(std::move(high));

    auto result = policy.check_release_gate(inv);
    CHECK(!result.passes);
    CHECK(result.effective_blockers == 1);
}

TEST_CASE("CompletionSeverityPolicy — release gate passes with only low items", "[v23][p01][gate]")
{
    CompletionSeverityPolicy policy;
    CompletionInventory inv;

    InventoryItem low;
    low.file_path = "src/core/Logger.cpp";
    low.line_number = 5;
    low.marker_type = MarkerType::kTodo;
    low.raw_text = "clean up logging format";
    low.severity = CompletionSeverity::kLow;
    low.is_production_path = true;
    inv.add_item(std::move(low));

    InventoryItem test_item;
    test_item.file_path = "tests/unit/test_git.cpp";
    test_item.line_number = 3;
    test_item.marker_type = MarkerType::kMock;
    test_item.raw_text = "mock git state";
    test_item.severity = CompletionSeverity::kLow;
    test_item.is_production_path = false;
    inv.add_item(std::move(test_item));

    auto result = policy.check_release_gate(inv);
    CHECK(result.passes);
    CHECK(result.is_clear());
}

// ============================================================================
// Export
// ============================================================================

TEST_CASE("CompletionSeverityPolicy — JSON gate export", "[v23][p01][export]")
{
    CompletionSeverityPolicy policy;
    CompletionInventory inv;

    InventoryItem item;
    item.file_path = "src/core/GitService.cpp";
    item.line_number = 42;
    item.marker_type = MarkerType::kStub;
    item.raw_text = "stubbed";
    item.severity = CompletionSeverity::kCritical;
    item.is_production_path = true;
    inv.add_item(std::move(item));

    auto result = policy.check_release_gate(inv);
    auto json = policy.export_gate_json(result);

    CHECK(json.find("\"passes\": false") != std::string::npos);
    CHECK(json.find("\"effective_blockers\": 1") != std::string::npos);
    CHECK(json.find("GitService.cpp") != std::string::npos);
}

TEST_CASE("CompletionSeverityPolicy — Markdown gate export", "[v23][p01][export]")
{
    CompletionSeverityPolicy policy;
    CompletionInventory inv;

    InventoryItem item;
    item.file_path = "src/core/SearchService.cpp";
    item.line_number = 10;
    item.marker_type = MarkerType::kPlaceholder;
    item.raw_text = "placeholder search";
    item.severity = CompletionSeverity::kHigh;
    item.is_production_path = true;
    inv.add_item(std::move(item));

    auto result = policy.check_release_gate(inv);
    auto md = policy.export_gate_markdown(result);

    CHECK(md.find("V23 Release Gate") != std::string::npos);
    CHECK(md.find("FAIL") != std::string::npos);
    CHECK(md.find("Blocking Reasons") != std::string::npos);
}

// ============================================================================
// Integration — policy + inventory + waivers
// ============================================================================

TEST_CASE("Integration — full severity policy lifecycle", "[v23][p01][integration]")
{
    CompletionSeverityPolicy policy;
    CompletionInventory inv;

    // 1. Add critical production blocker
    InventoryItem git;
    git.file_path = "src/core/GitService.cpp";
    git.line_number = 100;
    git.marker_type = MarkerType::kFake;
    git.raw_text = "fake commit data";
    git.severity = CompletionSeverity::kCritical;
    git.is_production_path = true;
    inv.add_item(std::move(git));

    // 2. Add high production item
    InventoryItem pdf;
    pdf.file_path = "src/ui/PDFViewerPanel.cpp";
    pdf.line_number = 20;
    pdf.marker_type = MarkerType::kPlaceholder;
    pdf.raw_text = "placeholder viewer";
    pdf.severity = CompletionSeverity::kHigh;
    pdf.is_production_path = true;
    inv.add_item(std::move(pdf));

    // 3. Add low test item (should not block)
    InventoryItem test_item;
    test_item.file_path = "tests/unit/test.cpp";
    test_item.line_number = 5;
    test_item.marker_type = MarkerType::kTodo;
    test_item.raw_text = "add more tests";
    test_item.severity = CompletionSeverity::kLow;
    test_item.is_production_path = false;
    inv.add_item(std::move(test_item));

    // Gate should fail (2 effective blockers)
    auto result1 = policy.check_release_gate(inv);
    CHECK(!result1.passes);
    CHECK(result1.effective_blockers == 2);

    // Waive the Git blocker
    SeverityWaiver git_waiver;
    git_waiver.item_id = "src/core/GitService.cpp:100";
    git_waiver.reason = "Shipped behind feature flag";
    git_waiver.waived_severity = CompletionSeverity::kLow;
    policy.add_waiver(std::move(git_waiver));

    // Gate should still fail (PDF high item)
    auto result2 = policy.check_release_gate(inv);
    CHECK(!result2.passes);
    CHECK(result2.effective_blockers == 1);
    CHECK(result2.waived == 1);

    // Waive the PDF item
    SeverityWaiver pdf_waiver;
    pdf_waiver.item_id = "src/ui/PDFViewerPanel.cpp:20";
    pdf_waiver.reason = "PDF feature gated";
    pdf_waiver.waived_severity = CompletionSeverity::kLow;
    policy.add_waiver(std::move(pdf_waiver));

    // Gate should pass
    auto result3 = policy.check_release_gate(inv);
    CHECK(result3.passes);
    CHECK(result3.is_clear());
    CHECK(result3.waived == 1); // Only the Critical blocker counts as "waived blocker"

    // Export passes
    auto json = policy.export_gate_json(result3);
    CHECK(json.find("\"passes\": true") != std::string::npos);

    auto md = policy.export_gate_markdown(result3);
    CHECK(md.find("PASS") != std::string::npos);
}
