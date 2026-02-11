# Phase 20: Editor Enhancements -- Line Numbers, Word Wrap, Large Files

## Objective

Enhance the text editor with advanced features: toggleable line number gutter, configurable word wrap, large file handling with virtualized rendering, minimap, current line highlighting, bracket matching, and auto-indent for markdown structures.

## Prerequisites

- Phase 12 (Text Editor Component)

## Deliverables

1. Line number gutter (toggleable, theme-aware)
2. Configurable word wrap modes
3. Large file optimization (files over 10,000 lines)
4. Current line highlighting
5. Bracket and quote matching
6. Auto-indent for markdown (lists, blockquotes, code blocks)
7. Minimap (optional enhancement)
8. Editor preferences dialog

## Tasks

### Task 20.1: Implement line number gutter

Add a line number gutter to the left of the editor:

**Visual specifications:**
- Background: slightly darker than editor background (`bg_panel` or `bg_app`)
- Text color: `text_muted` at 50% opacity
- Font: same monospace font, same size
- Width: auto-sized to fit the largest line number + padding
- Right border: 1px `border_light` at 20% opacity
- Current line number: `accent_primary` color, bold

**Scintilla configuration:**
```cpp
void EditorPanel::SetShowLineNumbers(bool enabled)
{
    if (enabled) {
        int line_count = editor_->GetLineCount();
        int digits = std::max(3, static_cast<int>(std::log10(line_count)) + 1);
        int width = editor_->TextWidth(wxSTC_STYLE_LINENUMBER, std::string(digits + 1, '9'));
        editor_->SetMarginWidth(0, width);
        editor_->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    } else {
        editor_->SetMarginWidth(0, 0);
    }
}
```

**Dynamic width:**
- Update gutter width when line count changes significantly
- e.g., going from 99 to 100 lines should widen from 2 digits to 3

**Toggle:**
- Menu: View > Show Line Numbers (Ctrl+L / Cmd+L)
- Persisted in config: `editor.show_line_numbers`

**Acceptance criteria:**
- Line numbers display correctly
- Current line number is highlighted
- Gutter resizes for large files
- Toggle works and persists
- Theme colors applied to gutter

### Task 20.2: Implement configurable word wrap modes

Support multiple word wrap modes:

**Modes:**
1. **No wrap**: long lines extend horizontally, horizontal scroll appears
2. **Word wrap**: wrap at word boundaries, no horizontal scroll (default)
3. **Character wrap**: wrap at character boundaries

**Scintilla configuration:**
```cpp
void EditorPanel::SetWordWrapMode(WrapMode mode)
{
    switch (mode) {
        case WrapMode::None:
            editor_->SetWrapMode(wxSTC_WRAP_NONE);
            break;
        case WrapMode::Word:
            editor_->SetWrapMode(wxSTC_WRAP_WORD);
            break;
        case WrapMode::Character:
            editor_->SetWrapMode(wxSTC_WRAP_CHAR);
            break;
    }
}
```

**Word wrap indicator:** When wrapping is on, show a subtle visual indicator at the wrap point (optional):
- Scintilla's `SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_END)` shows a small arrow

**Toggle:**
- Menu: View > Word Wrap (Alt+Z)
- Persisted in config: `editor.word_wrap_mode`

**Acceptance criteria:**
- All three wrap modes work correctly
- Toggle shortcut works
- Setting persists across sessions
- Large lines with no-wrap show horizontal scrollbar

### Task 20.3: Implement large file handling

Optimize editor performance for files over 10,000 lines:

**Performance optimizations:**
1. **Disable syntax highlighting** for files over a configurable threshold (default: 50,000 lines)
2. **Disable bracket matching** for very large files
3. **Use Scintilla's built-in virtualization** (it already handles large files well)
4. **Lazy content loading** for extremely large files (over 1MB):
   - Load the first 100KB immediately
   - Load remaining content in chunks
   - Show a loading indicator

**Performance targets:**
- Open a 10,000-line file: under 500ms
- Scrolling through a 10,000-line file: 60fps (no lag)
- Typing in a 10,000-line file: no perceptible delay

**Scintilla performance settings:**
```cpp
// For large files
editor_->SetLayoutCache(wxSTC_CACHE_DOCUMENT);
editor_->SetBufferedDraw(true);
editor_->SetTechnology(wxSTC_TECHNOLOGY_DIRECTWRITE); // Windows only
```

**Acceptance criteria:**
- 10,000-line files open in under 500ms
- Scrolling is smooth at 60fps
- Typing has no perceptible lag
- Very large files (100K+ lines) degrade gracefully (disable highlighting, show warning)

### Task 20.4: Implement current line highlighting

Highlight the line where the cursor is positioned:

**Visual specification:**
- Background color: slightly lighter than editor background (2-5% lighter)
- Full-width highlight (extends to the edge of the editor)
- Moves with the cursor

**Scintilla configuration:**
```cpp
editor_->SetCaretLineVisible(true);
// Color: slightly lighter than bg_input
wxColour highlight = theme_engine().color(ThemeColorToken::BgInput);
highlight = highlight.ChangeLightness(105); // 5% lighter
editor_->SetCaretLineBackground(highlight);
editor_->SetCaretLineVisibleAlways(true);
```

