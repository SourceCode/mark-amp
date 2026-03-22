/// @file test_v23_completion_inventory.cpp
/// @brief V23 Phase 01 — Tests for CompletionInventory service.
///
/// Tests cover:
///   - InventoryItem creation, handler/blocker checks
///   - MarkerType and CompletionFamily label conversion
///   - Subsystem auto-tagging from file paths
///   - Severity classification rules
///   - Family classification logic
///   - Query APIs: by subsystem, severity, family, production, test
///   - Feature-chain registration and lookup
///   - Summary generation and blocker counting
///   - JSON and Markdown export

#include "core/CompletionInventory.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;

// ============================================================================
// MarkerType label tests
// ============================================================================

TEST_CASE("MarkerType — label conversion", "[v23][p01][marker]")
{
    CHECK(std::string(marker_type_label(MarkerType::kStub)) == "Stub");
    CHECK(std::string(marker_type_label(MarkerType::kPlaceholder)) == "Placeholder");
    CHECK(std::string(marker_type_label(MarkerType::kTodo)) == "Todo");
    CHECK(std::string(marker_type_label(MarkerType::kFixme)) == "Fixme");
    CHECK(std::string(marker_type_label(MarkerType::kDeferred)) == "Deferred");
    CHECK(std::string(marker_type_label(MarkerType::kForNow)) == "ForNow");
    CHECK(std::string(marker_type_label(MarkerType::kMock)) == "Mock");
    CHECK(std::string(marker_type_label(MarkerType::kFake)) == "Fake");
    CHECK(std::string(marker_type_label(MarkerType::kNotYet)) == "NotYet");
    CHECK(std::string(marker_type_label(MarkerType::kPending)) == "Pending");
    CHECK(std::string(marker_type_label(MarkerType::kRealImplementationWould)) == "RealImplementationWould");
    CHECK(std::string(marker_type_label(MarkerType::kHack)) == "Hack");
}

// ============================================================================
// CompletionFamily label tests
// ============================================================================

TEST_CASE("CompletionFamily — label conversion", "[v23][p01][family]")
{
    CHECK(std::string(completion_family_label(CompletionFamily::kServiceStub)) == "ServiceStub");
    CHECK(std::string(completion_family_label(CompletionFamily::kWorkflowPartial)) == "WorkflowPartial");
    CHECK(std::string(completion_family_label(CompletionFamily::kPanelShell)) == "PanelShell");
    CHECK(std::string(completion_family_label(CompletionFamily::kPlaceholderRendering)) == "PlaceholderRendering");
    CHECK(std::string(completion_family_label(CompletionFamily::kMigrationSeam)) == "MigrationSeam");
    CHECK(std::string(completion_family_label(CompletionFamily::kTestGap)) == "TestGap");
    CHECK(std::string(completion_family_label(CompletionFamily::kTransportStub)) == "TransportStub");
    CHECK(std::string(completion_family_label(CompletionFamily::kPersistenceStub)) == "PersistenceStub");
    CHECK(std::string(completion_family_label(CompletionFamily::kUIPlaceholder)) == "UIPlaceholder");
    CHECK(std::string(completion_family_label(CompletionFamily::kUnclassified)) == "Unclassified");
}

// ============================================================================
// CompletionSeverity label tests
// ============================================================================

TEST_CASE("CompletionSeverity — label conversion", "[v23][p01][severity]")
{
    CHECK(std::string(completion_severity_label(CompletionSeverity::kCritical)) == "Critical");
    CHECK(std::string(completion_severity_label(CompletionSeverity::kHigh)) == "High");
    CHECK(std::string(completion_severity_label(CompletionSeverity::kMedium)) == "Medium");
    CHECK(std::string(completion_severity_label(CompletionSeverity::kLow)) == "Low");
}

// ============================================================================
// InventoryItem struct tests
// ============================================================================

