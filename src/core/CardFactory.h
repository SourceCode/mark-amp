#pragma once

#include <regex>
#include <string>
#include <vector>

namespace markamp::core
{

/// Describes how a flashcard's front/back content is derived from a block.
enum class CardFormat
{
    FullBlock,   // Entire block is front, no explicit back (recall-based)
    MarkBased,   // Uses ==mark== syntax to split front/back
    ListBased,   // Uses list item structure (term :: definition)
    HeadingBased // Uses heading as front, content below as back
};

/// Extracted content for a flashcard.
struct CardContent
{
    std::string front; // Front text (question/prompt)
    std::string back;  // Back text (answer/response)
    CardFormat format{CardFormat::FullBlock};
    std::string source_block_id; // Original block ID
};

/// Factory for creating flashcard content from Markdown blocks.
class CardFactory
{
public:
    /// Create a CardContent from a block's raw markdown text.
    [[nodiscard]] auto create_card_content(const std::string& block_id,
                                           const std::string& markdown) const -> CardContent;

    /// Detect the card format from the markdown text.
    [[nodiscard]] auto detect_format(const std::string& markdown) const -> CardFormat;

    /// Check if the markdown contains mark-based indicators (==...==).
    [[nodiscard]] auto has_mark_syntax(const std::string& markdown) const -> bool;

    /// Check if the markdown contains list-based indicators (term :: definition).
    [[nodiscard]] auto has_list_definition(const std::string& markdown) const -> bool;

    /// Check if the markdown starts with a heading.
    [[nodiscard]] auto has_heading_structure(const std::string& markdown) const -> bool;

    /// Split mark-based content into front/back.
    [[nodiscard]] auto split_by_marks(const std::string& markdown) const
        -> std::pair<std::string, std::string>;

    /// Split list-based content into front/back.
    [[nodiscard]] auto split_by_list(const std::string& markdown) const
        -> std::pair<std::string, std::string>;

    /// Split heading-based content into front/back.
    [[nodiscard]] auto split_by_heading(const std::string& markdown) const
        -> std::pair<std::string, std::string>;

private:
    /// Strip mark syntax (== ==) from text, returning contained text.
    [[nodiscard]] static auto extract_mark_content(const std::string& text) -> std::string;
};

} // namespace markamp::core
