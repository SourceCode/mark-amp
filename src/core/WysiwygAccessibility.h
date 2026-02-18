#pragma once

/// @file WysiwygAccessibility.h
/// @brief V9 Phase 45 Task 17 – Accessibility metadata for WYSIWYG blocks.
///
/// Header-only, pure logic. Provides ARIA-like role, label, and description
/// metadata for WYSIWYG blocks to support assistive technologies.

#include "WysiwygTypes.h"

#include <string>

namespace markamp::core
{

/// Accessibility information for a WYSIWYG block.
struct BlockAccessibilityInfo
{
    std::string role;              ///< ARIA role (e.g. "heading", "img", "list", "table")
    std::string label;             ///< Short accessible label
    std::string description;       ///< Detailed description for screen readers
    int level{0};                  ///< Heading level (1–6), 0 for non-headings
    bool is_interactive{false};    ///< Whether the block supports click/keyboard interaction
    std::string keyboard_shortcut; ///< Keyboard shortcut hint (e.g. "Enter to edit")
};

/// Generates accessibility metadata for WYSIWYG blocks.
class WysiwygAccessibility
{
public:
    WysiwygAccessibility() = default;

    /// Get accessibility information for a WYSIWYG block.
    [[nodiscard]] static auto get_accessibility_info(const WysiwygBlock& block)
        -> BlockAccessibilityInfo
    {
        BlockAccessibilityInfo info;

        switch (block.block_type)
        {
            case WysiwygBlockType::kHeading:
            {
                info.role = "heading";
                auto level_it = block.metadata.find("level");
                if (level_it != block.metadata.end())
                {
                    try
                    {
                        info.level = std::stoi(level_it->second);
                    }
                    catch (...)
                    {
                        info.level = 1;
                    }
                }
                auto text_it = block.metadata.find("text");
                info.label = text_it != block.metadata.end() ? text_it->second : "Heading";
                info.description = "Heading level " + std::to_string(info.level);
                info.is_interactive = true;
                info.keyboard_shortcut = "Enter to edit, Escape to exit";
                break;
            }

            case WysiwygBlockType::kParagraph:
                info.role = "paragraph";
                info.label = truncate_label(block.raw_content, 60);
                info.description = "Text paragraph";
                info.is_interactive = true;
                info.keyboard_shortcut = "Enter to edit";
                break;

            case WysiwygBlockType::kCodeBlock:
            {
                info.role = "code";
                auto lang_it = block.metadata.find("language");
                auto lang = (lang_it != block.metadata.end() && !lang_it->second.empty())
                                ? lang_it->second
                                : "plain text";
                info.label = lang + " code block";
                auto lc_it = block.metadata.find("line_count");
                auto lc_str = lc_it != block.metadata.end() ? lc_it->second : "0";
                info.description = "Code block in " + lang + ", " + lc_str + " lines";
                info.is_interactive = true;
                info.keyboard_shortcut = "Enter to edit code, Escape to exit";
                break;
            }

            case WysiwygBlockType::kBlockquote:
                info.role = "blockquote";
                info.label = "Block quote";
                info.description = truncate_label(block.raw_content, 80);
                info.is_interactive = true;
                info.keyboard_shortcut = "Enter to edit";
                break;

            case WysiwygBlockType::kList:
                info.role = "list";
                info.label = "List";
                info.description =
                    "List with " + std::to_string(count_list_items(block.raw_content)) + " items";
                info.is_interactive = true;
                info.keyboard_shortcut = "Enter to edit, Space to toggle checkbox";
                break;

            case WysiwygBlockType::kTable:
                info.role = "table";
                info.label = "Table";
                info.description = "Data table";
                info.is_interactive = true;
                info.keyboard_shortcut = "Enter to edit, Tab to navigate cells";
                break;

            case WysiwygBlockType::kImage:
            {
                info.role = "img";
                auto alt_it = block.metadata.find("alt_text");
                info.label = alt_it != block.metadata.end() ? alt_it->second : "Image";
                auto url_it = block.metadata.find("url");
                info.description =
                    "Image" + (url_it != block.metadata.end() ? ": " + url_it->second : "");
                info.is_interactive = false;
                break;
            }

            case WysiwygBlockType::kCallout:
            {
                info.role = "note";
                auto type_it = block.metadata.find("type");
                auto type_str = type_it != block.metadata.end() ? type_it->second : "Note";
                info.label = type_str + " callout";
                info.description = type_str + " callout block";
                info.is_interactive = true;
                info.keyboard_shortcut = "Enter to edit, Space to collapse/expand";
                break;
            }

            case WysiwygBlockType::kMath:
                info.role = "math";
                info.label = "Math expression";
                info.description = "Mathematical formula (display mode)";
                info.is_interactive = true;
                info.keyboard_shortcut = "Enter to edit LaTeX";
                break;

            case WysiwygBlockType::kFrontmatter:
                info.role = "complementary";
                info.label = "Document frontmatter";
                info.description = "YAML metadata for this document";
                info.is_interactive = true;
                info.keyboard_shortcut = "Enter to edit, Space to collapse/expand";
                break;

            case WysiwygBlockType::kHtmlBlock:
                info.role = "region";
                info.label = "HTML block";
                info.description = "Raw HTML content";
                info.is_interactive = true;
                info.keyboard_shortcut = "Enter to edit HTML";
                break;

            case WysiwygBlockType::kThematicBreak:
                info.role = "separator";
                info.label = "Horizontal rule";
                info.description = "Thematic break / horizontal rule";
                info.is_interactive = false;
                break;
        }

        return info;
    }

    /// Generate an announcement string for mode switching (for screen readers).
    [[nodiscard]] static auto announce_mode_switch(WysiwygMode old_mode, WysiwygMode new_mode)
        -> std::string
    {
        return "Switched from " + mode_name(old_mode) + " to " + mode_name(new_mode) + " mode";
    }

    /// Get human-readable mode name.
    [[nodiscard]] static auto mode_name(WysiwygMode mode) -> std::string
    {
        switch (mode)
        {
            case WysiwygMode::kSource:
                return "Source";
            case WysiwygMode::kLivePreview:
                return "Live Preview";
            case WysiwygMode::kWysiwyg:
                return "WYSIWYG";
        }
        return "Unknown";
    }

private:
    /// Truncate a string for use as an accessible label.
    [[nodiscard]] static auto truncate_label(const std::string& text, std::size_t max_len)
        -> std::string
    {
        if (text.size() <= max_len)
        {
            return text;
        }
        return text.substr(0, max_len - 3) + "...";
    }

    /// Count list items in raw content.
    [[nodiscard]] static auto count_list_items(const std::string& content) -> int
    {
        int count = 0;
        std::size_t pos = 0;
        while (pos < content.size())
        {
            // Find start of line
            if (pos == 0 || content[pos - 1] == '\n')
            {
                // Skip leading whitespace
                auto line_start = pos;
                while (line_start < content.size() &&
                       (content[line_start] == ' ' || content[line_start] == '\t'))
                {
                    ++line_start;
                }
                if (line_start < content.size())
                {
                    const char chr = content[line_start];
                    if ((chr == '-' || chr == '*' || chr == '+') &&
                        line_start + 1 < content.size() && content[line_start + 1] == ' ')
                    {
                        ++count;
                    }
                    else if (std::isdigit(static_cast<unsigned char>(chr)) != 0)
                    {
                        ++count;
                    }
                }
            }
            auto next_newline = content.find('\n', pos);
            if (next_newline == std::string::npos)
            {
                break;
            }
            pos = next_newline + 1;
        }
        return count;
    }
};

} // namespace markamp::core
