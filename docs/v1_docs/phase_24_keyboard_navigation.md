# Phase 24: Keyboard Navigation and Shortcuts

## Objective

Implement comprehensive keyboard navigation and shortcuts throughout the application, ensuring power users can perform all major operations without a mouse. This includes global shortcuts, editor-specific shortcuts, navigation shortcuts, and a keyboard shortcut reference.

## Prerequisites

- Phase 12 (Text Editor Component)
- Phase 19 (Split View Mode Implementation)

## Deliverables

1. Global keyboard shortcut system
2. Context-aware shortcuts (different behavior in editor vs gallery vs sidebar)
3. Shortcut customization framework
4. Keyboard shortcut reference overlay
5. Focus management across panels

## Tasks

### Task 24.1: Design the keyboard shortcut system

Create `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.h` and `ShortcutManager.cpp`:

```cpp
namespace markamp::core {

struct Shortcut {
    std::string id;              // Unique action identifier
    std::string description;     // Human-readable description
    int key_code;               // wxWidgets key code
    int modifiers;              // wxMOD_CONTROL, wxMOD_ALT, wxMOD_SHIFT, wxMOD_META
    std::string context;        // "global", "editor", "sidebar", "gallery"
    std::function<void()> action;
};

class ShortcutManager
{
public:
    explicit ShortcutManager(EventBus& event_bus);

    // Register shortcuts
    void register_shortcut(Shortcut shortcut);
    void unregister_shortcut(const std::string& id);

    // Process a key event, return true if handled
    auto process_key_event(wxKeyEvent& event, const std::string& context) -> bool;

    // Query
    [[nodiscard]] auto get_all_shortcuts() const -> std::vector<Shortcut>;
    [[nodiscard]] auto get_shortcuts_for_context(const std::string& context) const
        -> std::vector<Shortcut>;
    [[nodiscard]] auto get_shortcut_text(const std::string& id) const -> std::string;

    // Customization
    void remap_shortcut(const std::string& id, int key_code, int modifiers);
    void reset_to_defaults();
    void save_custom_mappings();
    void load_custom_mappings();

private:
    std::vector<Shortcut> shortcuts_;
    EventBus& event_bus_;

    auto format_shortcut(int key_code, int modifiers) const -> std::string;
};

} // namespace markamp::core
```

**Acceptance criteria:**
- Shortcuts can be registered with action, context, and key combination
- Key events are correctly matched to registered shortcuts
- Context filtering works (editor shortcuts do not fire when in sidebar)

### Task 24.2: Register all global shortcuts

Define and register all application-wide shortcuts:

| Shortcut (macOS) | Shortcut (Win/Linux) | Action | Context |
|---|---|---|---|
| Cmd+O | Ctrl+O | Open file | global |
| Cmd+Shift+O | Ctrl+Shift+O | Open folder | global |
| Cmd+S | Ctrl+S | Save file | global |
| Cmd+Shift+S | Ctrl+Shift+S | Save as | global |
| Cmd+W | Ctrl+W | Close file | global |
| Cmd+Q | Ctrl+Q | Quit application | global |
| Cmd+1 | Ctrl+1 | Source view mode | global |
| Cmd+2 | Ctrl+2 | Split view mode | global |
| Cmd+3 | Ctrl+3 | Preview view mode | global |
| Cmd+B | Ctrl+B | Toggle sidebar | global |
| Cmd+, | Ctrl+, | Open settings | global |
| Cmd+T | Ctrl+T | Open theme gallery | global |
| Cmd+F | Ctrl+F | Find | editor |
| Cmd+H | Ctrl+H | Find and replace | editor |
| Cmd+Z | Ctrl+Z | Undo | editor |
| Cmd+Shift+Z | Ctrl+Y | Redo | editor |
| Cmd+A | Ctrl+A | Select all | editor |
| Cmd+C | Ctrl+C | Copy | editor |
| Cmd+X | Ctrl+X | Cut | editor |
| Cmd+V | Ctrl+V | Paste | editor |
| Cmd+L | Ctrl+L | Toggle line numbers | editor |
| Alt+Z | Alt+Z | Toggle word wrap | editor |
| Cmd+/ | Ctrl+/ | Toggle comment | editor |
| Cmd+] | Ctrl+] | Indent | editor |
| Cmd+[ | Ctrl+[ | Outdent | editor |
| Cmd++ | Ctrl++ | Increase font size | global |
| Cmd+- | Ctrl+- | Decrease font size | global |
| Cmd+0 | Ctrl+0 | Reset font size | global |
| F1 | F1 | Show shortcut reference | global |
| Escape | Escape | Close modal/find bar | global |

**Platform detection:**
Use `wxMOD_CMD` which maps to Cmd on macOS and Ctrl on other platforms.

**Acceptance criteria:**
- All shortcuts are registered
- Platform-appropriate modifier keys are used
- Shortcuts fire the correct actions
- No shortcut conflicts

### Task 24.3: Implement focus management

Manage keyboard focus across application panels:

**Focus zones:**
1. Sidebar (file tree)
2. Editor (text content)
3. Preview (rendered output)
4. Toolbar (buttons)
5. Status bar (items)

**Tab navigation:**
- Tab: move focus to next zone (sidebar -> editor -> preview -> sidebar)
- Shift+Tab: move focus to previous zone
- Focus indicator: subtle highlight border on the focused zone (1px accent_primary)

**Focus behavior:**
- When the sidebar gains focus: first file in the tree is highlighted
- When the editor gains focus: cursor is placed at the last position
- When the preview gains focus: focus indicator on the preview pane (for keyboard scrolling)

**Implementation:**
```cpp
void MainFrame::OnTabKey(wxKeyEvent& event)
{
    auto current = wxWindow::FindFocus();
    if (event.ShiftDown()) {
        // Move to previous zone
        FocusPreviousZone(current);
    } else {
        // Move to next zone
        FocusNextZone(current);
    }
}
```

**Acceptance criteria:**
- Tab cycles focus through all zones
- Focused zone has a visible indicator
- Focus is managed correctly after mode switches
- Modal dialogs capture focus

### Task 24.4: Implement sidebar keyboard navigation

When the sidebar has focus:

| Key | Action |
|---|---|
| Up arrow | Move selection to previous item |
| Down arrow | Move selection to next item |
| Right arrow | Expand folder / enter folder |
| Left arrow | Collapse folder / go to parent |
| Enter | Open selected file |
| Space | Toggle folder open/close |
| Home | Select first item |
| End | Select last item |
| Letter key | Jump to next item starting with that letter |

**Implementation:**
Add keyboard handling to `FileTreeCtrl`:
```cpp
void FileTreeCtrl::OnKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode()) {
        case WXK_UP: SelectPrevious(); break;
        case WXK_DOWN: SelectNext(); break;
        case WXK_RIGHT: ExpandOrEnter(); break;
        case WXK_LEFT: CollapseOrParent(); break;
        case WXK_RETURN: OpenSelected(); break;
        case WXK_SPACE: ToggleSelected(); break;
        default: event.Skip(); break;
    }
}
```

**Acceptance criteria:**
- All keyboard shortcuts work in the sidebar
- Selection is visible and moves correctly
- Expand/collapse works with arrow keys
- Letter-key jump works

### Task 24.5: Implement editor keyboard enhancements

Additional editor keyboard shortcuts beyond basic editing:

| Shortcut | Action |
|---|---|
| Ctrl+D / Cmd+D | Duplicate current line |
| Alt+Up / Option+Up | Move line up |
| Alt+Down / Option+Down | Move line down |
| Ctrl+Shift+K / Cmd+Shift+K | Delete current line |
| Ctrl+Enter / Cmd+Enter | Insert line below |
| Ctrl+Shift+Enter / Cmd+Shift+Enter | Insert line above |
| Ctrl+G / Cmd+G | Go to line number (dialog) |
| Ctrl+Home / Cmd+Home | Go to beginning of document |
| Ctrl+End / Cmd+End | Go to end of document |

**Markdown-specific shortcuts:**
| Shortcut | Action |
|---|---|
| Ctrl+B / Cmd+B | Toggle bold (wrap selection in `**`) |
| Ctrl+I / Cmd+I | Toggle italic (wrap selection in `*`) |
| Ctrl+K / Cmd+K | Insert link template `[text](url)` |
| Ctrl+Shift+C / Cmd+Shift+C | Toggle inline code (wrap in backticks) |
| Ctrl+Shift+. / Cmd+Shift+. | Toggle blockquote |

Note: Ctrl+B conflicts with "toggle sidebar" from global shortcuts. Resolution:
- Ctrl+B in editor context: toggle bold
- Ctrl+B when editor is not focused: toggle sidebar
- Or: use Ctrl+Shift+B for sidebar toggle

**Acceptance criteria:**
- Line manipulation shortcuts work (duplicate, move, delete)
- Markdown formatting shortcuts wrap/unwrap selected text
- Go-to-line dialog appears and navigates correctly
- No conflicts between editor and global shortcuts

### Task 24.6: Implement keyboard shortcut reference overlay

Create a keyboard shortcut reference that appears when F1 is pressed:

**Design:**
- Modal overlay similar to theme gallery
- Organized by category: File, Edit, View, Navigation, Editor, Markdown
- Shows platform-appropriate modifier names (Cmd vs Ctrl)
- Searchable (type to filter shortcuts)

**Content:**
```
FILE                          VIEW
--------------------------    --------------------------
Cmd+O    Open File           Cmd+1    Source Mode
Cmd+S    Save                Cmd+2    Split Mode
Cmd+W    Close               Cmd+3    Preview Mode
Cmd+Q    Quit                Cmd+B    Toggle Sidebar

EDIT                          MARKDOWN
--------------------------    --------------------------
Cmd+Z    Undo                Cmd+B    Bold
Cmd+Y    Redo                Cmd+I    Italic
Cmd+F    Find                Cmd+K    Insert Link
Cmd+H    Replace             Cmd+`    Inline Code
```

**Acceptance criteria:**
- Overlay appears on F1
- All registered shortcuts are listed
- Categories are logical and readable
- Overlay closes on Escape or F1 again
- Shows correct platform modifier keys

### Task 24.7: Implement shortcut customization (optional enhancement)

Allow users to remap shortcuts:

- Settings page or dialog for shortcut customization
- Detect conflicts when remapping
- "Reset to defaults" button
- Save custom mappings to config

**Acceptance criteria:**
- Shortcuts can be remapped
- Conflicts are detected and reported
- Custom mappings persist across sessions
- Reset restores all defaults

### Task 24.8: Write keyboard navigation tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_keyboard_navigation.cpp`:

Test cases:
1. Global shortcut registration and firing
2. Context-aware shortcut filtering (editor vs global)
3. Platform modifier key detection
4. Tab focus cycling
5. Sidebar arrow key navigation
6. Editor line manipulation (duplicate, move, delete)
7. Markdown formatting shortcuts (bold, italic)
8. Shortcut text formatting (human-readable)
9. Shortcut conflict detection
10. Escape key closes modals

**Acceptance criteria:**
- All tests pass
- At least 12 test assertions
- Platform-specific shortcuts are verified

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/ShortcutManager.h` | Created |
| `src/core/ShortcutManager.cpp` | Created |
| `src/ui/ShortcutReference.h` | Created |
| `src/ui/ShortcutReference.cpp` | Created |
| `src/ui/FileTreeCtrl.cpp` | Modified (keyboard nav) |
| `src/ui/EditorPanel.cpp` | Modified (editing shortcuts) |
| `src/ui/MainFrame.cpp` | Modified (focus management) |
| `src/app/MarkAmpApp.cpp` | Modified (register shortcuts) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_keyboard_navigation.cpp` | Created |

## Dependencies

- wxWidgets (wxAcceleratorTable, key events)
- Phase 12 EditorPanel
- Phase 10 FileTreeCtrl
- Phase 19 SplitView
- Phase 05 EventBus, Config

## Estimated Complexity

**Medium** -- Most keyboard shortcuts map directly to existing functionality. The main complexity is in focus management, context-aware shortcut routing, and markdown formatting shortcuts that modify selected text.