TEST_CASE("InventoryItem — blocker and feature checks", "[v23][p01][item]")
{
    InventoryItem item;
    item.file_path = "src/core/GitService.cpp";
    item.line_number = 42;
    item.marker_type = MarkerType::kStub;
    item.raw_text = "stubbed for testability";

    SECTION("Non-production item is not a blocker")
    {
        item.severity = CompletionSeverity::kCritical;
        item.is_production_path = false;
        CHECK(!item.is_blocker());
    }

    SECTION("Critical production-path item is a blocker")
    {
        item.severity = CompletionSeverity::kCritical;
        item.is_production_path = true;
        CHECK(item.is_blocker());
    }

    SECTION("High severity production-path item is not a blocker")
    {
        item.severity = CompletionSeverity::kHigh;
        item.is_production_path = true;
        CHECK(!item.is_blocker());
    }

    SECTION("blocks_feature is false when no feature assigned")
    {
        CHECK(!item.blocks_feature());
    }

    SECTION("blocks_feature is true when feature is assigned")
    {
        item.blocked_feature = "SourceControl";
        CHECK(item.blocks_feature());
    }
}

// ============================================================================
// Subsystem auto-tagging
// ============================================================================

TEST_CASE("CompletionInventory — subsystem_from_path", "[v23][p01][subsystem]")
{
    CHECK(CompletionInventory::subsystem_from_path("src/canvas/SelectTool.cpp") == "Canvas");
    CHECK(CompletionInventory::subsystem_from_path("src/core/CanvasWorkbench.cpp") == "Canvas");
    CHECK(CompletionInventory::subsystem_from_path("src/core/NotebookCellManager.cpp") == "Notebook");
    CHECK(CompletionInventory::subsystem_from_path("src/core/KernelManager.cpp") == "Notebook");
    CHECK(CompletionInventory::subsystem_from_path("src/core/AIService.cpp") == "AI");
    CHECK(CompletionInventory::subsystem_from_path("src/core/GitService.cpp") == "Git");
    CHECK(CompletionInventory::subsystem_from_path("src/core/CloudSyncService.cpp") == "CloudSync");
    CHECK(CompletionInventory::subsystem_from_path("src/core/SearchService.cpp") == "Search");
    CHECK(CompletionInventory::subsystem_from_path("src/ui/PDFViewerPanel.cpp") == "PDF");
    CHECK(CompletionInventory::subsystem_from_path("src/ui/SearchSidebarPanel.cpp") == "Search");
    CHECK(CompletionInventory::subsystem_from_path("src/ui/SettingsPanel.cpp") == "Settings");
    CHECK(CompletionInventory::subsystem_from_path("src/rendering/HtmlRenderer.cpp") == "Rendering");
    CHECK(CompletionInventory::subsystem_from_path("src/core/SessionRestoreService.cpp") == "Persistence");
    CHECK(CompletionInventory::subsystem_from_path("src/core/ExtensionManagement.cpp") == "Extension");
    CHECK(CompletionInventory::subsystem_from_path("src/core/ThemeEngine.cpp") == "Theme");
    CHECK(CompletionInventory::subsystem_from_path("tests/unit/test_search.cpp") == "Test");
    CHECK(CompletionInventory::subsystem_from_path("src/ui/Toolbar.cpp") == "UI");
    CHECK(CompletionInventory::subsystem_from_path("src/core/EventBus.cpp") == "Core");
}

// ============================================================================
// Severity classification
// ============================================================================

