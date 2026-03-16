#include "ImageObject.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>
#include <unordered_map>

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
    format_ = detect_image_format(path);
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

// --- Batch 7 (#40-42) ---

auto ImageObject::reset_to_original_size() -> void
{
    width_ = original_width_;
    height_ = original_height_;
    mark_dirty();
}

auto ImageObject::scale_to_fit(double max_width, double max_height) -> void
{
    if (original_width_ < 1e-6 || original_height_ < 1e-6)
    {
        return;
    }

    const double scale_x = max_width / original_width_;
    const double scale_y = max_height / original_height_;
    const double scale = std::min(scale_x, scale_y);

    width_ = original_width_ * scale;
    height_ = original_height_ * scale;
    mark_dirty();
}

auto ImageObject::is_loaded() const -> bool
{
    return !file_path_.empty();
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
    copy->format_ = format_;
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

auto ImageObject::from_json(const std::string& json) -> void
{
    auto extract_string = [&](const std::string& key) -> std::string
    {
        const std::string needle = "\"" + key + "\":\"";
        const auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return {};
        }
        const auto val_start = pos + needle.size();
        const auto val_end = json.find('"', val_start);
        if (val_end == std::string::npos)
        {
            return {};
        }
        return json.substr(val_start, val_end - val_start);
    };

    auto extract_number = [&](const std::string& key) -> double
    {
        const std::string needle = "\"" + key + "\":";
        const auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return 0.0;
        }
        const auto val_start = pos + needle.size();
        return std::stod(json.substr(val_start));
    };

    auto extract_bool = [&](const std::string& key) -> bool
    {
        const std::string needle = "\"" + key + "\":";
        const auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return false;
        }
        const auto val_start = pos + needle.size();
        return json.substr(val_start, 4) == "true";
    };

    const std::string path = extract_string("file_path");
    if (!path.empty())
    {
        load_from_file(path);
    }

    width_ = extract_number("width");
    height_ = extract_number("height");
    original_width_ = extract_number("original_width");
    original_height_ = extract_number("original_height");
    maintain_aspect_ = extract_bool("maintain_aspect");
    alt_text_ = extract_string("alt_text");

    // Parse optional crop region: "crop":{"min_x":...,"min_y":...,"max_x":...,"max_y":...}
    const auto crop_pos = json.find("\"crop\":{");
    if (crop_pos != std::string::npos)
    {
        // Extract numbers relative to the crop sub-object.
        const auto crop_end = json.find('}', crop_pos + 8);
        const std::string crop_json = json.substr(crop_pos, crop_end - crop_pos + 1);

        auto crop_num = [&](const std::string& key) -> double
        {
            const std::string needle = "\"" + key + "\":";
            const auto pos = crop_json.find(needle);
            if (pos == std::string::npos)
            {
                return 0.0;
            }
            return std::stod(crop_json.substr(pos + needle.size()));
        };

        set_crop_region({crop_num("min_x"), crop_num("min_y"), crop_num("max_x"), crop_num("max_y")});
    }

    mark_dirty();
}

auto ImageObject::format() const -> ImageFormat
{
    return format_;
}

// ── Free functions: Image format support ────────────────────────

