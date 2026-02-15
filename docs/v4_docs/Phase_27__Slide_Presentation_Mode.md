# Phase 27 -- Slide/Presentation Mode

## Objective

Implement a presentation mode that renders Markdown documents as slides. Headings define slide boundaries (H1 = new slide, `---` horizontal rule = slide break). Supports slide navigation (arrow keys, click), speaker notes, slide transitions, and fullscreen mode. Enables quick knowledge sharing from Markdown files.

## Prerequisites

- Existing HtmlRenderer, PreviewPanel
- Existing ThemeEngine

## Feature References (PRD)

- PRD #28: Slide Mode (Presentation)
- PRD #14: Notebook as Presentation (RISE)

## Data Structures to Implement

### File: `src/ui/PresentationPanel.h`

```cpp
#pragma once

#include <string>
#include <vector>
#include <wx/panel.h>

namespace markamp::core { class EventBus; class ThemeEngine; }
namespace markamp::rendering { class HtmlRenderer; }

namespace markamp::ui
{

struct Slide
{
    int index{0};
    std::string title;
    std::string markdown_content;
    std::string rendered_html;
    std::string speaker_notes;     // Content after <!-- notes --> marker
    bool has_code{false};
    bool has_image{false};
};

struct PresentationConfig
{
    std::string slide_separator{"---"};
    bool use_heading_breaks{true};   // H1 starts new slide
    int heading_break_level{1};      // 1 = H1, 2 = H1 and H2
    std::string transition{"fade"};  // "fade", "slide", "none"
    int transition_duration_ms{300};
    bool show_slide_numbers{true};
    bool show_progress_bar{true};
    double aspect_ratio{16.0/9.0};
};

class PresentationPanel : public wxPanel
{
public:
    PresentationPanel(wxWindow* parent,
                      markamp::core::EventBus& event_bus,
                      markamp::core::ThemeEngine& theme_engine,
                      markamp::rendering::HtmlRenderer& html_renderer);

    /// Load a document for presentation.
    auto load_document(const std::string& markdown) -> void;

    /// Enter fullscreen presentation mode.
    auto start_presentation() -> void;

    /// Exit presentation mode.
    auto stop_presentation() -> void;

    /// Navigate to a specific slide.
    auto go_to_slide(int index) -> void;

    /// Next slide.
    auto next_slide() -> void;

    /// Previous slide.
    auto previous_slide() -> void;

    /// Get total slide count.
    [[nodiscard]] auto slide_count() const -> int;

    /// Get current slide index.
    [[nodiscard]] auto current_slide() const -> int;

    /// Get speaker notes for current slide.
    [[nodiscard]] auto speaker_notes() const -> const std::string&;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::ThemeEngine& theme_engine_;
    markamp::rendering::HtmlRenderer& html_renderer_;

    std::vector<Slide> slides_;
    int current_index_{0};
    PresentationConfig config_;
    bool is_presenting_{false};

    auto parse_slides(const std::string& markdown) -> std::vector<Slide>;
    auto render_current_slide() -> void;
    auto on_paint(wxPaintEvent& event) -> void;
    auto on_key_down(wxKeyEvent& event) -> void;
    auto on_click(wxMouseEvent& event) -> void;
    auto draw_progress_bar(wxDC& dc) -> void;
    auto draw_slide_number(wxDC& dc) -> void;
    auto apply_theme() -> void;
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`parse_slides(markdown)`** -- Split markdown by `---` separators and/or H1 headings. For each section, extract title (first heading), content, and speaker notes (content after `<!-- notes -->`).

2. **`render_current_slide()`** -- Render the current slide's markdown to HTML. Apply presentation-specific CSS (centered, large text, dark background). Display in the panel.

3. **`on_key_down(event)`** -- Right arrow / Space / Enter = next slide. Left arrow / Backspace = previous. Escape = exit presentation. F = toggle fullscreen.

4. **`start_presentation()`** -- Enter fullscreen mode. Hide all UI except the presentation panel. Show first slide. Set is_presenting_ = true.

5. **`draw_progress_bar(dc)`** -- Draw a thin progress bar at the bottom showing position within the slide deck.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PresentationStartedEvent)
int slide_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(PresentationEndedEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SlideChangedEvent)
int slide_index{0};
int total_slides{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_presentation.cpp`

1. **Parse slides by separator** -- Markdown with 3 `---` separators. Verify 4 slides.
2. **Parse slides by heading** -- Markdown with 3 H1 headings. Verify 3 slides.
3. **Slide title extraction** -- First heading in slide is the title.
4. **Speaker notes extraction** -- Content after `<!-- notes -->`. Verify extracted separately.
5. **Navigation next/previous** -- Start at 0. next_slide() = 1. previous_slide() = 0.
6. **Bounds checking** -- At last slide, next_slide() stays at last. At first, previous stays.
7. **Slide count** -- 5 slides. slide_count() == 5.
8. **Keyboard navigation** -- Right arrow triggers next_slide.
9. **Fullscreen toggle** -- start_presentation() enters fullscreen. stop_presentation() exits.
10. **Progress bar** -- At slide 3/10. Progress bar at 30%.

## Acceptance Criteria

- [ ] Markdown splits into slides by `---` and heading breaks
- [ ] Slides render with presentation-appropriate styling
- [ ] Arrow keys and click navigate between slides
- [ ] Speaker notes extracted and available
- [ ] Fullscreen mode hides all UI
- [ ] Progress bar shows position in deck
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/PresentationPanel.h` | PresentationPanel, Slide, PresentationConfig |
| CREATE | `src/ui/PresentationPanel.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 3 presentation events |
| MODIFY | `src/CMakeLists.txt` | Add PresentationPanel.cpp |
| CREATE | `tests/unit/test_presentation.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_presentation target |

## Estimated Complexity

**M** -- Slide parsing, keyboard navigation, fullscreen mode, custom rendering, 10 tests.
