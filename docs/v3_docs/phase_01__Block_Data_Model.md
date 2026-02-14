# Phase 01 — Block Data Model Core

## Objective

Define the foundational Block data model in C++23, porting SiYuan's block-based content architecture. Every piece of content in the knowledgebase is a Block with a unique ID, type, parent-child relationships, and metadata. This is the single most important data structure in the entire system — all subsequent phases depend on it.

## Prerequisites

- None (foundation phase)

## SiYuan Source Reference

- `kernel/model/block.go` — Block struct definition (Box, Path, HPath, ID, RootID, ParentID, Name, Alias, Memo, Tag, Content, FContent, Markdown, Type, SubType, IAL map, Children, Refs, Defs, Created, Updated, RiffCard, Folded, Depth, Count, Sort, RefCount)
- `kernel/treenode/blocktree.go` — BlockTree struct (ID, RootID, ParentID, BoxID, Path, HPath, Updated, Type)
- `kernel/treenode/node.go` — Node type abbreviations and conversions (TypeAbbr, SubTypeAbbr)
- `kernel/sql/block.go` — SQL Block struct (id, parent_id, root_id, hash, box, path, hpath, name, alias, memo, tag, content, fcontent, markdown, length, type, subtype, ial, sort, created, updated)

## MarkAmp Integration Points

- New header: `src/core/Block.h`
- New source: `src/core/Block.cpp`
- Extends `Types.h` with BlockType enum
- EventBus events for block lifecycle
- No dependency on any existing MarkAmp component beyond Events.h and Types.h

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Block node types matching SiYuan's AST node types.
// Values chosen for stable serialization — do not reorder.
enum class BlockType : uint8_t
{
    Document = 0,
    Paragraph,
    Heading,
    BlockQuote,
    List,
    ListItem,
    CodeBlock,
    MathBlock,
    Table,
    TableRow,
    TableCell,
    HTMLBlock,
    ThematicBreak,
    Audio,
    Video,
    IFrame,
    Widget,
    SuperBlock,
    EmbedBlock,        // Block embed/transclusion
    AttributeView,     // Database view block
    CalloutBlock,
    MermaidBlock,
    PlantUMLBlock,
    GraphvizBlock,
    FlowchartBlock,
    MindmapBlock,
    EChartsBlock,
    ABCBlock,          // Music notation
    Unknown
};

enum class BlockSubType : uint8_t
{
    None = 0,
    Heading1, Heading2, Heading3, Heading4, Heading5, Heading6,
    OrderedList, UnorderedList, TaskList,
    Math, Code,
};

// Inline Attribute List — per-block key-value metadata.
// Keys are always lowercase ASCII. Values are UTF-8 strings.
using InlineAttributeList = std::unordered_map<std::string, std::string>;

struct BlockRef
{
    std::string block_id;       // Referenced block ID
    std::string ref_text;       // Display text for the reference
    std::string def_block_id;   // Defining block ID
    std::string def_block_path; // Path to the defining block's document
};

struct Block
{
    // Identity
    std::string id;             // Unique block ID (timestamp-based, 22 chars: YYYYMMDDHHmmss-xxxxxxx)
    std::string parent_id;      // Parent block ID (empty string for root/document blocks)
    std::string root_id;        // Document root block ID (same as id for document blocks)
    std::string box_id;         // Notebook ID this block belongs to

    // Path
    std::string path;           // Data file path (e.g., "/20210101120000-abc1234.sy")
    std::string hpath;          // Human-readable path (e.g., "/Notebook/My Document")

    // Content
    std::string name;           // Block name (heading text, document title, etc.)
    std::string alias;          // Block alias (comma-separated alternative names)
    std::string memo;           // Block memo/annotation
    std::string tag;            // Comma-separated tags (e.g., "tag1,tag2")
    std::string content;        // Plain text content (for search indexing, no markup)
    std::string fcontent;       // Full content including children text (for search)
    std::string markdown;       // Original markdown source

    // Type
    BlockType type = BlockType::Paragraph;
    BlockSubType sub_type = BlockSubType::None;

    // Metadata
    InlineAttributeList ial;    // Inline attributes {key: value}
    int64_t sort = 0;           // Sort order within parent
    int64_t length = 0;         // Content length in bytes
    bool folded = false;        // Whether block is collapsed in the UI
    int32_t depth = 0;          // Nesting depth from root (root = 0)
    int32_t count = 0;          // Direct child count
    int32_t ref_count = 0;      // Number of incoming references (backlinks) to this block