auto detect_image_format(const std::string& path) -> ImageFormat
{
    // Find the last dot.
    const auto dot_pos = path.rfind('.');
    if (dot_pos == std::string::npos || dot_pos + 1 >= path.size())
    {
        return ImageFormat::kUnknown;
    }

    std::string ext = path.substr(dot_pos + 1);
    std::transform(ext.begin(),
                   ext.end(),
                   ext.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    // Static lookup table.
    static const std::unordered_map<std::string, ImageFormat> kExtMap = {
        // PNG
        {"png", ImageFormat::kPng},
        // JPEG
        {"jpg", ImageFormat::kJpeg},
        {"jpeg", ImageFormat::kJpeg},
        {"jpe", ImageFormat::kJpeg},
        {"jfif", ImageFormat::kJpeg},
        // GIF
        {"gif", ImageFormat::kGif},
        // WebP
        {"webp", ImageFormat::kWebP},
        // AVIF
        {"avif", ImageFormat::kAvif},
        // SVG
        {"svg", ImageFormat::kSvg},
        {"svgz", ImageFormat::kSvg},
        // BMP
        {"bmp", ImageFormat::kBmp},
        {"dib", ImageFormat::kBmp},
        // TIFF
        {"tif", ImageFormat::kTiff},
        {"tiff", ImageFormat::kTiff},
        // ICO
        {"ico", ImageFormat::kIco},
        {"cur", ImageFormat::kIco},
        // HEIF
        {"heif", ImageFormat::kHeif},
        {"heic", ImageFormat::kHeif},
        {"hif", ImageFormat::kHeif},
        // JPEG XL
        {"jxl", ImageFormat::kJxl},
        // OpenEXR
        {"exr", ImageFormat::kExr},
        // DDS
        {"dds", ImageFormat::kDds},
        // TGA
        {"tga", ImageFormat::kTga},
        // PSD
        {"psd", ImageFormat::kPsd},
        {"psb", ImageFormat::kPsd},
        // RAW
        {"raw", ImageFormat::kRaw},
        {"cr2", ImageFormat::kRaw},
        {"cr3", ImageFormat::kRaw},
        {"nef", ImageFormat::kRaw},
        {"arw", ImageFormat::kRaw},
        {"dng", ImageFormat::kRaw},
        {"orf", ImageFormat::kRaw},
        {"rw2", ImageFormat::kRaw},
        // PDF
        {"pdf", ImageFormat::kPdf},
    };

    const auto iter = kExtMap.find(ext);
    if (iter != kExtMap.end())
    {
        return iter->second;
    }
    return ImageFormat::kUnknown;
}

auto image_format_name(ImageFormat fmt) -> std::string
{
    switch (fmt)
    {
        case ImageFormat::kUnknown:
            return "Unknown";
        case ImageFormat::kPng:
            return "PNG";
        case ImageFormat::kJpeg:
            return "JPEG";
        case ImageFormat::kGif:
            return "GIF";
        case ImageFormat::kWebP:
            return "WebP";
        case ImageFormat::kAvif:
            return "AVIF";
        case ImageFormat::kSvg:
            return "SVG";
        case ImageFormat::kBmp:
            return "BMP";
        case ImageFormat::kTiff:
            return "TIFF";
        case ImageFormat::kIco:
            return "ICO";
        case ImageFormat::kHeif:
            return "HEIF";
        case ImageFormat::kJxl:
            return "JPEG XL";
        case ImageFormat::kExr:
            return "OpenEXR";
        case ImageFormat::kDds:
            return "DDS";
        case ImageFormat::kTga:
            return "TGA";
        case ImageFormat::kPsd:
            return "PSD";
        case ImageFormat::kRaw:
            return "RAW";
        case ImageFormat::kPdf:
            return "PDF";
    }
    return "Unknown";
}

auto image_format_mime_type(ImageFormat fmt) -> std::string
{
    switch (fmt)
    {
        case ImageFormat::kPng:
            return "image/png";
        case ImageFormat::kJpeg:
            return "image/jpeg";
        case ImageFormat::kGif:
            return "image/gif";
        case ImageFormat::kWebP:
            return "image/webp";
        case ImageFormat::kAvif:
            return "image/avif";
        case ImageFormat::kSvg:
            return "image/svg+xml";
        case ImageFormat::kBmp:
            return "image/bmp";
        case ImageFormat::kTiff:
            return "image/tiff";
        case ImageFormat::kIco:
            return "image/x-icon";
        case ImageFormat::kHeif:
            return "image/heif";
        case ImageFormat::kJxl:
            return "image/jxl";
        case ImageFormat::kExr:
            return "image/x-exr";
        case ImageFormat::kDds:
            return "image/vnd-ms.dds";
        case ImageFormat::kTga:
            return "image/x-tga";
        case ImageFormat::kPsd:
            return "image/vnd.adobe.photoshop";
        case ImageFormat::kRaw:
            return "image/x-raw";
        case ImageFormat::kPdf:
            return "application/pdf";
        case ImageFormat::kUnknown:
            return "";
    }
    return "";
}

auto is_web_image_format(ImageFormat fmt) -> bool
{
    switch (fmt)
    {
        case ImageFormat::kPng:
        case ImageFormat::kJpeg:
        case ImageFormat::kGif:
        case ImageFormat::kWebP:
        case ImageFormat::kAvif:
        case ImageFormat::kSvg:
        case ImageFormat::kIco:
            return true;
        default:
            return false;
    }
}

} // namespace markamp::canvas
