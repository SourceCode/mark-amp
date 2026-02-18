# Phase 43: Presentation and Slide System

## Overview
PresentationEngine exists in UI with slide rendering infrastructure. However, the presentation system is not wired to a complete experience: slide creation, presenter view, audience view, and slide export are missing. This phase builds a full Markdown-to-slides presentation system.

## Prerequisites
- Phase 24 (Export for slide export)
- Phase 08 (Markdown rendering for slide content)
- Phase 28 (FX system for slide transitions)

## Tasks

### Task 1: Wire PresentationEngine Slide Parsing
**Files:** `src/ui/PresentationEngine.cpp`, `src/ui/PresentationEngine.h`
**Description:** Wire slide parsing from Markdown: `---` separators create slides, frontmatter defines presentation metadata (title, author, theme, aspect ratio).
**Acceptance Criteria:**
- `---` horizontal rule creates slide break
- First slide: title slide from frontmatter or H1
- Frontmatter: `title`, `author`, `date`, `theme`, `aspect_ratio`
- Aspect ratios: 16:9 (default), 4:3, 16:10
- Slide count calculated and shown
- `PresentationParsedEvent` emitted with slide count

### Task 2: Wire Slide Content Rendering
**Files:** `src/ui/PresentationEngine.cpp`, `src/rendering/HtmlRenderer.cpp`
**Description:** Each slide renders Markdown content: headings as titles, body text, code blocks, images, lists, Mermaid diagrams, and math.
**Acceptance Criteria:**
- H1/H2 as slide title (larger font)
- Body text scaled to fit slide
- Code blocks with syntax highlighting
- Images centered and scaled to fit
- Lists rendered with slide-appropriate spacing
- Mermaid diagrams rendered as SVG

### Task 3: Wire Presentation Mode Full Screen
**Files:** `src/ui/PresentationEngine.cpp`, `src/ui/MainFrame.cpp`
**Description:** "Start Presentation" enters full-screen mode: hide all chrome, render current slide full screen, keyboard navigation.
**Acceptance Criteria:**
- F5 or command starts presentation
- Full-screen: hide menu, toolbar, panels, status bar
- Slide rendered at maximum resolution
- Escape exits presentation
- Current slide number shown (subtle, bottom-right)
- Black/white blank screen (B/W keys)

### Task 4: Wire Slide Navigation
**Files:** `src/ui/PresentationEngine.cpp`
**Description:** Navigate slides during presentation: arrow keys, Page Up/Down, Home/End, number + Enter for specific slide.
**Acceptance Criteria:**
- Right arrow / Space: next slide
- Left arrow: previous slide
- Home: first slide
- End: last slide
- Number + Enter: go to slide number
- Touch/click: next slide (area-based: left=back, right=forward)

### Task 5: Wire Slide Transitions
**Files:** `src/ui/PresentationEngine.cpp`, `src/rendering/FxEngine.cpp`
**Description:** Slide transitions using FX system: fade, slide, dissolve. Per-slide transition configurable in Markdown.
**Acceptance Criteria:**
- Default transition: fade (300ms)
- Transitions: fade, slide-left, slide-right, dissolve, none
- Per-slide: `<!-- transition: slide-left -->` directive
- Transition duration configurable
- Reduced motion: instant transition
- FX quality tier respected

### Task 6: Wire Speaker Notes
**Files:** `src/ui/PresentationEngine.cpp`
**Description:** Speaker notes defined with `???` or `<!-- notes: -->` syntax. Notes visible only in presenter view, not audience view.
**Acceptance Criteria:**
- `???` separator: text below is speaker notes
- Notes parsed and stored per slide
- Notes not rendered in slide view
- Notes visible in presenter view
- Notes support Markdown formatting
- Notes searchable across slides

### Task 7: Wire Presenter View
**Files:** `src/ui/PresentationEngine.cpp`
**Description:** Presenter view (secondary display or panel): current slide, next slide preview, speaker notes, elapsed timer, slide count.
**Acceptance Criteria:**
- Layout: current slide (large), next slide (small), notes (bottom)
- Timer: elapsed time and optional countdown
- Slide counter: "5 / 20"
- Notes area: scrollable for long notes
- Presenter view on secondary display (or windowed)
- Clock display

### Task 8: Wire Slide Overview Grid
**Files:** `src/ui/PresentationEngine.cpp`
**Description:** Press G during presentation to show slide overview: grid of all slide thumbnails. Click to jump to slide.
**Acceptance Criteria:**
- Grid shows all slide thumbnails
- Thumbnails rendered at reduced scale
- Current slide highlighted
- Click thumbnail jumps to slide
- Keyboard: arrow keys navigate grid, Enter selects
- Escape returns to current slide

### Task 9: Wire Slide Incremental Reveal
**Files:** `src/ui/PresentationEngine.cpp`
**Description:** Incremental reveal: list items appear one at a time. Controlled by `<!-- incremental -->` directive or per-list.
**Acceptance Criteria:**
- `<!-- incremental -->` before list: items reveal on click
- Each click reveals next bullet point
- Revealed items: full opacity, unrevealed: hidden
- Option: unrevealed visible but dimmed
- Forward/back navigation respects reveal state
- Nested lists: parent reveals before children

