# Phase 08 — Inline Attribute List (IAL) Parser & Storage

## Objective

Implement full Inline Attribute List (IAL) support, porting SiYuan's per-block metadata system. IAL uses Kramdown-style syntax: `{: id="..." name="value" custom-attr="value" }`. Every block in the knowledge base has an IAL that is stored in the `.sy` file (as the `Properties` field), indexed in SQLite (as the `ial` column), and cached in the in-memory block tree index. Attributes control block identity (id, updated), naming (name, alias), annotation (memo, bookmark), visual styling (style, custom-*), and folding behavior (fold, heading-fold). This phase builds the parser, serializer, differ, merger, and the AttributeService that provides batch operations and queries.

## Prerequisites

- Phase 01 (Block Data Model) — Block struct with IAL field
- Phase 04 (SQLite Storage Layer) — IAL stored and queried in database
- Phase 05 (Document File System) — IAL persisted in .sy file Properties field
- Phase 07 (Block CRUD Operations) — SetAttrs operation type

## SiYuan Source Reference

- `kernel/model/attribute.go` — Core attribute functions:
  - `SetBlockAttrs(id string, nameValues map[string]string)` — Set attributes on a block
  - `GetBlockAttrs(id string)` — Get all attributes for a block
  - `BatchSetBlockAttrs(ids []string, nameValues map[string]string)` — Set same attrs on multiple blocks
  - `BatchGetBlockAttrs(ids []string)` — Get attrs for multiple blocks
  - `GetBlockAttrsByName(name, value string)` — Find blocks by attribute name/value
- `kernel/treenode/node.go` — IAL parsing and serialization:
  - `IAL2Map(ialStr string)` — Parse `{: key="value" }` string to map
  - `Map2IAL(m map[string]string)` — Serialize map to `{: key="value" }` string
  - `IALAttr(n *ast.Node, name string)` — Get attribute from a node
  - `SetIALAttr(n *ast.Node, name, value string)` — Set attribute on a node
- `kernel/sql/block.go` — IAL column stores the serialized string
- `kernel/model/attribute.go` — Built-in attribute names: `id`, `updated`, `name`, `alias`, `memo`, `bookmark`, `style`, `fold`, `heading-fold`, `type`, `subtype`, `parent-id`, `custom-*`

## MarkAmp Integration Points

- New header: `src/core/IALParser.h`
- New source: `src/core/IALParser.cpp`
- New header: `src/core/AttributeService.h`
- New source: `src/core/AttributeService.cpp`
- Depends on: Block.h, BlockDatabase.h, DocumentFileSystem.h, BlockTreeIndex.h, EventBus
- The IALParser is a pure utility (no dependencies) used by many components
- The AttributeService depends on the full stack for persistence

## IAL Format Specification

The IAL (Inline Attribute List) format is based on Kramdown's block IAL syntax.

### Syntax

```
{: key1="value1" key2="value2" key3="value with \"escaped\" quotes" }
```

### Rules

1. The string starts with `{:` and ends with `}`.
2. Between `{:` and `}` there are zero or more key-value pairs separated by whitespace.
3. Keys are bare identifiers: `[a-z][a-z0-9_-]*` (lowercase, alphanumeric, hyphens, underscores).
4. Values are double-quoted strings: `"..."`.
5. Within values, double quotes are escaped as `\"` and backslashes as `\\`.
6. Keys are unique within an IAL. If a key appears multiple times, the last value wins.
7. When serializing, keys are sorted alphabetically for deterministic output.
8. Whitespace between pairs is normalized to a single space.
9. There is a space after `{:` and before `}`.

### Examples

```
{: id="20260214120000-abcdefg" updated="20260214120000" }
{: id="20260214120000-abcdefg" name="My Block" alias="mb,myblock" memo="This is important" }
{: id="20260214120000-abcdefg" style="background-color: #ffeb3b;" custom-color="yellow" custom-priority="high" }
{: id="20260214120000-abcdefg" fold="1" }
```

