# MarkAmp Design System Migration Guide

## Overview

MarkAmp has migrated to a centralized, token-based design system (V10 Phase 01). Hard-coded UI constants such as `wxColour(r,g,b)`, `wxFont(...)`, `24px` heights, and `8px` paddings are deprecated. All new and existing UI controls must use the tokenized design system to ensure consistency, accessibility, and theme support.

## Key Subsystems

The design system is accessed via `markamp::ui::DesignSystemContext`. It bundles the following subsystems:

1. **`ThemeEngine`**: Resolves color tokens (`ThemeColorToken`) into physical `wxColour` values.
2. **`TypographyScale`**: Manages responsive font profiles using semantic `TypeSlot`s.
3. **`SpacingGrid`**: Provides a 4px-based grid system for consistent margins and padding (`SpacingToken`).
4. **`ComponentSizeResolver`**: Scalable metrics for high-level components (`ComponentKind`).
5. **`ElevationSystem`**: Translates `ElevationLevel`s (from `kFlat` to `kModal`) into consistent shadow specs.

---

## Migration Steps for Existing Controls

### 1. Identify Hard-Coded Constants

Locate any `constexpr int` or `#define` statements related to layout:

```cpp
// DEPRECATED
static constexpr int kButtonHeight = 24;
static constexpr int kPadding = 8;
```

Add the `[[deprecated]]` attribute to them as an intermediate step, or remove them entirely if you are migrating the file immediately:

```cpp
[[deprecated("Use ComponentSizeResolver::get().resolve(ComponentKind::kButton).height")]]
static constexpr int kButtonHeight = 24;
```

### 2. Context Injection

New and migrated UI controls should receive a `const DesignSystemContext& context` reference during construction or initialization. Do not instantiate new `ThemeEngine` or `ComponentSizeResolver` singletons inside individual controls if possible; pass the unified context.

### 3. Replace Sizes and Spacing

Instead of pixel values, query the design system:

**Before:**

```cpp
SetMinSize(wxSize(-1, kToolbarHeight));
int margin = 16;
```

**After:**

```cpp
auto metrics = context.sizes.resolve(markamp::ui::ComponentKind::kToolbarButton);
SetMinSize(wxSize(-1, metrics.height));

int margin = context.spacing.scaled(markamp::ui::SpacingToken::kLg);
```

### 4. Replace Colors

Hard-coded `wxColour` instantiations are explicitly forbidden.

**Before:**

```cpp
SetBackgroundColour(wxColour(40, 40, 40));
```

**After:**

```cpp
SetBackgroundColour(context.theme.color(ThemeColorToken::kBgPanel));
```

### 5. Replace Fonts

Fonts must adhere to the `TypeSlot` hierarchy and respect user preferences and density profiles.

**Before:**

```cpp
SetFont(wxFontInfo(12).Family(wxFONTFAMILY_DEFAULT).Weight(wxFONTWEIGHT_BOLD));
```

**After:**

```cpp
SetFont(context.typography.font(markamp::ui::TypeSlot::kBodyStrong));
```

### 6. Dynamic Updates

UI components should listen for the application-wide `ThemeChangedEvent` and `DensityChangedEvent` to trigger a re-layout (`Layout()`) and re-paint (`Refresh()`) when design system tokens are modified by the user.
