# Phase 05 — Document File System & .sy Storage Format

## Objective

Implement SiYuan-compatible document file storage. Documents are stored as `.sy` JSON files containing the serialized block tree. Each `.sy` file maps to exactly one root (Document) block and all its descendants. The directory structure within a notebook mirrors the human-readable path hierarchy. This phase handles reading, writing, creating, renaming, moving, duplicating, and deleting document files, along with maintaining path consistency across all operations.

## Prerequisites

- Phase 01 (Block Data Model) — Block struct serialized to/from JSON
- Phase 02 (Block ID Generation) — Document IDs generated for new files
- Phase 03 (Notebook Data Model) — Documents live inside notebooks
- Phase 04 (SQLite Storage Layer) — Index updated after filesystem operations

## SiYuan Source Reference

- `kernel/filesys/filesys.go` — Low-level file operations: `WriteTree()`, `ReadTree()`, `ParseTree()`. Handles `.sy` file format parsing and serialization.
- `kernel/model/file.go` — `CreateDocByMd()`, `RemoveDoc()`, `RenameDoc()`, `MoveDoc()`, `DuplicateDoc()`, `CreateDailyNote()`, `ListDocsByPath()`, `GetDoc()`. All high-level document operations.
- `kernel/model/tree.go` — `LoadTree()`, `loadTree()`, `IndexTree()`, `writeJSONQueue` for async file writing. Tree = root Block + all descendants.
- `kernel/treenode/node.go` — JSON marshaling: each node serializes as `{"ID":"...","Type":"NodeDocument","Properties":{...},"Children":[...]}`.
- `kernel/model/file.go` — `getHPathByPath()` converts filesystem path to human-readable path (strips IDs, uses doc titles).

## MarkAmp Integration Points

- New header: `src/core/DocumentFileSystem.h`
- New source: `src/core/DocumentFileSystem.cpp`
- Depends on: Block.h, BlockID.h, Notebook.h, BlockDatabase.h, EventBus
- Integrates with BlockDatabase for index updates after file operations

## .sy File Format

Each `.sy` file is a JSON document representing a tree of blocks. The root node is always of type `NodeDocument`.

```json
{
    "ID": "20260101120000-abcdefg",
    "Type": "NodeDocument",
    "Properties": {
        "id": "20260101120000-abcdefg",
        "title": "My Document Title",
        "updated": "20260214153000"
    },
    "Children": [
        {
            "ID": "20260101120001-hijklmn",
            "Type": "NodeParagraph",
            "Properties": {
                "id": "20260101120001-hijklmn",
                "updated": "20260214153000"
            },
            "Data": "This is a paragraph with **bold** text.",
            "Children": []
        },
        {
            "ID": "20260101120002-opqrstu",
            "Type": "NodeHeading",
            "Properties": {
                "id": "20260101120002-opqrstu",
                "updated": "20260214153000"
            },
            "HeadingLevel": 2,
            "Data": "Section Title",
            "Children": [
                {
                    "ID": "20260101120003-vwxyz01",
                    "Type": "NodeParagraph",
                    "Properties": {
                        "id": "20260101120003-vwxyz01"
                    },
                    "Data": "Content under the heading."
                }
            ]
        },
        {
            "ID": "20260101120004-2345678",
            "Type": "NodeCodeBlock",
            "Properties": {
                "id": "20260101120004-2345678"
            },
            "CodeBlockInfo": "cpp",
            "Data": "int main() { return 0; }"
        },
        {
            "ID": "20260101120005-9abcdef",
            "Type": "NodeList",
            "ListData": {
                "Typ": 0
            },
            "Children": [
                {
                    "ID": "20260101120006-ghijklm",
                    "Type": "NodeListItem",
                    "Children": [
                        {
                            "ID": "20260101120007-nopqrst",
                            "Type": "NodeParagraph",
                            "Data": "List item text"
                        }
                    ]
                }
            ]
        }
    ]
}
```

### Node Type Strings

The `Type` field in JSON uses SiYuan's node type names (not the C++ enum values):

