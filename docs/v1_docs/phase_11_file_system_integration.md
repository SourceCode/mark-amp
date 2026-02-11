# Phase 11: File System Integration

## Objective

Implement real file system operations: opening files from disk, opening folders for browsing, saving files, file watching for external changes, and recent files tracking. This replaces the in-memory sample file tree with actual disk-based file access.

## Prerequisites

- Phase 05 (Application Architecture and Module Design)
- Phase 10 (Sidebar File Tree Component)

## Deliverables

1. FileSystem service implementing IFileSystem interface
2. Open file dialog (single file and folder)
3. Save file (save and save-as)
4. File watching for external modifications
5. Recent files list
6. Dirty file tracking (unsaved changes indicator)
7. File encoding detection

## Tasks

### Task 11.1: Implement the FileSystem service

Create `/Users/ryanrentfro/code/markamp/src/core/FileSystem.h` and `FileSystem.cpp`:

```cpp
namespace markamp::core {

class FileSystem : public IFileSystem
{
public:
    explicit FileSystem(EventBus& event_bus);
    ~FileSystem() override;

    // IFileSystem interface
    auto read_file(const std::filesystem::path& path)
        -> std::expected<std::string, std::string> override;
    auto write_file(const std::filesystem::path& path, std::string_view content)
        -> std::expected<void, std::string> override;
    auto list_directory(const std::filesystem::path& path)
        -> std::expected<std::vector<FileNode>, std::string> override;
    auto watch_file(const std::filesystem::path& path, std::function<void()> callback)
        -> Subscription override;

    // Extended operations
    auto open_file_dialog(wxWindow* parent, const std::string& wildcard = "Markdown files (*.md)|*.md|All files (*.*)|*.*")
        -> std::optional<std::filesystem::path>;
    auto open_folder_dialog(wxWindow* parent)
        -> std::optional<std::filesystem::path>;
    auto save_file_dialog(wxWindow* parent, const std::string& default_name = "untitled.md")
        -> std::optional<std::filesystem::path>;

    // Directory scanning
    auto scan_directory_to_tree(const std::filesystem::path& dir_path, int max_depth = 10)
        -> std::expected<FileNode, std::string>;

private:
    EventBus& event_bus_;
    // File watcher implementation
    struct WatchEntry {
        std::filesystem::path path;
        std::filesystem::file_time_type last_modified;
        std::function<void()> callback;
    };
    std::vector<WatchEntry> watch_entries_;
    wxTimer watch_timer_;
    void OnWatchTimer(wxTimerEvent& event);
};

} // namespace markamp::core
```

**Acceptance criteria:**
- Files can be read from and written to disk
- Errors are returned as `std::expected` values (not exceptions)
- Unicode filenames and paths are handled correctly
- Large files (10MB+) are handled without blocking the UI

### Task 11.2: Implement directory scanning

Implement `scan_directory_to_tree()`:

1. Recursively scan the given directory
2. Create a `FileNode` tree mirroring the directory structure
3. Assign unique IDs based on relative path
4. Only include markdown-related files by default: `.md`, `.markdown`, `.mdown`, `.mkd`, `.txt`
5. Sort entries: folders first (alphabetical), then files (alphabetical)
6. Respect max depth to prevent runaway recursion
7. Skip hidden files/directories (starting with `.`)
8. Skip common non-relevant directories: `node_modules`, `.git`, `build`, `__pycache__`

File content is NOT loaded during scanning (lazy loading). Content is loaded when a file is selected.

**Acceptance criteria:**
- Directory tree is correctly built from disk contents
- Files are filtered by extension
- Hidden files and excluded directories are skipped
- Sorting matches the expected order (folders first)
- Maximum depth is respected
- Symlinks are handled safely (no infinite loops)

### Task 11.3: Implement file open workflow

Create the complete "open file" and "open folder" workflows:

**Open single file (Ctrl+O / Cmd+O):**
1. Show native file dialog (`wxFileDialog`)
2. Read the selected file
3. Create a FileNode for the file
4. Add to the file tree or create a standalone tree
5. Set as active file
6. Publish events

**Open folder (Ctrl+Shift+O / Cmd+Shift+O):**
1. Show native folder dialog (`wxDirDialog`)
2. Scan the directory to build a file tree
3. Replace the current sidebar tree with the new tree
4. Auto-select the first markdown file found
5. Publish events

**Acceptance criteria:**
- File open dialog shows and filters markdown files
- Folder open dialog shows
- Selected file/folder loads correctly
- File tree updates in the sidebar
- Active file is set after opening

### Task 11.4: Implement file save workflow

Create save operations:

**Save (Ctrl+S / Cmd+S):**
1. If the active file has a known path, save to that path
2. If the file is new (no path), redirect to Save As
3. On success: clear dirty flag, publish `FileSavedEvent`
4. On error: show error dialog

**Save As (Ctrl+Shift+S / Cmd+Shift+S):**
1. Show save file dialog (`wxFileDialog` with `wxFD_SAVE | wxFD_OVERWRITE_PROMPT`)
2. Write content to selected path
3. Update the file's path and name
4. Clear dirty flag, publish event

**Auto-save (optional, config-driven):**
- If `editor.auto_save` config is true
- Save dirty files every N seconds (configurable, default 30)
- Do not auto-save new files without paths

