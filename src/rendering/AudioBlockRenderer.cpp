// ============================================================================
// File: src/rendering/AudioBlockRenderer.cpp
// Phase 29: Advanced Block Types — Audio block renderer implementation
// ============================================================================
#include "AudioBlockRenderer.h"

#include <algorithm>
#include <sstream>

namespace markamp::rendering
{

auto AudioBlockRenderer::render(std::string_view source_path,
                                const std::filesystem::path& base_path) const -> std::string
{
    // Resolve relative path
    std::filesystem::path resolved = base_path / source_path;
    std::error_code error_code;
    const bool exists = std::filesystem::exists(resolved, error_code);

    // Determine extension for MIME type
    const auto ext_str = resolved.extension().string();
    const auto mime = mime_type(ext_str);

    std::ostringstream oss;
    oss << R"(<div class="audio-block">)"
        << "\n";

    if (exists && !error_code)
    {
        oss << R"(  <audio controls preload="metadata">)"
            << "\n"
            << R"(    <source src=")" << resolved.string() << R"(" type=")" << mime << R"(">)"
            << "\n"
            << "    Your browser does not support the audio element.\n"
            << "  </audio>\n";
    }
    else
    {
        oss << R"(  <p class="audio-missing">Audio file not found: )" << source_path << "</p>\n";
    }

    oss << "</div>\n";
    return oss.str();
}

auto AudioBlockRenderer::is_audio_file(std::string_view path) -> bool
{
    // Extract extension
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

auto AudioBlockRenderer::mime_type(std::string_view extension) -> std::string_view
{
    if (extension == ".mp3")
        return "audio/mpeg";
    if (extension == ".wav")
        return "audio/wav";
    if (extension == ".ogg")
        return "audio/ogg";
    if (extension == ".m4a")
        return "audio/mp4";
    if (extension == ".flac")
        return "audio/flac";
    if (extension == ".aac")
        return "audio/aac";
    if (extension == ".webm")
        return "audio/webm";
    return "audio/mpeg";
}

} // namespace markamp::rendering
