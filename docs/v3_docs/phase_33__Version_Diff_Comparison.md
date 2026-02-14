# Phase 33 — Version Diff & Comparison Engine

## Objective

Implement a diff engine for comparing document versions, providing both block-level structural diffing and text-level content diffing. This phase enables users to visually inspect what changed between two snapshots or history entries of a document, powering the history rollback preview functionality introduced in Phase 32. The engine produces structured diff results that can be rendered as side-by-side or inline views with highlighted additions and deletions.

The diff system operates at two distinct levels. Block-level diffing compares two Block trees by matching block IDs — blocks present in both versions are potentially modified, blocks only in the old version are deleted, and blocks only in the new version are added. For blocks that exist in both trees, the engine performs text-level diffing on the content and markdown fields using the Myers diff algorithm, producing a list of DiffHunk records that describe added, deleted, and context lines. This dual-level approach mirrors how SiYuan handles history comparison and gives users both a high-level structural overview and fine-grained content changes.

The visual rendering layer translates DiffResult data into HTML with color-coded additions (green background) and deletions (red background), suitable for display in both a dedicated DiffPanel (wxPanel) and inline within the preview panel. The DiffPanel supports two viewing modes: side-by-side (two scrollable columns with synchronized scrolling) and inline (single column with colored line markers). A stats summary header shows the total number of added, deleted, modified, and unchanged blocks.

## Prerequisites

- Phase 01 (Block Data Model Core)
- Phase 32 (Document History & Versioning)

## SiYuan Source Reference

- `app/src/history/` — History diff visualization and comparison UI
- `kernel/model/history.go` — GetDocHistoryContent, RollbackDocHistory, which load version snapshots for comparison
- `kernel/treenode/blocktree.go` — BlockTree traversal used during diff comparison
- Standard diff algorithm reference: Myers, E.W. "An O(ND) Difference Algorithm and its Variations" (1986), Algorithmica 1(2):251-266

## MarkAmp Integration Points

- New header: `src/core/DiffEngine.h`
- New source: `src/core/DiffEngine.cpp`
- New header: `src/core/DiffTypes.h`
- New header: `src/rendering/DiffRenderer.h`
- New source: `src/rendering/DiffRenderer.cpp`
- New header: `src/ui/DiffPanel.h`
- New source: `src/ui/DiffPanel.cpp`
- Extends `Events.h` with diff-related events
- Connects to Block model from Phase 01 and history system from Phase 32
- DiffPanel integrable into MainFrame layout via LayoutManager

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Type of change detected for a block between two versions.
enum class DiffType : uint8_t
{
    Added,      // Block exists only in the new version
    Deleted,    // Block exists only in the old version
    Modified,   // Block exists in both but content differs
    Unchanged   // Block exists in both with identical content
};

// Type of change for an individual line within a text diff.
enum class LineDiffType : uint8_t
{
    Add,        // Line added in new version
    Delete,     // Line removed from old version
    Context     // Line unchanged, shown for context
};

// A single line in a text diff hunk.
struct DiffLine
{
    LineDiffType type{LineDiffType::Context};
    std::string content;                         // Line text (without trailing newline)
    int old_line_number{-1};                     // Line number in old version (-1 if added)
    int new_line_number{-1};                     // Line number in new version (-1 if deleted)

    [[nodiscard]] auto is_addition() const -> bool
    {
        return type == LineDiffType::Add;
    }

    [[nodiscard]] auto is_deletion() const -> bool
    {
        return type == LineDiffType::Delete;
    }

    [[nodiscard]] auto is_context() const -> bool
    {
        return type == LineDiffType::Context;
    }
};

// A contiguous region of changes in a text diff.
// Analogous to a unified diff hunk header: @@ -old_start,old_count +new_start,new_count @@
struct DiffHunk
{
    int old_start{0};                            // Starting line in old text (0-based)
    int old_count{0};                            // Number of lines from old text
    int new_start{0};                            // Starting line in new text (0-based)
    int new_count{0};                            // Number of lines from new text
    std::vector<DiffLine> lines;                 // Lines in this hunk (adds, deletes, context)

    // Returns the unified diff header string: @@ -old_start,old_count +new_start,new_count @@
    [[nodiscard]] auto header() const -> std::string;

    // Returns true if this hunk contains any actual changes (not all context)
    [[nodiscard]] auto has_changes() const -> bool;

