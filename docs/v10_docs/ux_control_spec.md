# V10 Control Behavior Specification

## Overview

This document defines the **canonical behavior** for all interactive controls in MarkAmp v10.
Every control must adhere to these state transitions, cursor policies, and keyboard contracts.

---

## Control States

All interactive controls support the following states, defined in `src/ui/ControlState.h`:

| State        | Flag Value | Description                            |
| ------------ | ---------- | -------------------------------------- |
| **Normal**   | `0x00`     | Default idle appearance                |
| **Hover**    | `0x01`     | Pointer is over the control's hit zone |
| **Pressed**  | `0x02`     | Pointer is down on the control         |
| **Focused**  | `0x04`     | Control has keyboard focus             |
| **Disabled** | `0x08`     | Control is non-interactive             |
| **Selected** | `0x10`     | Control is in a selected/active state  |

States are composable via bitwise OR. Common combinations:

- `Hover | Selected` — hovering over an already-selected item
- `Focused | Selected` — keyboard focus on a selected item
- `Pressed | Focused` — pressing an item that has keyboard focus

---

## State Transition Rules

### Pointer Transitions

```
Normal → Hover       (pointer enters hit zone)
Hover → Pressed      (pointer down inside hit zone)
Pressed → Hover      (pointer up, still inside hit zone)
Pressed → Normal     (pointer up, outside hit zone — cancels action)
Hover → Normal       (pointer leaves hit zone)
```

### Focus Transitions

```
* → + Focused        (control receives keyboard focus)
* → - Focused        (control loses keyboard focus)
```

### Disabled Guard

- When `Disabled` is set, `Hover` and `Pressed` transitions are **blocked**
- Setting `Disabled` clears any active `Hover` or `Pressed` flags

---

## Control Types and Canonical Behavior

### Button

| Trigger     | Behavior                         |
| ----------- | -------------------------------- |
| Click       | Fire action on mouse-up (inside) |
| Enter/Space | Fire action                      |
| Escape      | Remove focus                     |
| Cursor      | `wxCURSOR_HAND`                  |

### Toggle

| Trigger     | Behavior                          |
| ----------- | --------------------------------- |
| Click       | Toggle selected state on mouse-up |
| Enter/Space | Toggle selected state             |
| Cursor      | `wxCURSOR_HAND`                   |

### Menu Item

| Trigger       | Behavior                    |
| ------------- | --------------------------- |
| Click         | Execute command, close menu |
| Enter         | Execute command, close menu |
| Arrow Up/Down | Move between items          |
| Escape        | Close menu                  |
| Cursor        | `wxCURSOR_HAND`             |

### Tab

| Trigger            | Behavior                                                   |
| ------------------ | ---------------------------------------------------------- |
| Click              | Activate tab                                               |
| Middle-click       | Close tab                                                  |
| Double-click       | (configurable)                                             |
| Close button click | Close tab                                                  |
| Arrow Left/Right   | Cycle tabs (when focused)                                  |
| Cursor             | `wxCURSOR_HAND` (tab body), `wxCURSOR_HAND` (close button) |

### List Row / Tree Row

| Trigger       | Behavior        |
| ------------- | --------------- |
| Click         | Select row      |
| Double-click  | Activate/open   |
| Arrow Up/Down | Move selection  |
| Enter         | Activate/open   |
| Right arrow   | Expand (tree)   |
| Left arrow    | Collapse (tree) |
| Cursor        | `wxCURSOR_HAND` |

### Split Handle

| Trigger      | Behavior                                                     |
| ------------ | ------------------------------------------------------------ |
| Drag         | Resize panes                                                 |
| Double-click | Reset to default split                                       |
| Cursor       | `wxCURSOR_SIZEWE` (horizontal), `wxCURSOR_SIZENS` (vertical) |

---

## Cursor Policy

| Surface Type                                                            | Cursor            |
| ----------------------------------------------------------------------- | ----------------- |
| Interactive controls (buttons, tabs, links, menu items, list/tree rows) | `wxCURSOR_HAND`   |
| Text input areas                                                        | `wxCURSOR_IBEAM`  |
| Splitters / resize handles (horizontal)                                 | `wxCURSOR_SIZEWE` |
| Splitters / resize handles (vertical)                                   | `wxCURSOR_SIZENS` |
| Passive / non-interactive regions                                       | `wxCURSOR_ARROW`  |
| Drag in progress                                                        | `wxCURSOR_HAND`   |
| Disabled controls                                                       | `wxCURSOR_ARROW`  |

Centralized via `ThemeAwareWindow::SetControlCursor()`.

---

## Visual Feedback Requirements

1. **Hover**: Background lightens/darkens by one step using `ControlBgHover` token
2. **Pressed**: Background shifts to `ControlBgPressed` token (slightly darker than hover)
3. **Focused**: 2px focus ring using `FocusRingColor` token, inset by 1px
4. **Disabled**: 50% opacity, no hover/press response
5. **Selected**: Persistent accent background using `ControlBgSelected` token

---

## Timing

| Transition     | Duration                                         |
| -------------- | ------------------------------------------------ |
| Hover in/out   | Instant (0ms) — no delay                         |
| Press feedback | Instant                                          |
| Focus ring     | Instant                                          |
| Tooltip        | 500ms delay before show, 100ms delay before hide |
