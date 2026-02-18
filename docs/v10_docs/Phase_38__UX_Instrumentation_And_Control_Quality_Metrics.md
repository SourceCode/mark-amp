# Phase 38: UX Instrumentation and Control Quality Metrics

## Overview
Add measurement for control quality so usability improvements are data-informed.

## Prerequisites
- Phase 34
- Existing observability systems in `src/core`

## Tasks

### Task 1: Define Control UX Metrics
**Files:** `docs/v10_docs/control_ux_metrics.md`
**Description:** Specify metrics like misclick rate, command success latency, undo-after-action rate.
**Acceptance Criteria:**
- Metrics include clear definitions and privacy boundaries

### Task 2: Add Instrumentation Hooks
**Files:** `src/core/ExtensionTelemetry.cpp`, `src/ui/*.cpp`
**Description:** Instrument key control interactions with lightweight events.
**Acceptance Criteria:**
- Instrumentation overhead remains low and configurable

### Task 3: Add In-App UX Health Panel
**Files:** `src/ui/WalkthroughPanel.cpp` or new `src/ui/UxHealthPanel.cpp`
**Description:** Show anonymized local control quality signals.
**Acceptance Criteria:**
- Panel surfaces trend lines for control performance/usability

### Task 4: Add Privacy and Opt-In Controls
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.cpp`
**Description:** Ensure users control telemetry participation.
**Acceptance Criteria:**
- Telemetry settings are explicit and transparent

### Task 5: Add Metrics Validation Tests
**Files:** `tests/unit/test_control_metrics_instrumentation.cpp`
**Description:** Validate event schemas and emission conditions.
**Acceptance Criteria:**
- Metrics contracts are test enforced

## Testing Requirements
- Instrumentation on/off behavior checks
- High interaction volume performance checks

## Phase Completion Criteria
- UX control quality can be measured and improved iteratively