| BlockType enum      | JSON Type string        |
|---------------------|-------------------------|
| Document            | `"NodeDocument"`        |
| Paragraph           | `"NodeParagraph"`       |
| Heading             | `"NodeHeading"`         |
| BlockQuote          | `"NodeBlockquote"`      |
| List                | `"NodeList"`            |
| ListItem            | `"NodeListItem"`        |
| CodeBlock           | `"NodeCodeBlock"`       |
| MathBlock           | `"NodeMathBlock"`       |
| Table               | `"NodeTable"`           |
| TableRow            | `"NodeTableRow"`        |
| TableCell           | `"NodeTableCell"`       |
| HTMLBlock           | `"NodeHTMLBlock"`       |
| ThematicBreak       | `"NodeThematicBreak"`   |
| Audio               | `"NodeAudio"`           |
| Video               | `"NodeVideo"`           |
| IFrame              | `"NodeIFrame"`          |
| Widget              | `"NodeWidget"`          |
| SuperBlock          | `"NodeSuperBlock"`      |
| EmbedBlock          | `"NodeBlockQueryEmbed"` |
| AttributeView       | `"NodeAttributeView"`   |
| CalloutBlock        | `"NodeCallout"`         |
| MermaidBlock        | `"NodeMermaid"`         |
| PlantUMLBlock       | `"NodePlantUML"`        |
| GraphvizBlock       | `"NodeGraphviz"`        |
| FlowchartBlock      | `"NodeFlowchart"`       |
| MindmapBlock        | `"NodeMindmap"`         |
| EChartsBlock        | `"NodeECharts"`         |
| ABCBlock            | `"NodeABC"`             |

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Represents a serialized node in the .sy JSON file.
// Used as an intermediate representation between JSON and Block.
struct SyNode
{
    std::string id;
    std::string type;                              // JSON type string (e.g., "NodeDocument")
    std::unordered_map<std::string, std::string> properties;  // IAL properties
    std::string data;                              // Text/code content
    int heading_level = 0;                         // For headings (1-6)
    std::string code_block_info;                   // For code blocks (language)
    int list_type = 0;                             // For lists (0=bullet, 1=ordered, 3=task)
    std::vector<SyNode> children;
};

// Manages document files on the filesystem.
// All filesystem operations use std::error_code (no throwing filesystem calls).
// Write operations use atomic write (temp file + rename) for crash safety.
class DocumentFileSystem
{
public:
    explicit DocumentFileSystem(
        EventBus& event_bus,
        Config& config,
        BlockDatabase& database
    );

    // --- Tree I/O ---

    // Load a document tree from a .sy file.
    // Returns the root Block with all children populated.
    // path: relative path within the notebook (e.g., "/20260101120000-abcdefg.sy")
    // box_id: notebook ID (to resolve full filesystem path)
    [[nodiscard]] auto load_tree(
        const std::string& box_id,
        const std::string& path
    ) -> std::expected<std::shared_ptr<Block>, std::string>;

    // Save a document tree to a .sy file.
    // Serializes the root block and all descendants to JSON.
    // Uses atomic write (write to .sy.tmp, then rename to .sy).
    auto save_tree(
        const std::string& box_id,
        const std::shared_ptr<Block>& root
    ) -> std::expected<void, std::string>;

    // --- Document CRUD ---

    // Create a new document from markdown source.
    // Parses markdown into a block tree, assigns IDs, saves .sy file, indexes.
    // hpath: human-readable path (e.g., "/My Notebook/My Document")
    // Returns the root block ID of the new document.
    [[nodiscard]] auto create_doc(
        const std::string& box_id,
        const std::string& hpath,
        const std::string& title,
        const std::string& markdown = ""
    ) -> std::expected<std::string, std::string>;

    // Create a new document from an existing block tree (for import).
    [[nodiscard]] auto create_doc_from_tree(
        const std::string& box_id,
        const std::string& hpath,
        std::shared_ptr<Block> root
    ) -> std::expected<std::string, std::string>;

    // Rename a document. Updates the title in the root block's properties
    // and the human-readable path.
    auto rename_doc(
        const std::string& box_id,
        const std::string& path,
        const std::string& new_title
    ) -> std::expected<void, std::string>;

    // Remove a document. Moves .sy file to history, removes from index.
    auto remove_doc(
        const std::string& box_id,
        const std::string& path
    ) -> std::expected<void, std::string>;

    // Move a document to a different directory within the same notebook
    // or to a different notebook entirely.
    auto move_doc(
        const std::string& src_box_id,
        const std::string& src_path,
        const std::string& dst_box_id,
        const std::string& dst_path
    ) -> std::expected<void, std::string>;