### Built-in Attribute Keys

| Key             | Description                                        | Example Value                          |
|-----------------|----------------------------------------------------|----------------------------------------|
| `id`            | Block ID (always present)                          | `"20260214120000-abcdefg"`             |
| `updated`       | Last update timestamp                              | `"20260214153000"`                     |
| `name`          | Block name (for named blocks)                      | `"My Important Note"`                  |
| `alias`         | Comma-separated aliases                            | `"note,important,todo"`               |
| `memo`          | Annotation/memo text                               | `"Review this before meeting"`         |
| `bookmark`      | Bookmark label                                     | `"Important"`                          |
| `style`         | CSS inline style                                   | `"color: red; font-weight: bold;"`     |
| `fold`          | Folded state: "1" = folded, absent = unfolded      | `"1"`                                  |
| `heading-fold`  | Heading fold: "1" = children hidden                | `"1"`                                  |
| `type`          | Block type override                                | `"doc"`                                |
| `subtype`       | Block subtype override                             | `"h1"`                                 |
| `parent-id`     | Parent block ID override                           | `"20260214120000-hijklmn"`             |
| `custom-*`      | User-defined attributes (prefix `custom-`)         | `"any user value"`                     |

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Result of diffing two IALs.
struct IALDiff
{
    // Keys that were added (present in new, absent in old).
    std::vector<std::pair<std::string, std::string>> added;

    // Keys that were removed (present in old, absent in new).
    std::vector<std::string> removed;

    // Keys that were changed (present in both, different values).
    // Each entry is {key, old_value, new_value}.
    std::vector<std::tuple<std::string, std::string, std::string>> changed;

    // Returns true if there are no differences.
    [[nodiscard]] auto is_empty() const -> bool;
};

// Pure utility class for parsing and serializing IAL strings.
// Stateless — all methods are static. No dependencies.
class IALParser
{
public:
    // Parse an IAL string into a key-value map.
    // Input: `{: id="20260214120000-abc" name="My Block" }`
    // Output: {"id": "20260214120000-abc", "name": "My Block"}
    // Returns empty map if input is empty or malformed.
    [[nodiscard]] static auto parse(const std::string& ial_string) -> InlineAttributeList;

    // Serialize a key-value map to an IAL string.
    // Keys are sorted alphabetically. Values are escaped.
    // Returns empty string if the map is empty.
    [[nodiscard]] static auto serialize(const InlineAttributeList& attrs) -> std::string;

    // Merge two IALs. Values from `overlay` take precedence over `base`.
    // Keys only in `base` are preserved. Keys only in `overlay` are added.
    [[nodiscard]] static auto merge(
        const InlineAttributeList& base,
        const InlineAttributeList& overlay
    ) -> InlineAttributeList;

    // Compute the difference between two IALs.
    [[nodiscard]] static auto diff(
        const InlineAttributeList& old_attrs,
        const InlineAttributeList& new_attrs
    ) -> IALDiff;

    // Validate an IAL string for correct syntax.
    // Returns true if the string is a valid IAL (or empty).
    [[nodiscard]] static auto is_valid(const std::string& ial_string) -> bool;

    // Extract a single attribute value from an IAL string without full parsing.
    // Optimization for when you only need one value.
    [[nodiscard]] static auto extract_value(
        const std::string& ial_string,
        const std::string& key
    ) -> std::optional<std::string>;

    // Check if a key name is a built-in attribute (id, name, alias, memo, etc.).
    [[nodiscard]] static auto is_builtin_key(const std::string& key) -> bool;

    // Check if a key name is a user-defined attribute (starts with "custom-").
    [[nodiscard]] static auto is_custom_key(const std::string& key) -> bool;

    // Validate a key name. Must match: [a-z][a-z0-9_-]*
    [[nodiscard]] static auto is_valid_key(const std::string& key) -> bool;

