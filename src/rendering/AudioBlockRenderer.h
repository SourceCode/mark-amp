// ============================================================================
// File: src/rendering/AudioBlockRenderer.h
// Phase 29: Advanced Block Types — Audio block renderer
// ============================================================================
#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace markamp::rendering
{

/// Renders audio blocks as HTML5 <audio> elements with playback controls.
class AudioBlockRenderer
{
public:
    /// Render an audio block from a file path or URL.
    [[nodiscard]] auto render(std::string_view source_path,
                              const std::filesystem::path& base_path) const -> std::string;

    /// Detect if a block references an audio file (by extension).
    [[nodiscard]] static auto is_audio_file(std::string_view path) -> bool;

    /// Supported audio extensions.
    static constexpr std::string_view kSupportedExtensions[] = {
        ".mp3", ".wav", ".ogg", ".m4a", ".flac", ".aac", ".webm"};

    /// Get the MIME type for an audio file extension.
    [[nodiscard]] static auto mime_type(std::string_view extension) -> std::string_view;

    /// Number of supported audio file extensions.
    [[nodiscard]] static constexpr auto supported_extension_count() noexcept -> int
    {
        return 7;
    }
};

} // namespace markamp::rendering
