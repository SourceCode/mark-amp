# Phase 09 -- Block Reference Syntax Parser & Resolver

**Priority:** Critical (foundation for phases 10-16)
**Estimated Scope:** ~12 files created/modified
**Dependencies:** Existing EventBus, Config, PluginContext, Types.h (MdNode/MarkdownDocument)

---

## Objective

Implement block reference parsing and resolution. SiYuan uses `((block-id "anchor text"))` syntax for block references (like wiki-links but at block granularity). The parser extracts references from markdown, resolves them to actual blocks, and maintains the reference index. This is the foundation for backlinks, graph view, and transclusion.

Reference syntax variants:
- **Block reference:** `((20210808180117-abc1234 "optional anchor text"))` or `((20210808180117-abc1234))`
- **Embed/transclusion:** `{{20210808180117-abc1234}}` (renders the referenced block inline)
- **Virtual references:** auto-detected mentions of block names/aliases without explicit link syntax
- **File annotation reference:** `<<assets/file.pdf/annotation-id "page 5">>`

The parser must be fast enough for real-time editing (incremental re-parse on keystroke) and produce a structured index that subsequent phases (backlinks, graph, search) can query in O(1).

---

## Prerequisites

- `src/core/EventBus.h` -- publish/subscribe for reference lifecycle events
- `src/core/Config.h` -- configuration for virtual reference detection, ref display style
- `src/core/Types.h` -- `MdNode`, `MarkdownDocument`, `MdNodeType` (block-level AST)
- `src/core/Md4cWrapper.h` -- markdown parsing pipeline (refs parsed as post-processing pass)
- `src/core/PluginContext.h` -- service injection point for RefParser/RefResolver
- `src/core/Events.h` -- event declaration macros
- SQLite3 library linked (for ref persistence; already available via cmake dependency chain)

---

## SiYuan Source Reference

| SiYuan File | Relevant Functions | MarkAmp Mapping |
|---|---|---|
| `kernel/treenode/node.go` | `GetBlockRef`, `GetEmbedBlockRef`, `IsBlockRef`, `IsEmbedBlockRef`, `IsFileAnnotationRef` | `RefParser::parse_refs()`, `RefParser::parse_embeds()`, `RefParser::parse_file_annotation_refs()` |
| `kernel/model/backlink.go` | `GetBacklink2` (ref resolution for display) | `RefResolver::resolve_ref()` |
| `kernel/sql/block_ref.go` | `DefRefs`, `CacheRef`, `GetRefsCacheByDefID`, `DeleteRefsByDefID` | `RefIndex::add_ref()`, `RefIndex::remove_refs_by_source()`, `RefIndex::get_refs_to()` |
| `kernel/model/transaction.go` | ref update on block edit | `RefResolver::on_block_changed()` |

---

## MarkAmp Integration Points

1. **Md4cWrapper post-processing** -- After md4c produces the AST, a second pass scans text nodes for ref syntax. Refs are extracted and stored but NOT removed from the AST (they render as links).
2. **HtmlRenderer** -- `render_node()` detects `MdNodeType::BlockRef` / `MdNodeType::BlockEmbed` and emits `<a class="block-ref" data-ref-id="...">` or `<div class="block-embed">`.
3. **EditorPanel** -- Autocomplete popup when user types `((` showing recent/matching block IDs.
4. **PreviewPanel** -- Click on a block reference navigates to the target block/document.
5. **PluginContext** -- `RefParser*` and `RefResolver*` pointers added for extension access.
6. **Config** -- Settings for virtual reference detection, ref tooltip display, embed rendering depth.

---

## Data Structures to Implement

All structures go in `src/core/BlockRef.h`:

