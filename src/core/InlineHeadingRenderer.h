#pragma once

/// @file InlineHeadingRenderer.h
/// @brief V9 Phase 45 Task 11 – Heading rendering metadata and font scaling.
///
/// Header-only, pure logic. Parses ATX headings and computes font scale factors
/// for WYSIWYG inline heading rendering.

#include <array>
#include <cstdint>
#include <string>

namespace markamp::core
{

/// Rendering metadata for a heading element.
struct HeadingRenderInfo
{
    int level{0};                  ///< Heading level 1–6
    std::string text;              ///< Heading text (without # markers)
    std::string anchor_id;         ///< Generated anchor slug for linking
    float font_scale_factor{1.0F}; ///< Scale factor relative to base font size
    int line_number{0};            ///< Source line number
    bool is_valid{false};          ///< Whether this is a valid heading

    /// Whether this heading is top-level.
    [[nodiscard]] auto is_top_level() const noexcept -> bool
    {
        return level == 1;
    }
};

/// Parses ATX headings and computes rendering metadata.
class InlineHeadingRenderer
{
public:
    /// Font scale factors for heading levels 1–6.
    static constexpr std::array<float, 6> kFontScales = {
        2.0F, // h1
        1.6F, // h2
        1.3F, // h3
        1.1F, // h4
        1.0F, // h5
        0.9F  // h6
    };

    InlineHeadingRenderer() = default;

    /// Parse a line as an ATX heading.
    /// Returns a valid HeadingRenderInfo if the line is a heading, invalid otherwise.
    [[nodiscard]] static auto parse_heading(const std::string& line, int line_number = 0)
        -> HeadingRenderInfo
    {
        HeadingRenderInfo info;
        info.line_number = line_number;

        if (line.empty() || line[0] != '#')
        {
            return info;
        }

        // Count heading level
        int level = 0;
        for (char chr : line)
        {
            if (chr == '#')
            {
                ++level;
            }
            else
            {
                break;
            }
        }

        if (level < 1 || level > 6)
        {
            return info;
        }

        // Must have space after # characters (or be just # chars)
        if (static_cast<std::size_t>(level) < line.size() &&
            line[static_cast<std::size_t>(level)] != ' ')
        {
            return info;
        }

        info.level = level;
        info.font_scale_factor = compute_font_scale(level);

        // Extract text (after "# ")
        auto text_start = static_cast<std::size_t>(level);
        if (text_start < line.size() && line[text_start] == ' ')
        {
            ++text_start;
        }

        auto text = line.substr(text_start);

        // Remove trailing # characters and whitespace
        while (!text.empty() && (text.back() == '#' || text.back() == ' '))
        {
            text.pop_back();
        }

        info.text = text;
        info.anchor_id = slugify(text);
        info.is_valid = true;

        return info;
    }

    /// Compute the font scale factor for a heading level.
    [[nodiscard]] static constexpr auto compute_font_scale(int level) -> float
    {
        if (level < 1 || level > 6)
        {
            return 1.0F;
        }
        return kFontScales[static_cast<std::size_t>(level - 1)];
    }

    /// Check if a line is a heading.
    [[nodiscard]] static auto is_heading(const std::string& line) -> bool
    {
        return parse_heading(line).is_valid;
    }

private:
    /// Generate a URL-friendly slug from heading text.
    [[nodiscard]] static auto slugify(const std::string& text) -> std::string
    {
        std::string slug;
        bool prev_was_dash = false;

        for (char chr : text)
        {
            if (chr == ' ' || chr == '-' || chr == '_')
            {
                if (!slug.empty() && !prev_was_dash)
                {
                    slug += '-';
                    prev_was_dash = true;
                }
            }
            else if (std::isalnum(static_cast<unsigned char>(chr)) != 0)
            {
                slug += static_cast<char>(std::tolower(static_cast<unsigned char>(chr)));
                prev_was_dash = false;
            }
            // Other characters are stripped
        }

        // Remove trailing dash
        while (!slug.empty() && slug.back() == '-')
        {
            slug.pop_back();
        }

        return slug;
    }
};

} // namespace markamp::core
