#pragma once

#include "core/AnsiParser.h"
#include "core/LogLevel.h"
#include "core/OutputChannelService.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A rendered text segment with resolved colors and attributes for painting.
struct RenderedSegment
{
    std::string text;
    uint8_t fg_red{204};
    uint8_t fg_green{204};
    uint8_t fg_blue{204};
    uint8_t bg_red{0};
    uint8_t bg_green{0};
    uint8_t bg_blue{0};
    bool bold{false};
    bool italic{false};
    bool underline{false};
    bool use_bg{false}; ///< True if background color should be drawn
};

/// Converts ANSI-encoded output lines into paint-ready segments.
/// Uses the core::AnsiParser to parse escape sequences, then maps
/// ANSI colors and log level severity to concrete RGB values.
class OutputPanelRenderer
{
public:
    OutputPanelRenderer() = default;

    /// Parse an output line's ANSI text into renderable segments.
    [[nodiscard]] auto render_line(const core::OutputLine& line) const
        -> std::vector<RenderedSegment>;

    /// Parse raw ANSI text into renderable segments.
    [[nodiscard]] auto render_ansi(const std::string& ansi_text) const
        -> std::vector<RenderedSegment>;

    /// Get the theme color for a log level.
    struct LogLevelColor
    {
        uint8_t red{204};
        uint8_t green{204};
        uint8_t blue{204};
    };
    [[nodiscard]] static auto log_level_color(core::LogLevel level) -> LogLevelColor;

    /// Format a timestamp for display.
    [[nodiscard]] static auto
    format_timestamp(const std::chrono::system_clock::time_point& time_point) -> std::string;

private:
    /// Convert AnsiColor to RGB values.
    static void resolve_ansi_color(const core::AnsiColor& color,
                                   uint8_t& out_red,
                                   uint8_t& out_green,
                                   uint8_t& out_blue);
};

} // namespace markamp::ui