    // Duplicate a document. Creates a copy with all new block IDs.
    // Returns the new root block ID.
    [[nodiscard]] auto duplicate_doc(
        const std::string& box_id,
        const std::string& path
    ) -> std::expected<std::string, std::string>;

    // --- Queries ---

    // List all documents in a notebook directory (non-recursive).
    // dir_path: directory path within the notebook (e.g., "/" or "/subfolder/")
    [[nodiscard]] auto list_docs_in_dir(
        const std::string& box_id,
        const std::string& dir_path
    ) -> std::vector<Block>;

    // List all documents in a notebook (recursive).
    [[nodiscard]] auto list_all_docs(
        const std::string& box_id
    ) -> std::vector<Block>;

    // Get the human-readable path for a document path.
    // Resolves "/20260101-abc.sy" to "/My Notebook/My Document" by reading titles.
    [[nodiscard]] auto get_hpath(
        const std::string& box_id,
        const std::string& path
    ) -> std::string;

    // Check if a .sy file exists at the given path.
    [[nodiscard]] auto doc_exists(
        const std::string& box_id,
        const std::string& path
    ) -> bool;

    // --- Directory Operations ---

    // Create a subdirectory in a notebook.
    auto create_dir(
        const std::string& box_id,
        const std::string& dir_path
    ) -> std::expected<void, std::string>;

    // Rename a subdirectory.
    auto rename_dir(
        const std::string& box_id,
        const std::string& old_path,
        const std::string& new_path
    ) -> std::expected<void, std::string>;

    // Remove a subdirectory and all documents within it.
    auto remove_dir(
        const std::string& box_id,
        const std::string& dir_path
    ) -> std::expected<void, std::string>;

private:
    EventBus& event_bus_;
    Config& config_;
    BlockDatabase& database_;

    // --- JSON Serialization ---

    // Serialize a block tree to a JSON string (.sy format).
    [[nodiscard]] auto serialize_tree(const std::shared_ptr<Block>& root) const -> std::string;

    // Deserialize a JSON string (.sy format) to a block tree.
    [[nodiscard]] auto deserialize_tree(const std::string& json_str)
        -> std::expected<std::shared_ptr<Block>, std::string>;

    // Convert a Block to a SyNode for serialization.
    [[nodiscard]] auto block_to_sy_node(const Block& block) const -> SyNode;

    // Convert a SyNode to a Block.
    [[nodiscard]] auto sy_node_to_block(const SyNode& node) const -> Block;

    // --- Type Conversion ---

    // Convert BlockType enum to JSON type string.
    [[nodiscard]] static auto block_type_to_node_type(BlockType type) -> std::string;

    // Convert JSON type string to BlockType enum.
    [[nodiscard]] static auto node_type_to_block_type(const std::string& node_type) -> BlockType;

    // --- Path Helpers ---

    // Resolve a notebook-relative path to an absolute filesystem path.
    [[nodiscard]] auto resolve_path(
        const std::string& box_id,
        const std::string& relative_path
    ) const -> std::filesystem::path;

    // Generate a .sy filename from a block ID: "{id}.sy"
    [[nodiscard]] static auto sy_filename(const std::string& block_id) -> std::string;

    // Atomic write: write to temp file, then rename.
    auto atomic_write(
        const std::filesystem::path& target,
        const std::string& content
    ) -> std::expected<void, std::string>;

    // Move a file to the history directory for recovery.
    auto move_to_history(
        const std::string& box_id,
        const std::filesystem::path& file_path
    ) -> std::expected<void, std::string>;

    // Recursively assign new block IDs to a tree (for duplication).
    auto reassign_ids(std::shared_ptr<Block>& root) -> void;

    // Flatten a block tree into a vector of blocks (for indexing).
    [[nodiscard]] auto flatten_tree(const std::shared_ptr<Block>& root) -> std::vector<Block>;

    // Index a document tree: flatten and upsert all blocks into the database.
    auto index_tree(
        const std::string& box_id,
        const std::string& path,
        const std::shared_ptr<Block>& root
    ) -> void;

