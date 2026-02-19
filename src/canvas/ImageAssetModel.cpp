#include "ImageAssetModel.h"

#include <algorithm>

namespace markamp::canvas
{

void ImageAssetModel::set_source(const std::string& path)
{
    source_ = path;
}
auto ImageAssetModel::source() const -> const std::string&
{
    return source_;
}

void ImageAssetModel::replace_source(const std::string& new_path)
{
    source_ = new_path;
    ++replacement_count_;
}

auto ImageAssetModel::replacement_count() const -> int
{
    return replacement_count_;
}

void ImageAssetModel::set_fit_mode(ImageFitMode mode)
{
    fit_mode_ = mode;
}
auto ImageAssetModel::fit_mode() const -> ImageFitMode
{
    return fit_mode_;
}

void ImageAssetModel::set_crop(CropRegion region)
{
    crop_ = region;
}
auto ImageAssetModel::crop() const -> const CropRegion&
{
    return crop_;
}

auto ImageAssetModel::is_cropped() const -> bool
{
    return crop_.left > 0.0 || crop_.top > 0.0 || crop_.right < 1.0 || crop_.bottom < 1.0;
}

void ImageAssetModel::reset_crop()
{
    crop_ = {0.0, 0.0, 1.0, 1.0};
}

void ImageAssetModel::set_render_quality(double quality)
{
    render_quality_ = std::clamp(quality, 0.0, 1.0);
}
auto ImageAssetModel::render_quality() const -> double
{
    return render_quality_;
}

} // namespace markamp::canvas
