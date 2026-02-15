# Phase 28 -- Embedded Video

## Objective

Implement video embedding on the canvas: paste a URL (YouTube, Vimeo, etc.) to create a video card object with oEmbed metadata (title, thumbnail, provider). Includes an inline video player for local files using wxMediaCtrl.

## Prerequisites

- Phase 01 (CanvasObject)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 13 (ImageObject for thumbnail display)
- Existing HttpClient (`src/core/HttpClient.h`)

## Feature References (PRD)

- PRD #66: Embedded Video

## Data Structures to Implement

### File: `src/canvas/VideoEmbedObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <filesystem>
#include <string>

namespace markamp::canvas
{

struct OEmbedData
{
    std::string title;
    std::string author_name;
    std::string provider_name;
    std::string thumbnail_url;
    std::filesystem::path thumbnail_path; // Local cached thumbnail
    int width{480};
    int height{270};
};

class VideoEmbedObject : public CanvasObject
{
public:
    VideoEmbedObject();

    [[nodiscard]] auto url() const -> const std::string&;
    auto set_url(const std::string& url) -> void;

    [[nodiscard]] auto oembed() const -> const OEmbedData&;
    auto set_oembed(const OEmbedData& data) -> void;

    [[nodiscard]] auto display_width() const -> double;
    [[nodiscard]] auto display_height() const -> double;
    auto set_display_dimensions(double w, double h) -> void;

    [[nodiscard]] auto is_local_file() const -> bool;
    auto set_local_file(const std::filesystem::path& path) -> void;
    [[nodiscard]] auto local_file() const -> const std::filesystem::path&;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string url_;
    OEmbedData oembed_;
    double display_width_{480.0};
    double display_height_{270.0};
    std::filesystem::path local_file_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `VideoEmbedRenderer::render()` -- Draw thumbnail image with a play button overlay (triangle in circle). Draw title below. For local files, optionally show a playback control bar.

2. oEmbed fetching: When a URL is pasted, detect video providers (YouTube, Vimeo). Fetch oEmbed JSON endpoint. Parse metadata. Download thumbnail.

3. Double-click to play: For local files, open in a wxMediaCtrl overlay. For URLs, open in the default browser.

## Test Cases

File: `tests/unit/test_video_embed.cpp`

1. **Construction** -- Verify defaults.
2. **Set URL** -- Set YouTube URL, verify.
3. **OEmbed data** -- Set oembed, verify title, thumbnail.
4. **Local file** -- Set local path, verify is_local_file.
5. **JSON round-trip** -- Serialize/deserialize.
6. **Display dimensions** -- Set dimensions, verify.

## Acceptance Criteria

- [ ] VideoEmbedObject with URL, oEmbed metadata, local file support
- [ ] Thumbnail + play button rendering
- [ ] oEmbed metadata fetching for YouTube/Vimeo URLs
- [ ] Double-click opens video
- [ ] JSON serialization
- [ ] All 6 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/VideoEmbedObject.h` | Video embed data model |
| CREATE | `src/canvas/VideoEmbedObject.cpp` | Implementation |
| CREATE | `src/canvas/VideoEmbedRenderer.h` | Video rendering |
| CREATE | `src/canvas/VideoEmbedRenderer.cpp` | Thumbnail + play overlay |
| CREATE | `src/canvas/OEmbedFetcher.h` | oEmbed metadata fetching |
| CREATE | `src/canvas/OEmbedFetcher.cpp` | HTTP fetch + parse |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_video_embed.cpp` | 6 Catch2 tests |

## Architecture Notes

- oEmbed fetching uses the existing HttpClient. The fetch runs asynchronously; the object displays a loading placeholder until metadata arrives.
- Thumbnail images are cached in the board's assets directory.

## Estimated Complexity

**M** -- Video object, oEmbed HTTP fetch, thumbnail rendering, play overlay, 6 tests.
