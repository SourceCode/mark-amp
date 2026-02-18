# Phase 54: Video and Audio Embed Fundamentals

## Overview
Deliver baseline media embeds that are stable, inspectable, and manipulable as first-class canvas objects.

## Prerequisites
- `src/canvas/VideoEmbedObject.*`
- `src/rendering/AudioBlockRenderer.*`
- `src/rendering/VideoBlockRenderer.*`

## Tasks

### Task 1: Improve embed object metadata model
**Files:** `src/canvas/VideoEmbedObject.h`, `src/canvas/CanvasObject.h`
**Description:** Normalize source URL/local path, poster/thumbnail, and duration fields.
**Acceptance Criteria:**
- Media object metadata is complete and serializable

### Task 2: Add playback control shell
**Files:** `src/ui/CanvasPanel.cpp`, `src/canvas/VideoEmbedObject.cpp`
**Description:** Add basic play/pause/mute and open-source actions.
**Acceptance Criteria:**
- Controls are accessible and non-blocking to canvas interactions

### Task 3: Add aspect ratio locking controls
**Files:** `src/canvas/VideoEmbedObject.cpp`, `src/canvas/SelectionManager.cpp`
**Description:** Preserve media proportions during resize by default.
**Acceptance Criteria:**
- Aspect lock can be toggled per object

### Task 4: Add media placeholder/error states
**Files:** `src/canvas/VideoEmbedObject.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Improve UX when media fails to load.
**Acceptance Criteria:**
- Error states provide retry and fallback actions

### Task 5: Add media embed tests
**Files:** `tests/unit/test_canvas_media_embeds.cpp`
**Description:** Validate metadata parsing and object behavior.
**Acceptance Criteria:**
- Embed objects remain stable across load/save cycles

## Testing Requirements
- Local and remote media source scenarios
- Invalid URL/path edge cases

## Phase Completion Criteria
- Media embeds meet baseline whiteboard utility standards