TEST_CASE("CompletionInventory — severity_for", "[v23][p01][severity]")
{
    SECTION("Non-production items are always Low")
    {
        CHECK(CompletionInventory::severity_for(MarkerType::kStub, false) == CompletionSeverity::kLow);
        CHECK(CompletionInventory::severity_for(MarkerType::kFake, false) == CompletionSeverity::kLow);
    }

    SECTION("Production stubs are Critical")
    {
        CHECK(CompletionInventory::severity_for(MarkerType::kStub, true) == CompletionSeverity::kCritical);
        CHECK(CompletionInventory::severity_for(MarkerType::kFake, true) == CompletionSeverity::kCritical);
        CHECK(CompletionInventory::severity_for(MarkerType::kRealImplementationWould, true) == CompletionSeverity::kCritical);
    }

    SECTION("Production placeholders are High")
    {
        CHECK(CompletionInventory::severity_for(MarkerType::kPlaceholder, true) == CompletionSeverity::kHigh);
        CHECK(CompletionInventory::severity_for(MarkerType::kMock, true) == CompletionSeverity::kHigh);
    }

    SECTION("Production deferred are Medium")
    {
        CHECK(CompletionInventory::severity_for(MarkerType::kDeferred, true) == CompletionSeverity::kMedium);
        CHECK(CompletionInventory::severity_for(MarkerType::kNotYet, true) == CompletionSeverity::kMedium);
        CHECK(CompletionInventory::severity_for(MarkerType::kPending, true) == CompletionSeverity::kMedium);
    }

    SECTION("Production TODOs are Low")
    {
        CHECK(CompletionInventory::severity_for(MarkerType::kTodo, true) == CompletionSeverity::kLow);
        CHECK(CompletionInventory::severity_for(MarkerType::kFixme, true) == CompletionSeverity::kLow);
        CHECK(CompletionInventory::severity_for(MarkerType::kForNow, true) == CompletionSeverity::kLow);
        CHECK(CompletionInventory::severity_for(MarkerType::kHack, true) == CompletionSeverity::kLow);
    }
}

// ============================================================================
// Family classification
// ============================================================================

TEST_CASE("CompletionInventory — classify_family", "[v23][p01][family]")
{
    SECTION("Non-production mocks are TestGap")
    {
        CHECK(CompletionInventory::classify_family(MarkerType::kMock, "Search", false)
              == CompletionFamily::kTestGap);
        CHECK(CompletionInventory::classify_family(MarkerType::kFake, "Git", false)
              == CompletionFamily::kTestGap);
    }

    SECTION("Transport-related stubs are TransportStub")
    {
        CHECK(CompletionInventory::classify_family(MarkerType::kStub, "CloudSync", true)
              == CompletionFamily::kTransportStub);
        CHECK(CompletionInventory::classify_family(MarkerType::kStub, "AI", true)
              == CompletionFamily::kTransportStub);
        CHECK(CompletionInventory::classify_family(MarkerType::kStub, "Git", true)
              == CompletionFamily::kTransportStub);
    }

    SECTION("Persistence-related stubs are PersistenceStub")
    {
        CHECK(CompletionInventory::classify_family(MarkerType::kStub, "Search", true)
              == CompletionFamily::kPersistenceStub);
        CHECK(CompletionInventory::classify_family(MarkerType::kStub, "Persistence", true)
              == CompletionFamily::kPersistenceStub);
    }

    SECTION("Other stubs are ServiceStub")
    {
        CHECK(CompletionInventory::classify_family(MarkerType::kStub, "Core", true)
              == CompletionFamily::kServiceStub);
    }

    SECTION("Panel placeholders are PanelShell")
    {
        CHECK(CompletionInventory::classify_family(MarkerType::kPlaceholder, "Panel", true)
              == CompletionFamily::kPanelShell);
        CHECK(CompletionInventory::classify_family(MarkerType::kPlaceholder, "Sidebar", true)
              == CompletionFamily::kPanelShell);
    }

    SECTION("Rendering placeholders are PlaceholderRendering")
    {
        CHECK(CompletionInventory::classify_family(MarkerType::kPlaceholder, "Rendering", true)
              == CompletionFamily::kPlaceholderRendering);
    }

    SECTION("Deferred markers are WorkflowPartial")
    {
        CHECK(CompletionInventory::classify_family(MarkerType::kDeferred, "Core", true)
              == CompletionFamily::kWorkflowPartial);
    }

    SECTION("ForNow markers are MigrationSeam")
    {
        CHECK(CompletionInventory::classify_family(MarkerType::kForNow, "Core", true)
              == CompletionFamily::kMigrationSeam);
    }
}

