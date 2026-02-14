# Phase 03 — Notebook (Box) Data Model & Management

## Objective

Implement the Notebook (called "Box" in SiYuan's codebase) data model and management service. A Notebook is the top-level container that holds documents. Each notebook has its own ID, name, icon, sort order, and can be opened/closed independently. Notebooks map to directories on disk, and their configuration is stored in a `.markamp/conf.json` file within each notebook directory.

## Prerequisites

- Phase 01 (Block Data Model) — Notebook contains documents which contain blocks
- Phase 02 (Block ID Generation) — Notebook IDs use the same generation format

## SiYuan Source Reference

- `kernel/model/box.go` — Box struct definition: `ID`, `Name`, `Icon`, `Sort`, `Closed`, `NewFlashcardCount`, `DueFlashcardCount`, `FlashcardCount`
- `kernel/model/box.go` — Functions: `ListNotebooks()`, `OpenNotebook()`, `CloseNotebook()`, `CreateNotebook()`, `RemoveNotebook()`, `RenameNotebook()`, `SetNotebookIcon()`, `SortNotebooks()`, `MountNotebook()`, `UnmountNotebook()`
- `kernel/conf/box.go` — BoxConf struct: `Name`, `Sort`, `Closed`, `RefCreateSavePath`, `CreateDocNameTemplate`, `DailyNoteSavePath`, `DailyNoteTemplatePath`
- `kernel/model/mount.go` — Notebook mount/unmount operations (mounting = making a notebook visible/accessible)
- `kernel/model/box.go` — `Notebook.GetConf()` reads `.siyuan/conf.json` from the notebook directory
- `kernel/model/box.go` — `Notebook.SaveConf()` writes `.siyuan/conf.json`

## MarkAmp Integration Points

- New header: `src/core/Notebook.h`
- New source: `src/core/Notebook.cpp`
- New service header: `src/core/NotebookManager.h`
- New service source: `src/core/NotebookManager.cpp`
- Integrates with EventBus for notebook lifecycle events
- Integrates with Config for data directory path
- Uses BlockIDGenerator from Phase 02 for new notebook IDs

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Configuration for a single notebook, persisted to .markamp/conf.json
// inside the notebook's directory.
struct NotebookConf
{
    std::string name;                          // Display name
    int32_t sort = 0;                          // Sort position among notebooks
    bool closed = false;                       // Whether the notebook is closed (not indexed)

    // Document creation settings
    std::string ref_create_save_path;          // Where to save docs created from refs (e.g., "/")
    std::string create_doc_name_template;      // Template for new doc names (e.g., "{{.title}}")
    std::string daily_note_save_path;          // Daily note directory (e.g., "/daily notes/")
    std::string daily_note_template_path;      // Path to daily note template doc
};

// Represents a single notebook (top-level container for documents).
struct Notebook
{
    std::string id;                            // Notebook ID (block ID format, 22 chars)
    std::string name;                          // Display name
    std::string icon;                          // Emoji or icon identifier (e.g., "1f4d4")
    int32_t sort = 0;                          // Sort position
    bool closed = false;                       // True = closed (not indexed, not searchable)

    // Flashcard statistics (populated on demand, not persisted in conf)
    int32_t new_flashcard_count = 0;
    int32_t due_flashcard_count = 0;
    int32_t flashcard_count = 0;

    // Computed statistics (populated on demand, not persisted in conf)
    int64_t doc_count = 0;                     // Number of documents in notebook
    int64_t block_count = 0;                   // Total blocks across all docs
    int64_t size_bytes = 0;                    // Total size on disk

    // Full configuration
    NotebookConf conf;

    // Query helpers
    [[nodiscard]] auto is_open() const -> bool;
};

// Manages all notebooks: creation, deletion, opening/closing, sorting.
// Thread-safe via shared_mutex. All filesystem operations use std::error_code
// overloads (no exceptions from filesystem).
class NotebookManager
{
public:
    // Constructor injection of dependencies.
    // event_bus: for publishing notebook lifecycle events
    // config: for reading data_dir and default settings
    explicit NotebookManager(EventBus& event_bus, Config& config);

    // --- CRUD ---

    // Create a new notebook with the given name.
    // Creates directory at {data_dir}/{generated_id}/ with .markamp/conf.json.
    // save_path: optional override for the notebook directory location.
    // Returns the new notebook ID on success.
    [[nodiscard]] auto create_notebook(
        const std::string& name,
        const std::string& save_path = ""
    ) -> std::expected<std::string, std::string>;

    // Rename an existing notebook.
    auto rename_notebook(
        const std::string& id,
        const std::string& new_name
    ) -> std::expected<void, std::string>;

    // Remove a notebook. Moves its directory to {data_dir}/.markamp/history/
    // rather than permanently deleting it.
    auto remove_notebook(const std::string& id) -> std::expected<void, std::string>;

    // Set the notebook icon (emoji code or icon identifier).
    auto set_notebook_icon(const std::string& id, const std::string& icon) -> void;

    // --- Open/Close ---

    // Open a closed notebook. This triggers indexing of all its documents.
    auto open_notebook(const std::string& id) -> std::expected<void, std::string>;

    // Close an open notebook. This removes its documents from the index.
    auto close_notebook(const std::string& id) -> std::expected<void, std::string>;

    // --- Query ---

    // List all notebooks sorted by sort field.
    [[nodiscard]] auto list_notebooks() const -> std::vector<Notebook>;

    // Get a specific notebook by ID. Returns nullopt if not found.
    [[nodiscard]] auto get_notebook(const std::string& id) const -> std::optional<Notebook>;

    // Get only open (not closed) notebooks.
    [[nodiscard]] auto get_open_notebooks() const -> std::vector<Notebook>;

    // --- Sorting ---

    // Reorder notebooks. ordered_ids contains all notebook IDs in desired order.
    // Notebooks not in the list keep their current sort values.
    auto sort_notebooks(const std::vector<std::string>& ordered_ids) -> void;

    // --- Configuration ---

    // Get the configuration for a specific notebook.
    [[nodiscard]] auto get_notebook_conf(const std::string& id) const
        -> std::optional<NotebookConf>;

    // Update the configuration for a specific notebook. Persists to disk.
    auto set_notebook_conf(const std::string& id, const NotebookConf& conf) -> void;

    // --- Initialization ---

    // Scan the data directory and load all notebook configurations.
    // Called once at startup.
    auto load_notebooks() -> void;

private:
    EventBus& event_bus_;
    Config& config_;
    std::vector<Notebook> notebooks_;
    mutable std::shared_mutex mutex_;   // Protects notebooks_ vector

    // Load notebooks from disk by scanning data_dir for notebook directories.
    // A notebook directory contains a .markamp/conf.json file.
    auto load_notebooks_from_disk(const std::filesystem::path& data_dir) -> void;

    // Persist a single notebook's conf.json to disk.
    auto save_notebook_conf(const std::string& id) -> void;

    // Compute the filesystem path for a notebook's data directory.
    [[nodiscard]] auto notebook_data_path(const std::string& id) const -> std::filesystem::path;

    // Compute the filesystem path for a notebook's conf.json.
    [[nodiscard]] auto notebook_conf_path(const std::string& id) const -> std::filesystem::path;

    // Get the root data directory from config.
    [[nodiscard]] auto data_dir() const -> std::filesystem::path;

    // Find a notebook by ID in the notebooks_ vector (caller must hold lock).
    [[nodiscard]] auto find_notebook_unlocked(const std::string& id)
        -> std::vector<Notebook>::iterator;
    [[nodiscard]] auto find_notebook_unlocked(const std::string& id) const
        -> std::vector<Notebook>::const_iterator;
};

} // namespace markamp::core
```

## Directory Structure on Disk

```
~/.markamp/data/                           # data_dir (from config)
  20260101120000-abcdefg/                  # Notebook directory (ID as folder name)
    .markamp/
      conf.json                            # NotebookConf serialized as JSON
    20260101120100-hijklmn.sy              # Document file (root block)
    20260101120200-opqrstu.sy              # Another document
    subfolder/                             # Subdirectory (for nested docs)
      20260101120300-vwxyz01.sy
  20260102130000-1234567/                  # Another notebook
    .markamp/
      conf.json
    ...
  .markamp/                                # Global config/history
    history/                               # Removed notebooks moved here
