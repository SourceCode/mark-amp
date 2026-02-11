# MarkAmp v1.0.0 – End-to-End Integration Test Plan

Each workflow lists sequential steps and the expected result (pass criteria). Run against a clean build with default config.

---

## Workflow 1: First Launch Experience

| Step | Action                              | Expected Result                        |
| ---- | ----------------------------------- | -------------------------------------- |
| 1.1  | Launch MarkAmp with no prior config | Window opens without crash             |
| 1.2  | Check active theme                  | "Midnight Neon" is applied             |
| 1.3  | Check sidebar                       | Sample file tree visible with ≥1 entry |
| 1.4  | Check selected file                 | README.md is selected and displayed    |
| 1.5  | Check view mode                     | Split view is active                   |
| 1.6  | Check status bar                    | Shows "READY", cursor 1:1, UTF-8       |

## Workflow 2: File Editing

| Step | Action                  | Expected Result                |
| ---- | ----------------------- | ------------------------------ |
| 2.1  | Click a file in sidebar | Content loads in editor        |
| 2.2  | Type text               | Preview updates after debounce |
| 2.3  | Check status bar        | Cursor position updated        |
| 2.4  | Check dirty state       | File marked as modified        |
| 2.5  | Save (Ctrl+S)           | Dirty indicator clears         |
| 2.6  | Undo (Ctrl+Z)           | Dirty indicator returns        |

## Workflow 3: Theme Management

| Step | Action                | Expected Result                         |
| ---- | --------------------- | --------------------------------------- |
| 3.1  | Open theme gallery    | All 8 built-in themes displayed         |
| 3.2  | Click different theme | Entire UI updates to new colors         |
| 3.3  | Verify all zones      | Editor, preview, chrome, sidebar themed |
| 3.4  | Export theme          | Valid JSON file saved                   |
| 3.5  | Import exported theme | Theme appears in gallery                |
| 3.6  | Press Escape          | Gallery closes                          |

## Workflow 4: View Mode Switching

| Step | Action                   | Expected Result               |
| ---- | ------------------------ | ----------------------------- |
| 4.1  | Click SRC                | Only editor visible           |
| 4.2  | Click SPLIT              | Editor + preview side by side |
| 4.3  | Click VIEW               | Only preview visible          |
| 4.4  | Ctrl+1 / Ctrl+2 / Ctrl+3 | Same as button clicks         |
| 4.5  | Switch back to SPLIT     | Content consistent            |

## Workflow 5: Markdown Features

| Step | Action                  | Expected Result                       |
| ---- | ----------------------- | ------------------------------------- |
| 5.1  | Open file with headings | H1–H6 render correctly                |
| 5.2  | Check lists             | Ordered, unordered, task lists render |
| 5.3  | Check code blocks       | Syntax highlighted with language      |
| 5.4  | Check tables            | Table with headers and rows           |
| 5.5  | Check Mermaid diagram   | Renders as SVG                        |
| 5.6  | Enter invalid Mermaid   | Error overlay appears                 |
| 5.7  | Fix the syntax          | Diagram renders again                 |

## Workflow 6: File Management

| Step | Action                     | Expected Result            |
| ---- | -------------------------- | -------------------------- |
| 6.1  | Open folder (Ctrl+Shift+O) | File tree populates        |
| 6.2  | Navigate tree (arrow keys) | Focus moves correctly      |
| 6.3  | Expand/collapse folders    | Toggle works               |
| 6.4  | Select file                | Content loaded             |
| 6.5  | Save As (Ctrl+Shift+S)     | File saved to new location |

## Workflow 7: Find and Replace

| Step | Action                | Expected Result        |
| ---- | --------------------- | ---------------------- |
| 7.1  | Ctrl+F                | Find bar opens         |
| 7.2  | Type search term      | Matches highlighted    |
| 7.3  | Next/Previous buttons | Cycles through matches |
| 7.4  | Ctrl+H                | Replace mode opens     |
| 7.5  | Replace one           | Single match replaced  |
| 7.6  | Replace all           | All matches replaced   |
| 7.7  | Escape                | Find bar closes        |

## Workflow 8: Sidebar Management

| Step | Action                  | Expected Result                 |
| ---- | ----------------------- | ------------------------------- |
| 8.1  | Toggle sidebar (Ctrl+B) | Sidebar hides, content expands  |
| 8.2  | Toggle again            | Sidebar reappears at same width |
| 8.3  | Drag divider            | Sidebar resizes                 |
| 8.4  | Hide + show             | Width persists                  |

## Workflow 9: Keyboard Navigation

| Step | Action                | Expected Result                   |
| ---- | --------------------- | --------------------------------- |
| 9.1  | Tab                   | Cycles focus through zones        |
| 9.2  | Arrow keys in sidebar | Navigate files/folders            |
| 9.3  | Enter on file         | Opens file                        |
| 9.4  | Ctrl+F                | Opens find bar                    |
| 9.5  | Escape                | Closes find bar                   |
| 9.6  | F1                    | Opens keyboard shortcut reference |
| 9.7  | All operations        | Work without mouse                |

## Workflow 10: Performance Under Load

| Step | Action                    | Expected Result                 |
| ---- | ------------------------- | ------------------------------- |
| 10.1 | Open 10,000-line markdown | File loads in <2s               |
| 10.2 | Type in editor            | No perceptible lag              |
| 10.3 | Scroll preview            | Smooth scrolling                |
| 10.4 | Switch themes 5 times     | Instant switching (<100ms each) |
