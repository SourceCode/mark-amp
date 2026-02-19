# Control State Styling Matrix

## Overview

This matrix specifies how each interactive control must render in every canonical state.
All styling is driven by `ThemeColorToken` values defined in `ThemeEngine.h` — no hardcoded colors.

## Token Reference

| Token                 | Default (Dark)       | Purpose                              |
| --------------------- | -------------------- | ------------------------------------ |
| `ControlBgNormal`     | `rgb(35, 35, 55)`    | Control resting background           |
| `ControlBgHover`      | `rgb(50, 50, 80)`    | Control hover background             |
| `ControlBgPressed`    | `rgb(30, 30, 50)`    | Control pressed/active background    |
| `ControlBgFocus`      | `rgb(40, 40, 70)`    | Control focused background           |
| `ControlBgDisabled`   | `rgb(35, 35, 55)`    | Control disabled background          |
| `ControlBgSelected`   | `rgb(45, 45, 85)`    | Control selected background          |
| `ControlFgNormal`     | `rgb(220, 220, 240)` | Control normal foreground            |
| `ControlFgDisabled`   | `rgb(120, 120, 140)` | Control disabled foreground (dimmed) |
| `ControlBorderNormal` | `rgb(60, 60, 80)`    | Control normal border                |
| `ControlBorderFocus`  | `rgb(100, 99, 255)`  | Control focus border                 |
| `FocusRingColor`      | `rgb(100, 99, 255)`  | Focus ring outline color             |

## State Styling Matrix

|                | Normal       | Hover        | Pressed      | Focused        | Disabled     | Selected    |
| -------------- | ------------ | ------------ | ------------ | -------------- | ------------ | ----------- |
| **Background** | BgNormal     | BgHover      | BgPressed    | BgFocus        | BgDisabled   | BgSelected  |
| **Foreground** | FgNormal     | FgNormal     | FgNormal     | FgNormal       | FgDisabled   | FgNormal    |
| **Border**     | BorderNormal | BorderNormal | BorderNormal | BorderFocus    | BorderNormal | BorderFocus |
| **Focus Ring** | None         | None         | None         | FocusRingColor | None         | None        |
| **Cursor**     | Arrow        | Hand         | Hand         | (inherit)      | Arrow        | Hand        |
| **Opacity**    | 100%         | 100%         | 100%         | 100%           | 50%          | 100%        |

## Per-Control Rendering

### Button

- Normal: `ControlBgNormal` + `ControlFgNormal`
- Hover: `ControlBgHover`, cursor → Hand
- Pressed: `ControlBgPressed`, 1px inward offset
- Focused: `ControlBorderFocus` + 2px `FocusRingColor` outline
- Disabled: `ControlBgDisabled` + `ControlFgDisabled`, no hover response

### Toggle / Checkbox

- Same as Button, plus Selected state uses `ControlBgSelected`
- Selected + Hover: `ControlBgSelected` with hover overlay

### Tab

- Active: `ControlBgSelected` + `ControlFgNormal`
- Inactive: `ControlBgNormal` + `ControlFgDisabled`
- Hover (inactive): `ControlBgHover`
- Focused: `ControlBorderFocus` bottom indicator

### Menu Item / List Row / Tree Row

- Normal: transparent background
- Hover: `ControlBgHover`
- Selected: `ControlBgSelected`
- Focused: `FocusRingColor` 2px inset ring
- Disabled: `ControlFgDisabled`, no hover response

### Toolbar Button

- Normal: transparent background
- Hover: `ControlBgHover` with 4px border-radius
- Pressed: `ControlBgPressed`
- Active (toggled): `ControlBgSelected`

### Split Handle

- Normal: `ControlBorderNormal` 1px line
- Hover: `AccentPrimary` highlight, cursor → ResizeWE/NS
- Dragging: `AccentPrimary` at full opacity

## Elevation Rules

- Hover ↑: background lightens (dark themes) or darkens (light themes)
- Pressed ↓: background compresses (darker than normal)
- Selected: elevated above normal but below hover
- Focus ring: always 2px, drawn outside the control border

## Accessibility

- All state transitions must meet WCAG AA contrast (4.5:1 for text)
- Focus ring provides minimum 3:1 contrast against adjacent colors
- Disabled controls retain label readability at reduced opacity
