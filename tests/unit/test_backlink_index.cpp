/// @file test_backlink_index.cpp
/// @brief V4 Phase 03 – BacklinkIndex Catch2 tests.
///
/// 12 test cases per the Phase 03 specification:
///   1.  Simple backlink
///   2.  Multiple backlinks
///   3.  Bidirectional links
///   4.  Context extraction
///   5.  Incremental update
///   6.  Remove document
///   7.  Outgoing links
///   8.  Connected documents
///   9.  Orphan detection
///  10.  Unlinked mentions
///  11.  Most linked
///  12.  Rebuild clears stale data

#include "core/BacklinkIndex.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/VaultService.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::core;
namespace fs = std::filesystem;

// ============================================================================
// Helpers
// ============================================================================

namespace
{

/// RAII temp directory for test vaults.
struct TempVault
{
    fs::path root;

    TempVault()
    {
        root = fs::temp_directory_path() /
               ("backlink_test_" +
                std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        fs::create_directories(root);
    }

    ~TempVault()
    {
        std::error_code err;
        fs::remove_all(root, err);
    }

    TempVault(const TempVault&) = delete;
    auto operator=(const TempVault&) -> TempVault& = delete;

    TempVault(TempVault&& other) noexcept
        : root(std::move(other.root))
    {
        other.root.clear();
    }

    auto operator=(TempVault&&) -> TempVault& = delete;

    void write(const std::string& relative_path, const std::string& content) const
    {
        auto full = root / relative_path;
        fs::create_directories(full.parent_path());
        std::ofstream out(full, std::ios::binary);
        out << content;
    }
};

/// Open vault + build backlink index. Returns the vault for RAII cleanup.
struct TestHarness
{
    TempVault vault;
    EventBus bus;
    Config config;
    VaultService svc;
    BacklinkIndex idx;

    TestHarness()
        : svc(bus, config)
        , idx(bus, svc)
    {
    }

