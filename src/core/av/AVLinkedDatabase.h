#pragma once

#include "AVFilter.h"
#include "AVSort.h"
#include "AVTypes.h"
#include "AttributeView.h"
#include "AttributeViewStore.h"

#include <expected>
#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Linked Database Configuration
// ════════════════════════════════════════════════════════════

struct AVLinkedConfig
{
    std::string link_id;
    std::string source_av_id;

    // Local overrides
    AVFilterGroup filter_overrides;
    std::vector<AVSortSpec> sort_overrides;
    std::unordered_set<std::string> hidden_key_ids;

    // View customisation
    AVViewType view_type{AVViewType::Table};
    std::string view_name;
};

// ════════════════════════════════════════════════════════════
// Resolved Linked View (snapshot of source + overrides)
// ════════════════════════════════════════════════════════════

struct AVLinkedSnapshot
{
    std::string link_id;
    std::string source_av_id;
    std::string source_av_name;
    std::vector<AVKey> visible_keys;
    std::vector<std::string> block_ids; // filtered + sorted rows
    int total_rows{0};
};

// ════════════════════════════════════════════════════════════
// Linked Database Service
// ════════════════════════════════════════════════════════════

class AVLinkedDatabase
{
public:
    explicit AVLinkedDatabase(AttributeViewStore& store);

    /// Create a new linked view to a source AV.
    [[nodiscard]] auto create_link(const std::string& source_av_id,
                                   const std::string& view_name = "") -> AVLinkedConfig;

    /// Resolve a linked config to a snapshot of the source data.
    [[nodiscard]] auto resolve(const AVLinkedConfig& config) const
        -> std::expected<AVLinkedSnapshot, std::string>;

    /// Set filter overrides on a linked config.
    void set_filter_override(AVLinkedConfig& config, AVFilterGroup filters);

    /// Set sort overrides on a linked config.
    void set_sort_override(AVLinkedConfig& config, std::vector<AVSortSpec> sort_specs);

    /// Hide specific columns in the linked view.
    void hide_columns(AVLinkedConfig& config, const std::vector<std::string>& key_ids);

    /// Show previously hidden columns.
    void show_columns(AVLinkedConfig& config, const std::vector<std::string>& key_ids);

    /// Refresh a snapshot (re-resolve from source).
    [[nodiscard]] auto refresh(const AVLinkedConfig& config) const
        -> std::expected<AVLinkedSnapshot, std::string>;

    /// Check if the source AV still exists.
    [[nodiscard]] auto is_valid(const AVLinkedConfig& config) const -> bool;

    /// Get the number of active links.
    [[nodiscard]] auto link_count() const -> size_t;

private:
    AttributeViewStore& store_;
    std::vector<AVLinkedConfig> links_;

    /// Generate a unique link ID.
    [[nodiscard]] static auto generate_link_id() -> std::string;
};

} // namespace markamp::core::av
