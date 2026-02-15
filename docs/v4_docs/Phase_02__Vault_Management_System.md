# Phase 02 -- Vault Management System

## Objective

Implement a VaultService that manages the workspace as an Obsidian-style vault: a directory of Markdown files with metadata indexing, document lifecycle management, and a file watcher for external changes. Extends the existing NotebookManager (v3) with vault-level operations: open vault, index all documents, resolve wikilinks across the vault, and maintain a live in-memory index of all DocumentModels.

## Prerequisites

- Phase 01 (DocumentModel, WikiLinkParser, WikiLink, Frontmatter)
- v3 Phase 03 (Notebook, NotebookManager)
- Existing EventBus, Config, FileSystem

## Feature References (PRD)

- PRD #3: Markdown-Based Storage
- PRD #4: Local First + Sync Optional
- PRD #33: Folder/Vault Isolation

## Data Structures to Implement

### File: `src/core/VaultService.h`

```cpp
#pragma once

#include "DocumentModel.h"
#include "WikiLinkParser.h"

#include <expected>
#include <filesystem>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

/// Index entry for fast lookup of documents by various keys.
struct VaultIndexEntry
{
    std::string document_id;
    std::string title;
    std::filesystem::path file_path;
    std::string notebook_id;
    std::vector<std::string> aliases;
    std::vector<std::string> tags;
    int64_t modified_time{0};    // Unix milliseconds
    int64_t file_size{0};

    [[nodiscard]] auto matches_name(const std::string& name) const -> bool;
};

/// Manages the vault: file discovery, document indexing, lifecycle, link resolution.
class VaultService
{
public:
    VaultService(EventBus& event_bus, Config& config);
    ~VaultService();

    /// Open a vault directory. Scans all .md files and builds the index.
    [[nodiscard]] auto open_vault(const std::filesystem::path& vault_path)
        -> std::expected<void, std::string>;

    /// Close the current vault and release all resources.
    auto close_vault() -> void;

    /// Get the current vault root path.
    [[nodiscard]] auto vault_path() const -> const std::filesystem::path&;

    /// Check if a vault is currently open.
    [[nodiscard]] auto is_open() const -> bool;

    // --- Document Lifecycle ---

    /// Create a new document in the vault.
    [[nodiscard]] auto create_document(const std::string& title,
                                        const std::string& folder = "",
                                        const Frontmatter& frontmatter = {})
        -> std::expected<std::shared_ptr<DocumentModel>, std::string>;

    /// Open/load a document by file path (relative to vault root).
    [[nodiscard]] auto open_document(const std::filesystem::path& relative_path)
        -> std::expected<std::shared_ptr<DocumentModel>, std::string>;

    /// Save a document to disk.
    [[nodiscard]] auto save_document(const std::string& document_id)
        -> std::expected<void, std::string>;

    /// Delete a document from the vault.
    [[nodiscard]] auto delete_document(const std::string& document_id)
        -> std::expected<void, std::string>;

    /// Rename/move a document within the vault. Updates all wikilinks referencing it.
    [[nodiscard]] auto rename_document(const std::string& document_id,
                                        const std::string& new_title)
        -> std::expected<void, std::string>;

    // --- Index Queries ---

    /// Find a document by title or alias (for wikilink resolution).
    [[nodiscard]] auto find_by_name(const std::string& name) const
        -> std::optional<VaultIndexEntry>;

    /// Find all documents matching a tag.
    [[nodiscard]] auto find_by_tag(const std::string& tag) const
        -> std::vector<VaultIndexEntry>;

    /// List all documents in the vault.
    [[nodiscard]] auto list_documents() const -> std::vector<VaultIndexEntry>;

    /// List all unique tags across the vault.
    [[nodiscard]] auto list_all_tags() const -> std::vector<std::string>;

    /// List all unique tag hierarchies as a tree structure.
    [[nodiscard]] auto list_tag_tree() const
        -> std::vector<std::pair<std::string, std::vector<std::string>>>;

    /// Get the number of indexed documents.
    [[nodiscard]] auto document_count() const -> int;

    // --- Link Resolution ---

    /// Resolve a wikilink target to a document ID.
    [[nodiscard]] auto resolve_wikilink(const std::string& target) const
        -> std::optional<std::string>;

    /// Get all unresolved (broken) wikilinks in the vault.
    [[nodiscard]] auto find_broken_links() const
        -> std::vector<std::pair<std::string, WikiLink>>;

    // --- File Watching ---

    /// Start watching the vault directory for external file changes.
    auto start_file_watcher() -> void;

    /// Stop the file watcher.
    auto stop_file_watcher() -> void;

    // --- Re-indexing ---

    /// Full re-index of the vault (scans all files).
    auto reindex() -> void;

    /// Incremental re-index of a single document.
    auto reindex_document(const std::string& document_id) -> void;

private:
    EventBus& event_bus_;
    Config& config_;

    std::filesystem::path vault_path_;
    bool is_open_{false};

    mutable std::shared_mutex index_mutex_;
    std::unordered_map<std::string, VaultIndexEntry> index_by_id_;
    std::unordered_map<std::string, std::string> name_to_id_;      // title/alias -> doc ID
    std::unordered_map<std::string, std::vector<std::string>> tag_to_ids_;

    std::unordered_map<std::string, std::shared_ptr<DocumentModel>> open_documents_;

    WikiLinkParser link_parser_;

    // File watcher state
    bool watcher_running_{false};

    auto scan_vault_directory() -> std::vector<std::filesystem::path>;
    auto index_file(const std::filesystem::path& file_path) -> std::expected<void, std::string>;
    auto remove_from_index(const std::string& document_id) -> void;
    auto rebuild_name_index() -> void;
    auto rebuild_tag_index() -> void;

    Subscription file_changed_sub_;
    Subscription file_saved_sub_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`open_vault(vault_path)`** -- Validate directory exists. Set vault_path_. Call scan_vault_directory() to enumerate all .md files recursively. For each file, call index_file() to parse frontmatter and build index entry. Build name_to_id_ and tag_to_ids_ lookup maps. Start file watcher. Publish VaultOpenedEvent. Set is_open_ = true.

2. **`scan_vault_directory()`** -- Recursively enumerate all `.md` files under vault_path_. Skip hidden directories (starting with `.`). Skip files in `.trash/` directory. Return sorted list of paths.

3. **`index_file(file_path)`** -- Read file content. Parse frontmatter with WikiLinkParser. Extract title (from frontmatter title field, or first H1, or filename). Generate or extract document ID. Build VaultIndexEntry. Insert into index_by_id_.

4. **`create_document(title, folder, frontmatter)`** -- Generate new BlockID for document. Create Frontmatter with title. Write markdown file to vault. Index the new file. Publish DocumentCreatedEvent. Return the new DocumentModel.

5. **`resolve_wikilink(target)`** -- Look up target in name_to_id_ map. Handle case-insensitive matching. Handle aliases. Handle path-qualified links ("folder/Note"). Return document ID if found, nullopt if not.

6. **`rename_document(id, new_title)`** -- Rename file on disk. Update index entry. Scan all open documents for wikilinks targeting the old name. Update them to the new name. Publish DocumentRenamedEvent.

7. **`find_broken_links()`** -- Iterate all indexed documents. For each outgoing wikilink, attempt resolve. Collect unresolvable links with their source document.

8. **`start_file_watcher()`** -- Use platform file watcher (FSEvents on macOS, inotify on Linux, ReadDirectoryChangesW on Windows via PlatformAbstraction). On file change, queue VaultFileChangedEvent on EventBus. Handle debouncing (100ms) for rapid saves.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultOpenedEvent)
std::string vault_path;
int document_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultClosedEvent)
std::string vault_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentCreatedEvent)
std::string document_id;
std::string title;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentRenamedEvent)
std::string document_id;
std::string old_title;
std::string new_title;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentDeletedEvent)
std::string document_id;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultFileChangedEvent)
std::string file_path;
enum class ChangeType { Created, Modified, Deleted } change_type;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultReindexedEvent)
int document_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `vault.default_path` | string | `""` | Default vault directory |
| `vault.auto_index` | bool | `true` | Auto-index on vault open |
| `vault.watch_external` | bool | `true` | Watch for external file changes |
| `vault.new_file_location` | string | `"root"` | Where new files are created: root, current, specific |
| `vault.excluded_dirs` | string | `".trash,.obsidian,.git"` | Comma-separated dirs to skip |
| `vault.attachment_folder` | string | `"assets"` | Default folder for attachments |

## Test Cases

File: `tests/unit/test_vault_service.cpp`

1. **Open vault -- scans files** -- Create temp dir with 3 .md files. Open vault. Verify document_count() == 3.
2. **Open vault -- skips hidden dirs** -- Add .hidden/ dir with .md file. Verify it is not indexed.
3. **Create document** -- Create document with title. Verify file exists on disk. Verify in index.
4. **Create document with frontmatter** -- Create with tags and aliases. Verify frontmatter persisted.
5. **Find by name** -- Index doc titled "My Note". find_by_name("My Note") returns it.
6. **Find by alias** -- Doc with alias "MN". find_by_name("MN") returns it.
7. **Find by tag** -- 2 docs tagged #project. find_by_tag("project") returns both.
8. **Resolve wikilink** -- Doc titled "Target". resolve_wikilink("Target") returns its ID.
9. **Resolve wikilink case-insensitive** -- resolve_wikilink("target") matches "Target".
10. **Broken links detection** -- Doc links to [[NonExistent]]. find_broken_links() returns it.
11. **Rename document updates links** -- Doc A links to [[Doc B]]. Rename B to "Doc C". Verify A's link updated.
12. **Delete document** -- Delete doc. Verify removed from index and disk.
13. **List all tags** -- Vault with docs having various tags. list_all_tags() returns sorted unique list.
14. **Reindex** -- Modify file externally. Call reindex(). Verify index reflects change.

## Acceptance Criteria

- [ ] VaultService opens a directory and indexes all .md files
- [ ] Document CRUD (create, read, update, delete) works end-to-end
- [ ] Wikilink resolution handles titles, aliases, case-insensitive matching
- [ ] Rename propagates link updates across all referencing documents
- [ ] File watcher detects external changes and triggers re-index
- [ ] Broken link detection finds all unresolvable wikilinks
- [ ] Tag index maintains all unique tags across the vault
- [ ] Thread-safe via shared_mutex on index operations
- [ ] All 14 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/VaultService.h` | VaultService class and VaultIndexEntry |
| CREATE | `src/core/VaultService.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 7 vault/document events |
| MODIFY | `src/core/PluginContext.h` | Add `VaultService* vault_service{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add VaultService.cpp |
| CREATE | `tests/unit/test_vault_service.cpp` | 14 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_vault_service target |

## Architecture Notes

- VaultService depends on DocumentModel and WikiLinkParser from Phase 01
- File watching uses the existing PlatformAbstraction layer for OS-specific APIs
- The index is in-memory only -- no SQLite dependency at this layer (search uses separate FTS)
- Constructor injection: VaultService(EventBus&, Config&)
- open_documents_ cache keeps recently-opened DocumentModels alive for fast access

## Estimated Complexity

**L** -- File system scanning, indexing data structures, link resolution, file watching, rename propagation, 14 tests.
