#pragma once

/// @file RichDescriptionRenderer.h
/// @brief Phase 36 – Parses setting descriptions for rich text rendering.

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Types of segments in a rich description.
enum class DescriptionSegmentType : std::uint8_t
{
    kText = 0,  ///< Plain text.
    kLink,      ///< Markdown link [text](url).
    kCode,      ///< Inline code `code`.
    kSettingRef ///< Setting reference {setting.id}.
};

/// A parsed segment of a setting description.
struct DescriptionSegment
{
    DescriptionSegmentType type{DescriptionSegmentType::kText};
    std::string text;   ///< Display text.
    std::string target; ///< URL for links, setting ID for refs.
};

/// Parses setting description text into rich segments.
/// Recognizes:
///   - `[link text](url)` -> kLink with text="link text", target="url"
///   - `` `code` `` -> kCode with text="code"
///   - `{setting.id}` -> kSettingRef with text="setting.id", target="setting.id"
///   - Everything else -> kText
class RichDescriptionRenderer
{
public:
    /// Parse a description string into segments.
    [[nodiscard]] static auto parse(const std::string& description)
        -> std::vector<DescriptionSegment>;

    /// Get the plain text (all segments concatenated, stripping markup).
    [[nodiscard]] static auto plain_text(const std::string& description) -> std::string;
};

} // namespace markamp::ui
