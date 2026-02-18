#pragma once

/// @file InlineFrontmatterRenderer.h
/// @brief V9 Phase 45 Task 13 – Frontmatter rendering metadata and summary generation.
///
/// Header-only, pure logic. Parses YAML frontmatter from markdown documents
/// and provides rendering metadata for WYSIWYG display.

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Rendering metadata for a frontmatter block.
struct FrontmatterRenderInfo
{
    std::unordered_map<std::string, std::string> fields; ///< Key-value pairs
    std::vector<std::string> field_order;                ///< Keys in order of appearance
    std::string raw_yaml;                                ///< Raw YAML text (without delimiters)
    int line_count{0};                                   ///< Total lines including delimiters
    int start_line{0};                                   ///< Line of opening ---
    int end_line{0};                                     ///< Line of closing ---
    bool is_collapsed{true};                             ///< Default to collapsed in WYSIWYG
    bool is_valid{false};                                ///< Whether this is valid frontmatter

    /// Whether the frontmatter has a specific field.
    [[nodiscard]] auto has_field(const std::string& key) const -> bool
    {
        return fields.find(key) != fields.end();
    }

    /// Number of fields.
    [[nodiscard]] auto field_count() const noexcept -> int
    {
        return static_cast<int>(fields.size());
    }
};

/// Parses YAML frontmatter and provides rendering metadata.
class InlineFrontmatterRenderer
{
public:
    InlineFrontmatterRenderer() = default;

    /// Parse frontmatter from the beginning of a document (as lines).
    [[nodiscard]] static auto parse_frontmatter(const std::vector<std::string>& lines)
        -> FrontmatterRenderInfo
    {
        FrontmatterRenderInfo info;

        if (lines.empty() || lines[0] != "---")
        {
            return info;
        }

        // Find closing ---
        int close_idx = -1;
        for (std::size_t idx = 1; idx < lines.size(); ++idx)
        {
            if (lines[idx] == "---" || lines[idx] == "...")
            {
                close_idx = static_cast<int>(idx);
                break;
            }
        }

        if (close_idx < 0)
        {
            return info;
        }

        info.start_line = 0;
        info.end_line = close_idx;
        info.line_count = close_idx + 1;

        // Extract raw YAML (between delimiters)
        std::string raw;
        for (int idx = 1; idx < close_idx; ++idx)
        {
            if (idx > 1)
            {
                raw += '\n';
            }
            raw += lines[static_cast<std::size_t>(idx)];
        }
        info.raw_yaml = raw;

        // Parse simple key: value pairs (basic YAML subset)
        parse_yaml_fields(lines, 1, close_idx, info);

        info.is_valid = true;
        return info;
    }

    /// Parse frontmatter from a raw string.
    [[nodiscard]] static auto parse_frontmatter_string(const std::string& content)
        -> FrontmatterRenderInfo
    {
        std::vector<std::string> lines;
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line))
        {
            lines.push_back(line);
        }
        return parse_frontmatter(lines);
    }

    /// Extract a specific field value from frontmatter.
    [[nodiscard]] static auto extract_field(const FrontmatterRenderInfo& info,
                                            const std::string& key) -> std::string
    {
        auto iter = info.fields.find(key);
        if (iter != info.fields.end())
        {
            return iter->second;
        }
        return "";
    }

    /// Generate a compact summary string for collapsed frontmatter display.
    /// Example: "title: My Doc | tags: 3 | date: 2024-01-01"
    [[nodiscard]] static auto render_summary(const FrontmatterRenderInfo& info, int max_fields = 4)
        -> std::string
    {
        if (!info.is_valid || info.fields.empty())
        {
            return "Empty frontmatter";
        }

        std::string summary;
        int count = 0;

        // Use field_order to maintain appearance order
        for (const auto& key : info.field_order)
        {
            if (count >= max_fields)
            {
                auto remaining = static_cast<int>(info.field_order.size()) - count;
                summary += " | +" + std::to_string(remaining) + " more";
                break;
            }

            if (!summary.empty())
            {
                summary += " | ";
            }

            auto iter = info.fields.find(key);
            if (iter != info.fields.end())
            {
                auto value = iter->second;
                // Shorten long values
                if (value.size() > 30)
                {
                    value = value.substr(0, 27) + "...";
                }
                // Count array items: [item1, item2, ...]
                if (!value.empty() && value[0] == '[')
                {
                    int items = count_array_items(value);
                    value = std::to_string(items) + " items";
                }
                summary += key + ": " + value;
            }

            ++count;
        }

        return summary;
    }

    /// Check if lines start with frontmatter.
    [[nodiscard]] static auto has_frontmatter(const std::vector<std::string>& lines) -> bool
    {
        if (lines.empty() || lines[0] != "---")
        {
            return false;
        }
        for (std::size_t idx = 1; idx < lines.size(); ++idx)
        {
            if (lines[idx] == "---" || lines[idx] == "...")
            {
                return true;
            }
        }
        return false;
    }