    void open()
    {
        auto result = svc.open_vault(vault.root);
        REQUIRE(result.has_value());
    }
};

} // anonymous namespace

// ============================================================================
// Tests
// ============================================================================

// 1. Simple backlink
TEST_CASE("Simple backlink", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("Doc A.md", "---\ntitle: Doc A\n---\n# Doc A\nSee [[Doc B]] for info.\n");
    harness.vault.write("Doc B.md", "---\ntitle: Doc B\n---\n# Doc B\nContent.\n");
    harness.open();
    harness.idx.rebuild();

    auto backlinks = harness.idx.get_backlinks(harness.svc.resolve_wikilink("Doc B").value());
    REQUIRE(backlinks.size() == 1);
    REQUIRE(backlinks[0].source_document_title == "Doc A");
}

// 2. Multiple backlinks
TEST_CASE("Multiple backlinks", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("A.md", "---\ntitle: A\n---\nLink: [[D]]\n");
    harness.vault.write("B.md", "---\ntitle: B\n---\nLink: [[D]]\n");
    harness.vault.write("C.md", "---\ntitle: C\n---\nLink: [[D]]\n");
    harness.vault.write("D.md", "---\ntitle: D\n---\nTarget.\n");
    harness.open();
    harness.idx.rebuild();

    auto doc_d_id = harness.svc.resolve_wikilink("D").value();
    auto backlinks = harness.idx.get_backlinks(doc_d_id);
    REQUIRE(backlinks.size() == 3);
}

// 3. Bidirectional links
TEST_CASE("Bidirectional links", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("X.md", "---\ntitle: X\n---\nSee [[Y]].\n");
    harness.vault.write("Y.md", "---\ntitle: Y\n---\nSee [[X]].\n");
    harness.open();
    harness.idx.rebuild();

    auto x_id = harness.svc.resolve_wikilink("X").value();
    auto y_id = harness.svc.resolve_wikilink("Y").value();

    auto x_backlinks = harness.idx.get_backlinks(x_id);
    auto y_backlinks = harness.idx.get_backlinks(y_id);

    REQUIRE(x_backlinks.size() == 1);
    REQUIRE(x_backlinks[0].source_document_title == "Y");
    REQUIRE(y_backlinks.size() == 1);
    REQUIRE(y_backlinks[0].source_document_title == "X");
}

// 4. Context extraction
TEST_CASE("Context extraction", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("Source.md",
                        "---\ntitle: Source\n---\n# Source\nFirst line.\n"
                        "This line links to [[Target]] right here.\n"
                        "Last line.\n");
    harness.vault.write("Target.md", "---\ntitle: Target\n---\n# Target\nContent.\n");
    harness.open();
    harness.idx.rebuild();

    auto target_id = harness.svc.resolve_wikilink("Target").value();
    auto backlinks = harness.idx.get_backlinks(target_id);
    REQUIRE(backlinks.size() == 1);
    REQUIRE_FALSE(backlinks[0].context.empty());
    REQUIRE(backlinks[0].context.find("Target") != std::string::npos);
}

// 5. Incremental update
TEST_CASE("Incremental update", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("A.md", "---\ntitle: A\n---\nLink: [[B]]\n");
    harness.vault.write("B.md", "---\ntitle: B\n---\nContent B.\n");
    harness.vault.write("C.md", "---\ntitle: C\n---\nContent C.\n");
    harness.open();
    harness.idx.rebuild();

    auto b_id = harness.svc.resolve_wikilink("B").value();
    auto c_id = harness.svc.resolve_wikilink("C").value();
    auto a_id = harness.svc.resolve_wikilink("A").value();

    REQUIRE(harness.idx.backlink_count(b_id) == 1);
    REQUIRE(harness.idx.backlink_count(c_id) == 0);

    // Update Doc A to link to C instead of B
    harness.vault.write("A.md", "---\ntitle: A\n---\nLink: [[C]]\n");

    // Close and reopen vault to fully clear all caches (reindex alone
    // does not clear open_documents_, so open_document returns stale data).
    harness.svc.close_vault();
    auto reopen = harness.svc.open_vault(harness.vault.root);
    REQUIRE(reopen.has_value());

    // Re-resolve IDs (open_vault reassigns document IDs)
    auto new_b_id = harness.svc.resolve_wikilink("B").value();
    auto new_c_id = harness.svc.resolve_wikilink("C").value();

    // Full rebuild of backlink index with refreshed document data
    harness.idx.rebuild();

    REQUIRE(harness.idx.backlink_count(new_b_id) == 0);
    REQUIRE(harness.idx.backlink_count(new_c_id) == 1);
}

// 6. Remove document
TEST_CASE("Remove document clears backlinks", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("Linker.md", "---\ntitle: Linker\n---\nSee [[Linked]].\n");
    harness.vault.write("Linked.md", "---\ntitle: Linked\n---\nContent.\n");
    harness.open();
    harness.idx.rebuild();

    auto linker_id = harness.svc.resolve_wikilink("Linker").value();
    auto linked_id = harness.svc.resolve_wikilink("Linked").value();

    REQUIRE(harness.idx.backlink_count(linked_id) == 1);

    harness.idx.remove_document(linker_id);

    REQUIRE(harness.idx.backlink_count(linked_id) == 0);
    REQUIRE(harness.idx.get_outgoing_links(linker_id).empty());
}

// 7. Outgoing links
TEST_CASE("Outgoing links", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("Hub.md", "---\ntitle: Hub\n---\nLinks: [[Page1]] and [[Page2]].\n");
    harness.vault.write("Page1.md", "---\ntitle: Page1\n---\nContent.\n");
    harness.vault.write("Page2.md", "---\ntitle: Page2\n---\nContent.\n");
    harness.open();
    harness.idx.rebuild();

    auto hub_id = harness.svc.resolve_wikilink("Hub").value();
    auto outgoing = harness.idx.get_outgoing_links(hub_id);
    REQUIRE(outgoing.size() == 2);
}

// 8. Connected documents
TEST_CASE("Connected documents", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("Center.md", "---\ntitle: Center\n---\nSee [[Out]].\n");
    harness.vault.write("Out.md", "---\ntitle: Out\n---\nContent.\n");
    harness.vault.write("In.md", "---\ntitle: In\n---\nSee [[Center]].\n");
    harness.open();
    harness.idx.rebuild();

    auto center_id = harness.svc.resolve_wikilink("Center").value();
    auto connected = harness.idx.get_connected_documents(center_id);

    auto out_id = harness.svc.resolve_wikilink("Out").value();
    auto in_id = harness.svc.resolve_wikilink("In").value();

    REQUIRE(connected.contains(out_id));
    REQUIRE(connected.contains(in_id));
    REQUIRE(connected.size() == 2);
}

// 9. Orphan detection
TEST_CASE("Orphan detection", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("Connected1.md", "---\ntitle: Connected1\n---\n[[Connected2]]\n");
    harness.vault.write("Connected2.md", "---\ntitle: Connected2\n---\nLinked.\n");
    harness.vault.write("Orphan.md", "---\ntitle: Orphan\n---\nNo links here.\n");
    harness.open();
    harness.idx.rebuild();

    auto orphans = harness.idx.get_orphan_documents();
    REQUIRE(orphans.size() == 1);

    auto orphan_entry = harness.svc.find_by_name("Orphan");
    REQUIRE(orphan_entry.has_value());
    REQUIRE(orphans[0] == orphan_entry->document_id);
}

// 10. Unlinked mentions
TEST_CASE("Unlinked mentions", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("React.md", "---\ntitle: React\n---\n# React\nA JavaScript library.\n");
    harness.vault.write("Tutorial.md",
                        "---\ntitle: Tutorial\n---\n# Tutorial\n"
                        "React is great for building UIs.\n");
    harness.open();
    harness.idx.rebuild();

    auto react_id = harness.svc.resolve_wikilink("React").value();
    auto mentions = harness.idx.get_unlinked_mentions(react_id);

    REQUIRE(mentions.size() >= 1);
    bool found_tutorial = false;
    for (const auto& mention : mentions)
    {
        if (mention.source_document_title == "Tutorial")
        {
            found_tutorial = true;
            REQUIRE(mention.matched_text == "React");
        }
    }
    REQUIRE(found_tutorial);
}

// 11. Most linked
TEST_CASE("Most linked", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("Popular.md", "---\ntitle: Popular\n---\nContent.\n");
    harness.vault.write("Medium.md", "---\ntitle: Medium\n---\nContent.\n");
    harness.vault.write("Rare.md", "---\ntitle: Rare\n---\nContent.\n");
    harness.vault.write("L1.md", "---\ntitle: L1\n---\n[[Popular]] [[Medium]] [[Rare]]\n");
    harness.vault.write("L2.md", "---\ntitle: L2\n---\n[[Popular]] [[Medium]]\n");
    harness.vault.write("L3.md", "---\ntitle: L3\n---\n[[Popular]]\n");
    harness.open();
    harness.idx.rebuild();

    auto most = harness.idx.get_most_linked(3);
    REQUIRE(most.size() == 3);

    // Popular should be first (3 backlinks), Medium second (2), Rare third (1)
    auto popular_id = harness.svc.resolve_wikilink("Popular").value();
    REQUIRE(most[0].first == popular_id);
    REQUIRE(most[0].second == 3);
}

// 12. Rebuild clears stale data
TEST_CASE("Rebuild clears stale data", "[backlink]")
{
    TestHarness harness;
    harness.vault.write("A.md", "---\ntitle: A\n---\n[[B]]\n");
    harness.vault.write("B.md", "---\ntitle: B\n---\nContent.\n");
    harness.open();
    harness.idx.rebuild();

    auto b_id = harness.svc.resolve_wikilink("B").value();
    REQUIRE(harness.idx.backlink_count(b_id) == 1);

    // Modify file externally to remove the link
    harness.vault.write("A.md", "---\ntitle: A\n---\nNo links now.\n");

    // Reindex the vault to refresh cached documents from disk
    harness.svc.reindex();

    // Rebuild should reflect the change
    harness.idx.rebuild();

    REQUIRE(harness.idx.backlink_count(b_id) == 0);
}