    // Escape a value string for IAL serialization.
    // Escapes: " -> \", \ -> \\
    [[nodiscard]] static auto escape_value(const std::string& value) -> std::string;

    // Unescape a value string from IAL parsing.
    // Unescapes: \" -> ", \\ -> \
    [[nodiscard]] static auto unescape_value(const std::string& value) -> std::string;

private:
    // Built-in attribute key names.
    static const std::unordered_set<std::string> BUILTIN_KEYS;
};

// Service for batch attribute operations with persistence.
// All changes are persisted to: .sy file (Properties), SQLite (ial column),
// and in-memory index (BlockTreeEntry.updated).
class AttributeService
{
public:
    explicit AttributeService(
        EventBus& event_bus,
        BlockDatabase& database,
        DocumentFileSystem& doc_fs,
        BlockTreeIndex& tree_index
    );

    // --- Single Block Operations ---

    // Set attributes on a single block. Merges with existing.
    // Persists to .sy file, database, and index.
    auto set_attrs(
        const std::string& block_id,
        const InlineAttributeList& attrs
    ) -> std::expected<void, std::string>;

    // Get all attributes for a single block.
    [[nodiscard]] auto get_attrs(const std::string& block_id)
        -> std::expected<InlineAttributeList, std::string>;

    // Remove specific attributes from a block by key.
    auto remove_attrs(
        const std::string& block_id,
        const std::vector<std::string>& keys
    ) -> std::expected<void, std::string>;

    // Reset a block's attributes to only the required built-ins (id, updated).
    auto reset_attrs(const std::string& block_id) -> std::expected<void, std::string>;

    // --- Batch Operations ---

    // Set the same attributes on multiple blocks.
    auto batch_set_attrs(
        const std::vector<std::string>& block_ids,
        const InlineAttributeList& attrs
    ) -> std::expected<void, std::string>;

    // Get attributes for multiple blocks.
    [[nodiscard]] auto batch_get_attrs(
        const std::vector<std::string>& block_ids
    ) -> std::expected<std::vector<std::pair<std::string, InlineAttributeList>>, std::string>;

    // --- Query by Attribute ---

    // Find all blocks that have a specific attribute with a specific value.
    // Example: get_blocks_by_attr("bookmark", "Important") returns all bookmarked blocks.
    [[nodiscard]] auto get_blocks_by_attr(
        const std::string& key,
        const std::string& value
    ) -> std::vector<Block>;

    // Find all blocks that have a specific attribute (any value).
    [[nodiscard]] auto get_blocks_with_attr(const std::string& key)
        -> std::vector<Block>;

    // Find all blocks with custom attributes (key starts with "custom-").
    [[nodiscard]] auto get_blocks_with_custom_attrs() -> std::vector<Block>;

    // --- Named Block Operations ---

    // Find a block by its name attribute.
    [[nodiscard]] auto get_block_by_name(const std::string& name)
        -> std::optional<Block>;

    // Find blocks by alias (searches comma-separated alias values).
    [[nodiscard]] auto get_blocks_by_alias(const std::string& alias)
        -> std::vector<Block>;

    // --- Bookmark Operations ---

    // Get all bookmarked blocks, grouped by bookmark label.
    [[nodiscard]] auto get_bookmarks()
        -> std::unordered_map<std::string, std::vector<Block>>;

    // Set bookmark on a block.
    auto set_bookmark(const std::string& block_id, const std::string& label)
        -> std::expected<void, std::string>;

    // Remove bookmark from a block.
    auto remove_bookmark(const std::string& block_id)
        -> std::expected<void, std::string>;

    // --- Memo Operations ---

    // Get all blocks with memos.
    [[nodiscard]] auto get_memos() -> std::vector<Block>;

    // Set memo on a block.
    auto set_memo(const std::string& block_id, const std::string& memo)
        -> std::expected<void, std::string>;

private:
    EventBus& event_bus_;
    BlockDatabase& database_;
    DocumentFileSystem& doc_fs_;
    BlockTreeIndex& tree_index_;

