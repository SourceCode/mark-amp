#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/TagService.h"
#include "core/VaultService.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace
{

/// Create a temp vault with markdown files for testing.
class TagTestFixture
{
public:
    TagTestFixture()
        : vault_dir_(fs::temp_directory_path() /
                     ("markamp_tag_test_" +
                      std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
        , vault_service_(event_bus_, config_)
        , tag_service_(event_bus_, vault_service_)
    {
        fs::create_directories(vault_dir_);
    }

    ~TagTestFixture()
    {
        vault_service_.close_vault();
        fs::remove_all(vault_dir_);
    }

    auto create_file(const std::string& name, const std::string& content) -> void
    {
        const auto file_path = vault_dir_ / name;
        fs::create_directories(file_path.parent_path());
        std::ofstream file(file_path);
        file << content;
    }

    auto open_vault() -> void
    {
        auto result = vault_service_.open_vault(vault_dir_);
        REQUIRE(result.has_value());
    }

    auto reopen_vault() -> void
    {
        vault_service_.close_vault();
        auto result = vault_service_.open_vault(vault_dir_);
        REQUIRE(result.has_value());
    }

    markamp::core::EventBus event_bus_;
    fs::path vault_dir_;
    markamp::core::Config config_;
    markamp::core::VaultService vault_service_;
    markamp::core::TagService tag_service_;
};

} // anonymous namespace

// ============================================================================
// Test 1: Rebuild collects tags from frontmatter
// ============================================================================
TEST_CASE("TagService - Rebuild collects tags from frontmatter", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note_a.md",
                    "---\ntags:\n  - project\n  - status/active\n---\n# Note A\nContent.\n");
    fix.create_file("note_b.md", "---\ntags:\n  - project\n  - review\n---\n# Note B\nContent.\n");
    fix.create_file("note_c.md", "---\ntags:\n  - review\n---\n# Note C\nContent.\n");

    fix.open_vault();
    fix.tag_service_.rebuild();

    const auto all_tags = fix.tag_service_.get_all_tags();
    REQUIRE(fix.tag_service_.tag_count() >= 3);

    // "project" should appear in 2 docs
    auto docs_project = fix.tag_service_.get_documents_for_tag("project");
    CHECK(docs_project.size() == 2);

    // "review" should appear in 2 docs
    auto docs_review = fix.tag_service_.get_documents_for_tag("review");
    CHECK(docs_review.size() == 2);

    // "status/active" should appear in 1 doc
    auto docs_active = fix.tag_service_.get_documents_for_tag("status/active");
    CHECK(docs_active.size() == 1);
}

// ============================================================================
// Test 2: Tag tree hierarchy
// ============================================================================
TEST_CASE("TagService - Tag tree hierarchy", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note_a.md",
                    "---\ntags:\n  - project/alpha\n  - project/beta\n---\n# Note A\n");
    fix.create_file("note_b.md", "---\ntags:\n  - project/alpha\n  - review\n---\n# Note B\n");

    fix.open_vault();
    fix.tag_service_.rebuild();

    const auto tree = fix.tag_service_.get_tag_tree();
    REQUIRE(!tree.empty());

    // Find the "project" node
    bool found_project = false;
    for (const auto& node : tree)
    {
        if (node.name == "project")
        {
            found_project = true;
            // Should have alpha and beta as children
            CHECK(node.children.size() == 2);
            CHECK(node.recursive_count >= 2);
            break;
        }
    }
    CHECK(found_project);
}

// ============================================================================
// Test 3: Find tags by prefix (autocomplete)
// ============================================================================
TEST_CASE("TagService - Find tags by prefix", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note.md",
                    "---\ntags:\n  - project/alpha\n  - project/beta\n  - review\n---\n# Note\n");

    fix.open_vault();
    fix.tag_service_.rebuild();

    auto matches = fix.tag_service_.find_tags_by_prefix("project");
    CHECK(matches.size() >= 2);

    // "review" should not match
    auto review_matches = fix.tag_service_.find_tags_by_prefix("rev");
    CHECK(review_matches.size() == 1);
    CHECK(review_matches[0] == "review");
}

// ============================================================================
// Test 4: Rename tag
// ============================================================================
TEST_CASE("TagService - Rename tag", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note_a.md", "---\ntags:\n  - old_name\n---\n# Note A\n");
    fix.create_file("note_b.md", "---\ntags:\n  - old_name\n---\n# Note B\n");

    fix.open_vault();
    fix.tag_service_.rebuild();

    int modified = fix.tag_service_.rename_tag("old_name", "new_name");
    CHECK(modified == 2);

    // Old tag should be gone
    auto old_docs = fix.tag_service_.get_documents_for_tag("old_name");
    CHECK(old_docs.empty());

    // New tag should have the documents
    auto new_docs = fix.tag_service_.get_documents_for_tag("new_name");
    CHECK(new_docs.size() == 2);
}

// ============================================================================
// Test 5: Delete tag
// ============================================================================
TEST_CASE("TagService - Delete tag", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note.md", "---\ntags:\n  - to_delete\n  - keep_this\n---\n# Note\n");

    fix.open_vault();
    fix.tag_service_.rebuild();

    int modified = fix.tag_service_.delete_tag("to_delete");
    CHECK(modified == 1);

    auto deleted_docs = fix.tag_service_.get_documents_for_tag("to_delete");
    CHECK(deleted_docs.empty());

    // "keep_this" should still exist
    auto kept_docs = fix.tag_service_.get_documents_for_tag("keep_this");
    CHECK(kept_docs.size() == 1);
}

