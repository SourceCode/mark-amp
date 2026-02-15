#pragma once

#include "AVTypes.h"
#include "AVValue.h"

#include <string>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// KeyValues: All values for a single column across all rows
// ════════════════════════════════════════════════════════════

struct AVKeyValues
{
    AVKey key;
    std::vector<AVValue> values;
};

// ════════════════════════════════════════════════════════════
// AttributeView: Root aggregate
// ════════════════════════════════════════════════════════════

struct AttributeView
{
    std::string id;
    std::string name;
    int spec{kAVSpecVersion};
    std::vector<AVKeyValues> key_values;
    std::vector<AVView> views;

    // ── Key (column) operations ──

    [[nodiscard]] auto add_key(AVKey key) -> std::string;
    [[nodiscard]] auto remove_key(const std::string& key_id) -> bool;
    [[nodiscard]] auto find_key(const std::string& key_id) -> AVKey*;
    [[nodiscard]] auto find_key(const std::string& key_id) const -> const AVKey*;
    [[nodiscard]] auto key_ids() const -> std::vector<std::string>;
    [[nodiscard]] auto key_count() const -> size_t;

    // ── Row operations ──

    [[nodiscard]] auto add_row(const std::string& block_id) -> std::string;
    [[nodiscard]] auto remove_row(const std::string& block_id) -> bool;
    [[nodiscard]] auto row_block_ids() const -> std::vector<std::string>;
    [[nodiscard]] auto row_count() const -> size_t;

    // ── Value operations ──

    [[nodiscard]] auto get_value(const std::string& key_id, const std::string& block_id)
        -> AVValue*;
    [[nodiscard]] auto get_value(const std::string& key_id, const std::string& block_id) const
        -> const AVValue*;
    void set_value(const std::string& key_id, const std::string& block_id, AVValueVariant data);

    // ── View operations ──

    [[nodiscard]] auto add_view(AVView view) -> std::string;
    [[nodiscard]] auto remove_view(const std::string& view_id) -> bool;
    [[nodiscard]] auto find_view(const std::string& view_id) -> AVView*;
    [[nodiscard]] auto find_view(const std::string& view_id) const -> const AVView*;
    [[nodiscard]] auto default_view() -> AVView&;
};

} // namespace markamp::core::av