**Acceptance criteria:**
- Save writes correct content to disk
- Save As shows dialog and writes to chosen path
- Overwrite confirmation works
- Error handling shows user-friendly messages
- Dirty flag is managed correctly

### Task 11.5: Implement file watching

Monitor open files for external changes:

**Implementation using polling (cross-platform, simple):**
1. Check file modification times every 1000ms via `wxTimer`
2. When a change is detected:
   - If the file has no unsaved changes: silently reload
   - If the file has unsaved changes: show a dialog:
     "File has been modified externally. Reload and lose changes?"
     [Reload] [Keep My Version]
3. Unwatch files that are closed

**Alternative: platform-specific file watching:**
- macOS: `FSEvents`
- Windows: `ReadDirectoryChangesW`
- Linux: `inotify`

Use polling initially for simplicity; platform-specific watchers can be added later.

**Acceptance criteria:**
- External file modifications are detected within 2 seconds
- User is prompted when there are unsaved changes
- Silent reload works when there are no conflicts
- Watching stops when files are closed

### Task 11.6: Implement recent files tracking

Create `/Users/ryanrentfro/code/markamp/src/core/RecentFiles.h` and `RecentFiles.cpp`:

```cpp
namespace markamp::core {

class RecentFiles
{
public:
    explicit RecentFiles(Config& config);

    void add(const std::filesystem::path& path);
    void remove(const std::filesystem::path& path);
    void clear();

    [[nodiscard]] auto list() const -> std::vector<std::filesystem::path>;
    [[nodiscard]] auto max_entries() const -> size_t;

    void load();
    void save();

private:
    Config& config_;
    std::vector<std::filesystem::path> entries_;
    static constexpr size_t kMaxEntries = 20;
};

} // namespace markamp::core
```

Store in config as a JSON array of path strings.

**Acceptance criteria:**
- Recently opened files are tracked
- List is persisted across sessions
- Maximum entry count is enforced (oldest removed)
- Duplicate paths are moved to the top instead of added again
- Non-existent paths are pruned on load

### Task 11.7: Implement dirty file tracking

Track unsaved changes per file:

1. When file content changes in the editor: mark as dirty
2. When file is saved: clear dirty flag
3. Visual indicator in the sidebar: show a dot or asterisk next to dirty files
4. Visual indicator in the chrome: append `*` or a dot to the filename display
5. On close with dirty files: prompt to save
   "You have unsaved changes. Save before closing?"
   [Save] [Don't Save] [Cancel]

**Acceptance criteria:**
- Dirty files are visually indicated in sidebar and chrome
- Close prompt appears when there are unsaved changes
- Saving clears the dirty indicator
- Multiple dirty files are tracked independently

### Task 11.8: Implement file encoding detection and handling

Create `/Users/ryanrentfro/code/markamp/src/core/EncodingDetector.h` and `EncodingDetector.cpp`:

**Detection strategy:**
1. Check for BOM (Byte Order Mark):
   - UTF-8 BOM: `EF BB BF`
   - UTF-16 LE BOM: `FF FE`
   - UTF-16 BE BOM: `FE FF`
2. If no BOM, attempt UTF-8 validation
3. If valid UTF-8, assume UTF-8
4. Otherwise, fall back to the system's default encoding

**Supported encodings for display in status bar:**
- UTF-8 (primary and default)
- UTF-8 with BOM
- UTF-16 LE / BE (read-only, convert to UTF-8 for editing)
- ASCII (subset of UTF-8)

**Acceptance criteria:**
- UTF-8 files load correctly
- UTF-8 BOM files load correctly (BOM is preserved on save)
- Status bar shows the detected encoding
- Non-UTF-8 files show a warning and attempt to load

### Task 11.9: Write file system tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_file_system.cpp`:

Test cases (using temporary directories):
1. Read existing file
2. Read non-existent file (error case)
3. Write file to new path
4. Write file to existing path (overwrite)
5. Directory scanning with various structures
6. File extension filtering
7. Hidden file/directory skipping
8. Encoding detection (UTF-8, BOM, ASCII)
9. Recent files tracking (add, remove, persist, max entries)
10. Dirty file state transitions

**Acceptance criteria:**
- All tests pass
- Tests use temporary directories (cleaned up after)
- At least 15 test assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/FileSystem.h` | Created |
| `src/core/FileSystem.cpp` | Created |
| `src/core/RecentFiles.h` | Created |
| `src/core/RecentFiles.cpp` | Created |
| `src/core/EncodingDetector.h` | Created |
| `src/core/EncodingDetector.cpp` | Created |
| `src/ui/FileTreeCtrl.cpp` | Modified (dirty indicators) |
| `src/ui/CustomChrome.cpp` | Modified (dirty indicator in filename) |
| `src/app/MarkAmpApp.cpp` | Modified (register FileSystem service) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_file_system.cpp` | Created |

## Dependencies

- wxWidgets 3.2.9 (wxFileDialog, wxDirDialog, wxTimer)
- std::filesystem
- Phase 05 EventBus, Config, IFileSystem
- Phase 10 FileTreeCtrl, FileNode

## Estimated Complexity

**Medium** -- File I/O is straightforward, but file watching, encoding detection, and the dirty-file workflow add moderate complexity. Cross-platform path handling requires care.
