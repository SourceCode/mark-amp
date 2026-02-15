#include "core/DocumentModel.h"
#include "core/WikiLink.h"
#include "core/WikiLinkParser.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Test 1: WikiLink parsing -- simple link
// ============================================================================
TEST_CASE("WikiLink parsing -- simple link", "[v4][phase01][wikilink]")
{
    WikiLinkParser parser;
    auto links = parser.parse_links("See [[My Note]] for details");

    REQUIRE(links.size() == 1);
    CHECK(links[0].target == "My Note");
    CHECK(links[0].is_embed == false);
    CHECK(links[0].display_text.empty());
    CHECK(links[0].heading.empty());
    CHECK(links[0].block_ref.empty());
}

// ============================================================================
// Test 2: WikiLink parsing -- link with alias
// ============================================================================
TEST_CASE("WikiLink parsing -- link with alias", "[v4][phase01][wikilink]")
{
    WikiLinkParser parser;
    auto links = parser.parse_links("See [[My Note|custom text]]");

    REQUIRE(links.size() == 1);
    CHECK(links[0].target == "My Note");
    CHECK(links[0].display_text == "custom text");
}

// ============================================================================
// Test 3: WikiLink parsing -- link with heading
// ============================================================================
TEST_CASE("WikiLink parsing -- link with heading", "[v4][phase01][wikilink]")
{
    WikiLinkParser parser;
    auto links = parser.parse_links("See [[My Note#Section One]]");

    REQUIRE(links.size() == 1);
    CHECK(links[0].target == "My Note");
    CHECK(links[0].heading == "Section One");
    CHECK(links[0].has_heading() == true);
}

// ============================================================================
// Test 4: WikiLink parsing -- link with block ref
// ============================================================================
TEST_CASE("WikiLink parsing -- link with block ref", "[v4][phase01][wikilink]")
{
    WikiLinkParser parser;
    auto links = parser.parse_links("See [[My Note^abc123]]");

    REQUIRE(links.size() == 1);
    CHECK(links[0].target == "My Note");
    CHECK(links[0].block_ref == "abc123");
    CHECK(links[0].has_block_ref() == true);
}

// ============================================================================
// Test 5: WikiLink parsing -- embed
// ============================================================================
TEST_CASE("WikiLink parsing -- embed", "[v4][phase01][wikilink]")
{
    WikiLinkParser parser;
    auto links = parser.parse_links("![[Embedded Note]]");

    REQUIRE(links.size() == 1);
    CHECK(links[0].target == "Embedded Note");
    CHECK(links[0].is_embed == true);
}

// ============================================================================
// Test 6: WikiLink parsing -- skips code blocks
// ============================================================================
TEST_CASE("WikiLink parsing -- skips code blocks", "[v4][phase01][wikilink]")
{
    WikiLinkParser parser;

    SECTION("Fenced code block")
    {
        std::string input = "Before\n```\n[[Hidden Link]]\n```\nAfter";
        auto links = parser.parse_links(input);
        REQUIRE(links.empty());
    }

    SECTION("Inline code")
    {
        std::string input = "See `[[Hidden Link]]` here";
        auto links = parser.parse_links(input);
        REQUIRE(links.empty());
    }
}

// ============================================================================
// Test 7: Tag parsing -- simple tags
// ============================================================================
TEST_CASE("Tag parsing -- simple tags", "[v4][phase01][tags]")
{
    WikiLinkParser parser;
    auto tags = parser.parse_tags("Tagged #alpha and #beta");

    REQUIRE(tags.size() == 2);
    CHECK(tags[0].full_tag == "alpha");
    CHECK(tags[1].full_tag == "beta");
}

// ============================================================================
// Test 8: Tag parsing -- nested tags
// ============================================================================
TEST_CASE("Tag parsing -- nested tags", "[v4][phase01][tags]")
{
    WikiLinkParser parser;
    auto tags = parser.parse_tags("#project/work/deadline");

    REQUIRE(tags.size() == 1);
    CHECK(tags[0].full_tag == "project/work/deadline");
    REQUIRE(tags[0].parts.size() == 3);
    CHECK(tags[0].parts[0] == "project");
    CHECK(tags[0].parts[1] == "work");
    CHECK(tags[0].parts[2] == "deadline");
    CHECK(tags[0].depth() == 3);
    CHECK(tags[0].root_tag() == "project");
}