    // Timestamps (formatted as YYYYMMDDHHmmss strings, matching SiYuan)
    int64_t created = 0;        // Unix milliseconds
    int64_t updated = 0;        // Unix milliseconds

    // Relationships (populated on demand, NOT persisted inline in .sy files)
    std::vector<BlockRef> refs;                    // Outgoing references from this block
    std::vector<BlockRef> defs;                    // Incoming references (backlinks) to this block
    std::vector<std::shared_ptr<Block>> children;  // Child blocks in tree order

    // --- Query methods ---

    // Returns true if this block is a document root
    [[nodiscard]] auto is_document() const -> bool;

    // Returns true if this block is a heading (any level)
    [[nodiscard]] auto is_heading() const -> bool;

    // Returns true if this block can contain children
    // Container types: Document, BlockQuote, List, ListItem, SuperBlock, Table, TableRow
    [[nodiscard]] auto is_container() const -> bool;

    // Returns true if this block cannot have children
    [[nodiscard]] auto is_leaf() const -> bool;

    // Short abbreviation string for the block type, matching SiYuan conventions:
    //   Document="d", Paragraph="p", Heading="h", BlockQuote="b",
    //   List="l", ListItem="i", CodeBlock="c", MathBlock="m",
    //   Table="t", HTMLBlock="html", ThematicBreak="tb",
    //   Audio="audio", Video="video", IFrame="iframe",
    //   Widget="widget", SuperBlock="s", EmbedBlock="e",
    //   AttributeView="av", Unknown="unknown"
    [[nodiscard]] auto type_abbr() const -> std::string;

    // Short abbreviation string for the block subtype:
    //   Heading1="h1"..Heading6="h6", OrderedList="o",
    //   UnorderedList="u", TaskList="t", Math="m", Code="c", None=""
    [[nodiscard]] auto sub_type_abbr() const -> std::string;

    // Serialize IAL to SiYuan/Kramdown format:
    //   {: id="20210808180117-abc1234" name="value" custom-x="y" }
    // Keys are sorted alphabetically for deterministic output.
    [[nodiscard]] auto ial_string() const -> std::string;

    // --- IAL helpers ---

    // Set a key-value pair in the IAL. Overwrites if key exists.
    auto set_ial(const std::string& key, const std::string& value) -> void;

    // Get a value from the IAL by key. Returns nullopt if key not found.
    [[nodiscard]] auto get_ial(const std::string& key) const -> std::optional<std::string>;

    // Remove a key from the IAL. No-op if key not found.
    auto remove_ial(const std::string& key) -> void;
};

// Sort comparators

// Compare blocks by sort field ascending. Stable: ties broken by id.
[[nodiscard]] auto compare_blocks_by_sort(const Block& a, const Block& b) -> bool;

// Compare blocks by updated field descending (most recent first). Stable: ties broken by id.
[[nodiscard]] auto compare_blocks_by_updated(const Block& a, const Block& b) -> bool;

} // namespace markamp::core
```

## Key Functions to Implement

1. `Block::is_document()` — Returns `type == BlockType::Document`.
2. `Block::is_heading()` — Returns `type == BlockType::Heading`.
3. `Block::is_container()` — Returns true for types that contain children: Document, BlockQuote, List, ListItem, SuperBlock, Table, TableRow, CalloutBlock. All others return false.
4. `Block::is_leaf()` — Returns `!is_container()`.
5. `Block::type_abbr()` — Switch on BlockType, return short string. See SiYuan's `treenode/node.go` TypeAbbr for exact mappings. Unknown types return `"unknown"`.
6. `Block::sub_type_abbr()` — Switch on BlockSubType, return short string. None returns empty string.
7. `Block::ial_string()` — Iterate IAL entries sorted by key, produce `{: key1="val1" key2="val2" }`. Escape double quotes in values with `\"`. If IAL is empty, return empty string.
8. `Block::set_ial()` — `ial[key] = value`.
9. `Block::get_ial()` — Lookup key in ial map, return `std::optional`.
10. `Block::remove_ial()` — `ial.erase(key)`.
11. `compare_blocks_by_sort()` — `a.sort < b.sort`, with `a.id < b.id` as tiebreaker.
12. `compare_blocks_by_updated()` — `a.updated > b.updated`, with `a.id < b.id` as tiebreaker.

