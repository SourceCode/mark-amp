/// @file test_vault_service.cpp
/// @brief V4 Phase 02 – VaultService Catch2 tests.
///
/// 14 test cases per the Phase 02 specification:
///   1.  Open vault -- scans files
///   2.  Open vault -- skips hidden dirs
///   3.  Create document
///   4.  Create document with frontmatter
///   5.  Find by name
///   6.  Find by alias
///   7.  Find by tag
///   8.  Resolve wikilink
///   9.  Resolve wikilink case-insensitive
///  10.  Broken links detection
///  11.  Rename document updates links
///  12.  Delete document
///  13.  List all tags
///  14.  Reindex

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

/// RAII temp directory: creates on construction, removes on destruction.
struct TempVault
{
    fs::path root;

    TempVault()
    {
        root = fs::temp_directory_path() /
               ("vault_test_" +
                std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        fs::create_directories(root);
    }

    ~TempVault()
    {
        std::error_code error_code;
        fs::remove_all(root, error_code);
    }

    TempVault(const TempVault&) = delete;
    auto operator=(const TempVault&) -> TempVault& = delete;

    TempVault(TempVault&& other) noexcept
        : root(std::move(other.root))
    {
        other.root.clear();
    }

    auto operator=(TempVault&&) -> TempVault& = delete;

    /// Write a markdown file relative to the vault root.
    void write(const std::string& relative_path, const std::string& content) const
    {
        auto full_path = root / relative_path;
        fs::create_directories(full_path.parent_path());
        std::ofstream out(full_path, std::ios::binary);
        out << content;
    }
};

/// Standard 3-file vault used by many tests.
auto make_standard_vault() -> TempVault
{
    TempVault vault;
    vault.write("Alpha.md", "---\ntitle: Alpha\ntags:\n  - project\n---\n# Alpha\nContent.\n");
    vault.write("Beta.md",
                "---\ntitle: Beta\ntags:\n  - project\naliases:\n  - BetaAlias\n---\n# Beta\n");
    vault.write("Gamma.md", "---\ntitle: Gamma\ntags:\n  - notes\n---\n# Gamma\n");
    return vault;
}

} // anonymous namespace

// ============================================================================
// Tests
// ============================================================================

// 1. Open vault -- scans files
TEST_CASE("Open vault -- scans files", "[vault]")
{
    auto vault = make_standard_vault();
    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    auto result = svc.open_vault(vault.root);
    REQUIRE(result.has_value());
    REQUIRE(svc.is_open());
    REQUIRE(svc.document_count() == 3);

    auto docs = svc.list_documents();
    REQUIRE(docs.size() == 3);

    svc.close_vault();
    REQUIRE_FALSE(svc.is_open());
    REQUIRE(svc.document_count() == 0);
}

// 2. Open vault -- skips hidden dirs
TEST_CASE("Open vault -- skips hidden dirs", "[vault]")
{
    TempVault vault;
    vault.write("Visible.md", "# Visible\n");
    vault.write(".hidden/Secret.md", "# Secret\n");
    vault.write(".obsidian/config.md", "# Config\n");
    vault.write(".git/readme.md", "# Git\n");

    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    auto result = svc.open_vault(vault.root);
    REQUIRE(result.has_value());
    REQUIRE(svc.document_count() == 1);

    auto docs = svc.list_documents();
    REQUIRE(docs.size() == 1);
    REQUIRE(docs[0].title == "Visible");

    svc.close_vault();
}

// 3. Create document
TEST_CASE("Create document", "[vault]")
{
    TempVault vault;
    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    auto result = svc.open_vault(vault.root);
    REQUIRE(result.has_value());

    auto doc_result = svc.create_document("Hello World");
    REQUIRE(doc_result.has_value());

    auto doc_model = doc_result.value();
    REQUIRE(doc_model->title() == "Hello World");

    // File should exist on disk
    REQUIRE(fs::exists(vault.root / "Hello World.md"));

    // Document should be in the index
    REQUIRE(svc.document_count() == 1);
    auto entry = svc.find_by_name("Hello World");
    REQUIRE(entry.has_value());
    REQUIRE(entry->title == "Hello World");

    svc.close_vault();
}

