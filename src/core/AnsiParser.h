#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace markamp::core
{

/// Color representation for ANSI terminal sequences.
struct AnsiColor
{
    enum class Type : std::uint8_t
    {
        kDefault,
        kStandard,
        kBright,
        k256,
        kRgb
    };

    Type type{Type::kDefault};
    std::uint8_t index{0};                                          // For kStandard, kBright, k256
    std::uint8_t red_channel{0}, green_channel{0}, blue_channel{0}; // For kRgb
};

/// Text rendering attributes from SGR sequences.
struct AnsiAttributes
{
    AnsiColor foreground;
    AnsiColor background;
    bool bold{false};
    bool italic{false};
    bool underline{false};
    bool strikethrough{false};
    bool inverse{false};
    bool dim{false};
    bool hidden{false};
};

// ── AnsiOp variant types ──

/// A run of displayable text with attributes.
struct TextOutput
{
    std::string text;
    AnsiAttributes attrs;
};

/// Relative cursor movement.
struct CursorMove
{
    int delta_row{0};
    int delta_col{0};
};

/// Absolute cursor positioning.
struct CursorPosition
{
    int row{0};
    int col{0};
};

/// Erase display: 0=below, 1=above, 2=all.
struct EraseDisplay
{
    int mode{0};
};

/// Erase line: 0=right, 1=left, 2=all.
struct EraseLine
{
    int mode{0};
};

/// OSC title change.
struct SetTitle
{
    std::string title;
};

/// OSC 7 working directory change.
struct SetCwd
{
    std::string path;
};

/// BEL character.
struct Bell
{
};

/// Line feed.
struct LineFeed
{
};

/// Carriage return.
struct CarriageReturn
{
};

/// Union of all parsed ANSI operations.
using AnsiOp = std::variant<TextOutput,
                            CursorMove,
                            CursorPosition,
                            EraseDisplay,
                            EraseLine,
                            SetTitle,
                            SetCwd,
                            Bell,
                            LineFeed,
                            CarriageReturn>;

/// Streaming ANSI escape sequence parser.
/// Translates VT100/VT220/xterm sequences into structured AnsiOp commands.
/// Call parse() with incoming raw data; it returns a vector of operations.
/// Partial sequences are buffered across calls for streaming support.
class AnsiParser
{
public:
    AnsiParser() = default;

    /// Parse raw terminal output into structured operations.
    [[nodiscard]] auto parse(const std::string& raw) -> std::vector<AnsiOp>;

    /// Reset parser state (e.g., on terminal clear).
    void reset();

private:
    enum class State : std::uint8_t
    {
        kGround,
        kEscape,
        kCsi,
        kOsc,
        kOscString
    };

    State state_{State::kGround};
    std::string accumulator_;
    AnsiAttributes current_attrs_;

    void process_csi(const std::string& params, char final_char, std::vector<AnsiOp>& ops);
    void process_sgr(const std::string& params);
    void process_osc(const std::string& content, std::vector<AnsiOp>& ops);
};

} // namespace markamp::core
