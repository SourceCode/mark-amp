#pragma once

#include "FileNode.h"

#include <string>
#include <vector>

namespace markamp::core
{

// ═══════════════════════════════════════════════════════
// Markdown AST node types
// ═══════════════════════════════════════════════════════

enum class MdNodeType
{
    // Block-level
    Document,
    Paragraph,
    Heading,
    BlockQuote,
    UnorderedList,
    OrderedList,
    ListItem,
    CodeBlock,
    FencedCodeBlock,
    HorizontalRule,
    Table,
    TableHead,
    TableBody,
    TableRow,
    TableCell,
    HtmlBlock,

    // Inline-level
    Text,
    Emphasis,
    Strong,
    StrongEmphasis,
    Code,
    Link,
    Image,
    LineBreak,
    SoftBreak,
    HtmlInline,
    TaskListMarker,
    Strikethrough,

    // Custom
    MermaidBlock
};

enum class MdAlignment
{
    Default,
    Left,
    Center,
    Right
};

// ═══════════════════════════════════════════════════════
// AST node
// ═══════════════════════════════════════════════════════

struct MdNode
{
    MdNodeType type{MdNodeType::Document};
    std::string text_content; // For leaf nodes (Text, Code, etc.)

    // Heading
    int heading_level{0}; // 1-6

    // Code block
    std::string language;    // "javascript", "mermaid", "cpp", etc.
    std::string info_string; // Full info string after ```

    // Link / Image
    std::string url;
    std::string title;

    // List
    bool is_tight{false};
    int start_number{1}; // For ordered lists

    // Table cell
    MdAlignment alignment{MdAlignment::Default};
    bool is_header{false};

    // Task list
    bool is_checked{false};

    // Children
    std::vector<MdNode> children;

    // Helpers
    [[nodiscard]] auto is_block() const -> bool;
    [[nodiscard]] auto is_inline() const -> bool;
    [[nodiscard]] auto plain_text() const -> std::string;
    [[nodiscard]] auto find_all(MdNodeType target_type) const -> std::vector<const MdNode*>;
};

// ═══════════════════════════════════════════════════════
// Parsed markdown document
// ═══════════════════════════════════════════════════════

struct MarkdownDocument
{
    MdNode root;
    std::vector<std::string> mermaid_blocks; // Extracted mermaid sources
    std::vector<std::string> code_languages; // Unique languages used

    // Footnote support (pre-processed, not from md4c)
    std::string footnote_section_html;
    bool has_footnotes_{false};

    [[nodiscard]] auto heading_count() const -> size_t;
    [[nodiscard]] auto word_count() const -> size_t;
    [[nodiscard]] auto has_mermaid() const -> bool;
    [[nodiscard]] auto has_tables() const -> bool;
    [[nodiscard]] auto has_task_lists() const -> bool;
    [[nodiscard]] auto has_footnotes() const -> bool;
};

} // namespace markamp::core
