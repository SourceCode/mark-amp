// ============================================================================
// File: src/rendering/VideoBlockRenderer.h
// Phase 29: Advanced Block Types — Video block renderer
// ============================================================================
#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace markamp::rendering
{

/// Renders video blocks as HTML5 <video> elements with playback controls.
class VideoBlockRenderer
{
public:
    /// Render a video block from a file path or URL.
    [[nodiscard]] auto render(std::string_view source_path,
                              const std::filesystem::path& base_path,
                              int width = 0,
                              int height = 0) const -> std::string;

    /// Detect if a block references a video file (by extension).
    [[nodiscard]] static auto is_video_file(std::string_view path) -> bool;

    /// Supported video extensions.
    static constexpr std::string_view kSupportedExtensions[] = {
        ".mp4", ".webm", ".ogv", ".mov", ".avi", ".mkv"};

    /// Get the MIME type for a video file extension.
    [[nodiscard]] static auto mime_type(std::string_view extension) -> std::string_view;
};

} // namespace markamp::rendering