// ============================================================================
// Test 9: Tag parsing -- skips tags in code
// ============================================================================
TEST_CASE("Tag parsing -- skips tags in code", "[v4][phase01][tags]")
{
    WikiLinkParser parser;

    SECTION("Inline code")
    {
        auto tags = parser.parse_tags("See `#tag` here");
        REQUIRE(tags.empty());
    }

    SECTION("Fenced code block")
    {
        std::string input = "Before\n```\n#hidden_tag\n```\nAfter";
        auto tags = parser.parse_tags(input);
        REQUIRE(tags.empty());
    }
}

// ============================================================================
// Test 10: Frontmatter parsing -- complete
// ============================================================================
TEST_CASE("Frontmatter parsing -- complete", "[v4][phase01][frontmatter]")
{
    WikiLinkParser parser;
    std::string input = "---\n"
                        "title: My Document\n"
                        "date: 2024-01-15\n"
                        "cssclass: wide-page\n"
                        "publish: true\n"
                        "tags:\n"
                        "  - alpha\n"
                        "  - beta\n"
                        "aliases:\n"
                        "  - doc1\n"
                        "  - my-doc\n"
                        "custom_field: custom_value\n"
                        "---\n"
                        "# Content starts here\n";

    auto result = parser.parse_frontmatter(input);

    REQUIRE(result.has_value());
    auto& fm = result.value();
    CHECK(fm.title == "My Document");
    CHECK(fm.date == "2024-01-15");
    CHECK(fm.css_class == "wide-page");
    CHECK(fm.publish == true);
    REQUIRE(fm.tags.size() == 2);
    CHECK(fm.tags[0] == "alpha");
    CHECK(fm.tags[1] == "beta");
    REQUIRE(fm.aliases.size() == 2);
    CHECK(fm.aliases[0] == "doc1");
    CHECK(fm.aliases[1] == "my-doc");
    CHECK(fm.has_field("custom_field") == true);
    auto custom = fm.get_field("custom_field");
    REQUIRE(custom.has_value());
    CHECK(custom.value() == "custom_value");
}

// ============================================================================
// Test 11: Frontmatter parsing -- empty
// ============================================================================
TEST_CASE("Frontmatter parsing -- empty", "[v4][phase01][frontmatter]")
{
    WikiLinkParser parser;
    std::string input = "# Just a heading\nSome content";

    auto result = parser.parse_frontmatter(input);
    REQUIRE_FALSE(result.has_value());
}

// ============================================================================
// Test 12: Frontmatter round-trip
// ============================================================================
TEST_CASE("Frontmatter round-trip", "[v4][phase01][frontmatter]")
{
    Frontmatter original;
    original.title = "Test Title";
    original.date = "2024-06-01";
    original.css_class = "narrow";
    original.publish = true;
    original.tags = {"tag1", "tag2"};
    original.aliases = {"alias1"};
    original.set_field("author", "John");

    std::string yaml = original.to_yaml();

    // The serialized YAML should start and end with ---
    REQUIRE(yaml.starts_with("---\n"));
    REQUIRE(yaml.ends_with("---\n"));

    // Now parse it back
    WikiLinkParser parser;
    auto parsed = parser.parse_frontmatter(yaml);
    REQUIRE(parsed.has_value());

    auto& fm = parsed.value();
    CHECK(fm.title == original.title);
    CHECK(fm.date == original.date);
    CHECK(fm.css_class == original.css_class);
    CHECK(fm.publish == original.publish);
    CHECK(fm.tags == original.tags);
    CHECK(fm.aliases == original.aliases);
    auto author = fm.get_field("author");
    REQUIRE(author.has_value());
    CHECK(author.value() == "John");
}