    // Count of added lines in this hunk
    [[nodiscard]] auto added_line_count() const -> int;

    // Count of deleted lines in this hunk
    [[nodiscard]] auto deleted_line_count() const -> int;
};

// Diff result for a single block between two document versions.
struct BlockDiff
{
    std::string block_id;                        // Block ID being compared
    DiffType diff_type{DiffType::Unchanged};     // Type of structural change
    std::optional<Block> old_block;              // Block from old version (nullopt if Added)
    std::optional<Block> new_block;              // Block from new version (nullopt if Deleted)
    std::vector<DiffHunk> text_hunks;            // Text-level diff hunks (for Modified blocks)

    // Returns true if this block has any text-level changes
    [[nodiscard]] auto has_text_changes() const -> bool
    {
        return !text_hunks.empty();
    }

    // Returns total added lines across all hunks
    [[nodiscard]] auto total_added_lines() const -> int;

    // Returns total deleted lines across all hunks
    [[nodiscard]] auto total_deleted_lines() const -> int;
};

// Summary statistics for a diff operation.
struct DiffStats
{
    int added_blocks{0};                         // Blocks only in new version
    int deleted_blocks{0};                       // Blocks only in old version
    int modified_blocks{0};                      // Blocks changed between versions
    int unchanged_blocks{0};                     // Blocks identical between versions
    int total_added_lines{0};                    // Total lines added across all blocks
    int total_deleted_lines{0};                  // Total lines deleted across all blocks
    int64_t elapsed_ms{0};                       // Time taken to compute the diff

    [[nodiscard]] auto total_blocks() const -> int
    {
        return added_blocks + deleted_blocks + modified_blocks + unchanged_blocks;
    }

    [[nodiscard]] auto has_changes() const -> bool
    {
        return added_blocks > 0 || deleted_blocks > 0 || modified_blocks > 0;
    }
};

// Complete diff result between two document versions.
struct DiffResult
{
    std::string old_version_path;                // Path or ID of the old version
    std::string new_version_path;                // Path or ID of the new version
    std::string old_version_label;               // Display label (e.g., timestamp or tag)
    std::string new_version_label;               // Display label (e.g., "Current")
    std::vector<BlockDiff> block_diffs;           // Per-block diff results
    DiffStats stats;                             // Summary statistics

    // Filter block diffs to only those with changes (excludes Unchanged)
    [[nodiscard]] auto changed_blocks() const -> std::vector<const BlockDiff*>;

    // Find the diff entry for a specific block ID
    [[nodiscard]] auto find_block_diff(const std::string& block_id) const
        -> const BlockDiff*;
};

// Configuration for diff behavior.
struct DiffOptions
{
    int context_lines{3};                        // Lines of context around changes
    bool ignore_whitespace{false};               // Ignore leading/trailing whitespace
    bool ignore_case{false};                     // Case-insensitive comparison
    bool include_unchanged{true};                // Include unchanged blocks in result
    int max_file_size_kb{10240};                 // Skip diffing files larger than this
};

// Display mode for the DiffPanel.
enum class DiffViewMode : uint8_t
{
    SideBySide,  // Two-column view with scroll sync
    Inline       // Single-column with colored line markers
};

// DiffEngine — computes structural and textual diffs between document versions.
class DiffEngine
{
public:
    DiffEngine() = default;

    // Compare two documents loaded as Block trees.
    // Returns a complete DiffResult with block-level and text-level diffs.
    [[nodiscard]] auto diff_documents(
        const std::vector<std::shared_ptr<Block>>& old_blocks,
        const std::vector<std::shared_ptr<Block>>& new_blocks,
        const DiffOptions& options = {}) -> DiffResult;

    // Compare two individual blocks, producing text-level diff hunks.
    [[nodiscard]] auto diff_blocks(
        const Block& old_block,
        const Block& new_block,
        const DiffOptions& options = {}) -> std::vector<DiffHunk>;

    // Perform Myers diff on two text strings, producing diff hunks.
    [[nodiscard]] auto diff_text(
        const std::string& old_text,
        const std::string& new_text,
        const DiffOptions& options = {}) -> std::vector<DiffHunk>;

private:
    // Split text into lines for line-by-line comparison.
    [[nodiscard]] auto split_lines(const std::string& text) const
        -> std::vector<std::string>;

