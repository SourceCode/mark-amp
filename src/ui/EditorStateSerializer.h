#pragma once

#include <sstream>
#include <string>

namespace markamp::ui
{

/// Complete editor session state for serialization/deserialization.
struct EditorSessionSnapshot
{
    // Core state
    int cursor_position{0};
    int first_visible_line{0};
    int wrap_mode{1}; // 0=None, 1=Word, 2=Character

    // Configuration flags
    bool show_line_numbers{true};
    bool bracket_matching{true};
    bool auto_indent{true};
    bool code_folding{true};
    bool indentation_guides{true};
    bool show_whitespace{false};
    bool relative_line_numbers{false};
    bool bracket_pair_colorization{false};
    bool inline_diagnostics{true};
    bool minimap_visible{false};
    bool sticky_scroll{false};

    // Numeric settings
    int font_size{13};
    int tab_size{4};
    int edge_column{80};
    int zoom_level{0};
    int productivity_mode{0}; // 0=Writing, 1=Review, 2=Refactor

    // Font
    std::string font_family{"Menlo"};
};

/// Serializes and deserializes editor state to/from JSON strings.
///
/// This is a standalone utility that does not depend on wxWidgets,
/// making it testable without a GUI context.
class EditorStateSerializer
{
public:
    /// Serialize a snapshot to a JSON string.
    [[nodiscard]] static auto serialize(const EditorSessionSnapshot& snapshot) -> std::string
    {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"cursor_position\": " << snapshot.cursor_position << ",\n";
        oss << "  \"first_visible_line\": " << snapshot.first_visible_line << ",\n";
        oss << "  \"wrap_mode\": " << snapshot.wrap_mode << ",\n";
        oss << "  \"show_line_numbers\": " << bool_str(snapshot.show_line_numbers) << ",\n";
        oss << "  \"bracket_matching\": " << bool_str(snapshot.bracket_matching) << ",\n";
        oss << "  \"auto_indent\": " << bool_str(snapshot.auto_indent) << ",\n";
        oss << "  \"code_folding\": " << bool_str(snapshot.code_folding) << ",\n";
        oss << "  \"indentation_guides\": " << bool_str(snapshot.indentation_guides) << ",\n";
        oss << "  \"show_whitespace\": " << bool_str(snapshot.show_whitespace) << ",\n";
        oss << "  \"relative_line_numbers\": " << bool_str(snapshot.relative_line_numbers) << ",\n";
        oss << "  \"bracket_pair_colorization\": " << bool_str(snapshot.bracket_pair_colorization)
            << ",\n";
        oss << "  \"inline_diagnostics\": " << bool_str(snapshot.inline_diagnostics) << ",\n";
        oss << "  \"minimap_visible\": " << bool_str(snapshot.minimap_visible) << ",\n";
        oss << "  \"sticky_scroll\": " << bool_str(snapshot.sticky_scroll) << ",\n";
        oss << "  \"font_size\": " << snapshot.font_size << ",\n";
        oss << "  \"tab_size\": " << snapshot.tab_size << ",\n";
        oss << "  \"edge_column\": " << snapshot.edge_column << ",\n";
        oss << "  \"zoom_level\": " << snapshot.zoom_level << ",\n";
        oss << "  \"productivity_mode\": " << snapshot.productivity_mode << ",\n";
        oss << "  \"font_family\": \"" << snapshot.font_family << "\"\n";
        oss << "}";
        return oss.str();
    }

