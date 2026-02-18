#pragma once

/// @file WysiwygTypes.h
/// @brief V9 Phase 45 – WYSIWYG types: modes, render config, block model, and document model.
///
/// All types are header-only, pure logic with no wxWidgets dependencies.

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Task 1: Enums and configuration
// ============================================================================

/// WYSIWYG editing mode — determines how content is presented and edited.
enum class WysiwygMode : std::uint8_t
{
    kSource = 0,      ///< Raw markdown source editing (traditional editor)
    kLivePreview = 1, ///< Live preview with cursor-aware syntax folding
    kWysiwyg = 2      ///< Full WYSIWYG: rendered inline with block-level editing
};

/// Block-level element types recognized in a markdown document.
enum class WysiwygBlockType : std::uint8_t
{
    kParagraph = 0,
    kHeading = 1,
    kCodeBlock = 2,
    kBlockquote = 3,
    kList = 4,
    kTable = 5,
    kImage = 6,
    kCallout = 7,
    kMath = 8,
    kFrontmatter = 9,
    kHtmlBlock = 10,
    kThematicBreak = 11
};

/// Configuration flags controlling which elements render inline in WYSIWYG mode.
struct WysiwygRenderConfig
{
    bool show_markers{false};        ///< Show raw markdown syntax markers
    bool fold_inactive_syntax{true}; ///< Fold syntax on non-cursor lines
    bool render_images_inline{true}; ///< Show image thumbnails inline
    bool render_checkboxes{true};    ///< Show interactive checkboxes
    bool render_tables_inline{true}; ///< Render tables as formatted grids
    bool render_math_inline{true};   ///< Render LaTeX math inline
    bool render_callouts{true};      ///< Render callout boxes with icons
    bool render_code_blocks{true};   ///< Render code blocks with syntax highlight
};

// ============================================================================
// Task 2: Block and Document model
// ============================================================================

/// A single block-level element within the WYSIWYG document model.
struct WysiwygBlock
{
    WysiwygBlockType block_type{WysiwygBlockType::kParagraph};
    int start_line{0};         ///< First line of this block (0-based)
    int end_line{0};           ///< Last line of this block (0-based, inclusive)
    std::string raw_content;   ///< Original markdown source
    std::string rendered_html; ///< Pre-rendered HTML (if applicable)
    bool is_editable{true};    ///< Whether the block supports inline editing
    bool is_selected{false};   ///< Whether the block is currently selected
    bool is_active{false};     ///< Whether the cursor is inside this block

    /// Metadata map for type-specific properties:
    ///   Heading: "level" → "1"-"6"
    ///   CodeBlock: "language" → "python", "line_count" → "42"
    ///   Image: "alt_text" → "Photo", "url" → "img.png"
    ///   Callout: "type" → "NOTE", "title" → "Important"
    ///   Math: "display" → "true"/"false"
    ///   Frontmatter: "format" → "yaml"
    std::unordered_map<std::string, std::string> metadata;

    /// Convenience: number of lines in this block.
    [[nodiscard]] auto line_count() const noexcept -> int
    {
        return end_line - start_line + 1;
    }
};

/// Document model decomposing markdown into a sequence of typed blocks.
/// Pure logic — no UI dependencies.
class WysiwygDocumentModel
{
public:
    WysiwygDocumentModel() = default;

    /// Parse markdown content into blocks.
    void parse(const std::string& markdown)
    {
        blocks_.clear();
        if (markdown.empty())
        {
            return;
        }

        // Split into lines
        std::vector<std::string> lines;
        std::istringstream stream(markdown);
        std::string line;
        while (std::getline(stream, line))
        {
            lines.push_back(line);
        }

        parse_lines(lines);
    }

    /// Get all parsed blocks.
    [[nodiscard]] auto blocks() const noexcept -> const std::vector<WysiwygBlock>&
    {
        return blocks_;
    }

    /// Mutable access to blocks.
    [[nodiscard]] auto blocks_mut() noexcept -> std::vector<WysiwygBlock>&
    {
        return blocks_;
    }

