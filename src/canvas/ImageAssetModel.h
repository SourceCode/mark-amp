#pragma once

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Image fit mode.
enum class ImageFitMode : uint8_t
{
    kFit,     ///< Scale to fit, preserve aspect
    kFill,    ///< Scale to fill, crop overflow
    kStretch, ///< Stretch to exact dimensions
    kCrop,    ///< Manual crop region
};

/// Crop region (normalized 0–1).
struct CropRegion
{
    double left{0.0};
    double top{0.0};
    double right{1.0};
    double bottom{1.0};
};

/// Testable model for Image Asset Workflows (Phase 53).
///
/// Encapsulates:
/// - Image source with replacement
/// - Fit/fill/stretch/crop modes
/// - Non-destructive crop region
/// - Render quality settings
class ImageAssetModel
{
public:
    // ── Source ───────────────────────────────────────────────────────

    void set_source(const std::string& path);
    [[nodiscard]] auto source() const -> const std::string&;

    void replace_source(const std::string& new_path);
    [[nodiscard]] auto replacement_count() const -> int;

    // ── Fit mode ────────────────────────────────────────────────────

    void set_fit_mode(ImageFitMode mode);
    [[nodiscard]] auto fit_mode() const -> ImageFitMode;

    // ── Crop ────────────────────────────────────────────────────────

    void set_crop(CropRegion region);
    [[nodiscard]] auto crop() const -> const CropRegion&;
    [[nodiscard]] auto is_cropped() const -> bool;
    void reset_crop();

    // ── Quality ─────────────────────────────────────────────────────

    void set_render_quality(double quality); ///< 0.0–1.0
    [[nodiscard]] auto render_quality() const -> double;

private:
    std::string source_;
    int replacement_count_{0};
    ImageFitMode fit_mode_{ImageFitMode::kFit};
    CropRegion crop_;
    double render_quality_{0.8};
};

} // namespace markamp::canvas