// ============================================================================
// Test 13: DocumentModel reparse
// ============================================================================
TEST_CASE("DocumentModel reparse", "[v4][phase01][document_model]")
{
    DocumentModel doc("doc1", "nb1");

    std::string content = "---\n"
                          "title: Test Document\n"
                          "tags:\n"
                          "  - project\n"
                          "aliases:\n"
                          "  - td\n"
                          "---\n"
                          "# Introduction\n"
                          "\n"
                          "See [[Other Note]] and [[Reference#heading]].\n"
                          "\n"
                          "Also tagged #important and #review.\n"
                          "\n"
                          "Embed: ![[Image.png]]\n";

    doc.set_markdown(content);
    doc.reparse();

    // Check frontmatter
    CHECK(doc.title() == "Test Document");
    CHECK(doc.aliases().size() == 1);
    CHECK(doc.aliases()[0] == "td");

    // Check outgoing links (3: Other Note, Reference, Image.png embed)
    REQUIRE(doc.outgoing_links().size() == 3);

    // Check embeds
    auto embeds = doc.outgoing_embeds();
    REQUIRE(embeds.size() == 1);
    CHECK(embeds[0].target == "Image.png");
    CHECK(embeds[0].is_embed == true);

    // Check tags (2 content tags + 1 frontmatter tag = 3 total)
    auto all_tags = doc.all_tag_strings();
    REQUIRE(all_tags.size() == 3);

    // Dirty tracking
    CHECK(doc.is_dirty() == false); // reparse clears dirty
    doc.mark_dirty();
    CHECK(doc.is_dirty() == true);
    doc.mark_clean();
    CHECK(doc.is_dirty() == false);
}

// ============================================================================
// Test 14: WikiLink resolution -- shortest path
// ============================================================================
TEST_CASE("WikiLink resolution -- shortest path", "[v4][phase01][resolution]")
{
    WikiLinkParser parser;
    std::vector<std::string> vault_paths = {"dir1/Note.md", "dir2/Other.md"};

    WikiLink link;
    link.target = "Note";

    auto result = parser.resolve_link(link, vault_paths);
    REQUIRE(result.has_value());
    CHECK(result.value() == "dir1/Note.md");
}

// ============================================================================
// Test 15: WikiLink resolution -- ambiguous
// ============================================================================
TEST_CASE("WikiLink resolution -- ambiguous path matching", "[v4][phase01][resolution]")
{
    WikiLinkParser parser;
    std::vector<std::string> vault_paths = {"dir1/Note.md", "dir2/Note.md"};

    WikiLink link;
    link.target = "Note";

    // With ambiguous names, resolves to first match
    auto result = parser.resolve_link(link, vault_paths);
    REQUIRE(result.has_value());
    CHECK(result.value() == "dir1/Note.md");

    // Full path match disambiguates
    WikiLink path_link;
    path_link.target = "dir2/Note";
    auto path_result = parser.resolve_link(path_link, vault_paths);
    REQUIRE(path_result.has_value());
    CHECK(path_result.value() == "dir2/Note.md");
}

// ============================================================================
// Additional: WikiLink canonical_target
// ============================================================================
TEST_CASE("WikiLink canonical_target", "[v4][phase01][wikilink]")
{
    WikiLink link;
    link.target = "My Note";
    link.heading = "Section";
    link.block_ref = "abc";

    CHECK(link.canonical_target() == "My Note#Section^abc");

    WikiLink simple;
    simple.target = "Simple";
    CHECK(simple.canonical_target() == "Simple");
}

// ============================================================================
// Additional: Frontmatter inline array syntax
// ============================================================================
TEST_CASE("Frontmatter inline array syntax", "[v4][phase01][frontmatter]")
{
    WikiLinkParser parser;
    std::string input = "---\n"
                        "tags: [inline1, inline2, inline3]\n"
                        "aliases: [a1, a2]\n"
                        "---\n"
                        "Content\n";

    auto result = parser.parse_frontmatter(input);
    REQUIRE(result.has_value());
    auto& fm = result.value();
    REQUIRE(fm.tags.size() == 3);
    CHECK(fm.tags[0] == "inline1");
    CHECK(fm.tags[1] == "inline2");
    CHECK(fm.tags[2] == "inline3");
    REQUIRE(fm.aliases.size() == 2);
}

// ============================================================================
// Additional: Multiple links on one line
// ============================================================================
TEST_CASE("Multiple wikilinks on one line", "[v4][phase01][wikilink]")
{
    WikiLinkParser parser;
    auto links = parser.parse_links("See [[A]], [[B]], and [[C]]");

    REQUIRE(links.size() == 3);
    CHECK(links[0].target == "A");
    CHECK(links[1].target == "B");
    CHECK(links[2].target == "C");
}
