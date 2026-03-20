# P01-T03: Prompt Surface Contract

> **Phase 01 — Integration Inventory and Shared Contracts**
> **Status:** Complete
> **Scope:** Formalizes the interface for prompt services (Quick Pick, Input Box).
> **Rollback:** Documentation only — no code changes.

---

## 1 · Current State

### 1.1 Service-Side Implementation

| Service            | Source File                  | API                          | Event Published                  |
|--------------------|------------------------------|------------------------------|----------------------------------|
| `QuickPickService` | `QuickPickService.cpp`       | `show(items, options)`       | `ShowQuickPickRequestEvent`      |
| `InputBoxService`  | `InputBoxService.cpp`        | `show(options)`              | `ShowInputBoxRequestEvent`       |

### 1.2 The Gap

Both services publish their request events to the EventBus, but **no UI component subscribes** to these events. The prompt request is emitted and silently dropped:

```
Extension/Plugin → QuickPickService::show() → EventBus.publish(ShowQuickPickRequestEvent) → ❌ NO SUBSCRIBER
```

### 1.3 Existing Related UI

`CommandPalette` (`CommandPalette.cpp`) is the closest existing UI component, but it:
- Has its own item model (`PaletteCommand`)
- Is owned and triggered by `MainFrame`
- Has no knowledge of `QuickPickService` or `InputBoxService`

---

## 2 · Contract

### 2.1 Host Ownership

| Rule | Description |
|------|-------------|
| **Owner** | `MainFrame` creates and owns the `PromptOverlay` panel |
| **Subscription** | `MainFrame` subscribes to `ShowQuickPickRequestEvent` and `ShowInputBoxRequestEvent` |
| **Z-Order** | Prompt overlay renders above all content, below system dialogs |
| **Singleton** | Only one prompt overlay instance exists; follows CommandPalette pattern |

### 2.2 Lifecycle

```
┌─────────────────────────────────────────────────────┐
│ Service::show()                                      │
│   │                                                  │
│   ├─ Store options (items, placeholder, validation)  │
│   ├─ Store result callback / cancel callback          │
│   └─ EventBus.publish(ShowRequestEvent)              │
│                                                      │
│ MainFrame (subscriber)                               │
│   │                                                  │
│   ├─ Receive ShowRequestEvent                        │
│   ├─ Configure PromptOverlay with event data         │
│   ├─ Show PromptOverlay (animated, EaseOutCubic)     │
│   └─ Set input focus to PromptOverlay                │
│                                                      │
│ PromptOverlay (user interaction)                     │
│   │                                                  │
│   ├─ User types → filter / validate                  │
│   ├─ Enter → accept selection → hide → call result cb│
│   ├─ Escape → hide → call cancel callback            │
│   └─ Focus lost → hide → call cancel callback        │
└─────────────────────────────────────────────────────┘
```

### 2.3 Quick Pick Semantics

| Feature             | Behavior                                           |
|---------------------|----------------------------------------------------|
| **Single-select**   | Click or Enter selects item, prompt closes          |
| **Multi-select**    | Checkbox per item, Enter confirms selection         |
| **Filter**          | Fuzzy match on `label` + `description` fields      |
| **Grouping**        | Items with same `group` field render under headers  |
| **Validation**      | `canSelectItem(item)` callback — can veto selection |
| **Progress**        | Optional busy indicator while items load async      |
| **Step**            | `showQuickPick` can chain (step 1 → step 2)        |

### 2.4 Input Box Semantics

| Feature             | Behavior                                           |
|---------------------|----------------------------------------------------|
| **Placeholder**     | Grayed text shown when input is empty               |
| **Prompt**          | Header text above the input field                   |
| **Validation**      | `validateInput(value)` → string error or empty      |
| **Password**        | Input masked with `•` characters                    |
| **Default value**   | Pre-filled input text                               |

### 2.5 Keyboard Rules

| Key          | Action                                            |
|-------------|---------------------------------------------------|
| `Enter`      | Accept current selection / input value             |
| `Escape`     | Cancel prompt, invoke cancel callback              |
| `Tab`        | In multi-select: toggle checkbox on focused item   |
| `↑` / `↓`   | Navigate items (Quick Pick) — skip group headers   |
| `Cmd+A`      | In multi-select: select all visible items          |

### 2.6 Focus Rules

| Rule                | Description                                        |
|--------------------|-----------------------------------------------------|
| **Focus trap**      | Tab/Shift+Tab cycle within prompt overlay           |
| **Focus restore**   | On close, focus returns to the element that was focused before the prompt opened |
| **Background click**| Close prompt, invoke cancel callback                |

### 2.7 Reentrancy

| Rule                | Description                                        |
|--------------------|-----------------------------------------------------|
| **Queue model**     | Only one prompt at a time                           |
| **Queued requests** | If a prompt is active, new requests queue (FIFO)    |
| **Queue limit**     | Max 5 queued requests; excess requests auto-cancel  |

### 2.8 Extension Safety

| Rule                | Description                                        |
|--------------------|-----------------------------------------------------|
| **Timeout**         | Prompts auto-cancel after 60 seconds if no interaction |
| **Sandbox**         | Result callbacks run within `PluginSafeCall` wrapper |
| **Resource limits** | Max 1000 items per Quick Pick request               |
| **Error handling**  | Validation callback exceptions → log + allow selection |

---

## 3 · Result Callback Contract

### 3.1 Quick Pick Result

```cpp
// Single-select result
using QuickPickResult = std::optional<QuickPickItem>;

// Multi-select result
using QuickPickMultiResult = std::vector<QuickPickItem>;
```

- `std::nullopt` / empty vector = user cancelled
- Populated = user made selection(s)

### 3.2 Input Box Result

```cpp
using InputBoxResult = std::optional<std::string>;
```

- `std::nullopt` = user cancelled
- Value = user submitted text (may be empty string if allowed)

---

## 4 · Implementation Sequence (for Phase 02+)

1. Create `PromptOverlay` class (similar pattern to `CommandPalette`)
2. Subscribe to `ShowQuickPickRequestEvent` / `ShowInputBoxRequestEvent` in `MainFrame`
3. Wire `PromptOverlay` → result/cancel callbacks through `QuickPickService` / `InputBoxService`
4. Wire `InputBoxService::InputHistory` for history recall in input box
