// ============================================================================
// File: src/core/BlockTypeDetector.h
// Phase 29: Advanced Block Types — Detects advanced block types in Markdown
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Detected advanced block type identifiers.
enum class AdvancedBlockType : uint8_t
{
    kCallout,
    kEmbed,
    kAudio,
    kVideo,
    kIFrame,
    kWidget,
    kSuperBlock,
    kNone
};

/// Result of detecting an advanced block type.
struct BlockDetection
{
    AdvancedBlockType type{AdvancedBlockType::kNone};
    std::string content;  // The block's inner content
    std::string metadata; // Type-specific metadata (URL, path, block_id, etc.)
    int start_line{0};
    int end_line{0};

    [[nodiscard]] auto is_advanced_type() const -> bool
    {
        return type != AdvancedBlockType::kNone;
    }
};

/// Pre-processes Markdown content to detect advanced block types
/// that are not natively supported by md4c.
class BlockTypeDetector
{
public:
    /// Scan Markdown content for all advanced block types.
    [[nodiscard]] auto detect(std::string_view markdown) const -> std::vector<BlockDetection>;

    /// Detect callout blocks within blockquotes.
    [[nodiscard]] auto detect_callouts(std::string_view markdown) const
        -> std::vector<BlockDetection>;

    /// Detect embed blocks: ((block-id)) on its own line.
    [[nodiscard]] auto detect_embeds(std::string_view markdown) const
        -> std::vector<BlockDetection>;

    /// Detect audio/video blocks: !audio[](path) or !video[](path) syntax.
    [[nodiscard]] auto detect_media(std::string_view markdown) const -> std::vector<BlockDetection>;

    /// Detect iframe blocks: ```iframe syntax.
    [[nodiscard]] auto detect_iframes(std::string_view markdown) const
        -> std::vector<BlockDetection>;

    /// Detect super block markers: {{{ row/col ... }}}.
    [[nodiscard]] auto detect_super_blocks(std::string_view markdown) const
        -> std::vector<BlockDetection>;
};

} // namespace markamp::core