// 4. Create document with frontmatter
TEST_CASE("Create document with frontmatter", "[vault]")
{
    TempVault vault;
    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    REQUIRE(svc.open_vault(vault.root).has_value());

    Frontmatter fm;
    fm.title = "Tagged Note";
    fm.tags = {"project", "important"};
    fm.aliases = {"TN"};

    auto doc_result = svc.create_document("Tagged Note", "", fm);
    REQUIRE(doc_result.has_value());

    // Read back the file and verify frontmatter persisted
    std::ifstream reader(vault.root / "Tagged Note.md", std::ios::binary);
    REQUIRE(reader.is_open());
    std::string contents((std::istreambuf_iterator<char>(reader)),
                         std::istreambuf_iterator<char>());

    REQUIRE(contents.find("title: Tagged Note") != std::string::npos);
    REQUIRE(contents.find("project") != std::string::npos);
    REQUIRE(contents.find("important") != std::string::npos);

    // Tags should be in index
    auto tagged_docs = svc.find_by_tag("project");
    REQUIRE(tagged_docs.size() == 1);
    REQUIRE(tagged_docs[0].title == "Tagged Note");

    // Alias should be in index
    auto by_alias = svc.find_by_name("TN");
    REQUIRE(by_alias.has_value());
    REQUIRE(by_alias->title == "Tagged Note");

    svc.close_vault();
}

// 5. Find by name
TEST_CASE("Find by name", "[vault]")
{
    auto vault = make_standard_vault();
    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    REQUIRE(svc.open_vault(vault.root).has_value());

    auto entry = svc.find_by_name("Alpha");
    REQUIRE(entry.has_value());
    REQUIRE(entry->title == "Alpha");

    // Non-existent
    auto missing = svc.find_by_name("DoesNotExist");
    REQUIRE_FALSE(missing.has_value());

    svc.close_vault();
}

// 6. Find by alias
TEST_CASE("Find by alias", "[vault]")
{
    auto vault = make_standard_vault();
    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    REQUIRE(svc.open_vault(vault.root).has_value());

    auto entry = svc.find_by_name("BetaAlias");
    REQUIRE(entry.has_value());
    REQUIRE(entry->title == "Beta");

    svc.close_vault();
}

// 7. Find by tag
TEST_CASE("Find by tag", "[vault]")
{
    auto vault = make_standard_vault();
    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    REQUIRE(svc.open_vault(vault.root).has_value());

    auto project_docs = svc.find_by_tag("project");
    REQUIRE(project_docs.size() == 2);

    // Verify titles
    std::vector<std::string> titles;
    for (const auto& doc : project_docs)
    {
        titles.push_back(doc.title);
    }
    std::sort(titles.begin(), titles.end());
    REQUIRE(titles[0] == "Alpha");
    REQUIRE(titles[1] == "Beta");

    auto notes_docs = svc.find_by_tag("notes");
    REQUIRE(notes_docs.size() == 1);
    REQUIRE(notes_docs[0].title == "Gamma");

    svc.close_vault();
}

// 8. Resolve wikilink
TEST_CASE("Resolve wikilink", "[vault]")
{
    auto vault = make_standard_vault();
    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    REQUIRE(svc.open_vault(vault.root).has_value());

    auto resolved = svc.resolve_wikilink("Alpha");
    REQUIRE(resolved.has_value());

    // The resolved ID should be in the index
    auto entry = svc.find_by_name("Alpha");
    REQUIRE(entry.has_value());
    REQUIRE(resolved.value() == entry->document_id);

    svc.close_vault();
}

// 9. Resolve wikilink case-insensitive
TEST_CASE("Resolve wikilink case-insensitive", "[vault]")
{
    auto vault = make_standard_vault();
    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    REQUIRE(svc.open_vault(vault.root).has_value());

    auto resolved_lower = svc.resolve_wikilink("alpha");
    REQUIRE(resolved_lower.has_value());

    auto resolved_upper = svc.resolve_wikilink("ALPHA");
    REQUIRE(resolved_upper.has_value());

    // Both should resolve to the same document ID
    REQUIRE(resolved_lower.value() == resolved_upper.value());

    svc.close_vault();
}

// 10. Broken links detection
TEST_CASE("Broken links detection", "[vault]")
{
    TempVault vault;
    vault.write("Source.md", "---\ntitle: Source\n---\n# Source\nLink to [[NonExistent]].\n");
    vault.write("Target.md", "---\ntitle: Target\n---\n# Target\nContent.\n");

    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    REQUIRE(svc.open_vault(vault.root).has_value());

    // Open the source document so outgoing_links() is populated
    auto doc_result = svc.open_document("Source.md");
    REQUIRE(doc_result.has_value());

    auto broken = svc.find_broken_links();
    REQUIRE(broken.size() == 1);
    REQUIRE(broken[0].second.target == "NonExistent");

    svc.close_vault();
}

