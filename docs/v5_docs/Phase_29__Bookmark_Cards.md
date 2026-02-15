# Phase 29 -- Bookmark Cards

## Objective

Implement rich URL preview cards on the canvas: paste a URL to create a bookmark card that fetches the page's Open Graph metadata (title, description, image, favicon) and displays it as a styled card. Includes a metadata scraping service.

## Prerequisites

- Phase 01 (CanvasObject)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 13 (ImageCache for preview images)
- Existing HttpClient (`src/core/HttpClient.h`)

## Feature References (PRD)

- PRD #82: Bookmark Cards

## Data Structures to Implement

### File: `src/canvas/BookmarkCardObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <filesystem>
#include <string>

namespace markamp::canvas
{

struct BookmarkMetadata
{
    std::string title;
    std::string description;
    std::string site_name;
    std::string favicon_url;
    std::string image_url;
    std::filesystem::path cached_image_path;
    std::filesystem::path cached_favicon_path;
};

class BookmarkCardObject : public CanvasObject
{
public:
    BookmarkCardObject();

    [[nodiscard]] auto url() const -> const std::string&;
    auto set_url(const std::string& url) -> void;

    [[nodiscard]] auto bookmark_metadata() const -> const BookmarkMetadata&;
    auto set_bookmark_metadata(const BookmarkMetadata& meta) -> void;

    [[nodiscard]] auto card_width() const -> double;
    [[nodiscard]] auto card_height() const -> double;
    auto set_card_dimensions(double w, double h) -> void;

    [[nodiscard]] auto show_image() const -> bool;
    auto set_show_image(bool show) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string url_;
    BookmarkMetadata metadata_;
    double card_width_{320.0};
    double card_height_{180.0};
    bool show_image_{true};
};

} // namespace markamp::canvas
```

### File: `src/canvas/MetadataScraper.h`

```cpp
#pragma once

#include <functional>
#include <string>

namespace markamp::canvas
{

struct BookmarkMetadata;

class MetadataScraper
{
public:
    using OnCompleteCallback = std::function<void(const BookmarkMetadata& metadata)>;

    /// Fetch Open Graph / meta tag metadata from a URL. Runs asynchronously.
    auto scrape(const std::string& url, OnCompleteCallback on_complete) -> void;

    /// Synchronous scrape (for testing).
    [[nodiscard]] auto scrape_sync(const std::string& url) -> BookmarkMetadata;

private:
    [[nodiscard]] auto parse_og_tags(const std::string& html) const -> BookmarkMetadata;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `MetadataScraper::parse_og_tags()` -- Parse HTML for `<meta property="og:title">`, `og:description`, `og:image`, `og:site_name`. Fall back to `<title>` and `<meta name="description">`. Extract favicon from `<link rel="icon">`.

2. `BookmarkCardRenderer::render()` -- Draw card with: preview image (top), title (bold), description (truncated), URL (gray, small), favicon + site name (bottom row).

3. URL paste detection: When text is pasted and looks like a URL (starts with http:// or https://), offer to create a bookmark card instead of plain text.

## Test Cases

File: `tests/unit/test_bookmark_card.cpp`

1. **Construction** -- Verify defaults.
2. **Set URL** -- Set url, verify.
3. **Set metadata** -- Set title, description, verify.
4. **JSON round-trip** -- Serialize/deserialize.
5. **Parse OG tags** -- HTML with og:title, og:description. Verify parsed.
6. **Parse fallback** -- HTML without OG tags, with <title>. Verify fallback.
7. **Clone** -- Clone card, verify.

## Acceptance Criteria

- [ ] BookmarkCardObject with URL and scraped metadata
- [ ] MetadataScraper parses Open Graph tags from HTML
- [ ] Card rendering with image, title, description, favicon
- [ ] URL paste triggers bookmark card creation
- [ ] Async metadata fetching with loading placeholder
- [ ] All 7 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/BookmarkCardObject.h` | Bookmark data model |
| CREATE | `src/canvas/BookmarkCardObject.cpp` | Implementation |
| CREATE | `src/canvas/BookmarkCardRenderer.h` | Card rendering |
| CREATE | `src/canvas/BookmarkCardRenderer.cpp` | Card drawing |
| CREATE | `src/canvas/MetadataScraper.h` | HTML metadata scraping |
| CREATE | `src/canvas/MetadataScraper.cpp` | OG tag parser |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_bookmark_card.cpp` | 7 Catch2 tests |

## Architecture Notes

- MetadataScraper runs HTTP fetch asynchronously via std::async. The result is posted back to the main thread via EventBus::queue().
- Images and favicons are downloaded and cached in the board's assets directory.

## Estimated Complexity

**M** -- Bookmark object, HTML scraping, card rendering, async fetching, 7 tests.
