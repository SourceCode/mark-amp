#include "ImageObject.h"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace markamp::canvas
{

ImageObject::ImageObject()
    : CanvasObject(CanvasObjectType::Image)
{
}

// ── File ────────────────────────────────────────────────────────

auto ImageObject::load_from_file(const std::string& path) -> bool
{
    file_path_ = path;
    // In a real implementation, we'd use wxImage to read actual dimensions.
    // For now, we keep the defaults or previously-set values.
    mark_dirty();
    return !path.empty();
}

auto ImageObject::file_path() const -> const std::string&
{
    return file_path_;
}

// ── Dimensions ──────────────────────────────────────────────────

auto ImageObject::width() const -> double
{
    return width_;
}

auto ImageObject::height() const -> double
{
    return height_;
}

auto ImageObject::resize(double new_width, double new_height) -> void
{
    constexpr double kMinDim = 10.0;
    new_width = std::max(kMinDim, new_width);
    new_height = std::max(kMinDim, new_height);

    if (maintain_aspect_ && original_width_ > 0.0 && original_height_ > 0.0)
    {
        const double orig_aspect = original_width_ / original_height_;
        const double requested_aspect = new_width / new_height;

        if (requested_aspect > orig_aspect)
        {
            // Width-dominated: scale height from width.
            width_ = new_width;
            height_ = new_width / orig_aspect;
        }
        else
        {
            // Height-dominated: scale width from height.
            height_ = new_height;
            width_ = new_height * orig_aspect;
        }
    }
    else
    {
        width_ = new_width;
        height_ = new_height;
    }

    mark_dirty();
}

auto ImageObject::original_size() const -> Size2D
{
    return {original_width_, original_height_};
}

auto ImageObject::aspect_ratio() const -> double
{
    if (original_height_ < 1e-6)
    {
        return 1.0;
    }
    return original_width_ / original_height_;
}

// ── Aspect Ratio Lock ───────────────────────────────────────────

auto ImageObject::maintain_aspect() const -> bool
{
    return maintain_aspect_;
}

auto ImageObject::set_maintain_aspect(bool maintain) -> void
{
    maintain_aspect_ = maintain;
}

// ── Crop Region ─────────────────────────────────────────────────

auto ImageObject::has_crop_region() const -> bool
{
    return crop_region_.has_value();
}

auto ImageObject::crop_region() const -> std::optional<AABB>
{
    return crop_region_;
}

auto ImageObject::set_crop_region(AABB region) -> void
{
    crop_region_ = region;
    mark_dirty();
}

auto ImageObject::clear_crop_region() -> void
{
    crop_region_.reset();
    mark_dirty();
}

// ── Alt Text ────────────────────────────────────────────────────

auto ImageObject::alt_text() const -> const std::string&
{
    return alt_text_;
}

auto ImageObject::set_alt_text(const std::string& text) -> void
{
    alt_text_ = text;
}

// ── CanvasObject overrides ──────────────────────────────────────

auto ImageObject::local_bounds() const -> AABB
{
    return AABB{0.0, 0.0, width_, height_};
}

auto ImageObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<ImageObject>();
    copy->file_path_ = file_path_;
    copy->width_ = width_;
    copy->height_ = height_;
    copy->original_width_ = original_width_;
    copy->original_height_ = original_height_;
    copy->maintain_aspect_ = maintain_aspect_;
    copy->crop_region_ = crop_region_;
    copy->alt_text_ = alt_text_;
    copy->set_transform(transform());
    copy->set_z_index(z_index());
    copy->set_name(name());
    return copy;
}

auto ImageObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"Image\""
        << ",\"file_path\":\"" << file_path_ << "\""
        << ",\"width\":" << width_ << ",\"height\":" << height_
        << ",\"original_width\":" << original_width_ << ",\"original_height\":" << original_height_
        << ",\"maintain_aspect\":" << (maintain_aspect_ ? "true" : "false") << ",\"alt_text\":\""
        << alt_text_ << "\"";

    if (crop_region_.has_value())
    {
        const auto& crop = crop_region_.value();
        oss << ",\"crop\":{\"min_x\":" << crop.min_x << ",\"min_y\":" << crop.min_y
            << ",\"max_x\":" << crop.max_x << ",\"max_y\":" << crop.max_y << "}";
    }

    oss << "}";
    return oss.str();
}

auto ImageObject::from_json(const std::string& /*json*/) -> void
{
    // Stub: real JSON parsing would populate fields.
}

} // namespace markamp::canvas