    /// Find block containing the given line number (0-based).
    /// Returns nullptr if line is out of range.
    [[nodiscard]] auto get_block_at_line(int line_number) const -> const WysiwygBlock*
    {
        for (const auto& block : blocks_)
        {
            if (line_number >= block.start_line && line_number <= block.end_line)
            {
                return &block;
            }
        }
        return nullptr;
    }

    /// Mutable version of get_block_at_line.
    [[nodiscard]] auto get_block_at_line_mut(int line_number) -> WysiwygBlock*
    {
        for (auto& block : blocks_)
        {
            if (line_number >= block.start_line && line_number <= block.end_line)
            {
                return &block;
            }
        }
        return nullptr;
    }

    /// Get block by index.
    [[nodiscard]] auto get_block(int index) const -> const WysiwygBlock*
    {
        if (index < 0 || index >= static_cast<int>(blocks_.size()))
        {
            return nullptr;
        }
        return &blocks_[static_cast<std::size_t>(index)];
    }

    /// Mutable version of get_block.
    [[nodiscard]] auto get_block_mut(int index) -> WysiwygBlock*
    {
        if (index < 0 || index >= static_cast<int>(blocks_.size()))
        {
            return nullptr;
        }
        return &blocks_[static_cast<std::size_t>(index)];
    }

    /// Replace a block's raw content and re-classify it.
    auto replace_block(int index, const std::string& new_content) -> bool
    {
        if (index < 0 || index >= static_cast<int>(blocks_.size()))
        {
            return false;
        }
        auto& block = blocks_[static_cast<std::size_t>(index)];
        block.raw_content = new_content;
        block.block_type = classify_block(new_content);
        block.metadata = extract_metadata(new_content, block.block_type);
        return true;
    }

    /// Insert a new block after the given index.
    auto insert_block_after(int index, const std::string& content) -> bool
    {
        if (index < -1 || index >= static_cast<int>(blocks_.size()))
        {
            return false;
        }
        WysiwygBlock block;
        block.raw_content = content;
        block.block_type = classify_block(content);
        block.metadata = extract_metadata(content, block.block_type);

        auto insert_pos = static_cast<std::size_t>(index + 1);
        if (insert_pos <= blocks_.size())
        {
            // Compute line numbers based on previous block
            if (index >= 0)
            {
                block.start_line = blocks_[static_cast<std::size_t>(index)].end_line + 1;
            }
            else
            {
                block.start_line = 0;
            }
            // Count lines in content
            int line_count = 1;
            for (char chr : content)
            {
                if (chr == '\n')
                {
                    ++line_count;
                }
            }
            block.end_line = block.start_line + line_count - 1;

            blocks_.insert(blocks_.begin() + static_cast<std::ptrdiff_t>(insert_pos), block);
            recompute_line_numbers();
        }
        return true;
    }

    /// Remove a block by index.
    auto remove_block(int index) -> bool
    {
        if (index < 0 || index >= static_cast<int>(blocks_.size()))
        {
            return false;
        }
        blocks_.erase(blocks_.begin() + static_cast<std::ptrdiff_t>(index));
        recompute_line_numbers();
        return true;
    }

    /// Reconstruct the full markdown from all blocks.
    [[nodiscard]] auto to_markdown() const -> std::string
    {
        std::string result;
        for (std::size_t idx = 0; idx < blocks_.size(); ++idx)
        {
            if (idx > 0)
            {
                result += '\n';
            }
            result += blocks_[idx].raw_content;
        }
        return result;
    }

    /// Number of blocks.
    [[nodiscard]] auto block_count() const noexcept -> int
    {
        return static_cast<int>(blocks_.size());
    }

    /// Total number of lines across all blocks.
    [[nodiscard]] auto total_lines() const noexcept -> int
    {
        if (blocks_.empty())
        {
            return 0;
        }
        return blocks_.back().end_line + 1;
    }

private:
    std::vector<WysiwygBlock> blocks_;

