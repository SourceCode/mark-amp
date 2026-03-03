#include "AnsiParser.h"

#include <charconv>
#include <sstream>

namespace markamp::core
{

auto AnsiParser::parse(const std::string& raw) -> std::vector<AnsiOp>
{
    std::vector<AnsiOp> ops;
    std::string text_run;

    auto flush_text = [&]()
    {
        if (!text_run.empty())
        {
            ops.emplace_back(TextOutput{std::move(text_run), current_attrs_});
            text_run.clear();
        }
    };

    for (const char ch : raw)
    {
        switch (state_)
        {
            case State::kGround:
                if (ch == '\x1b')
                {
                    flush_text();
                    state_ = State::kEscape;
                    accumulator_.clear();
                }
                else if (ch == '\n')
                {
                    flush_text();
                    ops.emplace_back(LineFeed{});
                }
                else if (ch == '\r')
                {
                    flush_text();
                    ops.emplace_back(CarriageReturn{});
                }
                else if (ch == '\x07')
                {
                    flush_text();
                    ops.emplace_back(Bell{});
                }
                else if (ch == '\b')
                {
                    flush_text();
                    ops.emplace_back(CursorMove{0, -1});
                }
                else if (ch == '\t')
                {
                    // Tab: advance to next 8-col boundary
                    text_run += "        ";
                }
                else if (static_cast<unsigned char>(ch) >= 0x20)
                {
                    text_run += ch;
                }
                break;

            case State::kEscape:
                if (ch == '[')
                {
                    state_ = State::kCsi;
                    accumulator_.clear();
                }
                else if (ch == ']')
                {
                    state_ = State::kOsc;
                    accumulator_.clear();
                }
                else if (ch == '(' || ch == ')')
                {
                    // Character set designation — skip next byte
                    state_ = State::kGround;
                }
                else
                {
                    // Unknown escape — return to ground
                    state_ = State::kGround;
                }
                break;

            case State::kCsi:
                if ((ch >= '0' && ch <= '9') || ch == ';' || ch == '?' || ch == '>')
                {
                    accumulator_ += ch;
                }
                else
                {
                    // Final character
                    process_csi(accumulator_, ch, ops);
                    state_ = State::kGround;
                    accumulator_.clear();
                }
                break;

            case State::kOsc:
                if (ch == '\x07')
                {
                    // BEL terminates OSC
                    process_osc(accumulator_, ops);
                    state_ = State::kGround;
                    accumulator_.clear();
                }
                else if (ch == '\x1b')
                {
                    state_ = State::kOscString;
                }
                else
                {
                    accumulator_ += ch;
                }
                break;

            case State::kOscString:
                if (ch == '\\')
                {
                    // ST (String Terminator) = ESC + backslash
                    process_osc(accumulator_, ops);
                    state_ = State::kGround;
                    accumulator_.clear();
                }
                else
                {
                    // Not a valid ST, treat as data
                    accumulator_ += '\x1b';
                    accumulator_ += ch;
                    state_ = State::kOsc;
                }
                break;
        }
    }

    flush_text();
    return ops;
}

void AnsiParser::reset()
{
    state_ = State::kGround;
    accumulator_.clear();
    current_attrs_ = AnsiAttributes{};
}

void AnsiParser::process_csi(const std::string& params, char final_char, std::vector<AnsiOp>& ops)
{
    // Parse semicolon-separated parameters
    std::vector<int> param_values;
    std::istringstream stream(params);
    std::string token;
    while (std::getline(stream, token, ';'))
    {
        if (token.empty() || token[0] == '?')
        {
            param_values.push_back(0);
        }
        else
        {
            int val = 0;
            std::from_chars(token.data(), token.data() + token.size(), val);
            param_values.push_back(val);
        }
    }
    if (param_values.empty())
    {
        param_values.push_back(0);
    }

    switch (final_char)
    {
        case 'm': // SGR
            process_sgr(params);
            break;

        case 'A': // CUU — Cursor Up
            ops.emplace_back(CursorMove{-(param_values[0] == 0 ? 1 : param_values[0]), 0});
            break;

        case 'B': // CUD — Cursor Down
            ops.emplace_back(CursorMove{param_values[0] == 0 ? 1 : param_values[0], 0});
            break;

        case 'C': // CUF — Cursor Forward
            ops.emplace_back(CursorMove{0, param_values[0] == 0 ? 1 : param_values[0]});
            break;

        case 'D': // CUB — Cursor Back
            ops.emplace_back(CursorMove{0, -(param_values[0] == 0 ? 1 : param_values[0])});
            break;

        case 'H': // CUP — Cursor Position
        case 'f': // HVP — Horizontal Vertical Position
        {
            const int row =
                (param_values.size() >= 1 && param_values[0] > 0) ? param_values[0] - 1 : 0;
            const int col =
                (param_values.size() >= 2 && param_values[1] > 0) ? param_values[1] - 1 : 0;
            ops.emplace_back(CursorPosition{row, col});
            break;
        }

        case 'J': // ED — Erase in Display
            ops.emplace_back(EraseDisplay{param_values[0]});
            break;

        case 'K': // EL — Erase in Line
            ops.emplace_back(EraseLine{param_values[0]});
            break;

        case 'G': // CHA — Cursor Horizontal Absolute
        {
            const int col = (param_values[0] > 0) ? param_values[0] - 1 : 0;
            ops.emplace_back(CursorPosition{-1, col}); // -1 means keep current row
            break;
        }

        default:
            // Ignore unrecognized CSI sequences
            break;
    }
}

void AnsiParser::process_sgr(const std::string& params)
{
    std::vector<int> values;
    std::istringstream stream(params);
    std::string token;
    while (std::getline(stream, token, ';'))
    {
        if (token.empty())
        {
            values.push_back(0);
        }
        else
        {
            int val = 0;
            std::from_chars(token.data(), token.data() + token.size(), val);
            values.push_back(val);
        }
    }
    if (values.empty())
    {
        values.push_back(0);
    }

    for (std::size_t idx = 0; idx < values.size(); ++idx)
    {
        const int val = values[idx];

        if (val == 0)
        {
            current_attrs_ = AnsiAttributes{};
        }
        else if (val == 1)
        {
            current_attrs_.bold = true;
        }
        else if (val == 2)
        {
            current_attrs_.dim = true;
        }
        else if (val == 3)
        {
            current_attrs_.italic = true;
        }
        else if (val == 4)
        {
            current_attrs_.underline = true;
        }
        else if (val == 7)
        {
            current_attrs_.inverse = true;
        }
        else if (val == 8)
        {
            current_attrs_.hidden = true;
        }
        else if (val == 9)
        {
            current_attrs_.strikethrough = true;
        }
        else if (val == 22)
        {
            current_attrs_.bold = false;
            current_attrs_.dim = false;
        }
        else if (val == 23)
        {
            current_attrs_.italic = false;
        }
        else if (val == 24)
        {
            current_attrs_.underline = false;
        }
        else if (val == 27)
        {
            current_attrs_.inverse = false;
        }
        else if (val == 28)
        {
            current_attrs_.hidden = false;
        }
        else if (val == 29)
        {
            current_attrs_.strikethrough = false;
        }
        else if (val >= 30 && val <= 37)
        {
            // Standard foreground colors
            current_attrs_.foreground.type = AnsiColor::Type::kStandard;
            current_attrs_.foreground.index = static_cast<std::uint8_t>(val - 30);
        }
        else if (val == 38)
        {
            // Extended foreground: 38;5;N or 38;2;R;G;B
            if (idx + 1 < values.size() && values[idx + 1] == 5 && idx + 2 < values.size())
            {
                current_attrs_.foreground.type = AnsiColor::Type::k256;
                current_attrs_.foreground.index = static_cast<std::uint8_t>(values[idx + 2]);
                idx += 2;
            }
            else if (idx + 1 < values.size() && values[idx + 1] == 2 && idx + 4 < values.size())
            {
                current_attrs_.foreground.type = AnsiColor::Type::kRgb;
                current_attrs_.foreground.red_channel = static_cast<std::uint8_t>(values[idx + 2]);
                current_attrs_.foreground.green_channel =
                    static_cast<std::uint8_t>(values[idx + 3]);
                current_attrs_.foreground.blue_channel = static_cast<std::uint8_t>(values[idx + 4]);
                idx += 4;
            }
        }
        else if (val == 39)
        {
            current_attrs_.foreground = AnsiColor{};
        }
        else if (val >= 40 && val <= 47)
        {
            // Standard background colors
            current_attrs_.background.type = AnsiColor::Type::kStandard;
            current_attrs_.background.index = static_cast<std::uint8_t>(val - 40);
        }
        else if (val == 48)
        {
            // Extended background: 48;5;N or 48;2;R;G;B
            if (idx + 1 < values.size() && values[idx + 1] == 5 && idx + 2 < values.size())
            {
                current_attrs_.background.type = AnsiColor::Type::k256;
                current_attrs_.background.index = static_cast<std::uint8_t>(values[idx + 2]);
                idx += 2;
            }
            else if (idx + 1 < values.size() && values[idx + 1] == 2 && idx + 4 < values.size())
            {
                current_attrs_.background.type = AnsiColor::Type::kRgb;
                current_attrs_.background.red_channel = static_cast<std::uint8_t>(values[idx + 2]);
                current_attrs_.background.green_channel =
                    static_cast<std::uint8_t>(values[idx + 3]);
                current_attrs_.background.blue_channel = static_cast<std::uint8_t>(values[idx + 4]);
                idx += 4;
            }
        }
        else if (val == 49)
        {
            current_attrs_.background = AnsiColor{};
        }
        else if (val >= 90 && val <= 97)
        {
            // Bright foreground colors
            current_attrs_.foreground.type = AnsiColor::Type::kBright;
            current_attrs_.foreground.index = static_cast<std::uint8_t>(val - 90);
        }
        else if (val >= 100 && val <= 107)
        {
            // Bright background colors
            current_attrs_.background.type = AnsiColor::Type::kBright;
            current_attrs_.background.index = static_cast<std::uint8_t>(val - 100);
        }
    }
}

void AnsiParser::process_osc(const std::string& content, std::vector<AnsiOp>& ops)
{
    if (content.starts_with("0;") || content.starts_with("2;"))
    {
        // OSC 0 or OSC 2: Set title
        ops.emplace_back(SetTitle{content.substr(2)});
    }
    else if (content.starts_with("7;"))
    {
        // OSC 7: Set working directory (file://hostname/path)
        std::string url = content.substr(2);
        // Strip file://hostname prefix
        if (url.starts_with("file://"))
        {
            const auto slash_pos = url.find('/', 7);
            if (slash_pos != std::string::npos)
            {
                ops.emplace_back(SetCwd{url.substr(slash_pos)});
            }
        }
    }
}

} // namespace markamp::core
