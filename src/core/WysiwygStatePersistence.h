#pragma once

/// @file WysiwygStatePersistence.h
/// @brief V9 Phase 45 Task 16 – State persistence for WYSIWYG mode.
///
/// Header-only, pure logic. Serializes/deserializes per-file WYSIWYG state
/// to/from JSON-like string format for session restoration.

#include "WysiwygTypes.h"

#include <set>
#include <sstream>
#include <string>

namespace markamp::core
{

/// Per-file WYSIWYG state for session persistence.
struct WysiwygFileState
{
    WysiwygMode mode{WysiwygMode::kLivePreview};
    int cursor_line{0};
    int active_block_index{-1};
    double scroll_position{0.0};
    std::set<int> collapsed_blocks; ///< Indices of collapsed blocks
    std::string file_path;          ///< Associated file path

    /// Whether this is a default (unmodified) state.
    [[nodiscard]] auto is_default() const noexcept -> bool
    {
        return mode == WysiwygMode::kLivePreview && cursor_line == 0 && active_block_index == -1 &&
               scroll_position == 0.0 && collapsed_blocks.empty();
    }
};

/// Serializes and deserializes WYSIWYG file state.
class WysiwygStatePersistence
{
public:
    WysiwygStatePersistence() = default;

    /// Serialize state to a simple key=value string format.
    /// Format: "mode=1;cursor=42;block=3;scroll=0.75;collapsed=2,5,8;path=/a/b.md"
    [[nodiscard]] static auto serialize(const WysiwygFileState& state) -> std::string
    {
        std::ostringstream stream;
        stream << "mode=" << static_cast<int>(state.mode);
        stream << ";cursor=" << state.cursor_line;
        stream << ";block=" << state.active_block_index;
        stream << ";scroll=" << state.scroll_position;

        stream << ";collapsed=";
        bool first = true;
        for (int idx : state.collapsed_blocks)
        {
            if (!first)
            {
                stream << ',';
            }
            stream << idx;
            first = false;
        }

        if (!state.file_path.empty())
        {
            stream << ";path=" << state.file_path;
        }

        return stream.str();
    }

    /// Deserialize state from a serialized string.
    [[nodiscard]] static auto deserialize(const std::string& data) -> WysiwygFileState
    {
        WysiwygFileState state;

        if (data.empty())
        {
            return state;
        }

        // Split by semicolons
        std::istringstream stream(data);
        std::string token;
        while (std::getline(stream, token, ';'))
        {
            auto eq_pos = token.find('=');
            if (eq_pos == std::string::npos)
            {
                continue;
            }

            auto key = token.substr(0, eq_pos);
            auto value = token.substr(eq_pos + 1);

            if (key == "mode")
            {
                int mode_val = parse_int(value, 1);
                if (mode_val >= 0 && mode_val <= 2)
                {
                    state.mode = static_cast<WysiwygMode>(mode_val);
                }
            }
            else if (key == "cursor")
            {
                state.cursor_line = parse_int(value, 0);
            }
            else if (key == "block")
            {
                state.active_block_index = parse_int(value, -1);
            }
            else if (key == "scroll")
            {
                state.scroll_position = parse_double(value, 0.0);
            }
            else if (key == "collapsed")
            {
                state.collapsed_blocks = parse_int_set(value);
            }
            else if (key == "path")
            {
                state.file_path = value;
            }
        }

        return state;
    }

    /// Return a default state.
    [[nodiscard]] static auto default_state() -> WysiwygFileState
    {
        return WysiwygFileState{};
    }

    /// Serialize/deserialize round-trip validation.
    [[nodiscard]] static auto round_trip(const WysiwygFileState& state) -> WysiwygFileState
    {
        return deserialize(serialize(state));
    }

private:
    /// Parse an integer from string with a default value.
    [[nodiscard]] static auto parse_int(const std::string& str, int default_val) -> int
    {
        try
        {
            return std::stoi(str);
        }
        catch (...)
        {
            return default_val;
        }
    }

    /// Parse a double from string with a default value.
    [[nodiscard]] static auto parse_double(const std::string& str, double default_val) -> double
    {
        try
        {
            return std::stod(str);
        }
        catch (...)
        {
            return default_val;
        }
    }

    /// Parse a comma-separated list of integers into a set.
    [[nodiscard]] static auto parse_int_set(const std::string& str) -> std::set<int>
    {
        std::set<int> result;
        if (str.empty())
        {
            return result;
        }

        std::istringstream stream(str);
        std::string item;
        while (std::getline(stream, item, ','))
        {
            if (!item.empty())
            {
                try
                {
                    result.insert(std::stoi(item));
                }
                catch (...)
                {
                    // Skip invalid entries
                }
            }
        }

        return result;
    }
};

} // namespace markamp::core
