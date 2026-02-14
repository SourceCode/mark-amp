# Phase 20: Performance Optimization & Final Polish

**Priority:** Low
**Estimated Scope:** ~10 files affected
**Dependencies:** Phase 10

## Objective

Optimize performance-critical paths that were identified during the review: make `publish_fast()` truly lock-free after Phase 10's foundation, optimize event dispatch overhead, reduce memory allocations in hot paths, and apply final polish to the codebase.

## Background/Context

### Performance Hotspots Identified

1. **EventBus publish() mutex contention** -- Every event publish (including UI thread events like cursor movement, scroll, and keystroke) acquires a `std::mutex`. On a single-threaded UI application this is unnecessary overhead. Phase 10 fixes the fast path; this phase optimizes the general case.

2. **Events.h code bloat** -- After Phase 01 reduces the macro boilerplate, the compiled event infrastructure is still large. Each event type generates a virtual function table and RTTI entry. With 120+ events, this contributes to binary size and instruction cache pressure.

3. **HtmlRenderer string concatenation** -- The HTML rendering pipeline builds output through string concatenation (`output += ...`). For large documents, this causes repeated reallocations. A pre-allocated string buffer or `std::stringstream` with reserve would reduce allocation overhead.

4. **PreviewPanel re-render frequency** -- PreviewPanel re-renders the full HTML on every content change. The debounce interval (300ms in the scheduler) helps, but the renderer itself does not diff or cache partial results.

5. **Config get_* methods** -- Each `Config::get_string/get_int/get_bool` call creates a `std::string` from the `string_view` key, then does a YAML node lookup. For frequently-read config keys (called every frame or every keystroke), this adds up.

6. **ThemeEngine color lookups** -- Theme colors are accessed via string-keyed lookups. Hot-path code (syntax highlighting, caret rendering) should use cached values.

### What Phase 10 Already Covers

Phase 10 fixes the `publish_fast()` mutex issue and wires `process_queued()`/`drain_fast_queue()` into the idle handler. This phase builds on that foundation with broader optimization.

## Detailed Tasks

### Task 1: Add publish_fast() consumers for hot-path events

**File:** `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
**File:** `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

After Phase 10 makes `publish_fast()` truly lock-free, identify events that are published on every keystroke or scroll and switch them from `publish()` to `publish_fast()`:

Candidates for `publish_fast()`:
- `CursorPositionChangedEvent` (published on every cursor move)
- `ContentChangedEvent` (published on every keystroke)
- `ScrollPositionChangedEvent` (published on every scroll)
- `SelectionChangedEvent` (published on selection changes)

```cpp
// Before:
event_bus_->publish(cursorEvent);

// After:
event_bus_->publish_fast(cursorEvent);
```

Only use `publish_fast()` for events where:
1. Handlers do not modify the handler list (no subscribe/unsubscribe during dispatch)
2. Handlers are lightweight (no heavy I/O or allocations)
3. The event is published frequently (> 10 times per second)

### Task 2: Pre-allocate HtmlRenderer output buffer

**File:** `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`

Reserve output string capacity based on input size:

```cpp
auto HtmlRenderer::render(const std::string& markdown_source) -> std::string
{
    std::string output;
    // HTML output is typically 2-4x the size of Markdown source
    output.reserve(markdown_source.size() * 3);
    // ... existing rendering logic ...
    return output;
}
```

Also apply to intermediate buffers (CSS generation, code block rendering).

### Task 3: Cache frequently-accessed Config values

**File:** `/Users/ryanrentfro/code/markamp/src/core/Config.h`

Add a config value cache for hot-path reads:

```cpp
class Config
{
public:
    // ... existing API ...

    /// Cached access for frequently-read keys.
    /// Call invalidate_cache() when config is reloaded.
    struct CachedValues
    {
        int font_size{14};
        bool word_wrap{true};
        bool show_line_numbers{true};
        bool highlight_current_line{true};
        int tab_size{4};
        std::string font_family{"Menlo"};
        int cursor_width{2};
        bool show_minimap{false};

        void refresh(const Config& config);
    };

    [[nodiscard]] auto cached() const -> const CachedValues& { return cached_; }
    void invalidate_cache();

private:
    CachedValues cached_;
};
```

Implementation:
```cpp
void Config::CachedValues::refresh(const Config& config)
{
    font_size = config.get_int("font_size", 14);
    word_wrap = config.get_bool("word_wrap", true);
    show_line_numbers = config.get_bool("show_line_numbers", true);
    highlight_current_line = config.get_bool("highlight_current_line", true);
    tab_size = config.get_int("tab_size", 4);
    font_family = config.get_string("font_family", "Menlo");
    cursor_width = config.get_int("editor.cursor_width", 2);
    show_minimap = config.get_bool("show_minimap", false);
}

void Config::invalidate_cache()
{
    cached_.refresh(*this);
}
```