// 11. Rename document updates links
TEST_CASE("Rename document updates links", "[vault]")
{
    TempVault vault;
    vault.write("Doc A.md", "---\ntitle: Doc A\n---\n# Doc A\nSee [[Doc B]] for details.\n");
    vault.write("Doc B.md", "---\ntitle: Doc B\n---\n# Doc B\nContent.\n");

    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    REQUIRE(svc.open_vault(vault.root).has_value());

    // Open both documents so open_documents_ contains them
    auto doc_a_result = svc.open_document("Doc A.md");
    REQUIRE(doc_a_result.has_value());

    auto doc_b_result = svc.open_document("Doc B.md");
    REQUIRE(doc_b_result.has_value());

    // Find Doc B's document ID
    auto entry_b = svc.find_by_name("Doc B");
    REQUIRE(entry_b.has_value());

    // Rename Doc B to Doc C
    auto rename_result = svc.rename_document(entry_b->document_id, "Doc C");
    REQUIRE(rename_result.has_value());

    // Verify file renamed on disk
    REQUIRE_FALSE(fs::exists(vault.root / "Doc B.md"));
    REQUIRE(fs::exists(vault.root / "Doc C.md"));

    // Verify Doc A's link was updated
    auto updated_doc_a = doc_a_result.value();
    REQUIRE(updated_doc_a->markdown().find("[[Doc C]]") != std::string::npos);
    REQUIRE(updated_doc_a->markdown().find("[[Doc B]]") == std::string::npos);

    // Verify rename is reflected in the index
    auto entry_c = svc.find_by_name("Doc C");
    REQUIRE(entry_c.has_value());
    REQUIRE(entry_c->document_id == entry_b->document_id);

    // Old name should no longer resolve
    REQUIRE_FALSE(svc.find_by_name("Doc B").has_value());

    svc.close_vault();
}

// 12. Delete document
TEST_CASE("Delete document", "[vault]")
{
    TempVault vault;
    vault.write("Deleteme.md", "---\ntitle: Deleteme\n---\n# Deleteme\n");

    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    REQUIRE(svc.open_vault(vault.root).has_value());
    REQUIRE(svc.document_count() == 1);

    auto entry = svc.find_by_name("Deleteme");
    REQUIRE(entry.has_value());

    auto delete_result = svc.delete_document(entry->document_id);
    REQUIRE(delete_result.has_value());

    // Verify removed from index
    REQUIRE(svc.document_count() == 0);
    REQUIRE_FALSE(svc.find_by_name("Deleteme").has_value());

    // Verify removed from disk
    REQUIRE_FALSE(fs::exists(vault.root / "Deleteme.md"));

    svc.close_vault();
}

// 13. List all tags
TEST_CASE("List all tags", "[vault]")
{
    auto vault = make_standard_vault();
    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    REQUIRE(svc.open_vault(vault.root).has_value());

    auto tags = svc.list_all_tags();

    // The standard vault has tags: project (Alpha, Beta), notes (Gamma)
    REQUIRE(tags.size() == 2);

    // Tags should be sorted (set-based collection)
    REQUIRE(tags[0] == "notes");
    REQUIRE(tags[1] == "project");

    svc.close_vault();
}

// 14. Reindex
TEST_CASE("Reindex", "[vault]")
{
    TempVault vault;
    vault.write("Original.md", "---\ntitle: Original\ntags:\n  - v1\n---\n# Original\n");

    EventBus bus;
    Config config;
    VaultService svc(bus, config);

    REQUIRE(svc.open_vault(vault.root).has_value());
    REQUIRE(svc.document_count() == 1);

    // Track event
    bool reindex_event_received = false;
    int reindex_count = 0;
    auto sub = bus.subscribe<events::VaultReindexedEvent>(
        std::function<void(const events::VaultReindexedEvent&)>(
            [&](const events::VaultReindexedEvent& evt)
            {
                reindex_event_received = true;
                reindex_count = evt.document_count;
            }));

    // Externally add a file
    vault.write("External.md", "---\ntitle: External\ntags:\n  - v2\n---\n# External\n");

    // Reindex should pick it up
    svc.reindex();

    REQUIRE(svc.document_count() == 2);
    REQUIRE(reindex_event_received);
    REQUIRE(reindex_count == 2);

    auto ext = svc.find_by_name("External");
    REQUIRE(ext.has_value());
    REQUIRE(ext->title == "External");

    svc.close_vault();
}
