# Phase 12: Text Editor Component

## Objective

Implement a full-featured text editor component using wxStyledTextCtrl (Scintilla wrapper) for editing markdown source. The editor must support basic editing operations, undo/redo, find/replace, and integrate with the theme engine for styled appearance matching the reference's textarea.

## Prerequisites

- Phase 09 (Layout Manager)

## Deliverables

1. EditorPanel wrapping wxStyledTextCtrl with markdown-aware configuration
2. Theme-integrated editor styling (background, text, cursor, selection colors)
3. Basic markdown syntax highlighting (bold, italic, headings, code blocks, links)
4. Undo/redo integration with CommandHistory
5. Find and replace functionality
6. Word count and cursor position tracking
7. Line number gutter (optional, toggleable)

## Tasks

### Task 12.1: Create the EditorPanel component

Create `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.h` and `EditorPanel.cpp`:

```cpp
namespace markamp::ui {

class EditorPanel : public ThemeAwareWindow
{
public:
    EditorPanel(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    // Content management
    void SetContent(const std::string& content);
    [[nodiscard]] auto GetContent() const -> std::string;
    [[nodiscard]] auto IsModified() const -> bool;
    void ClearModified();

    // Cursor
    [[nodiscard]] auto GetCursorLine() const -> int;
    [[nodiscard]] auto GetCursorColumn() const -> int;
    void SetCursorPosition(int line, int column);

    // Editor operations
    void Undo();
    void Redo();
    [[nodiscard]] auto CanUndo() const -> bool;
    [[nodiscard]] auto CanRedo() const -> bool;

    // Find and replace
    void ShowFindBar();
    void HideFindBar();

    // Configuration
    void SetWordWrap(bool enabled);
    void SetShowLineNumbers(bool enabled);
    void SetFontSize(int size);
    void SetTabSize(int size);

    // Callbacks
    using ContentChangedCallback = std::function<void(const std::string&)>;
    void SetOnContentChanged(ContentChangedCallback callback);

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    wxStyledTextCtrl* editor_{nullptr};
    core::EventBus& event_bus_;

    // Event handlers
    void OnEditorChange(wxStyledTextEvent& event);
    void OnEditorUpdateUI(wxStyledTextEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // Styling
    void ApplyThemeToEditor();
    void ConfigureEditorDefaults();
    void SetupMarkdownLexer();

    ContentChangedCallback on_content_changed_;

    // Configuration state
    bool word_wrap_{true};
    bool show_line_numbers_{false};
    int font_size_{13};
    int tab_size_{4};
};

} // namespace markamp::ui
```

**Editor configuration (matching reference textarea behavior):**
- No border (borderless within the bevel panel)
- Font: JetBrains Mono, 13px
- Background: `bg_input` color
- Text color: `text_main`
- Caret color: `text_main`
- Selection background: `accent_primary` at 20%
- No spell check
- Monospace font
- Smooth scrolling
- Custom scrollbar (from Phase 08)

**Acceptance criteria:**
- Editor appears in the content area
- Text can be typed and edited
- Theme colors are applied correctly
- Cursor is visible and correctly colored

### Task 12.2: Configure wxStyledTextCtrl defaults

Set up the Scintilla editor with appropriate defaults:

```cpp
void EditorPanel::ConfigureEditorDefaults()
{
    editor_->SetUseTabs(false);
    editor_->SetTabWidth(4);
    editor_->SetIndent(4);
    editor_->SetBackSpaceUnIndents(true);
    editor_->SetViewEOL(false);
    editor_->SetViewWhiteSpace(false);
    editor_->SetIndentationGuides(false);
    editor_->SetEdgeMode(wxSTC_EDGE_NONE);

    // Scrolling
    editor_->SetEndAtLastLine(false);
    editor_->SetScrollWidthTracking(true);

    // Caret
    editor_->SetCaretPeriod(500);
    editor_->SetCaretWidth(2);

    // Word wrap
    editor_->SetWrapMode(wxSTC_WRAP_WORD);
    editor_->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_NONE);

    // Margins (line numbers off by default, gutter for folding off)
    editor_->SetMarginWidth(0, 0);  // Line numbers
    editor_->SetMarginWidth(1, 0);  // Symbols
    editor_->SetMarginWidth(2, 0);  // Folding

    // Disable auto-complete and call tips (we handle our own)
    editor_->AutoCompCancel();
}
```