    /// Classify a block of text into its type.
    [[nodiscard]] static auto classify_block(const std::string& content) -> WysiwygBlockType
    {
        if (content.empty())
        {
            return WysiwygBlockType::kParagraph;
        }

        // Frontmatter (only at document start, but classify by content)
        if (content.size() >= 3 && content.substr(0, 3) == "---")
        {
            return WysiwygBlockType::kFrontmatter;
        }

        // Heading
        if (!content.empty() && content[0] == '#')
        {
            int level = 0;
            for (char chr : content)
            {
                if (chr == '#')
                {
                    ++level;
                }
                else
                {
                    break;
                }
            }
            if (level >= 1 && level <= 6 &&
                (static_cast<std::size_t>(level) >= content.size() ||
                 content[static_cast<std::size_t>(level)] == ' '))
            {
                return WysiwygBlockType::kHeading;
            }
        }

        // Code fence
        if (content.size() >= 3 && content.substr(0, 3) == "```")
        {
            return WysiwygBlockType::kCodeBlock;
        }

        // Blockquote
        if (!content.empty() && content[0] == '>')
        {
            // Check for callout
            if (content.find("> [!") != std::string::npos)
            {
                return WysiwygBlockType::kCallout;
            }
            return WysiwygBlockType::kBlockquote;
        }

        // List (unordered or ordered)
        if (content.size() >= 2)
        {
            const char first = content[0];
            if ((first == '-' || first == '*' || first == '+') && content[1] == ' ')
            {
                return WysiwygBlockType::kList;
            }
            // Ordered list: digit(s) followed by . or )
            if (std::isdigit(static_cast<unsigned char>(first)) != 0)
            {
                auto dot_pos = content.find(". ");
                auto paren_pos = content.find(") ");
                if (dot_pos != std::string::npos && dot_pos < 4)
                {
                    return WysiwygBlockType::kList;
                }
                if (paren_pos != std::string::npos && paren_pos < 4)
                {
                    return WysiwygBlockType::kList;
                }
            }
        }

        // Table
        if (!content.empty() && content[0] == '|')
        {
            return WysiwygBlockType::kTable;
        }

        // Image (standalone image line)
        if (content.size() >= 4 && content.substr(0, 2) == "![")
        {
            return WysiwygBlockType::kImage;
        }

        // Math display block
        if (content.size() >= 2 && content.substr(0, 2) == "$$")
        {
            return WysiwygBlockType::kMath;
        }

        // HTML block
        if (!content.empty() && content[0] == '<')
        {
            return WysiwygBlockType::kHtmlBlock;
        }

        // Thematic break
        if (content.size() >= 3)
        {
            auto trimmed = content;
            trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), ' '), trimmed.end());
            if (trimmed == "---" || trimmed == "***" || trimmed == "___")
            {
                return WysiwygBlockType::kThematicBreak;
            }
        }

        return WysiwygBlockType::kParagraph;
    }

    /// Extract type-specific metadata from block content.
    [[nodiscard]] static auto extract_metadata(const std::string& content,
                                               WysiwygBlockType block_type)
        -> std::unordered_map<std::string, std::string>
    {
        std::unordered_map<std::string, std::string> meta;

        switch (block_type)
        {
            case WysiwygBlockType::kHeading:
            {
                int level = 0;
                for (char chr : content)
                {
                    if (chr == '#')
                    {
                        ++level;
                    }
                    else
                    {
                        break;
                    }
                }
                meta["level"] = std::to_string(level);
                // Extract heading text
                auto text_start = content.find(' ');
                if (text_start != std::string::npos)
                {
                    meta["text"] = content.substr(text_start + 1);
                }
                break;
            }

            case WysiwygBlockType::kCodeBlock:
            {
                // Extract language from opening fence: ```python
                if (content.size() > 3)
                {
                    auto first_newline = content.find('\n');
                    auto fence_line = content.substr(
                        3,
                        first_newline != std::string::npos ? first_newline - 3 : std::string::npos);
                    // Trim whitespace
                    auto lang_start = fence_line.find_first_not_of(" \t");
                    if (lang_start != std::string::npos)
                    {
                        meta["language"] = fence_line.substr(lang_start);
                    }
                }
                // Count lines
                int line_ct = 0;
                for (char chr : content)
                {
                    if (chr == '\n')
                    {
                        ++line_ct;
                    }
                }
                meta["line_count"] = std::to_string(line_ct + 1);
                break;
            }

            case WysiwygBlockType::kImage:
            {
                // Parse ![alt](url)
                auto bracket_close = content.find("](");
                if (bracket_close != std::string::npos && content.size() >= 4)
                {
                    meta["alt_text"] = content.substr(2, bracket_close - 2);
                    auto url_start = bracket_close + 2;
                    auto url_end = content.find(')', url_start);
                    if (url_end != std::string::npos)
                    {
                        meta["url"] = content.substr(url_start, url_end - url_start);
                    }
                }
                break;
            }

            case WysiwygBlockType::kCallout:
            {
                // Parse > [!TYPE]
                auto type_start = content.find("[!");
                auto type_end = content.find(']', type_start != std::string::npos ? type_start : 0);
                if (type_start != std::string::npos && type_end != std::string::npos)
                {
                    meta["type"] = content.substr(type_start + 2, type_end - type_start - 2);
                }
                break;
            }

            case WysiwygBlockType::kMath:
                meta["display"] = "true";
                break;

            case WysiwygBlockType::kFrontmatter:
                meta["format"] = "yaml";
                break;

            default:
                break;
        }

        return meta;
    }

    /// Parse lines into blocks by detecting block boundaries.
    void parse_lines(const std::vector<std::string>& lines)
    {
        int idx = 0;
        int total = static_cast<int>(lines.size());

        // Check for frontmatter at document start
        if (total > 0 && lines[0] == "---")
        {
            int fm_end = -1;
            for (int scan = 1; scan < total; ++scan)
            {
                if (lines[static_cast<std::size_t>(scan)] == "---")
                {
                    fm_end = scan;
                    break;
                }
            }
            if (fm_end > 0)
            {
                WysiwygBlock block;
                block.block_type = WysiwygBlockType::kFrontmatter;
                block.start_line = 0;
                block.end_line = fm_end;
                std::string content;
                for (int ln = 0; ln <= fm_end; ++ln)
                {
                    if (ln > 0)
                    {
                        content += '\n';
                    }
                    content += lines[static_cast<std::size_t>(ln)];
                }
                block.raw_content = content;
                block.metadata = extract_metadata(content, block.block_type);
                blocks_.push_back(std::move(block));
                idx = fm_end + 1;
            }
        }

        while (idx < total)
        {
            const auto& current = lines[static_cast<std::size_t>(idx)];

            // Skip blank lines between blocks
            if (current.empty())
            {
                ++idx;
                continue;
            }

            // Code fence block
            if (current.size() >= 3 && current.substr(0, 3) == "```")
            {
                int block_start = idx;
                ++idx;
                while (idx < total)
                {
                    if (lines[static_cast<std::size_t>(idx)].size() >= 3 &&
                        lines[static_cast<std::size_t>(idx)].substr(0, 3) == "```")
                    {
                        break;
                    }
                    ++idx;
                }
                int block_end = std::min(idx, total - 1);
                add_block(lines, block_start, block_end);
                ++idx;
                continue;
            }

            // Math display block $$
            if (current.size() >= 2 && current.substr(0, 2) == "$$")
            {
                int block_start = idx;
                ++idx;
                while (idx < total)
                {
                    if (lines[static_cast<std::size_t>(idx)].size() >= 2 &&
                        lines[static_cast<std::size_t>(idx)].substr(0, 2) == "$$")
                    {
                        break;
                    }
                    ++idx;
                }
                int block_end = std::min(idx, total - 1);
                add_block(lines, block_start, block_end);
                ++idx;
                continue;
            }

            // Table block (consecutive pipe lines)
            if (!current.empty() && current[0] == '|')
            {
                int block_start = idx;
                while (idx < total && !lines[static_cast<std::size_t>(idx)].empty() &&
                       lines[static_cast<std::size_t>(idx)][0] == '|')
                {
                    ++idx;
                }
                add_block(lines, block_start, idx - 1);
                continue;
            }

            // Blockquote / callout block (consecutive > lines)
            if (!current.empty() && current[0] == '>')
            {
                int block_start = idx;
                while (idx < total && !lines[static_cast<std::size_t>(idx)].empty() &&
                       lines[static_cast<std::size_t>(idx)][0] == '>')
                {
                    ++idx;
                }
                add_block(lines, block_start, idx - 1);
                continue;
            }

            // List block (consecutive list-item lines, including indented continuation)
            if (is_list_start(current))
            {
                int block_start = idx;
                ++idx;
                while (idx < total)
                {
                    const auto& next_line = lines[static_cast<std::size_t>(idx)];
                    if (next_line.empty())
                    {
                        break;
                    }
                    // Continue list if indented or another list item
                    if (is_list_start(next_line) ||
                        (!next_line.empty() && (next_line[0] == ' ' || next_line[0] == '\t')))
                    {
                        ++idx;
                    }
                    else
                    {
                        break;
                    }
                }
                add_block(lines, block_start, idx - 1);
                continue;
            }

            // HTML block
            if (!current.empty() && current[0] == '<')
            {
                int block_start = idx;
                // Find closing tag or end of block
                ++idx;
                // Simple heuristic: continue until blank line or matching close tag
                while (idx < total && !lines[static_cast<std::size_t>(idx)].empty())
                {
                    ++idx;
                }
                add_block(lines, block_start, idx - 1);
                continue;
            }

            // Single-line blocks: heading, image, thematic break, paragraph
            {
                int block_start = idx;
                auto block_type = classify_block(current);

                if (block_type == WysiwygBlockType::kHeading ||
                    block_type == WysiwygBlockType::kImage ||
                    block_type == WysiwygBlockType::kThematicBreak)
                {
                    // These are single-line blocks
                    add_block(lines, block_start, block_start);
                    ++idx;
                }
                else
                {
                    // Paragraph: continues until blank line or other block type
                    ++idx;
                    while (idx < total)
                    {
                        const auto& next_line = lines[static_cast<std::size_t>(idx)];
                        if (next_line.empty() ||
                            classify_block(next_line) != WysiwygBlockType::kParagraph)
                        {
                            break;
                        }
                        ++idx;
                    }
                    add_block(lines, block_start, idx - 1);
                }
            }
        }
    }

    /// Helper: create a block from a range of lines and add to blocks_.
    void add_block(const std::vector<std::string>& lines, int start, int end_inclusive)
    {
        WysiwygBlock block;
        block.start_line = start;
        block.end_line = end_inclusive;

        std::string content;
        for (int ln = start; ln <= end_inclusive; ++ln)
        {
            if (ln > start)
            {
                content += '\n';
            }
            content += lines[static_cast<std::size_t>(ln)];
        }
        block.raw_content = content;
        block.block_type = classify_block(content);
        block.metadata = extract_metadata(content, block.block_type);
        blocks_.push_back(std::move(block));
    }

    /// Helper: check if a line starts a list item.
    [[nodiscard]] static auto is_list_start(const std::string& line) -> bool
    {
        if (line.size() < 2)
        {
            return false;
        }
        const char first = line[0];
        if ((first == '-' || first == '*' || first == '+') && line[1] == ' ')
        {
            return true;
        }
        if (std::isdigit(static_cast<unsigned char>(first)) != 0)
        {
            for (std::size_t pos = 1; pos < line.size(); ++pos)
            {
                if (line[pos] == '.' || line[pos] == ')')
                {
                    return pos + 1 < line.size() && line[pos + 1] == ' ';
                }
                if (std::isdigit(static_cast<unsigned char>(line[pos])) == 0)
                {
                    return false;
                }
            }
        }
        return false;
    }

    /// Recompute line numbers after insert/remove.
    void recompute_line_numbers()
    {
        int current_line = 0;
        for (auto& block : blocks_)
        {
            block.start_line = current_line;
            int line_ct = 1;
            for (char chr : block.raw_content)
            {
                if (chr == '\n')
                {
                    ++line_ct;
                }
            }
            block.end_line = current_line + line_ct - 1;
            current_line = block.end_line + 1;
        }
    }
};

} // namespace markamp::core
