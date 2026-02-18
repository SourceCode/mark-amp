/// @file BlockReferenceResolver.h
/// @brief V9 Phase 17 Tasks 1–2 — Block reference resolution for ^block-id syntax.
///
/// Header-only. Scans markdown content for Obsidian-style ^block-id markers,
/// builds an index for cross-document resolution, and provides slug-based
/// auto-generation and validation of block IDs.

#pragma once

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Block reference types
// ============================================================================

/// The type of block that a ^block-id is attached to.
enum class BlockType : uint8_t
{
    kParagraph,
    kHeading,
    kList,
    kCodeBlock,
    kBlockquote,
    kTable,
    kUnknown
};

/// A single block reference discovered in a document.
struct BlockReference
{
    std::string document_id; ///< Source document containing this block
    std::string block_id;    ///< The ^block-id value (without ^)
    int line_number{0};      ///< Line where the block ID was found (0-based)
    BlockType block_type{BlockType::kUnknown};
    std::string content_preview; ///< First ~80 chars of the block content
};

// ============================================================================
// BlockReferenceIndex — cross-document block reference registry
// ============================================================================

/// Cross-document index of ^block-id references.
///
/// Usage:
/// ```cpp
/// BlockReferenceIndex index;
/// index.index_document("doc1", "Some paragraph ^my-block-id\n...");
/// auto ref = index.resolve("doc1", "my-block-id");
/// ```
class BlockReferenceIndex
{
public:
    BlockReferenceIndex() = default;

    /// Index all ^block-id markers in a document's content.
    /// Replaces any previously indexed data for this document.
    void index_document(const std::string& document_id, const std::string& content)
    {
        // Remove old entries for this document
        auto iter = references_.begin();
        while (iter != references_.end())
        {
            if (iter->document_id == document_id)
            {
                iter = references_.erase(iter);
            }
            else
            {
                ++iter;
            }
        }

        // Scan and add new references
        auto scanned = scan_block_ids(content);
        for (auto& ref : scanned)
        {
            ref.document_id = document_id;
            references_.push_back(std::move(ref));
        }
    }

    /// Resolve a block reference by document and block ID.
    [[nodiscard]] auto resolve(const std::string& document_id, const std::string& block_id) const
        -> std::optional<BlockReference>
    {
        for (const auto& ref : references_)
        {
            if (ref.document_id == document_id && ref.block_id == block_id)
            {
                return ref;
            }
        }
        return std::nullopt;
    }

    /// Resolve a block reference across all documents (first match).
    [[nodiscard]] auto resolve_any(const std::string& block_id) const
        -> std::optional<BlockReference>
    {
        for (const auto& ref : references_)
        {
            if (ref.block_id == block_id)
            {
                return ref;
            }
        }
        return std::nullopt;
    }

    /// Get all indexed block references.
    [[nodiscard]] auto all_references() const -> const std::vector<BlockReference>&
    {
        return references_;
    }

    /// Find all documents that contain references to the given block ID.
    [[nodiscard]] auto find_references_to(const std::string& block_id) const
        -> std::vector<BlockReference>
    {
        std::vector<BlockReference> results;
        for (const auto& ref : references_)
        {
            if (ref.block_id == block_id)
            {
                results.push_back(ref);
            }
        }
        return results;
    }

    /// Get all block references in a specific document.
    [[nodiscard]] auto references_in(const std::string& document_id) const
        -> std::vector<BlockReference>
    {
        std::vector<BlockReference> results;
        for (const auto& ref : references_)
        {
            if (ref.document_id == document_id)
            {
                results.push_back(ref);
            }
        }
        return results;
    }

    /// Total number of indexed block references.
    [[nodiscard]] auto size() const -> std::size_t
    {
        return references_.size();
    }

    /// Clear all indexed data.
    void clear()
    {
        references_.clear();
    }

    // ========================================================================
    // Static utilities (Task 2)
    // ========================================================================

    /// Scan content for all ^block-id markers and return them.
    /// Each block ID must appear at the end of a line: `some text ^block-id`
    [[nodiscard]] static auto scan_block_ids(const std::string& content)
        -> std::vector<BlockReference>
    {
        std::vector<BlockReference> results;
        if (content.empty())
        {
            return results;
        }

        int line_num = 0;
        std::size_t pos = 0;
        bool in_code_fence = false;

        while (pos < content.size())
        {
            // Find end of current line
            auto eol = content.find('\n', pos);
            if (eol == std::string::npos)
            {
                eol = content.size();
            }

            const std::string line = content.substr(pos, eol - pos);

            // Toggle code fence tracking (``` or ~~~)
            const auto trimmed = trim_left(line);
            if (trimmed.rfind("```", 0) == 0 || trimmed.rfind("~~~", 0) == 0)
            {
                in_code_fence = !in_code_fence;
            }

            // Only look for ^block-id outside code fences
            if (!in_code_fence)
            {
                auto block_id = extract_block_id(line);
                if (!block_id.empty())
                {
                    BlockReference ref;
                    ref.block_id = block_id;
                    ref.line_number = line_num;
                    ref.block_type = detect_block_type(line);
                    ref.content_preview = make_preview(line);
                    results.push_back(std::move(ref));
                }
            }

            pos = eol + 1;
            ++line_num;
        }

        return results;
    }