Call `invalidate_cache()` after `load()`, `set()`, and when `SettingChangedEvent` is handled.

### Task 4: Reduce event RTTI overhead

**File:** `/Users/ryanrentfro/code/markamp/src/core/EventBus.h`
**File:** `/Users/ryanrentfro/code/markamp/src/core/Events.h`

After Phase 01's macro refactor, consider adding a compile-time event ID to avoid `typeid()` and `std::type_index` at runtime:

```cpp
// In the event macro:
template <typename T>
struct EventTypeId
{
    static auto id() -> std::size_t
    {
        static const std::size_t value = next_id();
        return value;
    }
private:
    static auto next_id() -> std::size_t
    {
        static std::atomic<std::size_t> counter{0};
        return counter.fetch_add(1, std::memory_order_relaxed);
    }
};
```

Replace `std::type_index(typeid(T))` lookups in EventBus with `EventTypeId<T>::id()` integer lookups:

```cpp
// Before:
auto it = handlers_.find(std::type_index(typeid(T)));

// After:
auto it = handlers_.find(EventTypeId<T>::id());
```

This avoids RTTI altogether and uses a flat integer hash map, which is faster for large event type counts.

### Task 5: Add incremental preview rendering

**File:** `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.h` and `PreviewPanel.cpp`

Instead of re-rendering the entire document on every change, implement incremental updates:

```cpp
class PreviewPanel
{
private:
    /// Track which sections of the document changed
    struct DirtyRange
    {
        size_t start_line;
        size_t end_line;
    };

    std::vector<DirtyRange> dirty_ranges_;

    /// Cache rendered HTML per section (heading-delimited)
    std::unordered_map<size_t, std::string> section_cache_;

    /// Only re-render dirty sections
    void render_incremental(const std::string& content);
};
```

For the initial implementation, split the document at heading boundaries and cache each section's rendered HTML. When content changes, only re-render the section containing the change.

### Task 6: Optimize ThemeEngine color access

**File:** `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.h`

Add typed accessors that return pre-cached `wxColour` values instead of requiring string lookups:

```cpp
class ThemeEngine
{
public:
    // Fast typed accessors for hot-path colors
    [[nodiscard]] auto bg_color() const -> const wxColour& { return cached_bg_; }
    [[nodiscard]] auto fg_color() const -> const wxColour& { return cached_fg_; }
    [[nodiscard]] auto accent_color() const -> const wxColour& { return cached_accent_; }
    [[nodiscard]] auto border_color() const -> const wxColour& { return cached_border_; }
    [[nodiscard]] auto selection_color() const -> const wxColour& { return cached_selection_; }

private:
    wxColour cached_bg_;
    wxColour cached_fg_;
    wxColour cached_accent_;
    wxColour cached_border_;
    wxColour cached_selection_;

    void refresh_cached_colors();
};
```

Call `refresh_cached_colors()` whenever the theme changes. Consumers that access colors on every paint call can use these typed accessors instead of `resolve_color("bg")`.

### Task 7: Fix duplicate step 10 comment

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

Lines 118 and 124 both have "// 10." comment:

```cpp
// Line 118: // 10. Initialize Mermaid renderer
// Line 124: // 10. Publish app ready event
```

Fix the second to `// 11.`:

```cpp
// 11. Publish app ready event
```

(This is a minor polish item that can also be done in Phase 14, but including here as part of final polish.)

### Task 8: Profile and verify improvements

Create a benchmark script or test that measures:
1. Time to publish 10,000 events via `publish()` vs `publish_fast()`
2. Time to render a 1000-line Markdown document
3. Time to look up 100 config values (cached vs uncached)
4. Binary size before and after event RTTI optimization

## Acceptance Criteria

1. `publish_fast()` is used for high-frequency events (cursor, content, scroll)
2. HtmlRenderer pre-allocates output buffers
3. Config has a cached values struct for hot-path reads
4. ThemeEngine has typed color accessors
5. Preview panel supports incremental rendering for heading-delimited sections
6. All existing tests pass
7. No regressions in rendering correctness
8. Measurable performance improvement in event dispatch and rendering

## Testing Requirements

- Add performance benchmark test for EventBus throughput
- Add benchmark for HtmlRenderer rendering time
- Add test verifying cached Config values match uncached values
- Add test verifying incremental rendering produces identical output to full rendering
- All existing tests pass
- Manual testing: verify smooth typing and scrolling experience
