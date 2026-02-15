# Phase 01 -- Block Model and Document Architecture

## Objective

Extend the existing Block data model (from v3 Phase 01-03) with Obsidian-compatible link syntax (`[[wikilink]]`, `![[embed]]`), frontmatter metadata parsing, and a unified DocumentModel that wraps a block tree with parsed YAML frontmatter, outgoing link registry, and tag index. This is the foundation for bidirectional linking, graph view, embedding, and all knowledge-management features.

## Prerequisites

- v3 Phase 01 (`Block.h`, `Block.cpp`) -- Block data model already exists
- v3 Phase 02 (`BlockID.h`, `BlockID.cpp`) -- Block ID generation already exists
- v3 Phase 03 (`Notebook.h`, `Notebook.cpp`) -- Notebook model already exists
- Existing `EventBus`, `Config`, `MarkdownParser`

## Feature References (PRD)

- PRD #1: Bidirectional Linking (link syntax parsing)
- PRD #3: Markdown-Based Storage (document model)
- PRD #23: Custom Metadata / YAML Frontmatter
- PRD #9: Tagging System (tag extraction from content)
- PRD #32: Nested Tags

## Data Structures to Implement

### File: `src/core/WikiLink.h`

```cpp
#pragma once

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Represents a parsed [[wikilink]] or ![[embed]] from Markdown content.
struct WikiLink
{
    std::string target;              // The target note name or path (e.g., "My Note")
    std::string display_text;        // Optional display text after | (e.g., "alias")
    std::string heading;             // Optional heading anchor after # (e.g., "Section")
    std::string block_ref;           // Optional block reference after ^ (e.g., "block-id")
    bool is_embed{false};            // True if ![[embed]] syntax
    int source_line{0};              // Line number in source document
    int source_col{0};               // Column offset in source line

    /// Full canonical target string (target#heading^block_ref)
    [[nodiscard]] auto canonical_target() const -> std::string;

    /// True if this link targets a heading section
    [[nodiscard]] auto has_heading() const -> bool;

    /// True if this link targets a specific block
    [[nodiscard]] auto has_block_ref() const -> bool;
};

/// Represents a #tag extracted from document content.
struct ParsedTag
{
    std::string full_tag;            // Full tag text (e.g., "project/alpha")
    std::vector<std::string> parts;  // Hierarchy parts (e.g., ["project", "alpha"])
    int source_line{0};
    int source_col{0};
    bool from_frontmatter{false};    // True if tag came from YAML frontmatter

    /// The top-level tag (first part of hierarchy)
    [[nodiscard]] auto root_tag() const -> std::string;

    /// Nesting depth (number of / separators + 1)
    [[nodiscard]] auto depth() const -> int;
};

/// Parsed YAML frontmatter from a document.
struct Frontmatter
{
    std::unordered_map<std::string, std::string> string_fields;
    std::vector<std::string> tags;         // tags: [tag1, tag2]
    std::vector<std::string> aliases;      // aliases: [name1, name2]
    std::string title;                     // title field or empty
    std::string date;                      // date field or empty
    std::string css_class;                 // cssclass field or empty
    bool publish{false};                   // publish: true/false

    [[nodiscard]] auto has_field(const std::string& key) const -> bool;
    [[nodiscard]] auto get_field(const std::string& key) const -> std::optional<std::string>;
    auto set_field(const std::string& key, const std::string& value) -> void;

    /// Serialize back to YAML string (for frontmatter round-trip)
    [[nodiscard]] auto to_yaml() const -> std::string;
};

} // namespace markamp::core
```

### File: `src/core/DocumentModel.h`