// ============================================================================
// Registration and queries
// ============================================================================

TEST_CASE("CompletionInventory — registration and counting", "[v23][p01][inventory]")
{
    CompletionInventory inv;
    CHECK(inv.item_count() == 0);

    InventoryItem item1;
    item1.file_path = "src/core/GitService.cpp";
    item1.line_number = 42;
    item1.marker_type = MarkerType::kStub;
    item1.subsystem_tag = "Git";
    item1.severity = CompletionSeverity::kCritical;
    item1.is_production_path = true;
    inv.add_item(std::move(item1));

    CHECK(inv.item_count() == 1);

    InventoryItem item2;
    item2.file_path = "src/core/SearchService.cpp";
    item2.line_number = 10;
    item2.marker_type = MarkerType::kPlaceholder;
    item2.subsystem_tag = "Search";
    item2.severity = CompletionSeverity::kHigh;
    item2.is_production_path = true;

    InventoryItem item3;
    item3.file_path = "tests/unit/test_search.cpp";
    item3.line_number = 5;
    item3.marker_type = MarkerType::kMock;
    item3.subsystem_tag = "Search";
    item3.severity = CompletionSeverity::kLow;
    item3.is_production_path = false;

    std::vector<InventoryItem> batch;
    batch.push_back(std::move(item2));
    batch.push_back(std::move(item3));
    inv.add_items(std::move(batch));

    CHECK(inv.item_count() == 3);
}

TEST_CASE("CompletionInventory — clear", "[v23][p01][inventory]")
{
    CompletionInventory inv;
    InventoryItem item;
    item.file_path = "test.cpp";
    inv.add_item(std::move(item));

    BlockedFeatureChain chain;
    chain.feature_name = "Chain";
    inv.add_feature_chain(std::move(chain));

    CHECK(inv.item_count() == 1);
    CHECK(inv.feature_chain_count() == 1);

    inv.clear();
    CHECK(inv.item_count() == 0);
    CHECK(inv.feature_chain_count() == 0);
}

TEST_CASE("CompletionInventory — query by subsystem", "[v23][p01][query]")
{
    CompletionInventory inv;

    InventoryItem git1;
    git1.subsystem_tag = "Git";
    inv.add_item(std::move(git1));

    InventoryItem git2;
    git2.subsystem_tag = "Git";
    inv.add_item(std::move(git2));

    InventoryItem search1;
    search1.subsystem_tag = "Search";
    inv.add_item(std::move(search1));

    CHECK(inv.items_by_subsystem("Git").size() == 2);
    CHECK(inv.items_by_subsystem("Search").size() == 1);
    CHECK(inv.items_by_subsystem("Canvas").empty());
}

TEST_CASE("CompletionInventory — query by severity", "[v23][p01][query]")
{
    CompletionInventory inv;

    InventoryItem crit;
    crit.severity = CompletionSeverity::kCritical;
    inv.add_item(std::move(crit));

    InventoryItem high;
    high.severity = CompletionSeverity::kHigh;
    inv.add_item(std::move(high));

    InventoryItem low1;
    low1.severity = CompletionSeverity::kLow;
    inv.add_item(std::move(low1));

    InventoryItem low2;
    low2.severity = CompletionSeverity::kLow;
    inv.add_item(std::move(low2));

    CHECK(inv.items_by_severity(CompletionSeverity::kCritical).size() == 1);
    CHECK(inv.items_by_severity(CompletionSeverity::kHigh).size() == 1);
    CHECK(inv.items_by_severity(CompletionSeverity::kLow).size() == 2);
    CHECK(inv.items_by_severity(CompletionSeverity::kMedium).empty());
}