```cpp
#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// Unique block identifier following SiYuan's timestamp-hash format.
/// Format: "YYYYMMDDHHmmss-xxxxxxx" (14 digits + dash + 7 alphanumeric chars).
struct BlockId
{
    std::string value;

    [[nodiscard]] auto operator==(const BlockId& other) const -> bool = default;
    [[nodiscard]] auto operator<=>(const BlockId& other) const = default;

    /// Validate that this ID matches the expected format.
    [[nodiscard]] auto is_valid() const -> bool;

    /// Generate a new unique block ID using current timestamp + random suffix.
    [[nodiscard]] static auto generate() -> BlockId;
};

/// Hash support for BlockId in unordered containers.
struct BlockIdHash
{
    [[nodiscard]] auto operator()(const BlockId& id) const noexcept -> std::size_t
    {
        return std::hash<std::string>{}(id.value);
    }
};

/// Type of block reference.
enum class RefType : std::uint8_t
{
    BlockRef,            // ((id "text"))
    BlockEmbed,          // {{id}}
    FileAnnotationRef,   // <<path/annotation "text">>
    VirtualRef           // auto-detected name mention
};

/// A single parsed reference found in a block's content.
struct ParsedRef
{
    RefType type{RefType::BlockRef};

    /// The ID of the block being referenced (the definition/target).
    BlockId def_block_id;

    /// Optional anchor text provided in the reference syntax.
    std::optional<std::string> anchor_text;

    /// For file annotation refs: the asset path component.
    std::optional<std::string> asset_path;

    /// Character offset of the reference start within the source block's content.
    std::size_t source_offset{0};

    /// Character length of the entire reference syntax in source.
    std::size_t source_length{0};
};

/// A resolved reference with full context for display.
struct ResolvedRef
{
    ParsedRef parsed;

    /// The source block that contains this reference.
    BlockId source_block_id;

    /// The root document ID containing the source block.
    std::string source_root_id;

    /// Resolved display text (anchor text if provided, else target block's first line).
    std::string display_text;

    /// Whether the target block was found in the index.
    bool target_exists{false};

    /// If target exists, its block type (paragraph, heading, code, etc.).
    std::string target_type;

    /// If target exists, a content snippet (first 100 chars).
    std::string target_snippet;
};

/// Entry in the reference index: maps a definition block to all blocks that reference it.
struct RefIndexEntry
{
    BlockId def_block_id;

    /// All source block IDs that reference this definition.
    std::unordered_set<BlockId, BlockIdHash> referencing_block_ids;

    /// Total reference count (including multiple refs from same source block).
    std::size_t total_ref_count{0};
};

/// In-memory reference index providing O(1) lookups by def or source block.
class RefIndex
{
public:
    /// Add a reference from source_block to def_block.
    void add_ref(const BlockId& source_block, const BlockId& def_block, RefType type);

    /// Remove all references originating from the given source block.
    void remove_refs_by_source(const BlockId& source_block);

    /// Get all blocks that reference the given definition block.
    [[nodiscard]] auto get_refs_to(const BlockId& def_block) const
        -> std::vector<BlockId>;

    /// Get all blocks that the given source block references.
    [[nodiscard]] auto get_refs_from(const BlockId& source_block) const
        -> std::vector<BlockId>;

    /// Get the reference count for a definition block.
    [[nodiscard]] auto ref_count(const BlockId& def_block) const -> std::size_t;

    /// Check if a circular reference exists between two blocks.
    [[nodiscard]] auto has_circular_ref(const BlockId& block_a, const BlockId& block_b) const
        -> bool;

    /// Clear the entire index.
    void clear();

    /// Total number of unique references tracked.
    [[nodiscard]] auto total_refs() const -> std::size_t;

private:
    /// def_block_id -> set of source_block_ids
    std::unordered_map<BlockId, std::unordered_set<BlockId, BlockIdHash>, BlockIdHash>
        forward_index_;

    /// source_block_id -> set of def_block_ids
    std::unordered_map<BlockId, std::unordered_set<BlockId, BlockIdHash>, BlockIdHash>
        reverse_index_;

    std::size_t total_ref_count_{0};
};

} // namespace markamp::core
```

---

## Key Functions to Implement

### RefParser class (`src/core/RefParser.h` / `src/core/RefParser.cpp`)

1. **`auto parse_refs(std::string_view content) const -> std::vector<ParsedRef>`** -- Scan text content for `((id "text"))` or `((id))` patterns using regex. Return all found block references with their offsets. Uses `std::regex` with pattern `\(\((\d{14}-[a-z0-9]{7})(?:\s+"([^"]*)")?\)\)`.

2. **`auto parse_embeds(std::string_view content) const -> std::vector<ParsedRef>`** -- Scan for `{{id}}` embed/transclusion syntax. Pattern: `\{\{(\d{14}-[a-z0-9]{7})\}\}`. Sets `RefType::BlockEmbed`.

3. **`auto parse_file_annotation_refs(std::string_view content) const -> std::vector<ParsedRef>`** -- Scan for `<<path/annotation-id "text">>` syntax. Pattern: `<<([^>]+?)/([a-z0-9-]+)(?:\s+"([^"]*)")?\s*>>`. Sets asset_path and anchor_text.

4. **`auto parse_all(std::string_view content) const -> std::vector<ParsedRef>`** -- Convenience method combining refs + embeds + file annotation refs into a single sorted-by-offset vector.

5. **`auto detect_virtual_refs(std::string_view content, const std::vector<std::pair<BlockId, std::string>>& known_names) const -> std::vector<ParsedRef>`** -- Scan content for mentions of known block names/aliases. Case-insensitive substring match. Excludes matches inside existing ref syntax. Sets `RefType::VirtualRef`.

