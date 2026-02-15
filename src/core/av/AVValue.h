#pragma once

#include "AVTypes.h"

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Per-type value structs
// ════════════════════════════════════════════════════════════

struct AVValueBlock
{
    std::string id;
    std::string content;
    bool is_detached{false};

    [[nodiscard]] auto operator==(const AVValueBlock& other) const -> bool = default;
};

struct AVValueText
{
    std::string content;

    [[nodiscard]] auto operator==(const AVValueText& other) const -> bool = default;
};

struct AVValueNumber
{
    double content{0.0};
    bool is_not_empty{false};
    AVNumberFormat format{AVNumberFormat::None};
    std::string formatted_content;

    [[nodiscard]] auto operator==(const AVValueNumber& other) const -> bool = default;
};

struct AVValueDate
{
    int64_t content{0};
    int64_t content2{0};
    bool is_not_empty{false};
    bool is_not_time{false};
    bool has_end_date{false};
    std::string formatted_content;

    [[nodiscard]] auto operator==(const AVValueDate& other) const -> bool = default;
};

struct AVValueSelect
{
    AVSelectOption option;

    [[nodiscard]] auto operator==(const AVValueSelect& other) const -> bool = default;
};

struct AVValueMSelect
{
    std::vector<AVSelectOption> options;

    [[nodiscard]] auto operator==(const AVValueMSelect& other) const -> bool = default;
};

struct AVValueURL
{
    std::string content;
    std::string smart_url;

    [[nodiscard]] auto operator==(const AVValueURL& other) const -> bool = default;
};

struct AVValueEmail
{
    std::string content;

    [[nodiscard]] auto operator==(const AVValueEmail& other) const -> bool = default;
};

struct AVValuePhone
{
    std::string content;

    [[nodiscard]] auto operator==(const AVValuePhone& other) const -> bool = default;
};

struct AVValueAssetEntry
{
    std::string name;
    std::string content;
    std::string type;

    [[nodiscard]] auto operator==(const AVValueAssetEntry& other) const -> bool = default;
};

struct AVValueMAsset
{
    std::vector<AVValueAssetEntry> assets;

    [[nodiscard]] auto operator==(const AVValueMAsset& other) const -> bool = default;
};

struct AVValueTemplate
{
    std::string content;

    [[nodiscard]] auto operator==(const AVValueTemplate& other) const -> bool = default;
};

struct AVValueCreated
{
    int64_t content{0};
    bool is_not_empty{false};
    std::string formatted_content;

    [[nodiscard]] auto operator==(const AVValueCreated& other) const -> bool = default;
};

struct AVValueUpdated
{
    int64_t content{0};
    bool is_not_empty{false};
    std::string formatted_content;

    [[nodiscard]] auto operator==(const AVValueUpdated& other) const -> bool = default;
};

struct AVValueCheckbox
{
    bool checked{false};

    [[nodiscard]] auto operator==(const AVValueCheckbox& other) const -> bool = default;
};

struct AVRelationContent
{
    std::string block_id;
    std::string content;

    [[nodiscard]] auto operator==(const AVRelationContent& other) const -> bool = default;
};

struct AVValueRelation
{
    std::vector<std::string> block_ids;
    std::vector<AVRelationContent> contents;

    [[nodiscard]] auto operator==(const AVValueRelation& other) const -> bool = default;
};

// Forward declaration for recursive type
struct AVValue;

struct AVValueRollup
{
    std::vector<AVValue> contents;

    [[nodiscard]] auto operator==(const AVValueRollup& other) const -> bool;
};

// ════════════════════════════════════════════════════════════
// AVValue: Union of all typed values via std::variant
// ════════════════════════════════════════════════════════════

using AVValueVariant = std::variant<std::monostate,
                                    AVValueBlock,
                                    AVValueText,
                                    AVValueNumber,
                                    AVValueDate,
                                    AVValueSelect,
                                    AVValueMSelect,
                                    AVValueURL,
                                    AVValueEmail,
                                    AVValuePhone,
                                    AVValueMAsset,
                                    AVValueTemplate,
                                    AVValueCreated,
                                    AVValueUpdated,
                                    AVValueCheckbox,
                                    AVValueRelation,
                                    AVValueRollup>;

struct AVValue
{
    std::string id;
    std::string key_id;
    std::string block_id;
    AVKeyType type{AVKeyType::Text};
    AVValueVariant data;

    // ── Typed accessors ──

    [[nodiscard]] auto as_block() -> AVValueBlock*;
    [[nodiscard]] auto as_block() const -> const AVValueBlock*;
    [[nodiscard]] auto as_text() -> AVValueText*;
    [[nodiscard]] auto as_text() const -> const AVValueText*;
    [[nodiscard]] auto as_number() -> AVValueNumber*;
    [[nodiscard]] auto as_number() const -> const AVValueNumber*;
    [[nodiscard]] auto as_date() -> AVValueDate*;
    [[nodiscard]] auto as_date() const -> const AVValueDate*;
    [[nodiscard]] auto as_select() -> AVValueSelect*;
    [[nodiscard]] auto as_select() const -> const AVValueSelect*;
    [[nodiscard]] auto as_mselect() -> AVValueMSelect*;
    [[nodiscard]] auto as_mselect() const -> const AVValueMSelect*;
    [[nodiscard]] auto as_url() -> AVValueURL*;
    [[nodiscard]] auto as_url() const -> const AVValueURL*;
    [[nodiscard]] auto as_email() -> AVValueEmail*;
    [[nodiscard]] auto as_email() const -> const AVValueEmail*;
    [[nodiscard]] auto as_phone() -> AVValuePhone*;
    [[nodiscard]] auto as_phone() const -> const AVValuePhone*;
    [[nodiscard]] auto as_masset() -> AVValueMAsset*;
    [[nodiscard]] auto as_masset() const -> const AVValueMAsset*;
    [[nodiscard]] auto as_template() -> AVValueTemplate*;
    [[nodiscard]] auto as_template() const -> const AVValueTemplate*;
    [[nodiscard]] auto as_created() -> AVValueCreated*;
    [[nodiscard]] auto as_created() const -> const AVValueCreated*;
    [[nodiscard]] auto as_updated() -> AVValueUpdated*;
    [[nodiscard]] auto as_updated() const -> const AVValueUpdated*;
    [[nodiscard]] auto as_checkbox() -> AVValueCheckbox*;
    [[nodiscard]] auto as_checkbox() const -> const AVValueCheckbox*;
    [[nodiscard]] auto as_relation() -> AVValueRelation*;
    [[nodiscard]] auto as_relation() const -> const AVValueRelation*;
    [[nodiscard]] auto as_rollup() -> AVValueRollup*;
    [[nodiscard]] auto as_rollup() const -> const AVValueRollup*;

    /// Returns true if the value holds std::monostate.
    [[nodiscard]] auto is_empty() const -> bool;

    /// Basic string representation (full formatting in Phase 18).
    [[nodiscard]] auto to_display_string() const -> std::string;

    [[nodiscard]] auto operator==(const AVValue& other) const -> bool = default;
};

} // namespace markamp::core::av