**Acceptance criteria:**
- Current line is highlighted
- Highlight color is theme-aware and subtle (not distracting)
- Highlight updates when cursor moves
- Highlight is not visible in non-focused state (optional)

### Task 20.5: Implement bracket and quote matching

When the cursor is adjacent to a bracket or quote, highlight the matching pair:

**Matched characters:**
- Parentheses: `(` `)`
- Square brackets: `[` `]`
- Curly braces: `{` `}`
- Angle brackets: `<` `>` (optional, can conflict with markdown)
- Double quotes: `"` `"`
- Single quotes: `'` `'` (careful with apostrophes in markdown)

**Visual indicator:**
- Matching brackets: highlighted with `accent_secondary` background at 30%
- Mismatched/unmatched bracket: highlighted with error color (red-ish)

**Scintilla configuration:**
```cpp
editor_->StyleSetForeground(wxSTC_STYLE_BRACELIGHT, theme_color(AccentSecondary));
editor_->StyleSetBackground(wxSTC_STYLE_BRACELIGHT, bg_with_alpha(AccentSecondary, 30));
editor_->StyleSetForeground(wxSTC_STYLE_BRACEBAD, error_color);
```

Handle bracket matching in `OnUpdateUI`:
```cpp
void EditorPanel::OnEditorUpdateUI(wxStyledTextEvent& event)
{
    int pos = editor_->GetCurrentPos();
    // Check character at pos and pos-1 for brackets
    // If found, find matching bracket and highlight pair
}
```

**Acceptance criteria:**
- Matching brackets are highlighted
- Mismatched brackets show error indicator
- Works inside markdown content (code blocks, links)
- Does not false-match brackets inside strings in code blocks

### Task 20.6: Implement markdown auto-indent

When the user presses Enter, auto-indent based on markdown context:

**Rules:**
1. **After a list item** (`- `, `* `, `1. `):
   - Insert new line with the same list prefix
   - Ordered list: increment the number
   - If the list item is empty (user pressed Enter on empty item): remove the prefix and outdent
2. **After a blockquote** (`> `):
   - Insert new line with `> ` prefix
   - Nested quotes: preserve nesting level
3. **Inside a code block** (between ``` markers):
   - Normal indent behavior (no markdown prefixes)
4. **After a task list item** (`- [ ] `, `- [x] `):
   - Insert new line with `- [ ] ` prefix (unchecked)
5. **Normal text**:
   - No special indent

**Implementation:**
- Hook into `EVT_STC_CHARADDED` for newline character
- Inspect the previous line to determine context
- Insert appropriate prefix on the new line

**Acceptance criteria:**
- List continuation works for unordered lists (`-`, `*`)
- List continuation works for ordered lists (auto-increment number)
- Empty list item terminates the list
- Blockquote continuation works
- Task list continuation works
- Code block content is not auto-prefixed

### Task 20.7: Implement editor preferences

Create a simple preferences system for editor settings:

**Settings:**
| Setting | Default | Options |
|---|---|---|
| Font size | 13 | 8-32 |
| Tab size | 4 | 2, 4, 8 |
| Word wrap mode | Word | None, Word, Character |
| Show line numbers | false | true/false |
| Current line highlight | true | true/false |
| Bracket matching | true | true/false |
| Auto-indent | true | true/false |
| Large file threshold | 50000 lines | configurable |

All settings are stored in the Config (Phase 05) under the `editor.*` namespace.

For now, settings are changed via:
- Keyboard shortcuts (for toggles)
- Config file editing (for values)
- A future settings dialog (Phase later)

**Acceptance criteria:**
- All settings are stored in and loaded from config
- Settings take effect immediately when changed
- Defaults are sensible

### Task 20.8: Write editor enhancement tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_editor_enhancements.cpp`:

Test cases:
1. Line numbers toggle on/off
2. Line number gutter width calculation (for various line counts)
3. Word wrap mode switching
4. Current line highlighting follows cursor
5. Bracket matching for all bracket types
6. Mismatched bracket detection
7. Auto-indent after list item
8. Auto-indent after ordered list (number increment)
9. Auto-indent after blockquote
10. Empty list item terminates list
11. Editor preferences load/save cycle
12. Large file detection threshold

**Acceptance criteria:**
- All tests pass
- Auto-indent rules are verified for each markdown structure
- At least 15 test assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/ui/EditorPanel.h` | Modified (new features) |
| `src/ui/EditorPanel.cpp` | Modified (new features) |
| `src/core/Config.cpp` | Modified (editor settings) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_editor_enhancements.cpp` | Created |

## Dependencies

- wxWidgets 3.2.9 STC (Scintilla)
- Phase 12 EditorPanel base
- Phase 05 Config

## Estimated Complexity

**Medium** -- Most features leverage Scintilla's built-in capabilities. Auto-indent for markdown structures requires custom logic. Large file optimization requires performance testing and tuning.