## Events to Add (in Events.h)

```cpp
MARKAMP_DECLARE_EVENT(BlockCreatedEvent, std::string /* block_id */, BlockType /* type */)
MARKAMP_DECLARE_EVENT(BlockUpdatedEvent, std::string /* block_id */)
MARKAMP_DECLARE_EVENT(BlockDeletedEvent, std::string /* block_id */, std::string /* parent_id */)
MARKAMP_DECLARE_EVENT(BlockMovedEvent, std::string /* block_id */, std::string /* old_parent_id */, std::string /* new_parent_id */)
MARKAMP_DECLARE_EVENT(BlockFoldedEvent, std::string /* block_id */, bool /* folded */)
```

## Config Keys to Add

- None for this phase.

## Test Cases (Catch2)

File: `tests/unit/test_block_model.cpp`

1. **Block default construction** — Create a default Block, verify: id is empty, type is Paragraph, sub_type is None, sort is 0, folded is false, depth is 0, children is empty, refs is empty, defs is empty.
2. **Block type detection** — For each BlockType value, construct a Block with that type and verify is_document(), is_heading(), is_container(), is_leaf() return the correct booleans. Document should be container, Paragraph should be leaf, Heading should be leaf, List should be container, etc.
3. **Block type abbreviation** — For each BlockType, verify type_abbr() returns the expected short string. Test at minimum: Document->"d", Paragraph->"p", Heading->"h", List->"l", CodeBlock->"c", Table->"t", SuperBlock->"s", EmbedBlock->"e", AttributeView->"av", Unknown->"unknown".
4. **Block subtype abbreviation** — For each BlockSubType, verify sub_type_abbr(). Heading1->"h1", Heading6->"h6", OrderedList->"o", UnorderedList->"u", TaskList->"t", None->"".
5. **IAL set and get** — Set multiple IAL entries, verify get returns correct values, verify get on missing key returns nullopt.
6. **IAL remove** — Set entries, remove one, verify it returns nullopt, verify others still present.
7. **IAL string serialization** — Set id, name, custom-color; verify ial_string() produces `{: custom-color="value" id="..." name="..." }` (alphabetical order). Verify empty IAL produces empty string.
8. **IAL string with special characters** — Set a value containing double quotes, verify they are escaped as `\"` in ial_string() output.
9. **Block sorting by sort field** — Create 5 blocks with different sort values, sort using compare_blocks_by_sort, verify order. Test tiebreaker with same sort value but different IDs.
10. **Block sorting by updated** — Create 5 blocks with different updated timestamps, sort using compare_blocks_by_updated, verify descending order. Test tiebreaker.
11. **Block parent-child relationships** — Create a document block, add 3 child paragraph blocks via children vector, verify count matches, verify parent_id is set correctly on children, verify root_id is consistent.
12. **BlockRef construction** — Create BlockRef with all fields populated, verify each field.

## Acceptance Criteria

- [ ] Block struct compiles and all fields match SiYuan's Block model
- [ ] All BlockType enum values cover SiYuan's node types (29 values including Unknown)
- [ ] All BlockSubType enum values cover SiYuan's sub types (12 values including None)
- [ ] IAL serialization matches SiYuan format `{: key="value" }` with sorted keys
- [ ] All 12 test cases pass
- [ ] Header is self-contained (includes only standard library headers, no circular dependencies)
- [ ] Uses `[[nodiscard]]` on all query methods (is_document, is_heading, is_container, is_leaf, type_abbr, sub_type_abbr, ial_string, get_ial, compare_blocks_by_sort, compare_blocks_by_updated)
- [ ] No use of `any` or `unknown` types
- [ ] Allman brace style throughout
- [ ] 4-space indentation
- [ ] All functions use trailing return types

## Files to Create/Modify

- CREATE: `src/core/Block.h`
- CREATE: `src/core/Block.cpp`
- MODIFY: `src/core/Types.h` (add forward declaration of BlockType if needed for cross-referencing)
- MODIFY: `src/core/Events.h` (add 5 block lifecycle events)
- MODIFY: `src/CMakeLists.txt` (add `core/Block.cpp` to source list)
- CREATE: `tests/unit/test_block_model.cpp`
- MODIFY: `tests/CMakeLists.txt` (add `test_block_model` target linking Catch2)