```cpp
#pragma once

#include "Block.h"
#include "WikiLink.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace markamp::core
{

/// A unified document model that wraps a Block tree with parsed metadata.
/// This is the primary in-memory representation of an open document.
class DocumentModel
{
public:
    explicit DocumentModel(const std::string& id, const std::string& notebook_id);

    // --- Identity ---
    [[nodiscard]] auto id() const -> const std::string&;
    [[nodiscard]] auto notebook_id() const -> const std::string&;
    [[nodiscard]] auto title() const -> const std::string&;
    [[nodiscard]] auto file_path() const -> const std::filesystem::path&;

    auto set_title(const std::string& title) -> void;
    auto set_file_path(const std::filesystem::path& path) -> void;

    // --- Content ---
    [[nodiscard]] auto markdown() const -> const std::string&;
    auto set_markdown(const std::string& content) -> void;

    /// Re-parse markdown content: extracts frontmatter, wikilinks, tags, block tree
    auto reparse() -> void;

    // --- Frontmatter ---
    [[nodiscard]] auto frontmatter() const -> const Frontmatter&;
    auto set_frontmatter(const Frontmatter& fm) -> void;

    // --- Outgoing Links ---
    [[nodiscard]] auto outgoing_links() const -> const std::vector<WikiLink>&;
    [[nodiscard]] auto outgoing_embeds() const -> std::vector<WikiLink>;

    // --- Tags ---
    [[nodiscard]] auto tags() const -> const std::vector<ParsedTag>&;
    [[nodiscard]] auto all_tag_strings() const -> std::vector<std::string>;

    // --- Block Tree ---
    [[nodiscard]] auto root_block() const -> const std::shared_ptr<Block>&;
    auto set_root_block(std::shared_ptr<Block> root) -> void;

    // --- Aliases ---
    [[nodiscard]] auto aliases() const -> const std::vector<std::string>&;

    // --- Dirty tracking ---
    [[nodiscard]] auto is_dirty() const -> bool;
    auto mark_dirty() -> void;
    auto mark_clean() -> void;

private:
    std::string id_;
    std::string notebook_id_;
    std::string title_;
    std::filesystem::path file_path_;
    std::string markdown_;
    Frontmatter frontmatter_;
    std::vector<WikiLink> outgoing_links_;
    std::vector<ParsedTag> tags_;
    std::shared_ptr<Block> root_block_;
    bool dirty_{false};
};

} // namespace markamp::core
```

## Key Functions to Implement

### WikiLinkParser (`src/core/WikiLinkParser.h` / `.cpp`)

```cpp
namespace markamp::core
{

class WikiLinkParser
{
public:
    /// Parse all [[wikilinks]] and ![[embeds]] from markdown content.
    [[nodiscard]] auto parse_links(std::string_view content) const
        -> std::vector<WikiLink>;

    /// Parse all #tags (including nested #parent/child) from markdown content.
    /// Skips tags inside code blocks and code spans.
    [[nodiscard]] auto parse_tags(std::string_view content) const
        -> std::vector<ParsedTag>;

    /// Parse YAML frontmatter from the beginning of a markdown string.
    /// Returns nullopt if no valid frontmatter found.
    [[nodiscard]] auto parse_frontmatter(std::string_view content) const
        -> std::optional<Frontmatter>;

    /// Resolve a wikilink target to a file path within the vault.
    /// Handles shortest-path matching (Obsidian style).
    [[nodiscard]] auto resolve_link(const WikiLink& link,
                                     const std::vector<std::string>& vault_paths) const
        -> std::optional<std::string>;
};

} // namespace markamp::core
```

1. `parse_links()` -- Scan for `[[` ... `]]` patterns. Handle `|` for display text, `#` for heading, `^` for block ref. Handle `![[` prefix for embeds. Skip content inside fenced code blocks (``` ``` ```) and inline code (`` ` `` ``).

2. `parse_tags()` -- Scan for `#` followed by alphanumeric/hyphen/underscore/slash characters. Split on `/` for nested tag hierarchy. Skip tags inside code blocks, code spans, and URLs.

3. `parse_frontmatter()` -- Detect `---` at start of document. Parse until closing `---`. Use yaml-cpp to parse YAML. Extract known fields (title, tags, aliases, date, cssclass, publish). Store remaining fields in string_fields map.

4. `resolve_link()` -- Match link target against vault file paths. Use shortest-unique-path matching (Obsidian convention): "My Note" matches "folder/My Note.md" if unambiguous.

5. `DocumentModel::reparse()` -- Call WikiLinkParser for links, tags, frontmatter. Update internal collections. Mark document clean after parse.

## Events to Add