TEST_CASE("CompletionInventory — query by family", "[v23][p01][query]")
{
    CompletionInventory inv;

    InventoryItem svc;
    svc.family = CompletionFamily::kServiceStub;
    inv.add_item(std::move(svc));

    InventoryItem panel;
    panel.family = CompletionFamily::kPanelShell;
    inv.add_item(std::move(panel));

    CHECK(inv.items_by_family(CompletionFamily::kServiceStub).size() == 1);
    CHECK(inv.items_by_family(CompletionFamily::kPanelShell).size() == 1);
    CHECK(inv.items_by_family(CompletionFamily::kTransportStub).empty());
}

TEST_CASE("CompletionInventory — production and test queries", "[v23][p01][query]")
{
    CompletionInventory inv;

    InventoryItem prod;
    prod.is_production_path = true;
    inv.add_item(std::move(prod));

    InventoryItem test1;
    test1.is_production_path = false;
    inv.add_item(std::move(test1));

    InventoryItem test2;
    test2.is_production_path = false;
    inv.add_item(std::move(test2));

    CHECK(inv.production_path_items().size() == 1);
    CHECK(inv.test_only_items().size() == 2);
}

TEST_CASE("CompletionInventory — blocker queries", "[v23][p01][query]")
{
    CompletionInventory inv;

    InventoryItem blocker;
    blocker.severity = CompletionSeverity::kCritical;
    blocker.is_production_path = true;
    inv.add_item(std::move(blocker));

    InventoryItem non_blocker;
    non_blocker.severity = CompletionSeverity::kCritical;
    non_blocker.is_production_path = false;
    inv.add_item(std::move(non_blocker));

    CHECK(inv.blocker_items().size() == 1);
}

TEST_CASE("CompletionInventory — all_subsystems", "[v23][p01][query]")
{
    CompletionInventory inv;

    InventoryItem a;
    a.subsystem_tag = "Git";
    inv.add_item(std::move(a));

    InventoryItem b;
    b.subsystem_tag = "Search";
    inv.add_item(std::move(b));

    InventoryItem c;
    c.subsystem_tag = "Git";
    inv.add_item(std::move(c));

    auto subs = inv.all_subsystems();
    CHECK(subs.size() == 2);
}

// ============================================================================
// Feature-chain linkage
// ============================================================================

TEST_CASE("CompletionInventory — feature chain management", "[v23][p01][chain]")
{
    CompletionInventory inv;

    BlockedFeatureChain chain1;
    chain1.feature_name = "SourceControl";
    chain1.command_chain = "git.commit → GitService.commit()";
    chain1.transport_chain = "GitCommandRunner → process";
    chain1.blocking_item_indices = {0, 1, 2};
    inv.add_feature_chain(std::move(chain1));

    BlockedFeatureChain chain2;
    chain2.feature_name = "Search";
    chain2.persistence_chain = "SearchService → SearchEngine";
    chain2.blocking_item_indices = {3};
    inv.add_feature_chain(std::move(chain2));

    CHECK(inv.feature_chain_count() == 2);

    auto* sc = inv.feature_chain_for("SourceControl");
    REQUIRE(sc != nullptr);
    CHECK(sc->blocker_count() == 3);
    CHECK(sc->command_chain == "git.commit → GitService.commit()");

    auto* search = inv.feature_chain_for("Search");
    REQUIRE(search != nullptr);
    CHECK(search->blocker_count() == 1);

    CHECK(inv.feature_chain_for("Canvas") == nullptr);

    auto all = inv.all_feature_chains();
    CHECK(all.size() == 2);
}

// ============================================================================
// Summary
// ============================================================================

