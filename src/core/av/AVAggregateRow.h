#pragma once

#include "AVFormatters.h"
#include "AVTypes.h"
#include "AVValue.h"
#include "AttributeView.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Aggregate Type
// ════════════════════════════════════════════════════════════

enum class AVAggregateType : std::uint8_t
{
    None,
    Count,
    CountValues,
    CountEmpty,
    Sum,
    Average,
    Min,
    Max,
    Median,
    Range,
    PercentEmpty,
    PercentFilled
};

[[nodiscard]] auto aggregate_type_to_string(AVAggregateType type) -> std::string;
[[nodiscard]] auto string_to_aggregate_type(const std::string& str) -> AVAggregateType;

// ════════════════════════════════════════════════════════════
// Aggregate Configuration per Key
// ════════════════════════════════════════════════════════════

struct AVAggregateConfig
{
    std::string key_id;
    AVAggregateType aggregate{AVAggregateType::None};
};

// ════════════════════════════════════════════════════════════
// Aggregate Result for a single key
// ════════════════════════════════════════════════════════════

struct AVAggregateResult
{
    std::string key_id;
    AVAggregateType aggregate{AVAggregateType::None};
    double numeric_result{0.0};
    std::string formatted_result;
};

// ════════════════════════════════════════════════════════════
// Aggregate Row: compute summary aggregates per column
// ════════════════════════════════════════════════════════════

class AVAggregateRow
{
public:
    /// Set the aggregate type for a specific key.
    void set_aggregate(const std::string& key_id, AVAggregateType aggregate);

    /// Get the aggregate type for a specific key.
    [[nodiscard]] auto get_aggregate(const std::string& key_id) const -> AVAggregateType;

    /// Compute all configured aggregates for the given AttributeView.
    [[nodiscard]] auto compute(const AttributeView& attribute_view) const
        -> std::vector<AVAggregateResult>;

    /// Compute a single aggregate for a specific key.
    [[nodiscard]] auto compute_single(const AttributeView& attribute_view,
                                      const std::string& key_id) const -> AVAggregateResult;

    /// Format a result for display.
    [[nodiscard]] static auto format_result(const AVAggregateResult& result) -> std::string;

    /// Get all configured aggregates.
    [[nodiscard]] auto configs() const -> std::vector<AVAggregateConfig>;

    /// Clear all aggregate configurations.
    void clear();

private:
    std::unordered_map<std::string, AVAggregateType> aggregates_;

    /// Extract numeric values from a column.
    [[nodiscard]] static auto extract_numbers(const AttributeView& attribute_view,
                                              const std::string& key_id) -> std::vector<double>;

    /// Count non-empty values in a column.
    [[nodiscard]] static auto count_values(const AttributeView& attribute_view,
                                           const std::string& key_id)
        -> std::pair<size_t, size_t>; // (total, non-empty)

    /// Compute a single aggregate type.
    [[nodiscard]] static auto compute_aggregate(const AttributeView& attribute_view,
                                                const std::string& key_id,
                                                AVAggregateType aggregate) -> double;
};

} // namespace markamp::core::av
