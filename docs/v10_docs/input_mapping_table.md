# Input Mapping Table

## Overview

This table defines canonical gesture-to-action mappings for all major interactive surfaces.
Deviations from this table are bugs.

## Legend

| Symbol | Meaning                  |
| ------ | ------------------------ |
| ○      | No action                |
| ●      | Primary action           |
| ⦿      | Context/secondary action |
| ↕      | Vertical scroll          |
| ↔      | Horizontal scroll        |

## Mouse Gesture Mappings

### Click (Left Button)

| Surface          | Single Click        | Double Click | Ctrl+Click       | Shift+Click      |
| ---------------- | ------------------- | ------------ | ---------------- | ---------------- |
| **Activity Bar** | Toggle panel        | ○            | ○                | ○                |
| **File Tree**    | Select item         | Open/expand  | Toggle selection | Range select     |
| **Tab Strip**    | Activate tab        | ○            | Close tab        | ○                |
| **Toolbar**      | Execute action      | ○            | ○                | ○                |
| **Status Bar**   | Show quick menu     | ○            | ○                | ○                |
| **Editor**       | Position cursor     | Select word  | Add cursor       | Extend selection |
| **Settings**     | Focus control       | ○            | ○                | ○                |
| **Breadcrumb**   | Navigate to segment | ○            | ○                | ○                |

### Right Click (Context Menu)

| Surface          | Mouse Right-Click        | Keyboard Menu Key        | Shift+F10                |
| ---------------- | ------------------------ | ------------------------ | ------------------------ |
| **File Tree**    | Show context menu        | Show context menu        | Show context menu        |
| **Tab Strip**    | Show tab context menu    | Show tab context menu    | Show tab context menu    |
| **Editor**       | Show editor context menu | Show editor context menu | Show editor context menu |
| **Activity Bar** | Show panel context menu  | Show panel context menu  | ○                        |

### Scroll (Wheel / Trackpad)

| Surface       | Vertical Scroll | Horizontal Scroll | Ctrl+Scroll | Shift+Scroll        |
| ------------- | --------------- | ----------------- | ----------- | ------------------- |
| **File Tree** | ↕ Tree items    | ○                 | Zoom        | ↔ Horizontal scroll |
| **Tab Strip** | ○               | ↔ Tab strip       | ○           | ↔ Tab strip         |
| **Editor**    | ↕ Document      | ↔ Horizontal      | Zoom        | ↔ Horizontal        |
| **Settings**  | ↕ Settings list | ○                 | ○           | ○                   |
| **Canvas**    | ↕ Pan           | ↔ Pan             | Zoom        | ↔ Pan               |

## Keyboard Gesture Mappings

### Navigation Keys

| Key        | Activity Bar    | File Tree       | Tab Strip       | Editor                 | Settings         |
| ---------- | --------------- | --------------- | --------------- | ---------------------- | ---------------- |
| **↑**      | Previous item   | Previous item   | ○               | Previous line          | Previous setting |
| **↓**      | Next item       | Next item       | ○               | Next line              | Next setting     |
| **←**      | ○               | Collapse node   | Previous tab    | Left                   | ○                |
| **→**      | ○               | Expand node     | Next tab        | Right                  | ○                |
| **Enter**  | Toggle panel    | Open/expand     | ○               | New line               | Edit value       |
| **Space**  | Toggle panel    | Toggle select   | ○               | Type space             | Toggle boolean   |
| **Escape** | ○               | Clear selection | ○               | Cancel                 | Cancel edit      |
| **Tab**    | Next focus zone | Next focus zone | Next focus zone | Insert tab / Next zone | Next control     |
| **Home**   | First item      | First item      | First tab       | Line start             | First setting    |
| **End**    | Last item       | Last item       | Last tab        | Line end               | Last setting     |

### Modifier Combinations

| Combo                | Global Effect             |
| -------------------- | ------------------------- |
| **Cmd/Ctrl+P**       | Command Palette           |
| **Cmd/Ctrl+Shift+P** | Command Palette (actions) |
| **Cmd/Ctrl+,**       | Open Settings             |
| **Cmd/Ctrl+B**       | Toggle Sidebar            |
| **Cmd/Ctrl+J**       | Toggle Bottom Panel       |
| **Cmd/Ctrl+\\**      | Split Editor              |
| **F1**               | Command Palette           |

## Scroll Normalization Rules

1. **Scroll speed**: 3 lines per notch (configurable)
2. **Inertia**: Platform-native (macOS smooth scrolling respected)
3. **Horizontal scroll**: Shift+Wheel OR two-finger horizontal on trackpad
4. **Zoom**: Ctrl/Cmd+Wheel modifies font size or canvas zoom
5. **Pixel-level smoothing**: Enabled when `editor.smoothScrolling` is true

## Double-Click Safety Rules

1. Double-click on **destructive targets** (close buttons) is intercepted — only the first click fires
2. Double-click timing: 500ms platform default
3. Triple-click: Select entire line (editor) or entire value (settings)
