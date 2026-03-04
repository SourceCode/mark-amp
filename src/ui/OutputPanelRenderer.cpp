#include "OutputPanelRenderer.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <variant>

namespace markamp::ui
{

namespace
{

// ANSI standard color palette (dark / normal).
constexpr struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} kAnsiStandardColors[] = {
    {0, 0, 0},       // 0  black
    {205, 49, 49},   // 1  red
    {13, 188, 121},  // 2  green
    {229, 229, 16},  // 3  yellow
    {36, 114, 200},  // 4  blue
    {188, 63, 188},  // 5  magenta
    {17, 168, 205},  // 6  cyan
    {229, 229, 229}, // 7  white
};

constexpr struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} kAnsiBrightColors[] = {
    {102, 102, 102}, // 8   bright black (gray)
    {241, 76, 76},   // 9   bright red
    {35, 209, 139},  // 10  bright green
    {245, 245, 67},  // 11  bright yellow
    {59, 142, 234},  // 12  bright blue
    {214, 112, 214}, // 13  bright magenta
    {41, 184, 219},  // 14  bright cyan
    {255, 255, 255}, // 15  bright white
};

} // namespace

auto OutputPanelRenderer::render_line(const core::OutputLine& line) const
    -> std::vector<RenderedSegment>
{
    auto segments = render_ansi(line.ansi_text);

    // If no ANSI colors were applied, tint by log level.
    if (segments.size() == 1 && !segments[0].bold && !segments[0].italic)
    {
        auto color = log_level_color(line.level);
        segments[0].fg_red = color.red;
        segments[0].fg_green = color.green;
        segments[0].fg_blue = color.blue;
    }

    return segments;
}

auto OutputPanelRenderer::render_ansi(const std::string& ansi_text) const
    -> std::vector<RenderedSegment>
{
    std::vector<RenderedSegment> segments;

    core::AnsiParser parser;
    auto ops = parser.parse(ansi_text);

    for (const auto& op_variant : ops)
    {
        if (std::holds_alternative<core::TextOutput>(op_variant))
        {
            const auto& text_op = std::get<core::TextOutput>(op_variant);
            if (text_op.text.empty())
            {
                continue;
            }

            RenderedSegment seg;
            seg.text = text_op.text;
            seg.bold = text_op.attrs.bold;
            seg.italic = text_op.attrs.italic;
            seg.underline = text_op.attrs.underline;

            // Resolve foreground
            resolve_ansi_color(text_op.attrs.foreground, seg.fg_red, seg.fg_green, seg.fg_blue);

            // Resolve background
            if (text_op.attrs.background.type != core::AnsiColor::Type::kDefault)
            {
                resolve_ansi_color(text_op.attrs.background, seg.bg_red, seg.bg_green, seg.bg_blue);
                seg.use_bg = true;
            }

            segments.push_back(std::move(seg));
        }
        // LineFeed, CarriageReturn, etc. are handled by the caller (line splitting).
    }

    if (segments.empty())
    {
        // Plain text with no ANSI sequences — return as single default segment.
        segments.push_back(RenderedSegment{.text = ansi_text});
    }

    return segments;
}

auto OutputPanelRenderer::log_level_color(core::LogLevel level) -> LogLevelColor
{
    switch (level)
    {
        case core::LogLevel::kTrace:
            return {.red = 128, .green = 128, .blue = 128}; // Gray
        case core::LogLevel::kDebug:
            return {.red = 59, .green = 142, .blue = 234}; // Blue
        case core::LogLevel::kInfo:
            return {.red = 204, .green = 204, .blue = 204}; // White/default
        case core::LogLevel::kWarning:
            return {.red = 229, .green = 229, .blue = 16}; // Yellow
        case core::LogLevel::kError:
            return {.red = 241, .green = 76, .blue = 76}; // Red
        case core::LogLevel::kFatal:
            return {.red = 235, .green = 50, .blue = 50}; // Bright red
    }
    return {.red = 204, .green = 204, .blue = 204};
}

auto OutputPanelRenderer::format_timestamp(const std::chrono::system_clock::time_point& time_point)
    -> std::string
{
    auto time_t_val = std::chrono::system_clock::to_time_t(time_point);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch())
                  .count() %
              1000;

    std::tm local_tm{};
#if defined(_WIN32)
    localtime_s(&local_tm, &time_t_val);
#else
    localtime_r(&time_t_val, &local_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms;
    return oss.str();
}

void OutputPanelRenderer::resolve_ansi_color(const core::AnsiColor& color,
                                             uint8_t& out_red,
                                             uint8_t& out_green,
                                             uint8_t& out_blue)
{
    switch (color.type)
    {
        case core::AnsiColor::Type::kDefault:
            out_red = 204;
            out_green = 204;
            out_blue = 204;
            break;
        case core::AnsiColor::Type::kStandard:
            if (color.index < 8)
            {
                out_red = kAnsiStandardColors[color.index].red;
                out_green = kAnsiStandardColors[color.index].green;
                out_blue = kAnsiStandardColors[color.index].blue;
            }
            break;
        case core::AnsiColor::Type::kBright:
            if (color.index < 8)
            {
                out_red = kAnsiBrightColors[color.index].red;
                out_green = kAnsiBrightColors[color.index].green;
                out_blue = kAnsiBrightColors[color.index].blue;
            }
            break;
        case core::AnsiColor::Type::k256:
            if (color.index < 8)
            {
                out_red = kAnsiStandardColors[color.index].red;
                out_green = kAnsiStandardColors[color.index].green;
                out_blue = kAnsiStandardColors[color.index].blue;
            }
            else if (color.index < 16)
            {
                auto bright_idx = static_cast<uint8_t>(color.index - 8);
                out_red = kAnsiBrightColors[bright_idx].red;
                out_green = kAnsiBrightColors[bright_idx].green;
                out_blue = kAnsiBrightColors[bright_idx].blue;
            }
            else if (color.index < 232)
            {
                // 6x6x6 color cube (indices 16–231)
                auto cube_idx = static_cast<uint8_t>(color.index - 16);
                out_red = static_cast<uint8_t>((cube_idx / 36) * 51);
                out_green = static_cast<uint8_t>(((cube_idx / 6) % 6) * 51);
                out_blue = static_cast<uint8_t>((cube_idx % 6) * 51);
            }
            else
            {
                // Grayscale ramp (indices 232–255)
                auto gray = static_cast<uint8_t>(8 + (color.index - 232) * 10);
                out_red = gray;
                out_green = gray;
                out_blue = gray;
            }
            break;
        case core::AnsiColor::Type::kRgb:
            out_red = color.red_channel;
            out_green = color.green_channel;
            out_blue = color.blue_channel;
            break;
    }
}

} // namespace markamp::ui
