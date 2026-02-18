/// @file TextDirectionEngine.h
/// @brief V9 Phase 47 — Bidirectional text direction engine.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Text direction.
enum class TextDirection : uint8_t
{
    kLTR = 0,
    kRTL = 1,
    kAuto = 2,
};

/// A segment of text with a specific direction.
struct BidiSegment
{
    std::string text;
    TextDirection direction{TextDirection::kLTR};
    int start_offset{0};
    int length{0};
};

/// Handles bidirectional text detection, segmentation, and Unicode marker insertion.
class TextDirectionEngine
{
public:
    TextDirectionEngine() = default;

    // ── Detection ─────────────────────────────────────────────────────
    [[nodiscard]] auto detect_direction(const std::string& text) const -> TextDirection;
    [[nodiscard]] auto segment_text(const std::string& text) const -> std::vector<BidiSegment>;

    // ── Markers ───────────────────────────────────────────────────────
    [[nodiscard]] static auto apply_bidi_markers(const std::string& text, TextDirection direction)
        -> std::string;
    [[nodiscard]] static auto strip_bidi_markers(const std::string& text) -> std::string;

    // ── Default direction ─────────────────────────────────────────────
    void set_default_direction(TextDirection direction);
    [[nodiscard]] auto get_default_direction() const -> TextDirection;

private:
    TextDirection default_direction_{TextDirection::kLTR};

    [[nodiscard]] static auto is_rtl_char(char32_t codepoint) -> bool;
};

} // namespace markamp::core
