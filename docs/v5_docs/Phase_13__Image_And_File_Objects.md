# Phase 13 -- Image and File Objects

## Objective

Implement image embedding on the canvas via drag-drop, file dialog, or paste. Includes image objects with crop, basic filters (brightness, contrast, grayscale), aspect-ratio-locked resize, and a generic file attachment object for non-image files. Images are stored as sidecar files alongside the .markboard file.

## Prerequisites

- Phase 01 (CanvasObject, CanvasTypes)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 03 (CanvasInputManager, drag-drop handling)
- Phase 05 (Board, UndoRedoStack)

## Feature References (PRD)

- PRD #15: Upload Files
- PRD #17: Image Editing (crop, filters)

## Data Structures to Implement

### File: `src/canvas/ImageObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <filesystem>
#include <string>

namespace markamp::canvas
{

/// Image filter settings.
struct ImageFilters
{
    double brightness{0.0};  // -1.0 to 1.0 (0 = no change)
    double contrast{0.0};    // -1.0 to 1.0 (0 = no change)
    double saturation{0.0};  // -1.0 to 1.0 (0 = no change)
    bool grayscale{false};
    double blur{0.0};        // 0 = no blur, higher = more blur
    int rotation_90{0};      // 0, 90, 180, 270 degrees

    [[nodiscard]] auto has_adjustments() const -> bool;
};

/// Crop rectangle (normalized 0.0-1.0 of original image dimensions).
struct CropRect
{
    double left{0.0};
    double top{0.0};
    double right{1.0};
    double bottom{1.0};

    [[nodiscard]] auto is_full() const -> bool;
};

/// An image embedded on the canvas.
class ImageObject : public CanvasObject
{
public:
    ImageObject();

    // --- Image source ---
    [[nodiscard]] auto file_path() const -> const std::filesystem::path&;
    auto set_file_path(const std::filesystem::path& path) -> void;

    /// Original image dimensions in pixels.
    [[nodiscard]] auto original_width() const -> int;
    [[nodiscard]] auto original_height() const -> int;
    auto set_original_dimensions(int w, int h) -> void;

    // --- Display dimensions (world units) ---
    [[nodiscard]] auto display_width() const -> double;
    [[nodiscard]] auto display_height() const -> double;
    auto set_display_dimensions(double w, double h) -> void;

    [[nodiscard]] auto preserve_aspect_ratio() const -> bool;
    auto set_preserve_aspect_ratio(bool enabled) -> void;

    // --- Crop ---
    [[nodiscard]] auto crop() const -> const CropRect&;
    auto set_crop(const CropRect& crop) -> void;
    auto reset_crop() -> void;

    // --- Filters ---
    [[nodiscard]] auto filters() const -> const ImageFilters&;
    auto set_filters(const ImageFilters& filters) -> void;
    auto reset_filters() -> void;

    // --- Border ---
    [[nodiscard]] auto border_width() const -> double;
    auto set_border_width(double w) -> void;
    [[nodiscard]] auto border_color() const -> const CanvasColor&;
    auto set_border_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto border_radius() const -> double;
    auto set_border_radius(double r) -> void;

    // --- Shadow ---
    [[nodiscard]] auto has_shadow() const -> bool;
    auto set_has_shadow(bool enabled) -> void;

    // --- CanvasObject overrides ---
    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::filesystem::path file_path_;
    int original_width_{0};
    int original_height_{0};
    double display_width_{300.0};
    double display_height_{200.0};
    bool preserve_aspect_ratio_{true};
    CropRect crop_;
    ImageFilters filters_;
    double border_width_{0.0};
    CanvasColor border_color_{200, 200, 200, 255};
    double border_radius_{0.0};
    bool has_shadow_{false};
};

} // namespace markamp::canvas
```

### File: `src/canvas/ImageCache.h`

```cpp
#pragma once

#include <filesystem>
#include <memory>
#include <unordered_map>

class wxImage;
class wxBitmap;

namespace markamp::canvas
{

/// Caches loaded and processed images for canvas rendering.
/// Handles loading from disk, applying filters, and caching scaled versions.
class ImageCache
{
public:
    ImageCache();

    /// Load or retrieve a cached image. Returns nullptr on failure.
    [[nodiscard]] auto get_image(const std::filesystem::path& path) -> wxImage*;

    /// Get a bitmap scaled to the given dimensions (cached).
    [[nodiscard]] auto get_scaled_bitmap(const std::filesystem::path& path,
                                          int width, int height) -> wxBitmap*;