// ============================================================================
// Test 6: Merge tags
// ============================================================================
TEST_CASE("TagService - Merge tags", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note_a.md", "---\ntags:\n  - source_tag\n---\n# Note A\n");
    fix.create_file("note_b.md", "---\ntags:\n  - target_tag\n---\n# Note B\n");

    fix.open_vault();
    fix.tag_service_.rebuild();

    int modified = fix.tag_service_.merge_tags("source_tag", "target_tag");
    CHECK(modified == 1);

    auto source_docs = fix.tag_service_.get_documents_for_tag("source_tag");
    CHECK(source_docs.empty());

    auto target_docs = fix.tag_service_.get_documents_for_tag("target_tag");
    CHECK(target_docs.size() == 2);
}

// ============================================================================
// Test 7: Recursive tag query
// ============================================================================
TEST_CASE("TagService - Recursive tag query", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note_a.md", "---\ntags:\n  - project/alpha\n---\n# Note A\n");
    fix.create_file("note_b.md", "---\ntags:\n  - project/beta\n---\n# Note B\n");
    fix.create_file("note_c.md", "---\ntags:\n  - project\n---\n# Note C\n");
    fix.create_file("note_d.md", "---\ntags:\n  - review\n---\n# Note D\n");

    fix.open_vault();
    fix.tag_service_.rebuild();

    // Non-recursive: only "project" (1 doc)
    auto direct = fix.tag_service_.get_documents_for_tag("project", false);
    CHECK(direct.size() == 1);

    // Recursive: "project" + "project/alpha" + "project/beta" (3 docs)
    auto recursive = fix.tag_service_.get_documents_for_tag("project", true);
    CHECK(recursive.size() == 3);
}

// ============================================================================
// Test 8: Metadata field index
// ============================================================================
TEST_CASE("TagService - Metadata field index", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note_a.md", "---\nstatus: draft\ncategory: research\n---\n# Note A\n");
    fix.create_file("note_b.md", "---\nstatus: published\ncategory: research\n---\n# Note B\n");
    fix.create_file("note_c.md", "---\nstatus: draft\n---\n# Note C\n");

    fix.open_vault();
    fix.tag_service_.rebuild();

    auto status_stats = fix.tag_service_.get_field_values("status");
    CHECK(status_stats.total_documents >= 2);
    CHECK(status_stats.value_counts.count("draft") > 0);
    CHECK(status_stats.value_counts.at("draft") == 2);
    CHECK(status_stats.value_counts.count("published") > 0);
    CHECK(status_stats.value_counts.at("published") == 1);
}

// ============================================================================
// Test 9: Find by field value
// ============================================================================
TEST_CASE("TagService - Find by field value", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note_a.md", "---\nstatus: draft\n---\n# Note A\n");
    fix.create_file("note_b.md", "---\nstatus: published\n---\n# Note B\n");

    fix.open_vault();
    fix.tag_service_.rebuild();

    auto drafts = fix.tag_service_.find_by_field("status", "draft");
    CHECK(drafts.size() == 1);

    auto published = fix.tag_service_.find_by_field("status", "published");
    CHECK(published.size() == 1);

    auto unknown = fix.tag_service_.find_by_field("status", "archived");
    CHECK(unknown.empty());

    auto no_field = fix.tag_service_.find_by_field("nonexistent", "value");
    CHECK(no_field.empty());
}

// ============================================================================
// Test 10: Get all field names
// ============================================================================
TEST_CASE("TagService - Get all field names", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note.md",
                    "---\nstatus: draft\ncategory: research\nauthor: Alice\n---\n# Note\n");

    fix.open_vault();
    fix.tag_service_.rebuild();

    auto field_names = fix.tag_service_.get_all_field_names();
    CHECK(!field_names.empty());

    // Should include status, category, author (sorted)
    bool has_status =
        std::find(field_names.begin(), field_names.end(), "status") != field_names.end();
    bool has_category =
        std::find(field_names.begin(), field_names.end(), "category") != field_names.end();
    CHECK(has_status);
    CHECK(has_category);
}

// ============================================================================
// Test 11: TagIndexRebuiltEvent published
// ============================================================================
TEST_CASE("TagService - TagIndexRebuiltEvent published", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note.md", "---\ntags:\n  - alpha\n  - beta\n---\n# Note\n");

    bool event_received = false;
    int unique_tags = 0;
    int total_assignments = 0;

    auto sub = fix.event_bus_.subscribe<markamp::core::events::TagIndexRebuiltEvent>(
        [&](const markamp::core::events::TagIndexRebuiltEvent& event)
        {
            event_received = true;
            unique_tags = event.unique_tags;
            total_assignments = event.total_tag_assignments;
        });

    fix.open_vault();
    fix.tag_service_.rebuild();

    CHECK(event_received);
    CHECK(unique_tags >= 2);
    CHECK(total_assignments >= 2);
}

// ============================================================================
// Test 12: Incremental update
// ============================================================================
TEST_CASE("TagService - Incremental update adds new document tags", "[tag_service]")
{
    TagTestFixture fix;

    fix.create_file("note_a.md", "---\ntags:\n  - alpha\n---\n# Note A\n");

    fix.open_vault();
    fix.tag_service_.rebuild();

    CHECK(fix.tag_service_.tag_count() >= 1);

    // Add a new file and re-open vault
    fix.create_file("note_b.md", "---\ntags:\n  - beta\n  - gamma\n---\n# Note B\n");

    fix.reopen_vault();
    fix.tag_service_.rebuild();

    CHECK(fix.tag_service_.tag_count() >= 3);

    auto beta_docs = fix.tag_service_.get_documents_for_tag("beta");
    CHECK(beta_docs.size() == 1);
}