    // Myers diff algorithm implementation.
    // Returns an edit script as a sequence of (operation, old_index, new_index) tuples.
    [[nodiscard]] auto myers_diff(
        const std::vector<std::string>& old_lines,
        const std::vector<std::string>& new_lines) const
        -> std::vector<std::tuple<LineDiffType, int, int>>;

    // Convert an edit script to DiffHunk records with context lines.
    [[nodiscard]] auto build_hunks(
        const std::vector<std::tuple<LineDiffType, int, int>>& edit_script,
        const std::vector<std::string>& old_lines,
        const std::vector<std::string>& new_lines,
        int context_lines) const -> std::vector<DiffHunk>;

    // Normalize text for comparison (apply whitespace/case options).
    [[nodiscard]] auto normalize(const std::string& line,
                                  const DiffOptions& options) const -> std::string;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. `DiffEngine::diff_documents()` — Build hash maps of block IDs from both old and new block lists. Classify each block as Added (only in new), Deleted (only in old), Modified (in both, content differs), or Unchanged. For Modified blocks, call `diff_blocks()` to get text-level hunks. Populate DiffStats. Record elapsed time.
2. `DiffEngine::diff_blocks()` — Extract the `markdown` field from both blocks and call `diff_text()`. If markdown is empty, fall back to the `content` field. Return the resulting hunks.
3. `DiffEngine::diff_text()` — Split both texts into lines, run `myers_diff()` to get the edit script, then call `build_hunks()` to produce context-aware DiffHunk records.
4. `DiffEngine::myers_diff()` — Implement the Myers O(ND) difference algorithm. Use the shortest edit script approach: compute the D-path for increasing D until a path reaches the end of both sequences. Backtrack to produce the edit script. Handle edge cases (empty sequences, identical sequences).
5. `DiffEngine::build_hunks()` — Walk the edit script, group consecutive changes into hunks with the requested number of context lines. Merge hunks that overlap in their context regions. Assign line numbers to each DiffLine.
6. `DiffEngine::normalize()` — Apply DiffOptions transformations: trim whitespace if `ignore_whitespace`, lowercase if `ignore_case`. Return the normalized string.
7. `DiffHunk::header()` — Format the unified diff hunk header string `@@ -old_start,old_count +new_start,new_count @@`.
8. `DiffHunk::has_changes()` — Return true if any line in the hunk has type Add or Delete.
9. `DiffHunk::added_line_count()` / `deleted_line_count()` — Count lines of each type using `std::count_if`.
10. `BlockDiff::total_added_lines()` / `total_deleted_lines()` — Sum across all text_hunks.
11. `DiffResult::changed_blocks()` — Filter block_diffs where diff_type is not Unchanged, return vector of pointers.
12. `DiffResult::find_block_diff()` — Linear search block_diffs by block_id, return pointer or nullptr.
13. `DiffRenderer::render_inline()` — Render DiffResult as a single-column HTML document with green-highlighted added lines, red-highlighted deleted lines, and gray context lines. Include line numbers and hunk headers.
14. `DiffRenderer::render_side_by_side()` — Render DiffResult as a two-column HTML table. Left column shows old version lines, right column shows new version lines. Empty cells for added/deleted lines. Scroll sync via JavaScript.
15. `DiffPanel` constructor — Accept `EventBus&`, create wxSplitterWindow for side-by-side mode, wxHtmlWindow for inline mode, stats summary header with wxStaticText labels.

## Events to Add (in Events.h)

```cpp
// ============================================================================
// Diff engine events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiffComputedEvent)
std::string old_version_path;
std::string new_version_path;
int added_blocks{0};
int deleted_blocks{0};
int modified_blocks{0};
int unchanged_blocks{0};
int64_t elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiffViewModeChangedEvent)
DiffViewMode mode{DiffViewMode::Inline};

DiffViewModeChangedEvent() = default;
explicit DiffViewModeChangedEvent(DiffViewMode m)
    : mode(m)
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiffNavigateRequestEvent)
std::string block_id;     // Navigate to this block's diff
bool next{true};          // true = next change, false = previous change
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(DiffCloseRequestEvent);
```

## Config Keys to Add

- `knowledgebase.diff.context_lines` — Number of context lines around each change (default: `3`)
- `knowledgebase.diff.ignore_whitespace` — Ignore whitespace differences (default: `false`)
- `knowledgebase.diff.default_view` — Default diff view mode, `"inline"` or `"side_by_side"` (default: `"inline"`)
- `knowledgebase.diff.max_file_size_kb` — Maximum file size in KB to diff (default: `10240`)
- `knowledgebase.diff.syntax_highlight` — Apply syntax highlighting to diff output (default: `true`)

## Test Cases (Catch2)

File: `tests/unit/test_diff_engine.cpp`

1. **Identical documents produce no changes** — Create two identical Block vectors, run `diff_documents()`. Verify all blocks are Unchanged, stats show zero added/deleted/modified, `has_changes()` returns false.
2. **Single block added** — Old has blocks [A, B], new has blocks [A, B, C]. Verify block C has DiffType::Added, stats.added_blocks == 1, old_block is nullopt.
3. **Single block deleted** — Old has blocks [A, B, C], new has blocks [A, B]. Verify block C has DiffType::Deleted, stats.deleted_blocks == 1, new_block is nullopt.
4. **Block content modified** — Both have block A, but old A has markdown "hello" and new A has markdown "hello world". Verify DiffType::Modified, text_hunks is non-empty, hunks contain an Add line for " world".
5. **Block moved (appears as delete + add)** — Old has [A, B, C] with B having parent_id of A. New has [A, C, B] with B having parent_id of C. Since IDs match, B should be Modified (if content changed) or Unchanged (if only position changed). Verify correct classification.
6. **Multiple simultaneous changes** — Old has [A, B, C, D], new has [A, B', E, D] where B' is modified B and E replaces C. Verify stats: 1 modified (B), 1 deleted (C), 1 added (E), 2 unchanged (A, D).
7. **Text-level diff within a block** — Call `diff_text("line1\nline2\nline3", "line1\nmodified\nline3")`. Verify one hunk with a delete for "line2" and an add for "modified", plus context lines for "line1" and "line3".
8. **Empty document diff** — Both old and new are empty vectors. Verify empty block_diffs, all stats are zero, elapsed_ms >= 0.
9. **Large document diff performance** — Create two Block vectors with 1000 blocks each, 10% modified. Verify diff completes in under 500ms. Verify stats are correct.
10. **Render diff to inline HTML** — Compute a diff with added, deleted, and modified blocks. Call `DiffRenderer::render_inline()`. Verify output contains `class="diff-add"`, `class="diff-delete"`, and `class="diff-context"` CSS classes. Verify line numbers are present.
11. **Myers diff edge case: completely different texts** — `diff_text("aaa\nbbb\nccc", "xxx\nyyy\nzzz")`. Verify 3 deletions and 3 additions.
12. **DiffOptions ignore_whitespace** — `diff_text("  hello  ", "hello")` with `ignore_whitespace=true`. Verify no changes detected.

## Acceptance Criteria

- [ ] DiffEngine computes correct block-level diffs by matching block IDs
- [ ] Myers diff algorithm produces minimal edit scripts for text comparison
- [ ] DiffHunk records include correct line numbers and context lines
- [ ] DiffResult stats accurately count added, deleted, modified, and unchanged blocks
- [ ] DiffRenderer produces valid HTML with color-coded additions and deletions
- [ ] DiffPanel displays both side-by-side and inline views
- [ ] DiffPanel synchronizes scroll position in side-by-side mode
- [ ] All 12 test cases pass
- [ ] Performance: 1000-block diff completes in under 500ms
- [ ] Uses `[[nodiscard]]` on all query methods

## Files to Create/Modify

- CREATE: `src/core/DiffTypes.h`
- CREATE: `src/core/DiffEngine.h`
- CREATE: `src/core/DiffEngine.cpp`
- CREATE: `src/rendering/DiffRenderer.h`
- CREATE: `src/rendering/DiffRenderer.cpp`
- CREATE: `src/ui/DiffPanel.h`
- CREATE: `src/ui/DiffPanel.cpp`
- MODIFY: `src/core/Events.h` (add DiffComputedEvent, DiffViewModeChangedEvent, DiffNavigateRequestEvent, DiffCloseRequestEvent)
- MODIFY: `src/CMakeLists.txt` (add DiffEngine.cpp, DiffRenderer.cpp, DiffPanel.cpp to source lists)
- CREATE: `tests/unit/test_diff_engine.cpp`
- MODIFY: `tests/CMakeLists.txt` (add test_diff_engine target)
