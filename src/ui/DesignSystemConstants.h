#pragma once

/**
 * @file DesignSystemConstants.h
 * @brief Central reference for all Design System tokens and concepts in MarkAmp.
 *
 * This header serves as a single source of truth for developers building UI components.
 * It documents the relationships between different tokens and includes the fundamental
 * enums and structs that make up the Design System.
 *
 * Use these tokens instead of hard-coding pixel values, fonts, or colors.
 *
 * ## 1. Density Profiles (DensityProfile.h)
 * Defines the overall spacing and sizing density of the UI.
 * - kCompact: Tight spacing, smaller components. Good for information-dense views.
 * - kDefault: Standard spacing.
 * - kComfortable: Generous spacing, larger hit targets. Touch-friendly.
 *
 * ## 2. Spacing Tokens (SpacingGrid.h)
 * A 4px-based grid system for consistent margins, padding, and gaps.
 * - kNone (0px)
 * - kXxs  (2px)
 * - kXs   (4px)
 * - kSm   (8px)
 * - kMd   (12px)
 * - kLg   (16px)
 * - kXl   (24px)
 * - kXxl  (32px)
 * - kXxxl (48px)
 *
 * Note: Spacing values scale based on the active DensityProfile.
 *
 * ## 3. Typography Slots (TypographyScale.h)
 * Semantic font definitions that specify family, size, weight, and line-height.
 * - kCaption
 * - kBody
 * - kBodyStrong
 * - kSubtitle
 * - kTitle
 * - kHeadline
 * - kMono
 * - kMonoSmall
 *
 * ## 4. Elevation Levels (ElevationSystem.h)
 * Semantic depth levels for surfacing components (menus, dialogs, popovers).
 * Maps to corresponding drop shadow definitions.
 * - kFlat:    Level 0 (no shadow)
 * - kRaised:  Level 1 (buttons, cards)
 * - kOverlay: Level 2 (dropdowns, tooltips)
 * - kModal:   Level 3 (dialogs, popovers)
 * - kMax:     Level 4 (toast notifications)
 *
 * ## 5. Theme Colors (core/ThemeColorToken)
 * Defined in core::ThemeColorToken. Used to paint all UI surfaces, text, and borders.
 *
 * ## Usage:
 * To use these in a component, pass a `DesignSystemContext&` reference, which bundles:
 * - `ds.theme`
 * - `ds.registry`
 * - `ds.typography`
 * - `ds.spacing`
 * - `ds.component_sizes`
 * - `ds.elevation`
 * - `ds.metrics`
 */

#include "DensityProfile.h"
#include "ElevationSystem.h"
#include "SpacingGrid.h"
#include "TypographyScale.h"
