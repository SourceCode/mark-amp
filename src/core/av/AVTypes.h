#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Column Type Enumeration (17 types)
// ════════════════════════════════════════════════════════════

enum class AVKeyType : std::uint8_t
{
    Block,
    Text,
    Number,
    Date,
    Select,
    MSelect,
    URL,
    Email,
    Phone,
    MAsset,
    Template,
    Created,
    Updated,
    Checkbox,
    Relation,
    Rollup,
    LineNumber
};

[[nodiscard]] auto key_type_to_string(AVKeyType type) -> std::string;
[[nodiscard]] auto string_to_key_type(const std::string& str) -> AVKeyType;

// ════════════════════════════════════════════════════════════
// View Type Enumeration
// ════════════════════════════════════════════════════════════

enum class AVViewType : std::uint8_t
{
    Table,
    Gallery,
    Kanban
};

[[nodiscard]] auto view_type_to_string(AVViewType type) -> std::string;
[[nodiscard]] auto string_to_view_type(const std::string& str) -> AVViewType;

// ════════════════════════════════════════════════════════════
// Number Format
// ════════════════════════════════════════════════════════════

enum class AVNumberFormat : std::uint8_t
{
    None,
    Commas,
    Percent,
    USDollar,
    Euro,
    Pound,
    Yen,
    Yuan,
    Ruble,
    Rupee,
    Won,
    Custom
};

[[nodiscard]] auto number_format_to_string(AVNumberFormat fmt) -> std::string;
[[nodiscard]] auto string_to_number_format(const std::string& str) -> AVNumberFormat;

// ════════════════════════════════════════════════════════════
// Select Option
// ════════════════════════════════════════════════════════════

struct AVSelectOption
{
    std::string name;
    std::string color;

    [[nodiscard]] auto operator==(const AVSelectOption& other) const -> bool = default;
};

// ════════════════════════════════════════════════════════════
// Relation Configuration
// ════════════════════════════════════════════════════════════

struct AVRelationConfig
{
    std::string dest_av_id;
    bool is_two_way{false};
    std::string back_key_id;
};

// ════════════════════════════════════════════════════════════
// Rollup Configuration
// ════════════════════════════════════════════════════════════

enum class AVRollupCalc : std::uint8_t
{
    None,
    CountAll,
    CountValues,
    CountUniqueValues,
    CountEmpty,
    CountNotEmpty,
    PercentEmpty,
    PercentNotEmpty,
    Sum,
    Average,
    Median,
    Min,
    Max,
    Range,
    DateEarliest,
    DateLatest,
    Checked,
    Unchecked,
    PercentChecked,
    PercentUnchecked
};

[[nodiscard]] auto rollup_calc_to_string(AVRollupCalc calc) -> std::string;
[[nodiscard]] auto string_to_rollup_calc(const std::string& str) -> AVRollupCalc;

struct AVRollupConfig
{
    std::string relation_key_id;
    std::string target_key_id;
    AVRollupCalc calc{AVRollupCalc::None};
};

// ════════════════════════════════════════════════════════════
// Column Schema (Key)
// ════════════════════════════════════════════════════════════

struct AVKey
{
    std::string id;
    std::string name;
    AVKeyType type{AVKeyType::Text};
    std::string icon;
    std::string desc;

    AVNumberFormat number_format{AVNumberFormat::None};
    std::string custom_number_format;
    std::vector<AVSelectOption> options;
    std::string template_expr;

    AVRelationConfig relation_config;
    AVRollupConfig rollup_config;

    [[nodiscard]] auto operator==(const AVKey& other) const -> bool
    {
        return id == other.id;
    }
};

// ════════════════════════════════════════════════════════════
// View Column Configuration
// ════════════════════════════════════════════════════════════

struct AVViewColumn
{
    std::string key_id;
    bool hidden{false};
    int width{200};
    bool pinned{false};
    bool wrap{false};

    [[nodiscard]] auto operator==(const AVViewColumn& other) const -> bool = default;
};

// ════════════════════════════════════════════════════════════
// Table View Layout
// ════════════════════════════════════════════════════════════

struct AVTableView
{
    std::vector<AVViewColumn> columns;
    int row_height{32};
    bool show_row_numbers{false};
    bool stripe_rows{true};
};

// ════════════════════════════════════════════════════════════
// Gallery View Layout
// ════════════════════════════════════════════════════════════

enum class AVCardSize : std::uint8_t
{
    Small,
    Medium,
    Large
};

[[nodiscard]] auto card_size_to_string(AVCardSize size) -> std::string;
[[nodiscard]] auto string_to_card_size(const std::string& str) -> AVCardSize;

enum class AVCoverAspectRatio : std::uint8_t
{
    Ratio16x9,
    Ratio4x3,
    Ratio1x1,
    Auto
};

struct AVGalleryView
{
    std::vector<AVViewColumn> columns;
    int page_size{50};
    AVCardSize card_size{AVCardSize::Medium};
    std::string cover_key_id;
    std::string cover_field;
    bool hide_empty_values{true};
    AVCoverAspectRatio cover_ratio{AVCoverAspectRatio::Ratio16x9};
};

// ════════════════════════════════════════════════════════════
// Kanban View Layout
// ════════════════════════════════════════════════════════════

struct AVKanbanView
{
    std::vector<AVViewColumn> columns;
    std::string group_by_key_id;
    bool hide_empty_groups{false};
    int page_size{50};
};

// ════════════════════════════════════════════════════════════
// View (named layout over the same data)
// ════════════════════════════════════════════════════════════

struct AVView
{
    std::string id;
    std::string name;
    std::string icon;
    std::string desc;
    AVViewType type{AVViewType::Table};
    int page_size{50};

    AVTableView table;
    AVGalleryView gallery;
    AVKanbanView kanban;
};

// ════════════════════════════════════════════════════════════
// Specification Version
// ════════════════════════════════════════════════════════════

inline constexpr int kAVSpecVersion = 1;

} // namespace markamp::core::av
