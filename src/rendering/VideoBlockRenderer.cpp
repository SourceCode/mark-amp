// ============================================================================
// File: src/rendering/VideoBlockRenderer.cpp
// Phase 29: Advanced Block Types — Video block renderer implementation
// ============================================================================
#include "VideoBlockRenderer.h"

#include <algorithm>
#include <sstream>

namespace markamp::rendering
{

auto VideoBlockRenderer::render(std::string_view source_path,
                                const std::filesystem::path& base_path,
                                int width,
                                int height) const -> std::string
{
    std::filesystem::path resolved = base_path / source_path;
    std::error_code error_code;
    const bool exists = std::filesystem::exists(resolved, error_code);

    const auto ext_str = resolved.extension().string();
    const auto mime = mime_type(ext_str);

    // Check for poster image (same name with .jpg extension)
    auto poster_path = resolved;
    poster_path.replace_extension(".jpg");
    const bool has_poster = std::filesystem::exists(poster_path, error_code) && !error_code;

    std::ostringstream oss;
    oss << R"(<div class="video-block">)"
        << "\n";

    if (exists && !error_code)
    {
        oss << "  <video controls";
        if (width > 0)
        {
            oss << " width=\"" << width << "\"";
        }
        if (height > 0)
        {
            oss << " height=\"" << height << "\"";
        }
        if (has_poster)
        {
            oss << " poster=\"" << poster_path.string() << "\"";
        }
        oss << " preload=\"metadata\">\n"
            << R"(    <source src=")" << resolved.string() << R"(" type=")" << mime << R"(">)"
            << "\n"
            << "    Your browser does not support the video element.\n"
            << "  </video>\n";
    }
    else
    {
        oss << R"(  <p class="video-missing">Video file not found: )" << source_path << "</p>\n";
    }

    oss << "</div>\n";
    return oss.str();
}

auto VideoBlockRenderer::is_video_file(std::string_view path) -> bool
{
    const auto dot_pos = path.rfind('.');
    if (dot_pos == std::string_view::npos)
    {
        return false;
    }

    auto ext = std::string(path.substr(dot_pos));
    std::transform(
        ext.begin(), ext.end(), ext.begin(), [](unsigned char chr) { return std::tolower(chr); });

    for (const auto supported : kSupportedExtensions)
    {
        if (ext == supported)
        {
            return true;
        }
    }

    return false;
}

auto VideoBlockRenderer::mime_type(std::string_view extension) -> std::string_view
{
    if (extension == ".mp4")
        return "video/mp4";
    if (extension == ".webm")
        return "video/webm";
    if (extension == ".ogv")
        return "video/ogg";
    if (extension == ".mov")
        return "video/quicktime";
    if (extension == ".avi")
        return "video/x-msvideo";
    if (extension == ".mkv")
        return "video/x-matroska";
    return "video/mp4";
}

} // namespace markamp::rendering
