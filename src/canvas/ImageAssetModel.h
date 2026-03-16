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

    // ── Round 6 Batch 2 (#11-13) ────────────────────────────────

    /// (#11) Whether crop is full (no crop applied).
    [[nodiscard]] auto is_full() const noexcept -> bool
    {
        return left == 0.0 && top == 0.0 && right == 1.0 && bottom == 1.0;
    }

    /// (#12) Crop region width (normalized).
    [[nodiscard]] auto width() const noexcept -> double
    {
        return right - left;
    }

    /// (#13) Crop region height (normalized).
    [[nodiscard]] auto height() const noexcept -> double
    {
        return bottom - top;
    }
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

    // ── Round 6 Batch 2 (#14-17) ────────────────────────────────

    /// (#14) Whether a source path is set.
    [[nodiscard]] auto has_source() const noexcept -> bool
    {
        return !source_.empty();
    }

    /// (#15) Whether fit mode is fit.
    [[nodiscard]] auto is_fit() const noexcept -> bool
    {
        return fit_mode_ == ImageFitMode::kFit;
    }

    /// (#16) Whether fit mode is fill.
    [[nodiscard]] auto is_fill() const noexcept -> bool
    {
        return fit_mode_ == ImageFitMode::kFill;
    }

    /// (#17) Whether render quality is high.
    [[nodiscard]] auto is_high_quality() const noexcept -> bool
    {
        return render_quality_ >= 0.9;
    }
};

} // namespace markamp::canvas