**Acceptance criteria:**
- Tab inserts 4 spaces
- Word wrap is enabled by default
- No visible margins/gutters initially
- Scrolling is smooth
- Caret blinks at 500ms interval

### Task 12.3: Implement highest-quality markdown syntax highlighting

**CRITICAL REQUIREMENT**: The editor must have the **highest quality syntax highlighting available**. This is not a "basic" feature -- it is a core differentiator of MarkAmp.

**Approach: Two-tier highlighting system**

**Tier 1: Scintilla's markdown lexer for the source editor**
wxStyledTextCtrl supports `wxSTC_LEX_MARKDOWN` lexer. Configure styles for:

| Style ID | Element | Foreground | Background | Font Style |
|---|---|---|---|---|
| Default | Body text | text_main | bg_input | Regular |
| wxSTC_MARKDOWN_STRONG1/2 | Bold text | accent_primary | bg_input | Bold |
| wxSTC_MARKDOWN_EM1/2 | Italic text | accent_secondary | bg_input | Italic |
| wxSTC_MARKDOWN_HEADER1 | H1 | accent_primary | bg_input | Bold, 120% size |
| wxSTC_MARKDOWN_HEADER2 | H2 | accent_primary | bg_input | Bold, 110% size |
| wxSTC_MARKDOWN_HEADER3-6 | H3-H6 | accent_primary | bg_input | Bold |
| wxSTC_MARKDOWN_CODE/CODE2 | Inline code | accent_primary | bg_panel | Mono |
| wxSTC_MARKDOWN_CODEBK | Code blocks | text_main | bg_panel | Mono |
| wxSTC_MARKDOWN_LINK | Links | accent_secondary | bg_input | Underline |
| wxSTC_MARKDOWN_BLOCKQUOTE | Blockquotes | text_muted | bg_input | Italic |
| wxSTC_MARKDOWN_STRIKEOUT | Strikethrough | text_muted | bg_input | Strikethrough |
| wxSTC_MARKDOWN_HRULE | Horizontal rule | border_light | bg_input | |
| wxSTC_MARKDOWN_ULIST_ITEM | Unordered list | text_main | bg_input | |
| wxSTC_MARKDOWN_OLIST_ITEM | Ordered list | text_main | bg_input | |

**Tier 2: Tree-sitter integration for code blocks within markdown (Phase 16)**
For fenced code blocks, the editor should delegate syntax highlighting to Tree-sitter grammars embedded at build time. This provides IDE-quality highlighting for code snippets within markdown files, matching the quality of VS Code or Zed.

**Additional editor polish:**
- Heading lines should have subtle background tinting (bg_accent at 3%)
- Code block regions should have visible background change (bg_panel)
- Link URLs should be rendered in a dimmer color than link text
- Bold/italic syntax markers (**, _, etc.) should be rendered at reduced opacity (50%)
- Active line should have a subtle highlight (bg_accent at 5%)

**Acceptance criteria:**
- Markdown syntax is highlighted with professional quality matching or exceeding VS Code's markdown highlighting
- Headings are visually distinct (accent color, bold, scaled size for H1/H2)
- Code blocks have a clearly different background
- Bold and italic text are styled with proper font variants
- Links are underlined and use accent color
- Colors update instantly when theme changes
- Active line is subtly highlighted
- Syntax markers are visually de-emphasized

### Task 12.4: Integrate undo/redo with CommandHistory

Connect the editor's built-in undo/redo to the application's CommandHistory:

