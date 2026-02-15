#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// Block node types matching SiYuan's AST node types.
// Values chosen for stable serialization — do not reorder.
enum class BlockType : uint8_t
{
    Document = 0,
    Paragraph,
    Heading,
    BlockQuote,
    List,
    ListItem,
    CodeBlock,
    MathBlock,
    Table,
    TableRow,
    TableCell,
    HTMLBlock,
    ThematicBreak,
    Audio,
    Video,
    IFrame,
    Widget,
    SuperBlock,
    EmbedBlock,
    AttributeView,
    CalloutBlock,
    MermaidBlock,
    PlantUMLBlock,
    GraphvizBlock,
    FlowchartBlock,
    MindmapBlock,
    EChartsBlock,
    ABCBlock,
    Unknown
};

enum class BlockSubType : uint8_t
{
    None = 0,
    Heading1,
    Heading2,
    Heading3,
    Heading4,
    Heading5,
    Heading6,
    OrderedList,
    UnorderedList,
    TaskList,
    Math,
    Code,
};

// Inline Attribute List — per-block key-value metadata.
// Keys are always lowercase ASCII. Values are UTF-8 strings.
using InlineAttributeList = std::unordered_map<std::string, std::string>;

struct BlockRef
{
    std::string block_id;
    std::string ref_text;
    std::string def_block_id;
    std::string def_block_path;
    std::string def_block_root_id; // Root doc ID containing the definition
    std::string content;           // Reference anchor text (for DB storage)
};

struct Block
{
    // Identity
    std::string id;
    std::string parent_id;
    std::string root_id;
    std::string box; // Notebook ID (SiYuan calls this 'box')
    std::string hash;

    // Path
    std::string path;
    std::string hpath;

    // Content
    std::string name;
    std::string alias;
    std::string memo;
    std::string tag;
    std::string content;
    std::string fcontent;
    std::string markdown;

    // Type
    BlockType type = BlockType::Paragraph;
    BlockSubType sub_type = BlockSubType::None;

    // Metadata
    InlineAttributeList ial;
    int64_t sort = 0;
    int64_t length = 0;
    bool folded = false;
    int32_t depth = 0;
    int32_t count = 0;
    int32_t ref_count = 0;

    // Timestamps (stored as int64_t or as formatted strings depending on context)
    int64_t created = 0;
    int64_t updated = 0;
    std::string created_str; // YYYYMMDDHHmmss format for DB storage
    std::string updated_str; // YYYYMMDDHHmmss format for DB storage

    // Relationships (populated on demand, NOT persisted inline)
    std::vector<BlockRef> refs;
    std::vector<BlockRef> defs;
    std::vector<std::shared_ptr<Block>> children;

    // Query methods
    [[nodiscard]] auto is_document() const -> bool;
    [[nodiscard]] auto is_heading() const -> bool;
    [[nodiscard]] auto is_container() const -> bool;
    [[nodiscard]] auto is_leaf() const -> bool;
    [[nodiscard]] auto type_abbr() const -> std::string;
    [[nodiscard]] auto sub_type_abbr() const -> std::string;
    [[nodiscard]] auto ial_string() const -> std::string;
    auto set_ial_from_string(const std::string& ial_str) -> void;

    // IAL helpers
    auto set_ial(const std::string& key, const std::string& value) -> void;
    [[nodiscard]] auto get_ial(const std::string& key) const -> std::optional<std::string>;
    auto remove_ial(const std::string& key) -> void;
};

// Sort comparators
[[nodiscard]] auto compare_blocks_by_sort(const Block& a, const Block& b) -> bool;
[[nodiscard]] auto compare_blocks_by_updated(const Block& a, const Block& b) -> bool;

} // namespace markamp::core
