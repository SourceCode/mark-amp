# Phase 02: Control Visual Tokens and State Styling

## Overview
Create a complete token model for control visuals so all controls render theme-consistent hover, active, focus, and disabled states.

## Prerequisites
- Phase 01
- `src/core/Theme.h`, `src/core/ThemeEngine.h`

## Tasks

### Task 1: Expand Semantic Tokens for Controls
**Files:** `src/core/Theme.h`, `src/core/Theme.cpp`
**Description:** Add tokens for control bg/fg/border in all states.
**Acceptance Criteria:**
- Tokens exist for normal/hover/pressed/focus/disabled/selected
- No new control color hardcoding in updated surfaces

### Task 2: Add Focus Ring Token Set
**Files:** `src/core/Theme.h`, `themes/*.md`
**Description:** Define focus ring color, thickness, and opacity tokens.
**Acceptance Criteria:**
- Focus ring visible in all built-in themes
- Focus ring contrast is acceptable on light and dark backgrounds

### Task 3: Normalize Elevation and Border Rules
**Files:** `src/ui/Toolbar.cpp`, `src/ui/TabBar.cpp`, `src/ui/ActivityBar.cpp`
**Description:** Align border radius, border weight, and subtle elevation for interactive surfaces.
**Acceptance Criteria:**
- Updated controls use same elevation language
- Pressed state visibly compresses/eases without visual noise

### Task 4: Add Token Fallback Validation
**Files:** `src/core/ThemeRegistry.cpp`, `tests/unit/test_theme_control_tokens.cpp`
**Description:** Verify required control tokens exist for every theme.
**Acceptance Criteria:**
- Missing control token triggers fallback and warning
- Validation tests cover all built-in themes

### Task 5: Document State Styling Matrix
**Files:** `docs/v10_docs/control_state_matrix.md`
**Description:** Publish a matrix showing how each state should render.
**Acceptance Criteria:**
- Matrix includes all major controls and all states

## Testing Requirements
- Theme token validation tests
- Visual pass over at least 10 representative themes

## Phase Completion Criteria
- All upgraded controls render from semantic control tokens only