### Task 10: Wire Slide Layout Templates
**Files:** `src/ui/PresentationEngine.cpp`
**Description:** Slide layout templates: title slide, content, two-column, image-left, image-right, blank, quote.
**Acceptance Criteria:**
- `<!-- layout: two-column -->` splits slide into columns
- `<!-- layout: image-left -->` puts first image left, text right
- `<!-- layout: quote -->` centers text with large quote marks
- `<!-- layout: blank -->` empty slide (for pauses)
- Default: auto-detect from content
- Custom layouts via CSS

### Task 11: Wire Slide Theming
**Files:** `src/ui/PresentationEngine.cpp`, `src/core/ThemeEngine.cpp`
**Description:** Presentation theme from application theme. Optional presentation-specific theme override in frontmatter.
**Acceptance Criteria:**
- Slides use current application theme
- Frontmatter: `presentation_theme: "dark"` overrides
- Theme applied to: background, text, headings, code
- Custom CSS for presentations: `.markamp/presentation.css`
- High contrast mode: slides adapt
- Font size scaled for projection

### Task 12: Wire Slide Export to PDF
**Files:** `src/ui/PresentationEngine.cpp`, `src/core/ExportService.cpp`
**Description:** Export presentation as PDF: one page per slide, with or without speaker notes.
**Acceptance Criteria:**
- Export to PDF: one slide per page
- Option: include speaker notes (below slide)
- Aspect ratio preserved
- Page size: matches slide aspect ratio
- Transitions not included in PDF (static)
- "Export: Slides to PDF" command

### Task 13: Wire Slide Export to HTML
**Files:** `src/ui/PresentationEngine.cpp`, `src/core/ExportService.cpp`
**Description:** Export presentation as standalone HTML: self-contained file with navigation, transitions, and speaker notes.
**Acceptance Criteria:**
- Self-contained HTML with embedded CSS/JS
- Arrow key navigation in browser
- Transitions preserved
- Speaker notes: accessible via hotkey in browser
- Responsive: scales to browser window
- Offline: no external dependencies

### Task 14: Wire Slide Editor Integration
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/PresentationEngine.cpp`
**Description:** Split view for slide editing: Markdown editor on left, slide preview on right. Live preview updates as user types.
**Acceptance Criteria:**
- Split view: editor (left) and slide preview (right)
- Preview shows current slide based on cursor position
- Live update on typing (debounced 300ms)
- Navigate slides in preview syncs editor cursor
- Slide boundary indicators in editor
- "Toggle Slide Preview" command

### Task 15: Wire Slide Timer and Countdown
**Files:** `src/ui/PresentationEngine.cpp`
**Description:** Presentation timer: elapsed time, optional per-slide time, and countdown timer for time-limited talks.
**Acceptance Criteria:**
- Elapsed timer: starts when presentation begins
- Per-slide timer: time spent on current slide
- Countdown: set total time, shows remaining
- Warning: configurable (5 minutes, 1 minute)
- Warning: visual indicator (timer turns red/yellow)
- Timer visible in presenter view only

### Task 16: Wire Slide Annotations (Live Drawing)
**Files:** `src/ui/PresentationEngine.cpp`
**Description:** During presentation, toggle drawing mode: draw over slides with pen tool. Annotations temporary (cleared on slide change).
**Acceptance Criteria:**
- Toggle annotation mode: hotkey or button
- Pen tool: freehand drawing over slide
- Colors: red, blue, green, black (cycle with hotkey)
- Eraser: clear individual strokes
- Clear all: remove all annotations on current slide
- Annotations do not persist after presentation

### Task 17: Wire Slide Laser Pointer
**Files:** `src/ui/PresentationEngine.cpp`
**Description:** Laser pointer mode: show a colored dot that follows mouse/trackpad. Useful for pointing during presentations.
**Acceptance Criteria:**
- Toggle laser pointer: hotkey (L)
- Colored dot follows cursor
- Dot size: configurable
- Dot color: red (default), configurable
- Dot fades when mouse stops (2 seconds)
- No interaction with slide content

### Task 18: Wire Presentation Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register presentation commands: "Presentation: Start", "Presentation: Start from Current Slide", "Presentation: Export PDF", "Presentation: Export HTML", "Presentation: Toggle Preview".
**Acceptance Criteria:**
- All commands registered in command palette
- F5: start from beginning
- Shift+F5: start from current slide
- Commands categorized under "Presentation:" prefix
- Commands only when Markdown file active

### Task 19: Wire Presentation Accessibility
**Files:** `src/ui/PresentationEngine.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Accessible presentations: screen reader announces slide title and content, keyboard navigation complete.
**Acceptance Criteria:**
- Slide change: announce slide number and title
- Tab: navigate between slide elements
- Speaker notes: readable by screen reader
- High contrast mode: slides adapt
- Alt text for images announced
- Keyboard-only navigation possible

### Task 20: Add Presentation Tests
**Files:** `tests/unit/test_presentation.cpp`
**Description:** Test presentation system: slide parsing, navigation, transitions, export, and timer.
**Acceptance Criteria:**
- Slide parsing: correct slide count from separators
- Content rendering: headings, code, images
- Navigation: forward, backward, jump to slide
- Speaker notes: parsed and separated
- Incremental reveal: correct state tracking
- Export: HTML self-contained and navigable

## Testing Requirements
- Slide parsing from various Markdown formats
- Navigation state management
- Transition rendering
- Export: PDF and HTML validity

## Phase Completion Criteria
- Presentation mode from Markdown files
- Slide navigation with transitions
- Speaker notes and presenter view
- Slide overview grid
- Incremental reveal
- Export to PDF and HTML
- All tests pass
