# Phase 10: Sidebar File Tree Component

## Objective

Implement the recursive file tree component for the sidebar, matching the reference `Sidebar.tsx` implementation. The tree displays folders (expandable/collapsible) and files with appropriate icons, supports selection highlighting, and integrates with the event bus for file selection notifications.

## Prerequisites

- Phase 09 (Layout Manager)

## Deliverables

1. FileTreeCtrl -- custom tree control with themed rendering
2. Recursive file node display with proper indentation
3. Folder expand/collapse with chevron indicators
4. File and folder icons (custom-drawn or bitmap)
5. Selection highlighting with accent color
6. Hover effects
7. Integration with event bus for file selection

## Tasks

### Task 10.1: Implement the FileNode data structure

Create `/Users/ryanrentfro/code/markamp/src/core/FileNode.h` and `FileNode.cpp`:

Port the `FileNode` interface from `types.ts`:

```cpp
namespace markamp::core {

enum class FileNodeType { File, Folder };

struct FileNode
{
    std::string id;
    std::string name;
    FileNodeType type{FileNodeType::File};
    std::optional<std::string> content;           // Only for files
    std::vector<FileNode> children;               // Only for folders
    bool is_open{false};                          // Folder toggle state

    // Helpers
    [[nodiscard]] auto is_folder() const -> bool;
    [[nodiscard]] auto is_file() const -> bool;
    [[nodiscard]] auto find_by_id(const std::string& id) -> FileNode*;
    [[nodiscard]] auto find_by_id(const std::string& id) const -> const FileNode*;
    [[nodiscard]] auto file_count() const -> size_t;     // Recursive count of files
    [[nodiscard]] auto folder_count() const -> size_t;   // Recursive count of folders
    [[nodiscard]] auto extension() const -> std::string;  // File extension (e.g., ".md")
};

} // namespace markamp::core
```

**Acceptance criteria:**
- FileNode structure matches the TypeScript `FileNode` interface
- `find_by_id` works recursively through the tree
- File/folder counts are accurate
- Extension extraction handles edge cases (no extension, multiple dots)

### Task 10.2: Create initial file tree data (sample files)

Create `/Users/ryanrentfro/code/markamp/src/core/SampleFiles.h` and `SampleFiles.cpp`:

Port the `INITIAL_FILES` constant from `constants.ts`:

```
My Project/
  README.md       (contains MarkAmp v1.0 sample markdown)
  Diagrams/
    architecture.md   (contains mermaid graph diagram)
    sequence.md       (contains mermaid sequence diagram)
  TODO.md         (contains task list)
```

Each file's content must match the reference exactly, including the mermaid code blocks and task lists.

Also write these as physical sample files to `resources/sample_files/`:
- `resources/sample_files/README.md`
- `resources/sample_files/architecture.md`
- `resources/sample_files/sequence.md`
- `resources/sample_files/TODO.md`

**Acceptance criteria:**
- `get_sample_file_tree()` returns a tree structure matching the reference
- All file contents match the TypeScript constants exactly
- Sample files exist on disk as resources

### Task 10.3: Implement FileTreeCtrl custom control

Create `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.h` and `FileTreeCtrl.cpp`:

A custom-drawn tree control (not using wxTreeCtrl, which has limited theme support).

```cpp
namespace markamp::ui {

class FileTreeCtrl : public ThemeAwareWindow
{
public:
    FileTreeCtrl(wxWindow* parent, core::ThemeEngine& theme_engine);

    // Data
    void SetFileTree(const std::vector<core::FileNode>& roots);
    void SetActiveFileId(const std::string& file_id);

    // Callbacks
    using FileSelectCallback = std::function<void(const core::FileNode&)>;
    void SetOnFileSelect(FileSelectCallback callback);

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    // Rendering
    void OnPaint(wxPaintEvent& event);
    void DrawNode(wxDC& dc, const core::FileNode& node, int depth, int& y_offset);
    void DrawIcon(wxDC& dc, const core::FileNode& node, bool is_open, int x, int y);
    void DrawChevron(wxDC& dc, bool is_open, int x, int y);

    // Interaction
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);

    // Hit testing
    struct HitResult {
        const core::FileNode* node{nullptr};
        bool on_chevron{false};
    };
    auto HitTest(const wxPoint& point) const -> HitResult;

    // State
    std::vector<core::FileNode> roots_;
    std::string active_file_id_;
    std::string hovered_node_id_;
    FileSelectCallback on_file_select_;

    // Layout constants
    static constexpr int kRowHeight = 28;        // py-1 with text
    static constexpr int kIndentWidth = 16;      // 16px per depth level
    static constexpr int kLeftPadding = 8;       // px-2 = 8px
    static constexpr int kIconSize = 14;
    static constexpr int kChevronSize = 12;
    static constexpr int kIconTextGap = 8;       // mr-2 = 8px
};

} // namespace markamp::ui
```

**Drawing specifications (matching reference Sidebar.tsx):**

Each row:
```
[padding: depth*16 + 8px] [icon 14px] [gap 8px] [name text] [chevron 12px at right, folders only]
```