TEST_CASE("CompletionInventory — summary generation", "[v23][p01][summary]")
{
    CompletionInventory inv;

    // 1 critical blocker
    InventoryItem crit;
    crit.severity = CompletionSeverity::kCritical;
    crit.is_production_path = true;
    inv.add_item(std::move(crit));

    // 1 high non-blocker
    InventoryItem high;
    high.severity = CompletionSeverity::kHigh;
    high.is_production_path = true;
    inv.add_item(std::move(high));

    // 1 medium test-only
    InventoryItem med;
    med.severity = CompletionSeverity::kMedium;
    med.is_production_path = false;
    inv.add_item(std::move(med));

    // 1 low test-only
    InventoryItem low;
    low.severity = CompletionSeverity::kLow;
    low.is_production_path = false;
    inv.add_item(std::move(low));

    auto s = inv.summary();
    CHECK(s.total_items == 4);
    CHECK(s.critical == 1);
    CHECK(s.high == 1);
    CHECK(s.medium == 1);
    CHECK(s.low == 1);
    CHECK(s.blockers == 1);
    CHECK(s.production_path == 2);
    CHECK(s.test_only == 2);
    CHECK(s.has_blockers());
    CHECK(s.blocker_rate_pct() == 25.0);
}

TEST_CASE("InventorySummary — zero items", "[v23][p01][summary]")
{
    InventorySummary s;
    CHECK(s.blocker_rate_pct() == 0.0);
    CHECK(!s.has_blockers());
}

// ============================================================================
// JSON export
// ============================================================================

TEST_CASE("CompletionInventory — JSON export", "[v23][p01][export]")
{
    CompletionInventory inv;

    InventoryItem item;
    item.file_path = "src/core/GitService.cpp";
    item.line_number = 42;
    item.marker_type = MarkerType::kStub;
    item.raw_text = "stubbed for testability";
    item.subsystem_tag = "Git";
    item.severity = CompletionSeverity::kCritical;
    item.family = CompletionFamily::kTransportStub;
    item.is_production_path = true;
    inv.add_item(std::move(item));

    auto json = inv.export_json();
    CHECK(json.find("GitService.cpp") != std::string::npos);
    CHECK(json.find("\"line\": 42") != std::string::npos);
    CHECK(json.find("\"marker_type\": \"Stub\"") != std::string::npos);
    CHECK(json.find("\"severity\": \"Critical\"") != std::string::npos);
    CHECK(json.find("\"family\": \"TransportStub\"") != std::string::npos);
    CHECK(json.find("\"blocker\": true") != std::string::npos);
    CHECK(json.find("\"total\": 1") != std::string::npos);
}

// ============================================================================
// Markdown export
// ============================================================================

TEST_CASE("CompletionInventory — Markdown export", "[v23][p01][export]")
{
    CompletionInventory inv;

    InventoryItem item;
    item.file_path = "src/core/SearchService.cpp";
    item.line_number = 10;
    item.marker_type = MarkerType::kPlaceholder;
    item.subsystem_tag = "Search";
    item.severity = CompletionSeverity::kHigh;
    item.family = CompletionFamily::kPersistenceStub;
    item.is_production_path = true;
    inv.add_item(std::move(item));

    BlockedFeatureChain chain;
    chain.feature_name = "FullTextSearch";
    chain.persistence_chain = "SearchService → SearchEngine";
    chain.blocking_item_indices = {0};
    inv.add_feature_chain(std::move(chain));

    auto md = inv.export_markdown();
    CHECK(md.find("V23 Completion Inventory") != std::string::npos);
    CHECK(md.find("SearchService.cpp") != std::string::npos);
    CHECK(md.find("Blocked Feature Chains") != std::string::npos);
    CHECK(md.find("FullTextSearch") != std::string::npos);
}

// ============================================================================
// Integration — end-to-end inventory lifecycle
// ============================================================================

