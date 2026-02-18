# Phase 28: Theme Gallery and Customization Control UX

## Overview
Upgrade theme controls so browsing, previewing, and customizing themes is faster and safer.

## Prerequisites
- Existing `src/ui/ThemeGallery.cpp`, `src/ui/ThemeTokenEditor.cpp`
- Phase 02

## Tasks

### Task 1: Improve Theme Browser Controls
**Files:** `src/ui/ThemeGallery.cpp`
**Description:** Add stronger filter/sort categories and quick preview modes.
**Acceptance Criteria:**
- Gallery supports filter by style, contrast, and source

### Task 2: Add Side-by-Side Live Preview Controls
**Files:** `src/ui/ThemeGallery.cpp`, `src/ui/ThemePreviewCard.cpp`
**Description:** Let users preview multiple themes before applying.
**Acceptance Criteria:**
- Preview does not commit until explicit apply

### Task 3: Improve Token Editing UX
**Files:** `src/ui/ThemeTokenEditor.cpp`
**Description:** Add grouped token controls, search, and contrast warnings.
**Acceptance Criteria:**
- Token editor warns on poor contrast before save

### Task 4: Add Safe Apply/Rollback Controls
**Files:** `src/core/ThemeRegistry.cpp`, `src/ui/ThemeGallery.cpp`
**Description:** Provide rollback option after theme apply.
**Acceptance Criteria:**
- One-click rollback available for recent theme change

### Task 5: Add Theme Control Tests
**Files:** `tests/unit/test_theme_gallery_controls.cpp`
**Description:** Test filtering, preview/apply, and rollback flows.
**Acceptance Criteria:**
- Theme control regressions caught automatically

## Testing Requirements
- Theme switching while multiple panels are open
- High and low contrast theme validation

## Phase Completion Criteria
- Theme controls are low-risk and high-clarity
