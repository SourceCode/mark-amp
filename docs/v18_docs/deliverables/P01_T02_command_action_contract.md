# P01-T02: Shared Command and Action Contract

> **Phase 01 — Integration Inventory and Shared Contracts**
> **Status:** Complete
> **Scope:** Defines the canonical command descriptor, resolution rules, and migration order.
> **Rollback:** Documentation only — no code changes.

---

## 1 · Current State

### 1.1 Existing Command Infrastructure

The codebase has a `CommandEntry` struct used by provider classes:

```cpp
// From WindowCommandProvider.cpp, SidebarCommandProvider.cpp
struct CommandEntry {
    std::string id;          // "window.new", "sidebar.toggleLeft"
    std::string title;       // "New Window", "Toggle Left Sidebar"
    std::string category;    // "Window", "Sidebar"
    std::string description; // Human-readable description
    std::string shortcut;    // "Cmd+B" (display-only)
    std::string icon;        // "plus", "sidebar-left"
    std::function<bool()> execute_fn; // Returns success
};
```

### 1.2 Command Providers

| Provider                   | Commands | Source File |
|---------------------------|----------|-------------|
| `WindowCommandProvider`    | 8        | `WindowCommandProvider.cpp` |
| `SidebarCommandProvider`   | 8        | `SidebarCommandProvider.cpp` |
| `MainFrame` menu bindings  | 130+     | `MainFrame.cpp` (inline lambdas) |
| `MainFrame` shortcuts      | 50+      | `MainFrame.cpp:RegisterDefaultShortcuts()` |
| `MainFrame` palette        | ~8       | `MainFrame.cpp:RegisterPaletteCommands()` |

### 1.3 Problem: Four Divergent Command Paths

| Path                    | Registration Site           | Execution Model              |
|-------------------------|----------------------------|------------------------------|
| Menu bar                | `createMenuBar()` + `Bind(wxEVT_MENU, λ)` | wxWidgets accelerator → λ → event_bus_.publish() |
| Toolbar                 | `Toolbar::OnMouseDown()`    | Direct `event_bus_.publish()` |
| Command palette         | `RegisterPaletteCommands()` | `PaletteCommand::action` → λ → event_bus_.publish() |
| Shortcut manager        | `RegisterDefaultShortcuts()`| Mostly empty `{}` callbacks (dead) |

The same logical command (e.g., "Toggle Sidebar") is defined independently in 3+ locations with separate lambda closures that publish the same event.

---

## 2 · Canonical Command Descriptor

### 2.1 Schema

```cpp
struct CommandDescriptor {
    // Identity
    std::string id;           // "view.toggleSidebar" — dot-scoped, lowercase
    std::string title;        // "Toggle Sidebar" — human-readable
    std::string category;     // "View" — top-level grouping
    std::string description;  // Extended tooltip / detail text

    // Appearance
    std::string icon_id;      // "sidebar-left" — references MUI icon system (P01-T05)

    // Keybinding
    int key_code{0};          // wxWidgets key code
    int modifiers{0};         // wxMOD_CMD | wxMOD_SHIFT etc.
    std::string shortcut_display; // "⌘+B" — computed from key_code + modifiers

    // Preconditions (VS Code-style when clauses)
    std::string enabled_when;  // WhenClause expression, e.g. "editorFocus"
    std::string visible_when;  // WhenClause expression, e.g. "!isStartupScreen"

    // Execution
    std::function<void()> execute; // Single execution lambda
    // OR
    std::string event_type;   // Event to publish (alternative to lambda)

    // Taxonomy
    std::string source;       // "builtin", "extension", "user"
    std::string telemetry_key;// For usage tracking
};
```

### 2.2 Resolution Rules

When a command `id` is invoked, the system resolves through these surfaces in priority order:

1. **ShortcutManager** — Context-specific shortcuts take priority (e.g., Ctrl+B = "bold" in editor context)
2. **ContextKeyService** — Check `enabled_when` expression; skip if false
3. **CommandRegistry** — Look up `CommandDescriptor` by `id` and invoke `execute`

### 2.3 Surface Projection

Each surface projects the `CommandDescriptor` differently:

| Surface         | Projected Fields                                        |
|----------------|--------------------------------------------------------|
| Menu Bar        | `title`, `shortcut_display`, `enabled_when`, `visible_when` |
| Toolbar         | `icon_id`, `title` (tooltip), `enabled_when`           |
| Command Palette | `title`, `category`, `shortcut_display`, `description` |
| Activity Bar    | `icon_id`, `title` (tooltip)                           |
| Context Menu    | `title`, `icon_id`, `enabled_when`, `visible_when`     |
| Shortcuts Overlay | `title`, `category`, `shortcut_display`               |

---

## 3 · Migration Order

### Phase 1: Canonical Adapters (P02)
- Create `CommandRegistry::register_command(CommandDescriptor)` method
- Adapt `WindowCommandProvider`'s 8 and `SidebarCommandProvider`'s 8 commands to register via `CommandDescriptor`

### Phase 2: Menu Migration (P03-P04)
- Replace `MainFrame::createMenuBar()` inline lambdas with `CommandRegistry::execute("file.save")`
- Menu items bind to `command_id` instead of raw event publishing
- Menu `Bind(wxEVT_MENU, ...)` calls become `CommandRegistry::bind_to_menu(menu_id, command_id)`

### Phase 3: Palette Auto-Registration (P03)
- `RegisterPaletteCommands()` reads from `CommandRegistry` instead of manual registration
- All registered commands automatically appear in the palette

### Phase 4: Shortcut Unification (P04)
- `ShortcutManager` reads `key_code` + `modifiers` from `CommandDescriptor`
- Shortcut callbacks invoke `CommandRegistry::execute(id)` instead of duplicating event publish logic

### Phase 5: Context Menu Unification (P05)
- Context menus read from `CommandRegistry` filtered by `visible_when` + `enabled_when`

---

## 4 · Drift Sites (from Ledger P01-T01)

Code locations that must be migrated to use `CommandRegistry`:

| File                  | Lines       | What                                          |
|-----------------------|-------------|-----------------------------------------------|
| `MainFrame.cpp`       | 874–1313    | Menu creation (130+ inline lambdas)           |
| `MainFrame.cpp`       | 1316–1900   | Menu event bindings (100+ `Bind` calls)       |
| `MainFrame.cpp`       | 2349–2647   | Shortcut registrations (50+ with empty λ)     |
| `MainFrame.cpp`       | 2649–2900   | Palette command registration (~8 commands)     |
| `Toolbar.cpp`         | OnMouseDown | 7 button handlers with direct event publish   |
| `ActivityBar.cpp`     | 746–800     | Click handler publishing `ActivityBarSelectionEvent` |
| `StatusBarPanel.cpp`  | 405–800     | Click handlers on status items                |

---

## 5 · Compatibility Notes

- `CommandEntry` (existing) continues to work via an `ICommandProvider` → `CommandRegistry` adapter
- `WhenClause` engine already exists at `src/core/WhenClause.h` — reuse for `enabled_when`/`visible_when`
- `ContextKeyService` already exists at `src/core/ContextKeyService.h` — provides runtime context evaluation