**Approach A (Simple -- use Scintilla's built-in undo):**
- Scintilla already has undo/redo. Use it directly.
- Forward Ctrl+Z / Cmd+Z and Ctrl+Shift+Z / Cmd+Shift+Z to `editor_->Undo()` / `editor_->Redo()`

**Approach B (Full integration -- custom commands):**
- Capture text changes as `TextEditCommand` objects
- Push to `CommandHistory`
- Merge consecutive character insertions into single commands

Use **Approach A** initially for simplicity, with the option to switch to B later.

**Acceptance criteria:**
- Ctrl+Z / Cmd+Z undoes the last edit
- Ctrl+Shift+Z / Cmd+Shift+Z (or Ctrl+Y) redoes
- Undo/redo works correctly across multiple edits
- Undo history is cleared when new content is loaded

### Task 12.5: Implement content change notification

When the editor content changes:

1. Publish `EditorContentChangedEvent` via the event bus (debounced, not on every keystroke)
2. Call the `on_content_changed_` callback
3. Mark the file as dirty
4. Update cursor position in the event

**Debouncing strategy:**
- Start a 300ms timer on each change
- If another change occurs before the timer fires, reset the timer
- When the timer fires, publish the event with the current content

This prevents excessive re-rendering of the preview pane on every keystroke.

**Acceptance criteria:**
- Content changes are debounced at 300ms
- Events are published after the debounce period
- Rapid typing does not cause excessive events
- The final content state is always eventually published

### Task 12.6: Implement cursor position tracking

Report cursor position for the status bar:

1. Listen to `wxEVT_STC_UPDATEUI` events
2. Extract current line and column from the editor
3. Publish `CursorPositionChangedEvent` with line and column
4. Line numbers start at 1 (not 0) for display

**Acceptance criteria:**
- Cursor position is reported accurately
- Position updates on: typing, arrow keys, mouse click, selection
- Line and column are 1-based

### Task 12.7: Implement basic find/replace

Create a find bar that appears at the top of the editor panel:

**Find bar appearance:**
- Background: `bg_panel`
- Input field: `bg_input` background, `text_main` foreground
- Buttons: "Previous" (up arrow), "Next" (down arrow), "Close" (X)
- Match count: "N of M" indicator
- Options: "Match Case" toggle

**Keyboard shortcuts:**
- Ctrl+F / Cmd+F: Open find bar
- Escape: Close find bar
- Enter: Find next
- Shift+Enter: Find previous
- Ctrl+H / Cmd+H: Open find and replace

**Replace mode adds:**
- Replace input field
- "Replace" and "Replace All" buttons

**Implementation using Scintilla search APIs:**
```cpp
editor_->SearchInTarget(search_text);
editor_->SetTargetStart(0);
editor_->SetTargetEnd(editor_->GetLength());
```

**Acceptance criteria:**
- Find bar appears and disappears correctly
- Search highlights all matches
- Next/Previous navigate between matches
- Match count is accurate
- Replace and Replace All work correctly
- Case-sensitive toggle works

### Task 12.8: Implement placeholder text

When the editor has no content (empty file or no file selected):

- Display placeholder text: "Start typing..." (matching reference)
- Placeholder text is in `text_muted` color at 50% opacity
- Placeholder disappears when the user starts typing
- Placeholder reappears when all content is deleted

**Implementation:**
- Use Scintilla's annotation or indicator system
- Or: overlay a transparent panel with the placeholder text that hides on focus/input

**Acceptance criteria:**
- Placeholder text appears when editor is empty
- Placeholder disappears on first keystroke
- Placeholder reappears when content is cleared
- Placeholder does not interfere with actual content

### Task 12.9: Write editor tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_editor.cpp`:

Test cases:
1. Set and get content
2. Cursor position tracking
3. Modified state management
4. Word wrap toggle
5. Font size change
6. Content change callback fires
7. Undo/redo operations
8. Find text (forward and backward)
9. Replace text
10. Tab-to-spaces conversion

**Acceptance criteria:**
- All tests pass
- Tests cover both normal and edge cases
- At least 12 test assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/ui/EditorPanel.h` | Created |
| `src/ui/EditorPanel.cpp` | Created |
| `src/ui/LayoutManager.cpp` | Modified (add editor to content area) |
| `src/CMakeLists.txt` | Modified (add stc component to wxWidgets) |
| `tests/unit/test_editor.cpp` | Created |

## Dependencies

- wxWidgets 3.2.9 with STC (Styled Text Control / Scintilla) component
- Phase 05 EventBus, CommandHistory
- Phase 08 ThemeEngine, ThemeAwareWindow
- Phase 09 LayoutManager

## Estimated Complexity

**Critical** -- The text editor is the most used component. Getting Scintilla configured correctly for markdown editing, with proper theme integration, undo/redo, and find/replace is substantial work. This is the foundation that the entire editing experience is built on.
