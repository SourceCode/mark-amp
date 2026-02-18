#pragma once

#include "AVTypes.h"
#include "AVValue.h"
#include "AttributeView.h"

#include <cstdint>
#include <expected>
#include <string>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Timeline Granularity
// ════════════════════════════════════════════════════════════

enum class AVTimelineGranularity : std::uint8_t
{
    Day,
    Week,
    Month,
    Quarter,
    Year
};

[[nodiscard]] auto granularity_to_string(AVTimelineGranularity gran) -> std::string;
[[nodiscard]] auto string_to_granularity(const std::string& str) -> AVTimelineGranularity;

// ════════════════════════════════════════════════════════════
// Timeline Bucket
// ════════════════════════════════════════════════════════════

struct AVTimelineBucket
{
    int64_t start_ts{0};
    int64_t end_ts{0};
    std::string label;
    std::vector<std::string> block_ids;
};

// ════════════════════════════════════════════════════════════
// Timeline View
// ════════════════════════════════════════════════════════════

class AVTimelineView
{
public:
    /// Set the date key used for bucketing rows.
    void set_date_key(const std::string& key_id);

    /// Get the current date key.
    [[nodiscard]] auto date_key() const -> std::string;

    /// Set the granularity for bucketing.
    void set_granularity(AVTimelineGranularity granularity);

    /// Get the current granularity.
    [[nodiscard]] auto granularity() const -> AVTimelineGranularity;

    /// Bucket all rows by their date value at the configured granularity.
    [[nodiscard]] auto bucket_rows(const AttributeView& attribute_view) const
        -> std::vector<AVTimelineBucket>;

    /// Get rows within a specific date range.
    [[nodiscard]] auto rows_in_range(const AttributeView& attribute_view,
                                     int64_t range_start,
                                     int64_t range_end) const -> std::vector<std::string>;

    /// Move a row to a new date (drag-and-drop on calendar).
    [[nodiscard]] auto move_row(AttributeView& attribute_view,
                                const std::string& block_id,
                                int64_t new_timestamp) const -> std::expected<void, std::string>;

    /// Get the bucket that a specific timestamp falls into.
    [[nodiscard]] auto get_bucket_for_timestamp(int64_t timestamp) const -> AVTimelineBucket;

    /// Get rows that have no date set.
    [[nodiscard]] auto unscheduled_rows(const AttributeView& attribute_view) const
        -> std::vector<std::string>;

private:
    std::string date_key_id_;
    AVTimelineGranularity granularity_{AVTimelineGranularity::Month};

    /// Get the start of the period that contains a timestamp.
    [[nodiscard]] auto period_start(int64_t timestamp) const -> int64_t;

    /// Get the end of the period that a start belongs to.
    [[nodiscard]] auto period_end(int64_t period_start_ts) const -> int64_t;

    /// Generate a human-readable label for a period.
    [[nodiscard]] auto period_label(int64_t period_start_ts) const -> std::string;

    /// Extract the date timestamp from a row.
    [[nodiscard]] auto get_row_timestamp(const AttributeView& attribute_view,
                                         const std::string& block_id) const -> int64_t;
};

} // namespace markamp::core::av