    /// Deserialize a JSON string to a snapshot.
    [[nodiscard]] static auto deserialize(const std::string& json) -> EditorSessionSnapshot
    {
        EditorSessionSnapshot snap;

        snap.cursor_position = parse_int(json, "cursor_position", 0);
        snap.first_visible_line = parse_int(json, "first_visible_line", 0);
        snap.wrap_mode = parse_int(json, "wrap_mode", 1);
        snap.show_line_numbers = parse_bool(json, "show_line_numbers", true);
        snap.bracket_matching = parse_bool(json, "bracket_matching", true);
        snap.auto_indent = parse_bool(json, "auto_indent", true);
        snap.code_folding = parse_bool(json, "code_folding", true);
        snap.indentation_guides = parse_bool(json, "indentation_guides", true);
        snap.show_whitespace = parse_bool(json, "show_whitespace", false);
        snap.relative_line_numbers = parse_bool(json, "relative_line_numbers", false);
        snap.bracket_pair_colorization = parse_bool(json, "bracket_pair_colorization", false);
        snap.inline_diagnostics = parse_bool(json, "inline_diagnostics", true);
        snap.minimap_visible = parse_bool(json, "minimap_visible", false);
        snap.sticky_scroll = parse_bool(json, "sticky_scroll", false);
        snap.font_size = parse_int(json, "font_size", 13);
        snap.tab_size = parse_int(json, "tab_size", 4);
        snap.edge_column = parse_int(json, "edge_column", 80);
        snap.zoom_level = parse_int(json, "zoom_level", 0);
        snap.productivity_mode = parse_int(json, "productivity_mode", 0);
        snap.font_family = parse_string(json, "font_family", "Menlo");

        return snap;
    }

private:
    static auto bool_str(bool val) -> const char*
    {
        return val ? "true" : "false";
    }

    static auto parse_int(const std::string& json, const std::string& key, int default_val) -> int
    {
        const std::string kSearchKey = "\"" + key + "\":";
        auto pos = json.find(kSearchKey);
        if (pos == std::string::npos)
        {
            // Try with a space before the colon
            const std::string kAltKey = "\"" + key + "\": ";
            pos = json.find(kAltKey);
            if (pos == std::string::npos)
            {
                return default_val;
            }
            pos += kAltKey.size();
        }
        else
        {
            pos += kSearchKey.size();
        }

        // Skip whitespace
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t'))
        {
            ++pos;
        }

        // Parse integer (handle negative)
        std::string num_str;
        while (pos < json.size() && (std::isdigit(json[pos]) != 0 || json[pos] == '-'))
        {
            num_str += json[pos];
            ++pos;
        }

        if (num_str.empty())
        {
            return default_val;
        }

        try
        {
            return std::stoi(num_str);
        }
        catch (...)
        {
            return default_val;
        }
    }

    static auto parse_bool(const std::string& json, const std::string& key, bool default_val)
        -> bool
    {
        const std::string kSearchKey = "\"" + key + "\":";
        auto pos = json.find(kSearchKey);
        if (pos == std::string::npos)
        {
            const std::string kAltKey = "\"" + key + "\": ";
            pos = json.find(kAltKey);
            if (pos == std::string::npos)
            {
                return default_val;
            }
            pos += kAltKey.size();
        }
        else
        {
            pos += kSearchKey.size();
        }

        // Skip whitespace
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t'))
        {
            ++pos;
        }

        if (pos + 4 <= json.size() && json.substr(pos, 4) == "true")
        {
            return true;
        }
        if (pos + 5 <= json.size() && json.substr(pos, 5) == "false")
        {
            return false;
        }

        return default_val;
    }

    static auto parse_string(const std::string& json,
                             const std::string& key,
                             const std::string& default_val) -> std::string
    {
        const std::string kSearchKey = "\"" + key + "\":";
        auto pos = json.find(kSearchKey);
        if (pos == std::string::npos)
        {
            const std::string kAltKey = "\"" + key + "\": ";
            pos = json.find(kAltKey);
            if (pos == std::string::npos)
            {
                return default_val;
            }
            pos += kAltKey.size();
        }
        else
        {
            pos += kSearchKey.size();
        }

        // Skip whitespace
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t'))
        {
            ++pos;
        }

        // Expect opening quote
        if (pos >= json.size() || json[pos] != '"')
        {
            return default_val;
        }
        ++pos;

        // Read until closing quote
        std::string result;
        while (pos < json.size() && json[pos] != '"')
        {
            result += json[pos];
            ++pos;
        }

        return result;
    }
};

} // namespace markamp::ui
