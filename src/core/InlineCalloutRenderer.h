#pragma once

/// @file InlineCalloutRenderer.h
/// @brief V9 Phase 45 Task 10 – Callout/admonition detection and metadata extraction.
///
/// Header-only, pure logic. Parses GitHub-style callouts (> [!NOTE], > [!WARNING], etc.)
/// and extracts type, title, body, and rendering hints.

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Callout type classification.
enum class CalloutType : std::uint8_t
{
    kNote = 0,
    kTip = 1,
    kImportant = 2,
    kWarning = 3,
    kCaution = 4,
    kCustom = 5
};

/// Information extracted from a callout/admonition block.
struct CalloutInfo
{
    CalloutType type{CalloutType::kNote};
    std::string type_string;    ///< Raw type string (e.g. "NOTE", "WARNING", "CUSTOM")
    std::string title;          ///< Custom title (if provided after the type)
    std::string body;           ///< Body text of the callout
    std::string icon;           ///< Suggested icon name (e.g. "info", "warning", "alert")
    std::string color_hint;     ///< Suggested CSS color class (e.g. "blue", "yellow", "red")
    int start_line{0};          ///< First line of the callout
    int end_line{0};            ///< Last line of the callout
    bool is_collapsible{false}; ///< Whether the callout can be collapsed (> [!NOTE]-)
    bool is_collapsed{false};   ///< Initial collapsed state
    bool is_valid{false};       ///< Whether this is a valid callout

    /// Whether the callout has a custom title.
    [[nodiscard]] auto has_custom_title() const noexcept -> bool
    {
        return !title.empty();
    }
};

/// Parses GitHub-style callout blocks and extracts rendering metadata.
class InlineCalloutRenderer
{
public:
    InlineCalloutRenderer() = default;

    /// Check if a line starts a callout block.
    [[nodiscard]] static auto is_callout_start(const std::string& line) -> bool
    {
        // Must start with > [!
        auto trimmed = trim_leading_space(line);
        if (trimmed.size() < 6)
        {
            return false;
        }

        return trimmed[0] == '>' && trimmed.find("[!") != std::string::npos;
    }

    /// Parse a callout block starting at the given line index.
    [[nodiscard]] static auto parse_callout(const std::vector<std::string>& lines, int start_line)
        -> CalloutInfo
    {
        CalloutInfo info;
        info.start_line = start_line;

        const auto total = static_cast<int>(lines.size());
        if (start_line < 0 || start_line >= total)
        {
            return info;
        }

        const auto& first_line = lines[static_cast<std::size_t>(start_line)];

        // Parse the type marker: > [!TYPE]
        auto bracket_start = first_line.find("[!");
        if (bracket_start == std::string::npos)
        {
            return info;
        }

        auto bracket_end = first_line.find(']', bracket_start);
        if (bracket_end == std::string::npos)
        {
            return info;
        }

        // Extract type string
        info.type_string = first_line.substr(bracket_start + 2, bracket_end - bracket_start - 2);
        info.type = classify_type(info.type_string);

        // Check for collapsible marker: > [!NOTE]-
        if (bracket_end + 1 < first_line.size() && first_line[bracket_end + 1] == '-')
        {
            info.is_collapsible = true;
            info.is_collapsed = true;
        }
        else if (bracket_end + 1 < first_line.size() && first_line[bracket_end + 1] == '+')
        {
            info.is_collapsible = true;
            info.is_collapsed = false;
        }

        // Extract optional custom title after the bracket
        std::size_t title_start = bracket_end + 1;
        if (info.is_collapsible)
        {
            ++title_start;
        }
        if (title_start < first_line.size())
        {
            auto title_text = first_line.substr(title_start);
            auto trimmed = trim_leading_space(title_text);
            if (!trimmed.empty())
            {
                info.title = trimmed;
            }
        }

        // Set icon and color based on type
        set_type_metadata(info);

        // Collect body lines (subsequent > lines)
        std::string body_text;
        int end_idx = start_line;
        for (int idx = start_line + 1; idx < total; ++idx)
        {
            const auto& current = lines[static_cast<std::size_t>(idx)];
            if (current.empty() || current[0] != '>')
            {
                break;
            }
            end_idx = idx;

            // Strip leading > and optional space
            std::string content = current.substr(1);
            if (!content.empty() && content[0] == ' ')
            {
                content = content.substr(1);
            }

            if (!body_text.empty())
            {
                body_text += '\n';
            }
            body_text += content;
        }

        info.body = body_text;
        info.end_line = end_idx;
        info.is_valid = true;

        return info;
    }