    // Load a block's current IAL from the database.
    [[nodiscard]] auto load_block_ial(const std::string& block_id)
        -> std::expected<InlineAttributeList, std::string>;

    // Persist updated IAL to all storage layers.
    auto persist_ial(
        const std::string& block_id,
        const InlineAttributeList& attrs
    ) -> std::expected<void, std::string>;

    // Update the "updated" timestamp in an IAL.
    auto touch_ial(InlineAttributeList& attrs) -> void;
};

} // namespace markamp::core
```

## Key Functions to Implement

### IALParser

1. **`parse(ial_string)`** — Validate the string starts with `{:` and ends with `}`. Strip those delimiters. Tokenize the remaining string into key-value pairs. For each pair: extract the key (bare identifier), skip `=`, extract the quoted value (handling escape sequences `\"` and `\\`). Return the map. If the string is empty, return an empty map. If malformed, return an empty map and log a warning.

2. **`serialize(attrs)`** — If attrs is empty, return `""`. Sort keys alphabetically. Build string: `{:`, then for each key-value pair add ` key="escaped_value"`, then add ` }`. Escape values using `escape_value()`.

3. **`merge(base, overlay)`** — Copy `base`. For each entry in `overlay`, insert or overwrite in the copy. Return the merged map.

4. **`diff(old_attrs, new_attrs)`** — Compare the two maps. For each key in new but not old: add to `added`. For each key in old but not new: add to `removed`. For each key in both with different values: add to `changed`. Return the IALDiff.

5. **`is_valid(ial_string)`** — Empty string is valid. Otherwise check: starts with `{:`, ends with `}`, all key-value pairs are well-formed (valid key name, properly quoted and escaped value).

6. **`extract_value(ial_string, key)`** — Scan the string for `key="` prefix. If found, extract the value up to the next unescaped `"`. This avoids parsing the entire IAL when you only need one attribute. Return nullopt if key not found.

7. **`escape_value(value)`** — Replace `\` with `\\`, then replace `"` with `\"`. Return the escaped string.