    /// Generate a valid block ID from content text (slug-based).
    /// Example: "My Important Paragraph" → "my-important-paragraph"
    [[nodiscard]] static auto generate_block_id(const std::string& content_line) -> std::string
    {
        std::string slug;
        slug.reserve(content_line.size());

        bool prev_was_dash = true; // Avoid leading dash
        for (const char ch : content_line)
        {
            if (std::isalnum(static_cast<unsigned char>(ch)) != 0)
            {
                slug += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
                prev_was_dash = false;
            }
            else if (!prev_was_dash && (ch == ' ' || ch == '_' || ch == '-'))
            {
                slug += '-';
                prev_was_dash = true;
            }
            // Skip special chars like ^, #, etc.
        }

        // Remove trailing dash
        if (!slug.empty() && slug.back() == '-')
        {
            slug.pop_back();
        }

        // Truncate to reasonable length
        constexpr std::size_t kMaxSlugLength = 50;
        if (slug.size() > kMaxSlugLength)
        {
            slug.resize(kMaxSlugLength);
            // Don't end on a dash after truncation
            if (!slug.empty() && slug.back() == '-')
            {
                slug.pop_back();
            }
        }

        return slug;
    }

    /// Validate that a block ID has correct format.
    /// Rules: alphanumeric + dashes, no leading/trailing dash, non-empty,
    /// max 50 chars, lowercase.
    [[nodiscard]] static auto validate_block_id(const std::string& block_id) -> bool
    {
        if (block_id.empty() || block_id.size() > 50)
        {
            return false;
        }
        if (block_id.front() == '-' || block_id.back() == '-')
        {
            return false;
        }

        bool prev_was_dash = false;
        for (const char ch : block_id)
        {
            if (ch == '-')
            {
                if (prev_was_dash)
                {
                    return false; // No consecutive dashes
                }
                prev_was_dash = true;
            }
            else if (std::isalnum(static_cast<unsigned char>(ch)) != 0)
            {
                // Must be lowercase
                if (std::isupper(static_cast<unsigned char>(ch)) != 0)
                {
                    return false;
                }
                prev_was_dash = false;
            }
            else
            {
                return false; // Invalid character
            }
        }

        return true;
    }

private:
    std::vector<BlockReference> references_;

    /// Extract ^block-id from end of a line. Returns empty if none found.
    [[nodiscard]] static auto extract_block_id(const std::string& line) -> std::string
    {
        // Pattern: content ^block-id at end of line (optionally followed by whitespace)
        auto trimmed = trim_right(line);
        if (trimmed.empty())
        {
            return {};
        }

        // Find the last ^
        auto caret_pos = trimmed.rfind('^');
        if (caret_pos == std::string::npos || caret_pos == 0)
        {
            return {};
        }

        // Must be preceded by a space
        if (trimmed[caret_pos - 1] != ' ')
        {
            return {};
        }

        // Extract everything after ^
        auto candidate = trimmed.substr(caret_pos + 1);
        if (candidate.empty())
        {
            return {};
        }

        // Must be alphanumeric + dashes
        for (const char ch : candidate)
        {
            if (std::isalnum(static_cast<unsigned char>(ch)) == 0 && ch != '-')
            {
                return {};
            }
        }

        return candidate;
    }

    /// Detect the block type from line content.
    [[nodiscard]] static auto detect_block_type(const std::string& line) -> BlockType
    {
        const auto trimmed = trim_left(line);
        if (trimmed.empty())
        {
            return BlockType::kParagraph;
        }

        if (trimmed[0] == '#')
        {
            return BlockType::kHeading;
        }
        if (trimmed[0] == '-' || trimmed[0] == '*' || trimmed[0] == '+')
        {
            return BlockType::kList;
        }
        if (trimmed[0] == '>')
        {
            return BlockType::kBlockquote;
        }
        if (trimmed[0] == '|')
        {
            return BlockType::kTable;
        }
        if (trimmed.rfind("```", 0) == 0 || trimmed.rfind("~~~", 0) == 0)
        {
            return BlockType::kCodeBlock;
        }

        return BlockType::kParagraph;
    }

    /// Create a short preview of a line's content (without block-id).
    [[nodiscard]] static auto make_preview(const std::string& line) -> std::string
    {
        auto trimmed = trim_right(line);

        // Remove the ^block-id suffix
        auto caret_pos = trimmed.rfind('^');
        if (caret_pos != std::string::npos && caret_pos > 0 && trimmed[caret_pos - 1] == ' ')
        {
            trimmed = trim_right(trimmed.substr(0, caret_pos - 1));
        }

        constexpr std::size_t kMaxPreview = 80;
        if (trimmed.size() > kMaxPreview)
        {
            trimmed.resize(kMaxPreview);
            trimmed += "...";
        }
        return trimmed;
    }

    /// Trim whitespace from left side of string.
    [[nodiscard]] static auto trim_left(const std::string& str) -> std::string
    {
        auto start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
        {
            return {};
        }
        return str.substr(start);
    }

    /// Trim whitespace from right side of string.
    [[nodiscard]] static auto trim_right(const std::string& str) -> std::string
    {
        auto end_pos = str.find_last_not_of(" \t\r\n");
        if (end_pos == std::string::npos)
        {
            return {};
        }
        return str.substr(0, end_pos + 1);
    }
};

} // namespace markamp::core