6. **`auto validate_block_id(std::string_view candidate) const -> bool`** -- Validate that a string matches the block ID format (14-digit timestamp + dash + 7 alphanumeric characters).

### RefResolver class (`src/core/RefResolver.h` / `src/core/RefResolver.cpp`)

7. **`auto resolve_ref(const ParsedRef& ref, const BlockId& source_block) -> ResolvedRef`** -- Look up the target block in the block database, populate display text (anchor text or block content snippet), and return a fully resolved reference. If the target does not exist, returns `ResolvedRef` with `target_exists = false`.

8. **`auto resolve_embed(const ParsedRef& ref) -> std::optional<std::string>`** -- Resolve a block embed and return the full rendered HTML content of the target block for transclusion. Returns `std::nullopt` if target not found or max embed depth exceeded.

9. **`auto batch_resolve(const std::vector<ParsedRef>& refs, const BlockId& source_block) -> std::vector<ResolvedRef>`** -- Resolve multiple references in a single batch. Deduplicates target lookups for efficiency.

10. **`void on_block_changed(const BlockId& block_id, std::string_view new_content)`** -- Called when a block's content changes. Re-parses the block for refs, updates the RefIndex (remove old refs, add new ones), and publishes `BlockRefUpdatedEvent`.

11. **`void rebuild_index()`** -- Full re-index of all blocks in the workspace. Called on workspace open and after import operations.

12. **`void set_max_embed_depth(int depth)`** -- Configure maximum transclusion nesting depth to prevent infinite recursion (default: 5).

---

## Events to Add

Add to `src/core/Events.h` in a new `// Block reference events` section:

```cpp
// ============================================================================
// Block reference events (Phase 09)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockRefCreatedEvent)
std::string source_block_id;
std::string def_block_id;
std::string ref_type; // "block_ref", "embed", "file_annotation", "virtual"
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockRefDeletedEvent)
std::string source_block_id;
std::string def_block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockRefUpdatedEvent)
std::string source_block_id;
std::vector<std::string> old_def_ids;
std::vector<std::string> new_def_ids;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockRefResolvedEvent)
std::string source_block_id;
std::string def_block_id;
bool target_exists{false};
std::string display_text;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockRefNavigateRequestEvent)
std::string target_block_id;
MARKAMP_DECLARE_EVENT_END;
```

---

## Config Keys to Add

Add to `resources/config_defaults.json`:

| Key | Type | Default | Description |
|---|---|---|---|
| `knowledgebase.refs.enable_virtual_refs` | bool | `true` | Auto-detect mentions of block names as virtual references |
| `knowledgebase.refs.virtual_ref_min_length` | int | `4` | Minimum name length for virtual ref detection |
| `knowledgebase.refs.show_ref_count_badge` | bool | `true` | Show reference count badges on blocks |
| `knowledgebase.refs.embed_max_depth` | int | `5` | Maximum nesting depth for transclusion embeds |
| `knowledgebase.refs.tooltip_preview` | bool | `true` | Show content preview tooltip on ref hover |
| `knowledgebase.refs.tooltip_delay_ms` | int | `500` | Delay before showing ref tooltip |

---

## Test Cases

File: `tests/unit/test_block_refs.cpp`

```cpp
#include "core/BlockRef.h"
#include "core/RefParser.h"
#include "core/RefResolver.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
```

1. **Parse single block reference** -- Input: `"See ((20210808180117-abc1234 \"my note\"))"`. Verify: one `ParsedRef` returned, `type == RefType::BlockRef`, `def_block_id.value == "20210808180117-abc1234"`, `anchor_text == "my note"`, correct `source_offset` and `source_length`.

2. **Parse block reference without anchor text** -- Input: `"Link to ((20210808180117-abc1234))"`. Verify: one `ParsedRef` returned, `anchor_text == std::nullopt`.

3. **Parse embed/transclusion** -- Input: `"Content: {{20210808180117-abc1234}}"`. Verify: one `ParsedRef` returned, `type == RefType::BlockEmbed`, correct block ID.

4. **Parse multiple references in one block** -- Input: `"See ((20210808180117-aaa1111)) and ((20210808180117-bbb2222 \"second\")) and {{20210808180117-ccc3333}}"`. Verify: three `ParsedRef` items returned in offset order with correct types.

5. **Resolve reference to non-existent block** -- Create a `RefResolver` with an empty block database. Resolve a ref. Verify: `target_exists == false`, `display_text` falls back to the raw block ID.

6. **Circular reference detection** -- Add ref A->B and B->A to RefIndex. Verify: `has_circular_ref(A, B) == true`, `has_circular_ref(A, C) == false` where C is unrelated.