    // Remove a document from the database index.
    auto deindex_tree(const std::string& root_id) -> void;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`load_tree(box_id, path)`** — Resolve filesystem path. Read file contents. Call `deserialize_tree()`. Set `box_id` and `path` on all blocks. Return root block. On file read error, return `std::unexpected` with descriptive error.

2. **`save_tree(box_id, root)`** — Call `serialize_tree()` to get JSON. Resolve filesystem path. Call `atomic_write()` to write to disk. Call `index_tree()` to update the database.

3. **`create_doc(box_id, hpath, title, markdown)`** — Generate root block ID. Create Block with type=Document, set title in properties. If markdown is non-empty, parse it into child blocks (delegate to markdown parser or create a single paragraph block for now). Compute filesystem path from hpath. Ensure parent directories exist. Call `save_tree()`. Fire `DocumentCreatedEvent`. Return root ID.

4. **`rename_doc(box_id, path, new_title)`** — Load the tree. Update the root block's `name` and `Properties["title"]`. Update `hpath`. Save the tree. Fire `DocumentRenamedEvent`.

5. **`remove_doc(box_id, path)`** — Load the tree to get root_id. Move .sy file to history. Call `deindex_tree(root_id)`. Fire `DocumentRemovedEvent`.

6. **`move_doc(src_box_id, src_path, dst_box_id, dst_path)`** — Load tree from source. Update `box_id`, `path`, `hpath` on all blocks. Ensure destination directory exists. Save tree to destination. Remove source file. Reindex. Fire `DocumentMovedEvent`.

7. **`duplicate_doc(box_id, path)`** — Load tree. Call `reassign_ids()` to give all blocks new IDs. Generate new filename from new root ID. Save tree. Index. Fire `DocumentCreatedEvent`. Return new root ID.

8. **`serialize_tree(root)`** — Recursively convert Block tree to SyNode tree. Serialize to JSON with 4-space indentation for readability. Use `block_type_to_node_type()` for type strings.

9. **`deserialize_tree(json_str)`** — Parse JSON. Recursively convert SyNode tree to Block tree using `sy_node_to_block()`. Set parent_id and root_id on all children. Return root block.

10. **`atomic_write(target, content)`** — Write content to `{target}.tmp`. Call `std::filesystem::rename()` to atomically replace the target. On error, remove the temp file and return error.

11. **`reassign_ids(root)`** — Recursively traverse tree. For each block, generate a new ID via `BlockIDGenerator::generate()`. Update `id` and `Properties["id"]`. Update `parent_id` references in children. Update `root_id` to the new root ID.

12. **`flatten_tree(root)`** — Depth-first traversal of the block tree. Add each block to a flat vector with correct `depth`, `parent_id`, `root_id`. Used for database indexing.

13. **`index_tree(box_id, path, root)`** — Flatten the tree. Set `box_id` and `path` on each block. Compute `hpath`. Call `database_.upsert_blocks()`.

14. **`list_docs_in_dir(box_id, dir_path)`** — Resolve directory path. Iterate `.sy` files. For each, read just the root block (don't load full tree for performance). Return as vector of root-level Block.

## Events to Add (in Events.h)

```cpp
MARKAMP_DECLARE_EVENT(DocumentCreatedEvent, std::string /* box_id */, std::string /* root_id */, std::string /* path */)
MARKAMP_DECLARE_EVENT(DocumentRenamedEvent, std::string /* root_id */, std::string /* new_title */)
MARKAMP_DECLARE_EVENT(DocumentRemovedEvent, std::string /* box_id */, std::string /* root_id */, std::string /* path */)
MARKAMP_DECLARE_EVENT(DocumentMovedEvent, std::string /* root_id */, std::string /* old_path */, std::string /* new_path */)
MARKAMP_DECLARE_EVENT(DocumentDuplicatedEvent, std::string /* original_root_id */, std::string /* new_root_id */)
```

## Config Keys to Add

- `knowledgebase.history_dir` — Directory for removed/overwritten file history. Default: `~/.markamp/data/.markamp/history`. Type: string (filesystem path).
- `knowledgebase.atomic_writes` — Enable atomic writes (temp + rename). Disable only for debugging. Default: `true`. Type: bool.

## Test Cases (Catch2)

File: `tests/unit/test_document_filesystem.cpp`

All tests use a temporary directory as the data root. Create a test notebook directory structure in setup.

1. **Create document** — Create a document with title "Test Doc". Verify: `.sy` file exists at expected path, file contains valid JSON, root node has type "NodeDocument", root properties contain title.

2. **Load document** — Create a .sy file manually with known content. Call `load_tree()`. Verify: root block has correct ID, type is Document, children are populated correctly, parent_id and root_id are set on all descendants.

3. **Save and reload round-trip** — Create a block tree in memory (document with 3 paragraphs). Save via `save_tree()`. Load via `load_tree()`. Verify all blocks match: IDs, types, content, properties, parent-child relationships.

4. **Rename document** — Create a document with title "Original". Rename to "Renamed". Verify: root block's name and title property updated, file content reflects new title.

5. **Remove document** — Create a document. Remove it. Verify: `.sy` file no longer exists at original location, history directory contains the moved file.

6. **Move document within notebook** — Create a document at root. Create a subdirectory. Move document into subdirectory. Verify: file exists at new location, old location is empty, block paths updated.

7. **Move document between notebooks** — Create two notebook directories. Create document in notebook 1. Move to notebook 2. Verify: file in notebook 2, gone from notebook 1, box_id updated on all blocks.

8. **Duplicate document** — Create a document with 5 blocks. Duplicate. Verify: two .sy files exist, new document has all new IDs (none match original), content matches, parent-child structure preserved.

9. **List documents in directory** — Create 5 documents at notebook root. Call `list_docs_in_dir()`. Verify 5 results, each with type Document and correct titles.

10. **Concurrent write safety** — Launch 4 threads, each writing different documents to the same notebook. Verify no crashes, all files created, all files contain valid JSON.

11. **Large document (1000+ blocks)** — Create a block tree with 1000+ nested blocks (e.g., 100 headings each with 10 paragraphs). Save. Load. Verify all blocks preserved. Measure that save+load completes in under 2 seconds.

12. **Corrupt file recovery** — Write invalid JSON to a .sy file. Call `load_tree()`. Verify: returns `std::unexpected` with descriptive error message (not a crash). Write a valid JSON file that is missing the "ID" field. Verify: returns error.

13. **Node type string round-trip** — For every BlockType enum value, verify `block_type_to_node_type()` returns the expected string and `node_type_to_block_type()` converts it back to the same enum value.

14. **Atomic write crash safety** — Write a document. Verify `.sy` file exists and `.sy.tmp` does NOT exist. Simulate a partial write (create `.sy.tmp` manually). Call `save_tree()` again. Verify `.sy` is updated and `.sy.tmp` is cleaned up.

15. **Document with all block types** — Create a document containing one block of every BlockType (paragraph, heading, list, code, math, table, etc.). Save. Load. Verify all types round-trip correctly.

## Acceptance Criteria

- [ ] .sy file format matches SiYuan's JSON structure (ID, Type, Properties, Children, Data)
- [ ] All 28 BlockType values map to correct JSON node type strings and back
- [ ] Atomic writes prevent data loss on crash (temp file + rename)
- [ ] Remove moves files to history rather than permanent deletion
- [ ] Duplicate assigns entirely new IDs to all blocks in the copied tree
- [ ] Move updates box_id, path, and hpath on all blocks
- [ ] All filesystem operations use `std::error_code` overloads (no throwing)
- [ ] Index updated (upsert/delete) after every filesystem operation
- [ ] All 15 test cases pass
- [ ] JSON serialization uses 4-space indentation for human readability
- [ ] `[[nodiscard]]` on all query methods
- [ ] Allman brace style, 4-space indent, trailing return types

## Files to Create/Modify

- CREATE: `src/core/DocumentFileSystem.h`
- CREATE: `src/core/DocumentFileSystem.cpp`
- MODIFY: `src/core/Events.h` (add 5 document lifecycle events)
- MODIFY: `src/core/Config.cpp` (add `knowledgebase.history_dir` and `knowledgebase.atomic_writes` defaults)
- MODIFY: `src/CMakeLists.txt` (add `core/DocumentFileSystem.cpp` to source list)
- CREATE: `tests/unit/test_document_filesystem.cpp`
- MODIFY: `tests/CMakeLists.txt` (add `test_document_filesystem` target)

## JSON Library Note

This phase requires JSON serialization/deserialization. Recommended approach:
- Use `nlohmann/json` (header-only, widely used, C++11+). Add via `FetchContent` or `third_party/`.
- Alternative: Use `simdjson` for read-only parsing if performance is critical.
- Alternative: Use `rapidjson` for both read and write.
- CMake: `find_package(nlohmann_json)` or include the single header.