- Selected row: background `accent_primary` at 20% opacity, text in `accent_primary`
- Hovered row: background `accent_primary` at 10% opacity, text changes from `text_muted` to `text_main`
- Normal row: no background, text in `text_muted`
- Font: monospace, small (14px)
- Text overflow: truncated with ellipsis

**Acceptance criteria:**
- File tree renders with correct indentation
- Folders show folder icon and chevron
- Files show file icon
- Selection highlighting works
- Hover effects work
- Clicking a folder toggles expand/collapse
- Clicking a file triggers the callback

### Task 10.4: Implement file and folder icons

Draw file and folder icons programmatically using wxGraphicsContext:

**Folder icon (closed):**
- Simple folder shape in `text_muted` color, 14x14px
- Opacity: 70%

**Folder icon (open):**
- Open folder shape in `text_muted` color, 14x14px
- Opacity: 70%

**File icon:**
- Document shape (rectangle with folded corner) in `text_muted` color, 14x14px
- Opacity: 70%

**Chevron icons:**
- Right-pointing chevron (collapsed): `>` shape, 12x12px, `text_muted` at 50%
- Down-pointing chevron (expanded): `v` shape, 12x12px, `text_muted` at 50%

All icons should be drawn with anti-aliased lines using `wxGraphicsContext`.

**Acceptance criteria:**
- Icons are visually clear and match the reference style
- Icons are drawn in theme-appropriate colors
- Icons scale correctly with DPI

### Task 10.5: Implement folder expand/collapse behavior

When a folder node is clicked:

1. Toggle the `is_open` state on the `FileNode`
2. Recalculate the visible tree layout
3. Redraw the tree
4. Optionally animate the expand/collapse:
   - Expanding: children slide in from top (opacity 0 -> 1)
   - Collapsing: children slide out to top (opacity 1 -> 0)
   - Duration: 150ms

When a folder's chevron is clicked:
- Same behavior as clicking the folder row

**Acceptance criteria:**
- Clicking a folder toggles its children visibility
- Chevron rotates between right and down
- Nested folders work correctly (expand parent, then expand child)
- Expand state persists during the session

### Task 10.6: Implement file selection

When a file node is clicked:

1. Set the `active_file_id_` to the clicked file's ID
2. Apply selection highlighting (accent color background at 20%)
3. Call the `on_file_select_` callback with the FileNode
4. Publish `ActiveFileChangedEvent` via the event bus

**Selection behavior:**
- Only one file can be selected at a time
- Clicking the same file again does nothing (already selected)
- Clicking a folder does NOT change file selection (it just toggles expand)

**Acceptance criteria:**
- Single file selection works
- Selection highlighting is visible and uses accent color
- Event is published on selection change
- Previously selected file loses highlight

### Task 10.7: Integrate FileTreeCtrl into the sidebar

Update the sidebar container (from Phase 09) to include the FileTreeCtrl:

1. Place `FileTreeCtrl` in the scrollable middle section of the sidebar
2. Connect the file selection callback to the AppStateManager
3. Load the sample file tree on startup

**Sidebar footer updates:**
- "FILES" count: show actual file count from the tree
- "SIZE" value: show total content size or keep placeholder

**Acceptance criteria:**
- File tree is visible in the sidebar
- Scrolling works when the tree exceeds sidebar height
- File selection updates the application state
- Sidebar footer shows correct file count

### Task 10.8: Write file tree tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_file_tree.cpp`:

Test cases:
1. FileNode creation (file and folder)
2. Recursive `find_by_id`
3. File/folder count
4. Extension extraction
5. Sample file tree structure verification
6. Toggle expand/collapse state
7. Selection state management
8. Deep nesting (5+ levels)
9. Empty folder handling

**Acceptance criteria:**
- All tests pass
- At least 12 test assertions
- Edge cases covered (empty tree, deeply nested, file with no extension)

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/FileNode.h` | Created (replacing placeholder) |
| `src/core/FileNode.cpp` | Created |
| `src/core/SampleFiles.h` | Created |
| `src/core/SampleFiles.cpp` | Created |
| `src/ui/FileTreeCtrl.h` | Created |
| `src/ui/FileTreeCtrl.cpp` | Created |
| `src/ui/LayoutManager.cpp` | Modified (integrate sidebar) |
| `resources/sample_files/README.md` | Created |
| `resources/sample_files/architecture.md` | Created |
| `resources/sample_files/sequence.md` | Created |
| `resources/sample_files/TODO.md` | Created |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_file_tree.cpp` | Created |

## Dependencies

- wxWidgets 3.2.9 (wxPanel, wxDC, wxGraphicsContext)
- Phase 05 EventBus, AppState
- Phase 08 ThemeEngine, ThemeAwareWindow
- Phase 09 LayoutManager

## Estimated Complexity

**High** -- Custom-drawn tree controls require careful hit testing, scrolling, and paint management. Getting the visual fidelity to match the React reference (hover effects, selection highlighting, icon rendering) demands precise custom drawing code.