```cpp
// In Events.h

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentParsedEvent)
std::string document_id;
int link_count{0};
int tag_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FrontmatterChangedEvent)
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(WikiLinkResolvedEvent)
std::string source_doc_id;
std::string target_path;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `vault.link_style` | string | `"wikilink"` | Default link style: wikilink or markdown |
| `vault.auto_resolve_links` | bool | `true` | Resolve wikilinks to paths on parse |
| `vault.tag_separator` | string | `"/"` | Separator for nested tags |
| `vault.frontmatter_template` | string | `""` | Default frontmatter YAML for new documents |

## Test Cases

File: `tests/unit/test_document_model.cpp`

1. **WikiLink parsing -- simple link** -- Input: `"See [[My Note]] for details"`. Verify 1 WikiLink with target="My Note", is_embed=false, no heading, no block_ref.
2. **WikiLink parsing -- link with alias** -- Input: `"See [[My Note|custom text]]"`. Verify display_text="custom text".
3. **WikiLink parsing -- link with heading** -- Input: `"See [[My Note#Section One]]"`. Verify heading="Section One".
4. **WikiLink parsing -- link with block ref** -- Input: `"See [[My Note^abc123]]"`. Verify block_ref="abc123".
5. **WikiLink parsing -- embed** -- Input: `"![[Embedded Note]]"`. Verify is_embed=true.
6. **WikiLink parsing -- skips code blocks** -- Input with wikilink inside fenced code block. Verify 0 links extracted.
7. **Tag parsing -- simple tags** -- Input: `"Tagged #alpha and #beta"`. Verify 2 tags.
8. **Tag parsing -- nested tags** -- Input: `"#project/work/deadline"`. Verify parts=["project","work","deadline"], depth=3, root_tag="project".
9. **Tag parsing -- skips tags in code** -- Input with #tag inside inline code. Verify 0 tags.
10. **Frontmatter parsing -- complete** -- Input with full YAML frontmatter. Verify title, tags, aliases, date, custom fields.
11. **Frontmatter parsing -- empty** -- Input with no frontmatter. Verify nullopt returned.
12. **Frontmatter round-trip** -- Parse frontmatter, serialize with to_yaml(), re-parse, verify equality.
13. **DocumentModel reparse** -- Set markdown content, call reparse(), verify links, tags, frontmatter updated.
14. **WikiLink resolution -- shortest path** -- Vault paths: ["dir1/Note.md", "dir2/Other.md"]. Link target "Note". Verify resolves to "dir1/Note.md".
15. **WikiLink resolution -- ambiguous** -- Multiple files named "Note.md" in different directories. Verify resolution uses full path matching.

## Acceptance Criteria

- [ ] WikiLinkParser extracts `[[wikilinks]]` with target, alias, heading, block_ref
- [ ] WikiLinkParser extracts `![[embeds]]` with is_embed=true
- [ ] WikiLinkParser skips links inside code blocks and code spans
- [ ] Tag parser handles nested tags with `/` separator
- [ ] Frontmatter parser extracts standard Obsidian fields (title, tags, aliases, date, cssclass, publish)
- [ ] Frontmatter round-trips through parse/serialize without data loss
- [ ] DocumentModel.reparse() populates all parsed data from markdown content
- [ ] Link resolution uses shortest-unique-path matching
- [ ] All 15 test cases pass
- [ ] All query methods use `[[nodiscard]]`
- [ ] C++23, Allman brace style, 4-space indent, trailing return types

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/WikiLink.h` | WikiLink, ParsedTag, Frontmatter structs |
| CREATE | `src/core/WikiLink.cpp` | Frontmatter serialization, canonical_target |
| CREATE | `src/core/WikiLinkParser.h` | Parser class declaration |
| CREATE | `src/core/WikiLinkParser.cpp` | Link, tag, frontmatter parsing logic |
| CREATE | `src/core/DocumentModel.h` | DocumentModel class declaration |
| CREATE | `src/core/DocumentModel.cpp` | DocumentModel implementation |
| MODIFY | `src/core/Events.h` | Add DocumentParsedEvent, FrontmatterChangedEvent, WikiLinkResolvedEvent |
| MODIFY | `src/CMakeLists.txt` | Add new .cpp files to sources |
| CREATE | `tests/unit/test_document_model.cpp` | 15 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_document_model target |

## Architecture Notes

- WikiLinkParser is a stateless utility -- no EventBus dependency, pure parsing
- DocumentModel owns its block tree and parsed metadata; it does not reach into services
- The EventBus integration happens in the service layer (Phase 02 VaultService), not here
- yaml-cpp is already a project dependency (used by Config)
- Frontmatter parsing must be tolerant of malformed YAML (return partial results, log warnings)

## Estimated Complexity

**L** -- Multiple parsers (wikilink regex, tag regex, YAML), document model with dirty tracking, link resolution algorithm, 15 tests.