    /// Invalidate a specific image (e.g., after filter changes).
    auto invalidate(const std::filesystem::path& path) -> void;

    /// Clear the entire cache.
    auto clear() -> void;

    /// Set maximum cache size in bytes.
    auto set_max_cache_bytes(size_t bytes) -> void;

private:
    struct CacheEntry
    {
        std::unique_ptr<wxImage> image;
        std::unordered_map<uint64_t, std::unique_ptr<wxBitmap>> scaled_bitmaps;
        size_t byte_size{0};
    };

    std::unordered_map<std::string, CacheEntry> cache_;
    size_t max_cache_bytes_{256 * 1024 * 1024}; // 256 MB
    size_t current_bytes_{0};

    auto evict_if_needed() -> void;
    [[nodiscard]] static auto make_scale_key(int w, int h) -> uint64_t;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `ImageRenderer::render()` -- Load image from ImageCache. Apply crop (draw sub-region). Scale to display dimensions. Apply border radius by clipping. Draw shadow if enabled. Draw border.

2. `ImageCache::get_scaled_bitmap()` -- Look up the original image. If a scaled bitmap at the requested size exists, return it. Otherwise, create a scaled copy, cache it, return it.

3. Drag-drop handling: On CanvasPanel drop event (wxDropFilesEvent), check file extensions (.png, .jpg, .jpeg, .gif, .bmp, .svg). Copy file to board's assets directory. Create ImageObject at drop position.

4. Image crop tool: When an ImageObject is selected and the user activates crop mode (C key or toolbar button), show crop handles on the image. Dragging handles adjusts the CropRect. Commit crop on Enter, cancel on Escape.

5. Filter application: The ImageRenderer applies filters at render time using wxImage manipulation (SetRGB, ConvertToGreyscale, etc.). Filtered results are cached.

6. Asset storage: Images are stored in a `<board_name>_assets/` directory alongside the .markboard file. Board serialization stores relative paths.

## Test Cases

File: `tests/unit/test_image_object.cpp`

1. **Default construction** -- Verify 300x200 display, no crop, no filters.
2. **Aspect ratio preservation** -- Set width, verify height auto-adjusted.
3. **Crop** -- Set crop to 50% region, verify.
4. **Reset crop** -- Set crop, reset, verify full crop.
5. **Filters** -- Set brightness=0.5, contrast=-0.3, verify.
6. **Has adjustments** -- Default filters, verify has_adjustments=false. Set brightness, verify true.
7. **JSON round-trip** -- Full image with crop, filters, border, serialize/deserialize.
8. **Clone** -- Clone image, verify independent copy.
9. **Image cache load** -- Load a test image, verify non-null wxImage returned.
10. **Image cache eviction** -- Fill cache beyond max, verify oldest evicted.

## Acceptance Criteria

- [ ] ImageObject with file path, dimensions, crop, filters, border, shadow
- [ ] ImageCache loads and caches images with scaled bitmap variants
- [ ] Drag-drop image files onto canvas creates ImageObject
- [ ] Paste image from clipboard creates ImageObject
- [ ] Crop tool with interactive handles
- [ ] Brightness, contrast, saturation, grayscale filters
- [ ] Aspect-ratio-locked resize by default
- [ ] Images stored in sidecar assets directory
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/ImageObject.h` | Image data model |
| CREATE | `src/canvas/ImageObject.cpp` | Image implementation |
| CREATE | `src/canvas/ImageRenderer.h` | Image rendering |
| CREATE | `src/canvas/ImageRenderer.cpp` | Render with crop, filters, border |
| CREATE | `src/canvas/ImageCache.h` | Image loading and caching |
| CREATE | `src/canvas/ImageCache.cpp` | Cache implementation |
| MODIFY | `src/ui/CanvasPanel.cpp` | Add drag-drop handling for image files |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register ImageObject factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_image_object.cpp` | 10 Catch2 tests |

## Architecture Notes

- ImageObject stores a file path, not the image data itself. The ImageCache manages loading.
- The ImageCache uses an LRU-style eviction with a byte-size budget, not just a count limit.
- Scaled bitmaps are cached separately for each requested size. This avoids re-scaling on every frame during zoom.
- Asset storage paths are stored as relative paths in JSON, making boards portable.
- Filter application at render time means the original image is never modified, enabling non-destructive editing.

## Estimated Complexity

**L** -- Image loading/caching, crop tool with interactive handles, filter application, drag-drop integration, asset management, 10 tests.