TEST_CASE("Integration — full V23 inventory lifecycle", "[v23][p01][integration]")
{
    CompletionInventory inv;

    // Register production stubs
    InventoryItem git_stub;
    git_stub.file_path = "src/core/GitService.cpp";
    git_stub.line_number = 100;
    git_stub.marker_type = MarkerType::kFake;
    git_stub.raw_text = "fake commit data";
    git_stub.subsystem_tag = CompletionInventory::subsystem_from_path(git_stub.file_path);
    git_stub.severity = CompletionInventory::severity_for(git_stub.marker_type, true);
    git_stub.family = CompletionInventory::classify_family(
        git_stub.marker_type, git_stub.subsystem_tag, true);
    // kFake in production classifies as ServiceStub (not TransportStub)
    git_stub.is_production_path = true;
    git_stub.blocked_feature = "SourceControl";
    inv.add_item(std::move(git_stub));

    InventoryItem search_stub;
    search_stub.file_path = "src/core/SearchService.cpp";
    search_stub.line_number = 50;
    search_stub.marker_type = MarkerType::kStub;
    search_stub.raw_text = "returns empty results";
    search_stub.subsystem_tag = CompletionInventory::subsystem_from_path(search_stub.file_path);
    search_stub.severity = CompletionInventory::severity_for(search_stub.marker_type, true);
    search_stub.family = CompletionInventory::classify_family(
        search_stub.marker_type, search_stub.subsystem_tag, true);
    search_stub.is_production_path = true;
    inv.add_item(std::move(search_stub));

    // Register test mock
    InventoryItem test_mock;
    test_mock.file_path = "tests/unit/test_git.cpp";
    test_mock.line_number = 5;
    test_mock.marker_type = MarkerType::kMock;
    test_mock.raw_text = "mock git state";
    test_mock.subsystem_tag = CompletionInventory::subsystem_from_path(test_mock.file_path);
    test_mock.severity = CompletionInventory::severity_for(test_mock.marker_type, false);
    test_mock.family = CompletionInventory::classify_family(
        test_mock.marker_type, test_mock.subsystem_tag, false);
    test_mock.is_production_path = false;
    inv.add_item(std::move(test_mock));

    // Register feature chain
    BlockedFeatureChain sc_chain;
    sc_chain.feature_name = "SourceControl";
    sc_chain.command_chain = "git.commit → GitService.commit()";
    sc_chain.transport_chain = "GitCommandRunner → process";
    sc_chain.blocking_item_indices = {0};
    inv.add_feature_chain(std::move(sc_chain));

    // Verify counts
    CHECK(inv.item_count() == 3);
    CHECK(inv.production_path_items().size() == 2);
    CHECK(inv.test_only_items().size() == 1);
    CHECK(inv.blocker_items().size() == 2); // both critical + production

    // Verify subsystem classification
    CHECK(inv.items_by_subsystem("Git").size() == 1);
    CHECK(inv.items_by_subsystem("Search").size() == 1);

    // Verify family classification
    // kFake+production → ServiceStub; kStub+Search+production → PersistenceStub; kMock+test → TestGap
    CHECK(inv.items_by_family(CompletionFamily::kServiceStub).size() == 1);
    CHECK(inv.items_by_family(CompletionFamily::kPersistenceStub).size() == 1);
    CHECK(inv.items_by_family(CompletionFamily::kTestGap).size() == 1);

    // Verify summary
    auto s = inv.summary();
    CHECK(s.total_items == 3);
    CHECK(s.critical == 2);
    CHECK(s.blockers == 2);
    CHECK(s.has_blockers());

    // Verify feature chains
    CHECK(inv.feature_chain_count() == 1);
    auto* sc = inv.feature_chain_for("SourceControl");
    REQUIRE(sc != nullptr);
    CHECK(sc->blocker_count() == 1);

    // Verify exports
    auto json = inv.export_json();
    CHECK(!json.empty());
    CHECK(json.find("GitService.cpp") != std::string::npos);

    auto md = inv.export_markdown();
    CHECK(!md.empty());
    CHECK(md.find("SourceControl") != std::string::npos);
}