8. **`unescape_value(value)`** — Replace `\"` with `"`, then replace `\\` with `\`. Order matters to avoid double-unescaping.

9. **`is_builtin_key(key)`** — Check if key is in the set: `{id, updated, name, alias, memo, bookmark, style, fold, heading-fold, type, subtype, parent-id}`.

10. **`is_custom_key(key)`** — Check if key starts with `"custom-"`.

11. **`is_valid_key(key)`** — Check regex pattern `[a-z][a-z0-9_-]*`. Key must be non-empty, start with lowercase letter, contain only lowercase letters, digits, hyphens, underscores.

### AttributeService

12. **`set_attrs(block_id, attrs)`** — Load current IAL via `load_block_ial()`. Merge new attrs on top via `IALParser::merge()`. Update the `updated` timestamp. Persist via `persist_ial()`. Fire `BlockAttrsChangedEvent`.

13. **`batch_set_attrs(block_ids, attrs)`** — Group block IDs by document (using BlockTreeIndex to find root_id). For each document group: load the tree once, update all blocks' IALs, save once. This avoids loading/saving the same document multiple times.

14. **`get_blocks_by_attr(key, value)`** — Query database: `SELECT * FROM blocks WHERE ial LIKE '%key="value"%'`. Filter results to confirm exact match (LIKE is approximate). Return matching blocks.

15. **`get_bookmarks()`** — Query all blocks with `bookmark` attribute. Group by bookmark label value. Return the grouped map.

16. **`persist_ial(block_id, attrs)`** — Three-layer persistence:
    - **Database**: Update the `ial` column via `database_.set_block_attrs()`.
    - **File**: Load containing tree, find block, update its `ial` field and Properties, save tree.
    - **Index**: Update the `updated` field on the BlockTreeEntry.

## Events to Add (in Events.h)

```cpp
MARKAMP_DECLARE_EVENT(BlockAttrsChangedEvent, std::string /* block_id */, std::vector<std::string> /* changed_keys */)
MARKAMP_DECLARE_EVENT(BlockBookmarkChangedEvent, std::string /* block_id */, std::string /* bookmark_label */)
MARKAMP_DECLARE_EVENT(BlockMemoChangedEvent, std::string /* block_id */)
MARKAMP_DECLARE_EVENT(BlockNameChangedEvent, std::string /* block_id */, std::string /* new_name */)
```

## Config Keys to Add

- `knowledgebase.default_bookmark_labels` — Comma-separated list of preset bookmark labels for the UI. Default: `"Important,TODO,Review,Done,FAQ"`. Type: string.
- `knowledgebase.custom_attr_prefix` — Prefix for user-defined attributes. Default: `"custom-"`. Type: string. (Typically not changed, but configurable for compatibility.)

## Test Cases (Catch2)

File: `tests/unit/test_ial_parser.cpp`

### IALParser Tests

1. **Parse simple IAL** — Parse `{: id="20260214120000-abcdefg" name="Test" }`. Verify: map has 2 entries, id and name have correct values.

2. **Parse empty string** — Parse `""`. Verify: empty map returned.

3. **Parse IAL with no attributes** — Parse `{: }`. Verify: empty map returned.

4. **Parse IAL with escaped quotes** — Parse `{: memo="He said \"hello\" to me" }`. Verify: memo value is `He said "hello" to me` (unescaped).

5. **Parse IAL with escaped backslash** — Parse `{: style="path\\to\\file" }`. Verify: style value is `path\to\file`.

6. **Parse IAL with custom attributes** — Parse `{: custom-color="red" custom-priority="high" id="abc" }`. Verify: 3 entries. `is_custom_key("custom-color")` returns true. `is_custom_key("id")` returns false.

7. **Serialize empty map** — Serialize empty InlineAttributeList. Verify: returns empty string.

8. **Serialize to sorted keys** — Serialize map with keys `{name: "B", id: "A", custom-x: "C"}`. Verify output is `{: custom-x="C" id="A" name="B" }` (alphabetical).

9. **Serialize with escaping** — Serialize map with value containing `"` and `\`. Verify they are escaped in output.

10. **Parse-serialize round-trip** — Start with a map containing 5 entries (including custom-* and special chars). Serialize. Parse. Verify map matches original exactly.

11. **Merge two IALs** — Base: `{id: "x", name: "old"}`. Overlay: `{name: "new", memo: "added"}`. Merge. Verify: id="x", name="new", memo="added".

12. **Diff two IALs** — Old: `{id: "x", name: "old", alias: "a"}`. New: `{id: "x", name: "new", memo: "m"}`. Diff. Verify: added=[("memo","m")], removed=["alias"], changed=[("name","old","new")].

13. **Diff identical IALs** — Diff two identical maps. Verify: IALDiff.is_empty() returns true.

14. **Validate valid IAL strings** — Test `is_valid()` with: `""` (empty, valid), `{: id="abc" }` (valid), `{: a="1" b="2" c="3" }` (valid). All return true.

15. **Validate invalid IAL strings** — Test `is_valid()` with: `"not an ial"`, `{: id=noquotes }`, `{: =nokey }`, `{: 123invalid="x" }`, `{ missing-colon }`. All return false.

16. **Extract single value** — From `{: id="abc" name="test" custom-x="val" }`, extract "name". Verify returns "test". Extract "missing". Verify returns nullopt.

17. **Valid key names** — Test `is_valid_key()`: `"id"` (valid), `"custom-color"` (valid), `"my_attr"` (valid), `"a123"` (valid), `""` (invalid), `"123abc"` (invalid, starts with digit), `"UPPER"` (invalid, uppercase), `"-dash"` (invalid, starts with dash).

