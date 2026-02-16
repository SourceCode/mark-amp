#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

namespace markamp::canvas
{

/// Supported image formats for the canvas.
/// Covers the most popular formats in web and software development.
enum class ImageFormat : uint8_t
{
    kUnknown,
    // Web-native raster formats
    kPng,
    kJpeg,
    kGif,
    kWebP,
    kAvif,
    // Web-native vector
    kSvg,
    // Legacy/desktop raster
    kBmp,
    kTiff,
    kIco,
    // Modern / next-gen
    kHeif, ///< HEIF / HEIC (Apple)
    kJxl,  ///< JPEG XL
    kExr,  ///< OpenEXR (VFX / HDR)
    kDds,  ///< DirectDraw Surface (game textures)
    kTga,  ///< Targa (game / 3D pipelines)
    kPsd,  ///< Adobe Photoshop
    kRaw,  ///< Camera RAW (CR2, NEF, ARW, …)
    kPdf,  ///< PDF (rasterisable)
};

/// Detect the ImageFormat from a file path extension.
[[nodiscard]] auto detect_image_format(const std::string& path) -> ImageFormat;

/// Human-readable display name for an ImageFormat.
[[nodiscard]] auto image_format_name(ImageFormat fmt) -> std::string;

/// MIME type string for an ImageFormat (empty for kUnknown).
[[nodiscard]] auto image_format_mime_type(ImageFormat fmt) -> std::string;

/// Returns true if the format is natively supported by modern web browsers.
[[nodiscard]] auto is_web_image_format(ImageFormat fmt) -> bool;

/// A canvas object that embeds a raster image loaded from disk.
/// Supports resize with optional aspect-ratio lock and a crop region.
class ImageObject : public CanvasObject
{
public:
    ImageObject();

    // ── File ────────────────────────────────────────────────────

    /// Load image metadata (dimensions) from a file path.
    /// Returns true if the path was set (actual pixel loading is deferred to renderer).
    auto load_from_file(const std::string& path) -> bool;

    [[nodiscard]] auto file_path() const -> const std::string&;

    // ── Dimensions ──────────────────────────────────────────────

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;

    /// Resize the image. If maintain_aspect_ is true, the smaller
    /// dimension is scaled proportionally based on the original aspect ratio.
    auto resize(double new_width, double new_height) -> void;

    [[nodiscard]] auto original_size() const -> Size2D;
    [[nodiscard]] auto aspect_ratio() const -> double;

    // ── Aspect Ratio Lock ───────────────────────────────────────

    [[nodiscard]] auto maintain_aspect() const -> bool;
    auto set_maintain_aspect(bool maintain) -> void;

    // ── Crop Region ─────────────────────────────────────────────

    [[nodiscard]] auto has_crop_region() const -> bool;
    [[nodiscard]] auto crop_region() const -> std::optional<AABB>;
    auto set_crop_region(AABB region) -> void;
    auto clear_crop_region() -> void;

    // ── Alt Text ────────────────────────────────────────────────

    [[nodiscard]] auto alt_text() const -> const std::string&;
    auto set_alt_text(const std::string& text) -> void;

    // ── Batch 7 (#40-42) ──────────────────────────────────────────

    /// Reset dimensions to original_width × original_height.
    auto reset_to_original_size() -> void;

    /// Scale uniformly to fit within a max bounding box while preserving aspect ratio.
    auto scale_to_fit(double max_width, double max_height) -> void;

    /// Returns true if file_path is non-empty.
    [[nodiscard]] auto is_loaded() const -> bool;

    // ── Image Format ────────────────────────────────────────────

    /// Get the detected image format (set automatically on load_from_file).
    [[nodiscard]] auto format() const -> ImageFormat;

    // ── CanvasObject overrides ─────────────────────────────────

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;

private:
    std::string file_path_;
    double width_{200.0};
    double height_{200.0};
    double original_width_{200.0};
    double original_height_{200.0};
    bool maintain_aspect_{true};
    std::optional<AABB> crop_region_;
    std::string alt_text_;
    ImageFormat format_{ImageFormat::kUnknown};
};

} // namespace markamp::canvas