```

## conf.json Format

```json
{
    "name": "My Notebook",
    "sort": 0,
    "closed": false,
    "refCreateSavePath": "/",
    "createDocNameTemplate": "",
    "dailyNoteSavePath": "/daily notes/",
    "dailyNoteTemplatePath": ""
}
```

## Key Functions to Implement

1. **`create_notebook(name, save_path)`** — Generate a new ID via `BlockIDGenerator::generate()`. Create directory `{data_dir}/{id}/`. Create `.markamp/` subdirectory. Write `conf.json` with the name and defaults. Add to `notebooks_` vector. Fire `NotebookCreatedEvent`. Return the ID. On filesystem error, return the error message.

2. **`rename_notebook(id, new_name)`** — Find notebook by ID. Update name in memory. Persist conf.json. Fire `NotebookRenamedEvent`. Return error if ID not found.

3. **`remove_notebook(id)`** — Find notebook by ID. Move the directory to `{data_dir}/.markamp/history/{id}_{timestamp}/`. Remove from `notebooks_` vector. Fire `NotebookRemovedEvent`. Return error if ID not found or move fails.

4. **`open_notebook(id)`** — Find notebook. Set `closed = false`. Persist conf.json. Fire `NotebookOpenedEvent`. The event signals to the indexing system (Phase 04/06) that this notebook's documents should be indexed.

5. **`close_notebook(id)`** — Find notebook. Set `closed = true`. Persist conf.json. Fire `NotebookClosedEvent`. The event signals to the indexing system that this notebook's blocks should be removed from the index.

6. **`list_notebooks()`** — Acquire shared lock. Return copy of `notebooks_` sorted by `sort` field.

7. **`get_notebook(id)`** — Acquire shared lock. Find by ID. Return copy or nullopt.

8. **`load_notebooks_from_disk(data_dir)`** — Iterate directories in `data_dir`. For each directory that contains `.markamp/conf.json`, read the conf, construct a `Notebook` struct, add to `notebooks_`. Sort by `sort` field. Use `std::error_code` overloads for all filesystem operations.

9. **`save_notebook_conf(id)`** — Find notebook. Serialize its `conf` to JSON. Write to `{data_dir}/{id}/.markamp/conf.json`. Use atomic write (write to temp file, then rename) for crash safety.

10. **`sort_notebooks(ordered_ids)`** — Assign `sort = index` for each ID in the ordered_ids vector. Persist all changed conf files. Fire `NotebooksSortedEvent`.

## Events to Add (in Events.h)

```cpp
MARKAMP_DECLARE_EVENT(NotebookCreatedEvent, std::string /* notebook_id */, std::string /* name */)
MARKAMP_DECLARE_EVENT(NotebookRenamedEvent, std::string /* notebook_id */, std::string /* new_name */)
MARKAMP_DECLARE_EVENT(NotebookRemovedEvent, std::string /* notebook_id */)
MARKAMP_DECLARE_EVENT(NotebookOpenedEvent, std::string /* notebook_id */)
MARKAMP_DECLARE_EVENT(NotebookClosedEvent, std::string /* notebook_id */)
MARKAMP_DECLARE_EVENT(NotebooksSortedEvent)
```

## Config Keys to Add

- `knowledgebase.data_dir` — Root data directory for all notebooks. Default: `~/.markamp/data`. Type: string (filesystem path).
- `knowledgebase.default_notebook` — ID of the default notebook for new documents when no notebook is explicitly specified. Default: empty string (use first open notebook). Type: string.

## Test Cases (Catch2)

File: `tests/unit/test_notebook.cpp`

All tests should use a temporary directory (created in test setup, removed in teardown) as the data_dir to avoid polluting the real filesystem.

1. **Create notebook** — Create a notebook named "Test". Verify: returned ID is valid (22 chars, correct format), directory `{data_dir}/{id}/` exists, `.markamp/conf.json` exists and contains correct name, `list_notebooks()` returns 1 notebook with matching name and ID.

2. **Rename notebook** — Create a notebook, rename it to "Renamed". Verify: `get_notebook(id)` returns the new name, conf.json on disk reflects the new name.

3. **Remove notebook** — Create a notebook, remove it. Verify: `get_notebook(id)` returns nullopt, `list_notebooks()` is empty, original directory no longer exists, history directory contains the moved notebook.

4. **Open and close** — Create a notebook (starts open). Close it. Verify `is_open()` returns false and conf.json has `closed: true`. Open it. Verify `is_open()` returns true and conf.json has `closed: false`.

5. **List notebooks sorted** — Create 3 notebooks with names "C", "A", "B". Set their sort orders to 2, 0, 1 respectively. Call `list_notebooks()`. Verify they come back in order: A(0), B(1), C(2).

6. **Sort notebooks by ID list** — Create 3 notebooks. Call `sort_notebooks({id3, id1, id2})`. Verify `list_notebooks()` returns them in that order.

7. **Notebook conf persistence** — Create a notebook. Set custom conf (daily_note_save_path, create_doc_name_template). Destroy the NotebookManager. Create a new one. Call `load_notebooks()`. Verify the conf values survived the round-trip.

8. **Duplicate names allowed** — Create two notebooks with the same name "Duplicate". Verify both exist with different IDs.

9. **Invalid ID operations** — Call `rename_notebook("nonexistent", "x")`, `remove_notebook("nonexistent")`, `open_notebook("nonexistent")`, `close_notebook("nonexistent")`, `get_notebook("nonexistent")`. Verify all return error or nullopt without crashing.

10. **Concurrent read/write** — Launch 4 threads: 2 calling `list_notebooks()` in a loop, 1 calling `create_notebook()` in a loop, 1 calling `rename_notebook()` in a loop. Run for 100ms. Verify no crashes or data corruption (final notebook count matches expected).

11. **Set notebook icon** — Create a notebook. Set icon to "1f4d4". Verify `get_notebook(id)->icon == "1f4d4"`.

12. **Get open notebooks filter** — Create 3 notebooks. Close one. Call `get_open_notebooks()`. Verify it returns exactly 2 notebooks, all with `is_open() == true`.

## Acceptance Criteria

- [ ] Notebook struct matches SiYuan's Box model fields
- [ ] NotebookConf struct matches SiYuan's BoxConf fields
- [ ] Create/rename/remove perform proper filesystem operations with `std::error_code`
- [ ] Events fired for all lifecycle changes (created, renamed, removed, opened, closed, sorted)
- [ ] Thread-safe access via `std::shared_mutex` (readers-writer lock)
- [ ] Conf persisted to disk as JSON in `.markamp/conf.json`
- [ ] Notebook removal moves to history (not permanent delete)
- [ ] Atomic writes for conf.json (temp file + rename)
- [ ] All 12 test cases pass
- [ ] Uses `[[nodiscard]]` on all query methods
- [ ] No use of `catch(...)` — only typed exception handlers
- [ ] Allman brace style, 4-space indent, trailing return types

## Files to Create/Modify

- CREATE: `src/core/Notebook.h`
- CREATE: `src/core/Notebook.cpp`
- CREATE: `src/core/NotebookManager.h` (or combine into Notebook.h if small enough)
- CREATE: `src/core/NotebookManager.cpp` (or combine into Notebook.cpp)
- MODIFY: `src/core/Events.h` (add 6 notebook lifecycle events)
- MODIFY: `src/core/Config.cpp` (add knowledgebase.data_dir and knowledgebase.default_notebook defaults)
- MODIFY: `src/CMakeLists.txt` (add Notebook.cpp and/or NotebookManager.cpp to source list)
- CREATE: `tests/unit/test_notebook.cpp`
- MODIFY: `tests/CMakeLists.txt` (add `test_notebook` target linking Catch2)