7. **Virtual reference detection** -- Input content: `"I was reading about Project Alpha yesterday"`. Known names: `[{id1, "Project Alpha"}, {id2, "Beta"}]`. Verify: one virtual ref detected for "Project Alpha" with correct offset, none for "Beta" (too short when min_length=4 but "Beta" is 4 chars so it should match -- verify boundary).

8. **File annotation reference** -- Input: `"See <<assets/paper.pdf/annot-abc123 \"page 5\">>"`. Verify: one `ParsedRef` with `type == RefType::FileAnnotationRef`, `asset_path == "assets/paper.pdf"`, anchor_text == "page 5"`.

9. **Reference count update** -- Add 3 refs from different source blocks to the same def block in RefIndex. Verify: `ref_count(def) == 3`. Remove refs from one source. Verify: `ref_count(def) == 2`.

10. **Batch resolution** -- Batch-resolve 5 refs where 3 targets exist and 2 do not. Verify: 5 `ResolvedRef` items returned, 3 with `target_exists == true`, 2 with `target_exists == false`.

11. **Reference text update propagation** -- Change a block's content from having 2 refs to having 1 ref. Call `on_block_changed()`. Verify: `BlockRefUpdatedEvent` published with correct `old_def_ids` (2 entries) and `new_def_ids` (1 entry). RefIndex correctly reflects the change.

12. **Edge cases** -- Empty content returns empty vector. Malformed refs (`((invalid))`, `((123-short))`, `(())`) are not matched. Nested parens `((text ((inner)) text))` do not produce false matches. Refs inside code blocks are ignored (if content is pre-filtered).

---

## Acceptance Criteria

- [ ] `BlockId::generate()` produces valid IDs that pass `is_valid()` check
- [ ] `RefParser::parse_refs()` extracts all `((id "text"))` and `((id))` patterns correctly
- [ ] `RefParser::parse_embeds()` extracts all `{{id}}` patterns correctly
- [ ] `RefParser::parse_file_annotation_refs()` extracts `<<path/id "text">>` patterns
- [ ] `RefParser::detect_virtual_refs()` finds name mentions excluding existing ref syntax
- [ ] `RefResolver::resolve_ref()` returns complete `ResolvedRef` with display text and snippet
- [ ] `RefResolver::resolve_embed()` returns rendered HTML with depth limit enforcement
- [ ] `RefIndex` maintains correct forward/reverse index with O(1) lookups
- [ ] `RefIndex::has_circular_ref()` detects bidirectional references
- [ ] All 5 events (`BlockRefCreatedEvent`, `BlockRefDeletedEvent`, `BlockRefUpdatedEvent`, `BlockRefResolvedEvent`, `BlockRefNavigateRequestEvent`) fire at correct lifecycle points
- [ ] All 6 config keys load from `config_defaults.json` with correct defaults
- [ ] All 12 Catch2 test cases pass
- [ ] No use of `catch(...)` -- all exception handlers use typed catches
- [ ] All filesystem operations use `std::error_code` overloads
- [ ] All query methods marked `[[nodiscard]]`

---

## Files to Create/Modify

### New Files

| File | Description |
|---|---|
| `src/core/BlockRef.h` | `BlockId`, `ParsedRef`, `ResolvedRef`, `RefIndexEntry`, `RefIndex` data structures |
| `src/core/BlockRef.cpp` | `BlockId::generate()`, `BlockId::is_valid()`, `RefIndex` method implementations |
| `src/core/RefParser.h` | `RefParser` class declaration |
| `src/core/RefParser.cpp` | Regex-based reference parsing implementations |
| `src/core/RefResolver.h` | `RefResolver` class declaration |
| `src/core/RefResolver.cpp` | Reference resolution, embed rendering, index rebuild logic |
| `tests/unit/test_block_refs.cpp` | All 12 Catch2 test cases |

### Modified Files

| File | Change |
|---|---|
| `src/core/Events.h` | Add `BlockRefCreatedEvent`, `BlockRefDeletedEvent`, `BlockRefUpdatedEvent`, `BlockRefResolvedEvent`, `BlockRefNavigateRequestEvent` |
| `src/core/Types.h` | Add `MdNodeType::BlockRef`, `MdNodeType::BlockEmbed` to the `MdNodeType` enum |
| `src/core/PluginContext.h` | Add `RefParser* ref_parser{nullptr};` and `RefResolver* ref_resolver{nullptr};` pointers |
| `src/CMakeLists.txt` | Add `core/BlockRef.cpp`, `core/RefParser.cpp`, `core/RefResolver.cpp` to sources |
| `tests/CMakeLists.txt` | Add `test_block_refs` test target |
| `resources/config_defaults.json` | Add 6 `knowledgebase.refs.*` config keys |
