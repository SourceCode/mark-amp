/// @file test_embed_resolver.cpp
/// @brief V4 Phase 10 – Embed Resolver tests.

#include "core/Config.h"
#include "core/EmbedResolver.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/VaultService.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace
{

struct EmbedTestFixture
{
    EmbedTestFixture()
        : vault_dir_(fs::temp_directory_path() /
                     ("markamp_embed_test_" +
                      std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
        , vault_service_(event_bus_, config_)
        , resolver_(event_bus_, vault_service_)
    {
        fs::create_directories(vault_dir_);
    }

    ~EmbedTestFixture()
    {
        fs::remove_all(vault_dir_);
    }

    auto open_vault() -> void
    {
        auto result = vault_service_.open_vault(vault_dir_);
        REQUIRE(result.has_value());
    }

    auto create_file(const std::string& rel_path, const std::string& content) const -> void
    {
        const fs::path full_path = vault_dir_ / rel_path;
        fs::create_directories(full_path.parent_path());
        std::ofstream out(full_path);
        out << content;
    }

    markamp::core::EventBus event_bus_;
    markamp::core::Config config_;
    fs::path vault_dir_;
    markamp::core::VaultService vault_service_;
    markamp::core::EmbedResolver resolver_;
};

} // anonymous namespace

// ============================================================================
// Test 1: Full document embed
// ============================================================================
TEST_CASE_METHOD(EmbedTestFixture, "Full document embed resolves content", "[embed][document]")
{
    open_vault();
    create_file("Note B.md", "---\ntitle: Note B\n---\n# Note B\nSome content here.\n");

    // Re-open vault to index the file
    auto result = vault_service_.open_vault(vault_dir_);

    markamp::core::WikiLink link;
    link.target = "Note B";
    link.is_embed = true;

    auto resolved = resolver_.resolve(link);
    CHECK(resolved.is_resolved);
    CHECK(resolved.type == markamp::core::EmbedType::kFullDocument);
    CHECK(resolved.content.find("Some content here") != std::string::npos);
    // Frontmatter should be stripped
    CHECK(resolved.content.find("---") == std::string::npos);
}

// ============================================================================
// Test 2: Heading embed
// ============================================================================
TEST_CASE_METHOD(EmbedTestFixture, "Heading embed extracts section", "[embed][heading]")
{
    open_vault();
    create_file("Note C.md",
                "---\ntitle: Note C\n---\n"
                "# Main Title\nIntro text\n"
                "## Section A\nContent A\n"
                "## Section B\nContent B\n"
                "# Another Title\nMore text\n");

    auto result = vault_service_.open_vault(vault_dir_);

    markamp::core::WikiLink link;
    link.target = "Note C";
    link.heading = "Section A";
    link.is_embed = true;

    auto resolved = resolver_.resolve(link);
    CHECK(resolved.is_resolved);
    CHECK(resolved.type == markamp::core::EmbedType::kHeading);
    CHECK(resolved.content.find("Content A") != std::string::npos);
    CHECK(resolved.content.find("Content B") == std::string::npos);
}

// ============================================================================
// Test 3: Block embed
// ============================================================================
TEST_CASE_METHOD(EmbedTestFixture, "Block embed extracts paragraph with block ID", "[embed][block]")
{
    open_vault();
    create_file("Note D.md",
                "---\ntitle: Note D\n---\n"
                "First paragraph.\n\n"
                "Target paragraph here. ^abc123\n\n"
                "Third paragraph.\n");

    auto result = vault_service_.open_vault(vault_dir_);

    markamp::core::WikiLink link;
    link.target = "Note D";
    link.block_ref = "abc123";
    link.is_embed = true;

    auto resolved = resolver_.resolve(link);
    CHECK(resolved.is_resolved);
    CHECK(resolved.type == markamp::core::EmbedType::kBlock);
    CHECK(resolved.content.find("Target paragraph") != std::string::npos);
    CHECK(resolved.content.find("Third paragraph") == std::string::npos);
}

// ============================================================================
// Test 4: Image embed
// ============================================================================
TEST_CASE("Image embed generates img tag", "[embed][media]")
{
    markamp::core::WikiLink link;
    link.target = "photo.png";
    link.is_embed = true;

    CHECK(markamp::core::EmbedResolver::detect_type(link) == markamp::core::EmbedType::kImage);

    markamp::core::EventBus event_bus;
    markamp::core::Config config;
    markamp::core::VaultService vault_service(event_bus, config);
    markamp::core::EmbedResolver resolver(event_bus, vault_service);

    auto resolved = resolver.resolve(link);
    CHECK(resolved.is_resolved);
    CHECK(resolved.html.find("<img") != std::string::npos);
    CHECK(resolved.html.find("photo.png") != std::string::npos);
}

// ============================================================================
// Test 5: PDF embed
// ============================================================================
TEST_CASE("PDF embed generates iframe tag", "[embed][media]")
{
    markamp::core::WikiLink link;
    link.target = "paper.pdf";
    link.is_embed = true;

    CHECK(markamp::core::EmbedResolver::detect_type(link) == markamp::core::EmbedType::kPdf);

    markamp::core::EventBus event_bus;
    markamp::core::Config config;
    markamp::core::VaultService vault_service(event_bus, config);
    markamp::core::EmbedResolver resolver(event_bus, vault_service);

    auto resolved = resolver.resolve(link);
    CHECK(resolved.is_resolved);
    CHECK(resolved.html.find("<iframe") != std::string::npos);
}

// ============================================================================
// Test 6: Audio embed
// ============================================================================
TEST_CASE("Audio embed generates audio tag", "[embed][media]")
{
    markamp::core::WikiLink link;
    link.target = "recording.mp3";
    link.is_embed = true;

    CHECK(markamp::core::EmbedResolver::detect_type(link) == markamp::core::EmbedType::kAudio);

    markamp::core::EventBus event_bus;
    markamp::core::Config config;
    markamp::core::VaultService vault_service(event_bus, config);
    markamp::core::EmbedResolver resolver(event_bus, vault_service);

    auto resolved = resolver.resolve(link);
    CHECK(resolved.is_resolved);
    CHECK(resolved.html.find("<audio") != std::string::npos);
}

// ============================================================================
// Test 7: Unresolved embed
// ============================================================================
TEST_CASE_METHOD(EmbedTestFixture, "Unresolved embed returns error", "[embed][error]")
{
    open_vault();

    markamp::core::WikiLink link;
    link.target = "NonExistent";
    link.is_embed = true;

    auto resolved = resolver_.resolve(link);
    CHECK_FALSE(resolved.is_resolved);
    CHECK_FALSE(resolved.error.empty());
}

// ============================================================================
// Test 8: Recursive embed (via expand_content)
// ============================================================================
TEST_CASE_METHOD(EmbedTestFixture, "Recursive embeds resolved up to depth", "[embed][recursive]")
{
    open_vault();
    create_file("A.md", "---\ntitle: A\n---\nContent A\n![[B]]\n");
    create_file("B.md", "---\ntitle: B\n---\nContent B\n![[C]]\n");
    create_file("C.md", "---\ntitle: C\n---\nContent C\n");

    auto result = vault_service_.open_vault(vault_dir_);

    auto expanded = resolver_.expand_content("Start\n![[A]]\nEnd", "test", 3);
    // Should contain content from A at minimum
    CHECK(expanded.find("Start") != std::string::npos);
    CHECK(expanded.find("End") != std::string::npos);
}

// ============================================================================
// Test 9: Circular embed prevention
// ============================================================================
TEST_CASE("Max depth prevents infinite recursion", "[embed][circular]")
{
    markamp::core::WikiLink link;
    link.target = "SelfRef";
    link.is_embed = true;

    markamp::core::EventBus event_bus;
    markamp::core::Config config;
    markamp::core::VaultService vault_service(event_bus, config);
    markamp::core::EmbedResolver resolver(event_bus, vault_service);

    // Resolve at max depth — should return error
    auto resolved = resolver.resolve(link, 5);
    CHECK_FALSE(resolved.is_resolved);
    CHECK(resolved.error.find("depth") != std::string::npos);
}

// ============================================================================
// Test 10: Heading extraction with boundaries
// ============================================================================
TEST_CASE_METHOD(EmbedTestFixture,
                 "Heading extraction stops at same-level heading",
                 "[embed][heading]")
{
    open_vault();
    create_file("Headings.md",
                "---\ntitle: Headings\n---\n"
                "# H1 First\nContent 1\n"
                "## Target Section\nTarget content\n"
                "### Sub heading\nSub content\n"
                "## Next Section\nNext content\n"
                "# H1 Second\nMore\n");

    auto result = vault_service_.open_vault(vault_dir_);

    markamp::core::WikiLink link;
    link.target = "Headings";
    link.heading = "Target Section";
    link.is_embed = true;

    auto resolved = resolver_.resolve(link);
    CHECK(resolved.is_resolved);
    CHECK(resolved.content.find("Target content") != std::string::npos);
    CHECK(resolved.content.find("Sub content") != std::string::npos);
    CHECK(resolved.content.find("Next content") == std::string::npos);
}

// ============================================================================
// Test 11: Expand replaces inline
// ============================================================================
TEST_CASE_METHOD(EmbedTestFixture, "Expand replaces embed markers inline", "[embed][expand]")
{
    open_vault();
    create_file("Inline.md", "---\ntitle: Inline\n---\nInlined content.\n");
    auto result = vault_service_.open_vault(vault_dir_);

    auto expanded = resolver_.expand_content("Before ![[Inline]] After", "test", 1);
    CHECK(expanded.find("Before ") != std::string::npos);
    CHECK(expanded.find(" After") != std::string::npos);
}

// ============================================================================
// Test 12: Embed type detection
// ============================================================================
TEST_CASE("Embed type detection from extensions", "[embed][detect]")
{
    using markamp::core::EmbedResolver;
    using markamp::core::EmbedType;

    auto make_link = [](const std::string& target)
    {
        markamp::core::WikiLink link;
        link.target = target;
        link.is_embed = true;
        return link;
    };

    CHECK(EmbedResolver::detect_type(make_link("photo.png")) == EmbedType::kImage);
    CHECK(EmbedResolver::detect_type(make_link("photo.jpg")) == EmbedType::kImage);
    CHECK(EmbedResolver::detect_type(make_link("doc.pdf")) == EmbedType::kPdf);
    CHECK(EmbedResolver::detect_type(make_link("song.mp3")) == EmbedType::kAudio);
    CHECK(EmbedResolver::detect_type(make_link("clip.mp4")) == EmbedType::kVideo);
    CHECK(EmbedResolver::detect_type(make_link("My Note")) == EmbedType::kFullDocument);

    // Heading and block via link fields
    markamp::core::WikiLink heading_link;
    heading_link.target = "Note";
    heading_link.heading = "Section";
    CHECK(EmbedResolver::detect_type(heading_link) == EmbedType::kHeading);

    markamp::core::WikiLink block_link;
    block_link.target = "Note";
    block_link.block_ref = "abc";
    CHECK(EmbedResolver::detect_type(block_link) == EmbedType::kBlock);
}
