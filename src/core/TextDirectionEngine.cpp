/// @file TextDirectionEngine.cpp
/// @brief V9 Phase 47 — TextDirectionEngine implementation.

#include "TextDirectionEngine.h"

#include <algorithm>

namespace markamp::core
{

// Unicode LRM (U+200E) and RLM (U+200F) as UTF-8
static constexpr const char* kLRM = "\xE2\x80\x8E"; // U+200E Left-to-Right Mark
static constexpr const char* kRLM = "\xE2\x80\x8F"; // U+200F Right-to-Left Mark

auto TextDirectionEngine::detect_direction(const std::string& text) const -> TextDirection
{
    if (text.empty())
    {
        return default_direction_;
    }

    int rtl_count = 0;
    int ltr_count = 0;

    // Simple heuristic: check first strong directional characters
    for (size_t idx = 0; idx < text.size(); ++idx)
    {
        auto byte = static_cast<unsigned char>(text[idx]);

        // Skip ASCII control/space
        if (byte <= 0x7F)
        {
            // ASCII letters are LTR
            if ((byte >= 'A' && byte <= 'Z') || (byte >= 'a' && byte <= 'z'))
            {
                ++ltr_count;
            }
            continue;
        }

        // Check for RTL Unicode ranges (Arabic, Hebrew)
        // Arabic: U+0600-U+06FF (UTF-8: 0xD8 0x80 to 0xDB 0xBF)
        // Hebrew: U+0590-U+05FF (UTF-8: 0xD6 0x90 to 0xD7 0xBF)
        if (idx + 1 < text.size())
        {
            auto next_byte = static_cast<unsigned char>(text[idx + 1]);
            if ((byte == 0xD8 || byte == 0xD9 || byte == 0xDA || byte == 0xDB) ||
                (byte == 0xD6 && next_byte >= 0x90) || (byte == 0xD7))
            {
                ++rtl_count;
            }
        }
    }

    if (rtl_count > ltr_count)
    {
        return TextDirection::kRTL;
    }
    if (ltr_count > 0)
    {
        return TextDirection::kLTR;
    }
    return default_direction_;
}

auto TextDirectionEngine::segment_text(const std::string& text) const -> std::vector<BidiSegment>
{
    std::vector<BidiSegment> segments;
    if (text.empty())
    {
        return segments;
    }

    // Simple: treat entire text as one segment with detected direction
    BidiSegment seg;
    seg.text = text;
    seg.direction = detect_direction(text);
    seg.start_offset = 0;
    seg.length = static_cast<int>(text.size());
    segments.push_back(std::move(seg));
    return segments;
}

auto TextDirectionEngine::apply_bidi_markers(const std::string& text, TextDirection direction)
    -> std::string
{
    if (direction == TextDirection::kRTL)
    {
        return std::string(kRLM) + text + std::string(kLRM);
    }
    if (direction == TextDirection::kLTR)
    {
        return std::string(kLRM) + text + std::string(kLRM);
    }
    return text; // kAuto — no markers
}

auto TextDirectionEngine::strip_bidi_markers(const std::string& text) -> std::string
{
    std::string result;
    result.reserve(text.size());

    const std::string lrm_str(kLRM);
    const std::string rlm_str(kRLM);

    size_t idx = 0;
    while (idx < text.size())
    {
        // Check for LRM (3 bytes UTF-8)
        if (idx + 2 < text.size() && text.substr(idx, 3) == lrm_str)
        {
            idx += 3;
            continue;
        }
        // Check for RLM (3 bytes UTF-8)
        if (idx + 2 < text.size() && text.substr(idx, 3) == rlm_str)
        {
            idx += 3;
            continue;
        }
        result += text[idx];
        ++idx;
    }
    return result;
}

void TextDirectionEngine::set_default_direction(TextDirection direction)
{
    default_direction_ = direction;
}

auto TextDirectionEngine::get_default_direction() const -> TextDirection
{
    return default_direction_;
}

auto TextDirectionEngine::is_rtl_char(char32_t codepoint) -> bool
{
    // Arabic: U+0600-U+06FF, Hebrew: U+0590-U+05FF
    return (codepoint >= 0x0590 && codepoint <= 0x05FF) ||
           (codepoint >= 0x0600 && codepoint <= 0x06FF);
}

} // namespace markamp::core