private:
    /// Parse simple YAML key: value pairs.
    static void parse_yaml_fields(const std::vector<std::string>& lines,
                                  int start,
                                  int end_exclusive,
                                  FrontmatterRenderInfo& info)
    {
        for (int idx = start; idx < end_exclusive; ++idx)
        {
            const auto& line = lines[static_cast<std::size_t>(idx)];

            // Skip empty lines and comments
            if (line.empty() || line[0] == '#')
            {
                continue;
            }

            // Skip indented lines (list items, nested values)
            if (line[0] == ' ' || line[0] == '\t')
            {
                // Append to previous key's value if it exists
                if (!info.field_order.empty())
                {
                    auto& last_key = info.field_order.back();
                    auto trimmed = line;
                    auto first_char = trimmed.find_first_not_of(" \t");
                    if (first_char != std::string::npos)
                    {
                        trimmed = trimmed.substr(first_char);
                    }
                    if (trimmed.size() >= 2 && trimmed[0] == '-' && trimmed[1] == ' ')
                    {
                        // List item: append to value as comma-separated
                        auto item = trimmed.substr(2);
                        auto& value = info.fields[last_key];
                        if (!value.empty())
                        {
                            if (value[0] != '[')
                            {
                                value = "[" + value;
                            }
                            value += ", " + item;
                        }
                        else
                        {
                            value = "[" + item;
                        }
                    }
                }
                continue;
            }

            // Parse key: value
            auto colon_pos = line.find(':');
            if (colon_pos == std::string::npos)
            {
                continue;
            }

            auto key = line.substr(0, colon_pos);
            auto value = (colon_pos + 1 < line.size()) ? line.substr(colon_pos + 1) : "";

            // Trim key and value
            while (!key.empty() && (key.back() == ' ' || key.back() == '\t'))
            {
                key.pop_back();
            }
            while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
            {
                value = value.substr(1);
            }
            // Remove quotes from value
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                      (value.front() == '\'' && value.back() == '\'')))
            {
                value = value.substr(1, value.size() - 2);
            }

            // Close any open arrays from previous keys
            if (!info.field_order.empty())
            {
                auto& prev_key = info.field_order.back();
                auto& prev_val = info.fields[prev_key];
                if (!prev_val.empty() && prev_val[0] == '[' && prev_val.back() != ']')
                {
                    prev_val += "]";
                }
            }

            if (!key.empty())
            {
                info.fields[key] = value;
                info.field_order.push_back(key);
            }
        }

        // Close any trailing open array
        if (!info.field_order.empty())
        {
            auto& last_key = info.field_order.back();
            auto& last_val = info.fields[last_key];
            if (!last_val.empty() && last_val[0] == '[' && last_val.back() != ']')
            {
                last_val += "]";
            }
        }
    }

    /// Count items in a bracket-delimited list: [a, b, c] → 3.
    [[nodiscard]] static auto count_array_items(const std::string& value) -> int
    {
        if (value.size() < 2)
        {
            return 0;
        }
        int count = 1;
        for (char chr : value)
        {
            if (chr == ',')
            {
                ++count;
            }
        }
        return count;
    }
};

} // namespace markamp::core