    /// Get the display name for a callout type.
    [[nodiscard]] static auto type_display_name(CalloutType callout_type) -> std::string
    {
        switch (callout_type)
        {
            case CalloutType::kNote:
                return "Note";
            case CalloutType::kTip:
                return "Tip";
            case CalloutType::kImportant:
                return "Important";
            case CalloutType::kWarning:
                return "Warning";
            case CalloutType::kCaution:
                return "Caution";
            case CalloutType::kCustom:
                return "Note";
        }
        return "Note";
    }

    /// Get the callout type from a raw type string.
    [[nodiscard]] static auto get_callout_type(const std::string& line) -> std::string
    {
        auto bracket_start = line.find("[!");
        if (bracket_start == std::string::npos)
        {
            return "";
        }

        auto bracket_end = line.find(']', bracket_start);
        if (bracket_end == std::string::npos)
        {
            return "";
        }

        return line.substr(bracket_start + 2, bracket_end - bracket_start - 2);
    }

private:
    /// Classify a type string into a CalloutType enum.
    [[nodiscard]] static auto classify_type(const std::string& type_str) -> CalloutType
    {
        auto upper = to_upper(type_str);
        if (upper == "NOTE" || upper == "INFO")
        {
            return CalloutType::kNote;
        }
        if (upper == "TIP" || upper == "HINT")
        {
            return CalloutType::kTip;
        }
        if (upper == "IMPORTANT" || upper == "ATTENTION")
        {
            return CalloutType::kImportant;
        }
        if (upper == "WARNING" || upper == "WARN")
        {
            return CalloutType::kWarning;
        }
        if (upper == "CAUTION" || upper == "DANGER" || upper == "ERROR")
        {
            return CalloutType::kCaution;
        }
        return CalloutType::kCustom;
    }

    /// Set icon and color_hint based on callout type.
    static void set_type_metadata(CalloutInfo& info)
    {
        switch (info.type)
        {
            case CalloutType::kNote:
                info.icon = "info-circle";
                info.color_hint = "blue";
                break;
            case CalloutType::kTip:
                info.icon = "lightbulb";
                info.color_hint = "green";
                break;
            case CalloutType::kImportant:
                info.icon = "exclamation-circle";
                info.color_hint = "purple";
                break;
            case CalloutType::kWarning:
                info.icon = "exclamation-triangle";
                info.color_hint = "yellow";
                break;
            case CalloutType::kCaution:
                info.icon = "shield-exclamation";
                info.color_hint = "red";
                break;
            case CalloutType::kCustom:
                info.icon = "bookmark";
                info.color_hint = "gray";
                break;
        }
    }

    /// Convert string to uppercase.
    [[nodiscard]] static auto to_upper(const std::string& str) -> std::string
    {
        std::string result = str;
        std::transform(result.begin(),
                       result.end(),
                       result.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::toupper(chr)); });
        return result;
    }

    /// Trim leading whitespace.
    [[nodiscard]] static auto trim_leading_space(const std::string& str) -> std::string
    {
        auto pos = str.find_first_not_of(" \t");
        if (pos == std::string::npos)
        {
            return "";
        }
        return str.substr(pos);
    }
};

} // namespace markamp::core
