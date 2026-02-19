# V10 Control Adoption Checklist

## Usage

For each control surface adopting V10 standards, verify all items below.
Mark with ✅ when complete.

---

## Behavior Checks

- [ ] Uses `ControlStateTracker` or `IndexedControlState` for state management
- [ ] Pointer transitions follow canonical sequence: Normal → Hover → Pressed → Hover/Normal
- [ ] Disabled state blocks Hover and Pressed transitions
- [ ] Mouse-up outside control cancels action (does not fire click)
- [ ] Double-click action is defined and documented
- [ ] Right-click shows context menu (where applicable)

## Visual Checks

- [ ] Uses semantic `ControlBg*` theme tokens for all state backgrounds
- [ ] Focus ring rendered with `FocusRingColor` token, 2px, inset 1px
- [ ] Disabled appearance uses reduced opacity
- [ ] Selected state uses `ControlBgSelected` token
- [ ] No hardcoded `wxColour()` values — all from theme engine

## Keyboard Checks

- [ ] Enter/Space activates the control
- [ ] Escape removes focus or closes overlay
- [ ] Arrow keys navigate within the surface (where applicable)
- [ ] Tab key moves focus to the next chrome region
- [ ] Focus order is deterministic and cyclical

## Accessibility Checks

- [ ] Control has accessible name (tooltip or label)
- [ ] State changes fire appropriate events for screen readers
- [ ] Minimum hit target size: 24×24px (compact), 32×32px (comfortable)
- [ ] Color contrast ratio meets WCAG AA (4.5:1 for text, 3:1 for non-text)

## Cursor Policy Checks

- [ ] Interactive controls set `wxCURSOR_HAND`
- [ ] Text inputs set `wxCURSOR_IBEAM`
- [ ] Split handles set `wxCURSOR_SIZEWE` or `wxCURSOR_SIZENS`
- [ ] Disabled controls use `wxCURSOR_ARROW`
- [ ] Passive regions use `wxCURSOR_ARROW`

---

## Control Surface Status

| Surface        | ControlState | Tokens | Keyboard | Accessibility | Cursor |
| -------------- | :----------: | :----: | :------: | :-----------: | :----: |
| ActivityBar    |      ⬜      |   ⬜   |    ⬜    |      ⬜       |   ⬜   |
| FileTreeCtrl   |      ⬜      |   ⬜   |    ⬜    |      ⬜       |   ⬜   |
| TabBar         |      ⬜      |   ⬜   |    ⬜    |      ⬜       |   ⬜   |
| Toolbar        |      ⬜      |   ⬜   |    ⬜    |      ⬜       |   ⬜   |
| StatusBarPanel |      ⬜      |   ⬜   |    ⬜    |      ⬜       |   ⬜   |
| CommandPalette |      ⬜      |   ⬜   |    ⬜    |      ⬜       |   ⬜   |
| SettingsPanel  |      ⬜      |   ⬜   |    ⬜    |      ⬜       |   ⬜   |
| SplitView      |      ⬜      |   ⬜   |    ⬜    |      ⬜       |   ⬜   |
| BreadcrumbBar  |      ⬜      |   ⬜   |    ⬜    |      ⬜       |   ⬜   |
| SearchPanel    |      ⬜      |   ⬜   |    ⬜    |      ⬜       |   ⬜   |