18. **Builtin key detection** — Test `is_builtin_key()` for all 12 built-in keys. Verify all return true. Test "custom-x" and "random" return false.

### AttributeService Tests

File: `tests/unit/test_attribute_service.cpp`

These tests require a full stack (EventBus, Config, BlockDatabase, DocumentFileSystem, BlockTreeIndex, AttributeService) with a temporary data directory.

19. **Set and get attrs** — Create a document with a block. Set attrs `{name: "Test", custom-priority: "high"}`. Get attrs. Verify both present.

20. **Remove attrs** — Set 3 attrs. Remove 1. Verify: 2 remain. The removed one returns nullopt.

21. **Reset attrs** — Set 5 custom attrs. Call `reset_attrs()`. Verify: only `id` and `updated` remain.

22. **Batch set attrs** — Create 3 blocks. Batch set `{bookmark: "TODO"}` on all 3. Verify each block has the bookmark.

23. **Batch get attrs** — Create 3 blocks with different attrs. Batch get. Verify correct attrs for each.

24. **Get blocks by attribute** — Create 5 blocks. Set `bookmark="Important"` on 2 of them. Call `get_blocks_by_attr("bookmark", "Important")`. Verify: 2 results.

25. **Get bookmarks grouped** — Set bookmarks: block1="TODO", block2="TODO", block3="Important". Call `get_bookmarks()`. Verify: map has 2 keys, "TODO" has 2 blocks, "Important" has 1.

26. **Persistence round-trip** — Set attrs on a block. Destroy all services. Recreate. Load from disk. Verify attrs survived.

27. **Attr change fires event** — Subscribe to `BlockAttrsChangedEvent`. Set attrs on a block. Verify event was fired with correct block_id and changed keys.

## Acceptance Criteria

- [ ] IALParser correctly parses all valid IAL strings matching SiYuan/Kramdown format
- [ ] IALParser correctly serializes with sorted keys, proper escaping, and `{: ... }` delimiters
- [ ] Parse-serialize round-trip preserves all attributes exactly
- [ ] Merge correctly combines two IALs with overlay precedence
- [ ] Diff correctly identifies added, removed, and changed attributes
- [ ] Escape/unescape handles `\"` and `\\` correctly
- [ ] All 12 built-in attribute keys are recognized
- [ ] Custom attribute keys (prefix `custom-`) are recognized
- [ ] Key validation enforces lowercase alphanumeric with hyphens and underscores
- [ ] AttributeService persists to all 3 layers (file, database, index)
- [ ] Batch operations group by document to minimize file I/O
- [ ] Events fired for all attribute changes
- [ ] All 27 test cases pass (18 parser + 9 service)
- [ ] Uses `[[nodiscard]]` on all query methods
- [ ] No use of `catch(...)` — use typed exception handlers
- [ ] Allman brace style, 4-space indent, trailing return types

## Files to Create/Modify

- CREATE: `src/core/IALParser.h`
- CREATE: `src/core/IALParser.cpp`
- CREATE: `src/core/AttributeService.h`
- CREATE: `src/core/AttributeService.cpp`
- MODIFY: `src/core/Events.h` (add 4 attribute-related events)
- MODIFY: `src/core/Config.cpp` (add `knowledgebase.default_bookmark_labels` and `knowledgebase.custom_attr_prefix` defaults)
- MODIFY: `src/core/PluginContext.h` (add AttributeService pointer if extending PluginContext)
- MODIFY: `src/CMakeLists.txt` (add `core/IALParser.cpp` and `core/AttributeService.cpp` to source list)
- CREATE: `tests/unit/test_ial_parser.cpp`
- CREATE: `tests/unit/test_attribute_service.cpp`
- MODIFY: `tests/CMakeLists.txt` (add `test_ial_parser` and `test_attribute_service` targets)

## Implementation Priority

The IALParser should be implemented first as it is a pure utility with no dependencies. Once the parser is complete and all 18 parser tests pass, proceed to the AttributeService which depends on the full stack.
