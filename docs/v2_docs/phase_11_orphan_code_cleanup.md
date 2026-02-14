# Phase 11: Orphan Code & Dead File Cleanup

**Priority:** Medium
**Estimated Scope:** ~6 files affected
**Dependencies:** None

## Objective

Remove orphan source files that are not compiled, not included by any header, and not referenced by any build target. Consolidate orphaned logic into its correct owning file or delete it if already duplicated.

## Background/Context

The codebase contains at least two orphan `.cpp` files in `src/ui/` that define member functions for existing classes but are:
- NOT listed in `src/CMakeLists.txt`
- NOT `#include`d by any other file
- NOT part of any test target in `tests/CMakeLists.txt`

These files compile to nothing. If the functions they define are already implemented in the main class files, they are dead code. If they represent newer implementations that were meant to replace older versions, the older versions should be updated and the orphan files deleted.

### Orphan File 1: `snippet_mousewheel.cpp`

**File:** `/Users/ryanrentfro/code/markamp/src/ui/snippet_mousewheel.cpp`

Contains `EditorPanel::OnMouseWheel(wxMouseEvent&)` -- a Ctrl+Wheel zoom handler for the editor. This is a loose function definition with no `#include` directives and no header. It appears to be a snippet that was extracted for reference but never wired into the build.

### Orphan File 2: `snippet_preview_zoom.cpp`

**File:** `/Users/ryanrentfro/code/markamp/src/ui/snippet_preview_zoom.cpp`

Contains three functions:
- `PreviewPanel::SetZoomLevel(int)`
- `PreviewPanel::OnMouseWheel(wxMouseEvent&)`
- `PreviewPanel::OnKeyDown(wxKeyEvent&)`

These implement Ctrl+Wheel and Ctrl+Plus/Minus zoom for the preview panel. Like the above, this file has no `#include` directives and is not in any build target.

## Detailed Tasks

### Task 1: Determine if orphan functions are already implemented

**Files to check:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` -- search for `OnMouseWheel`
- `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp` -- search for `SetZoomLevel`, `OnMouseWheel`, `OnKeyDown`

If these methods are already defined in the main `.cpp` files (which is expected given that the application builds and runs without the orphan files), then the orphan files are redundant.

### Task 2: Delete orphan files (if confirmed redundant)

Delete the following files:
```
/Users/ryanrentfro/code/markamp/src/ui/snippet_mousewheel.cpp
/Users/ryanrentfro/code/markamp/src/ui/snippet_preview_zoom.cpp
```

If any functions in the orphan files are NOT present in the main class files, merge them into the main files before deleting the orphans.

### Task 3: Verify MarkdownDocument.h existence

**File:** `/Users/ryanrentfro/code/markamp/src/core/MarkdownDocument.cpp`

This file is listed in the `add_executable()` source list (line 93 of `src/CMakeLists.txt`) but `#include`s `Types.h`, not a `MarkdownDocument.h`. The class `MdNode` and related types are defined in `Types.h`. This is not technically broken (the file compiles), but:

- It is listed in the build but NOT in the `source_group()` IDE listing
- There is no corresponding `.h` file named `MarkdownDocument.h`

Either:
a) Rename the file to `MdNode.cpp` to match the class it implements, or
b) Add it to the `source_group()` section with a comment explaining it implements types from `Types.h`

### Task 4: Verify RecentWorkspaces in source_group

**File:** `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

`RecentWorkspaces.cpp` is listed in `add_executable()` (line 96) but also verify it appears in the `source_group()` block. Add if missing:
```cmake
core/RecentWorkspaces.h
core/RecentWorkspaces.cpp
```

### Task 5: Search for any other orphan files

Run a diff between the files listed in `add_executable()` and the actual files in `src/`:
```bash
# List all .cpp/.h files in src/ (excluding .mm)
find src/ -name "*.cpp" -o -name "*.h" | sort > /tmp/actual_files.txt
# List all files in CMakeLists.txt source lists
# Compare to find files not in any build target
```

Any `.cpp` file in `src/` that is not in `add_executable()` or any test target is a candidate for removal.

### Task 6: Remove dead code markers

After removing orphan files, grep for any `#include` or reference to the removed filenames and clean up.

## Acceptance Criteria

1. No `.cpp` files exist in `src/` that are outside all build targets (unless intentionally documentation/snippet files moved to `docs/`)
2. `snippet_mousewheel.cpp` and `snippet_preview_zoom.cpp` are deleted (or their content is merged)
3. `source_group()` in `src/CMakeLists.txt` lists all files from `add_executable()`
4. The application builds and all tests pass after cleanup

## Testing Requirements

- Full build succeeds (`cmake --build build/debug`)
- All 21+ test targets pass (`ctest --output-on-failure`)
- No linker errors from missing symbols
