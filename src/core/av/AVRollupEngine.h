#pragma once

#include "AVTypes.h"
#include "AVValue.h"
#include "AttributeView.h"

#include <string>
#include <vector>

namespace markamp::core::av
{

/// Rollup calculation engine: computes aggregates over related values.
class AVRollupEngine
{
public:
    /// Calculate a rollup result from a list of source values.
    [[nodiscard]] auto calculate(const std::vector<AVValue>& values,
                                 AVRollupCalc calc,
                                 AVKeyType target_type) const -> AVValueVariant;

private:
    [[nodiscard]] auto calc_count_all(const std::vector<AVValue>& values) const -> double;
    [[nodiscard]] auto calc_count_values(const std::vector<AVValue>& values) const -> double;
    [[nodiscard]] auto calc_count_unique(const std::vector<AVValue>& values) const -> double;
    [[nodiscard]] auto calc_count_empty(const std::vector<AVValue>& values) const -> double;
    [[nodiscard]] auto calc_sum(const std::vector<AVValue>& values) const -> double;
    [[nodiscard]] auto calc_average(const std::vector<AVValue>& values) const -> double;
    [[nodiscard]] auto calc_median(const std::vector<AVValue>& values) const -> double;
    [[nodiscard]] auto calc_min(const std::vector<AVValue>& values) const -> double;
    [[nodiscard]] auto calc_max(const std::vector<AVValue>& values) const -> double;
    [[nodiscard]] auto calc_range(const std::vector<AVValue>& values) const -> double;
    [[nodiscard]] auto calc_date_earliest(const std::vector<AVValue>& values) const -> int64_t;
    [[nodiscard]] auto calc_date_latest(const std::vector<AVValue>& values) const -> int64_t;
    [[nodiscard]] auto calc_checked_count(const std::vector<AVValue>& values) const -> double;
    [[nodiscard]] auto calc_unchecked_count(const std::vector<AVValue>& values) const -> double;

    [[nodiscard]] auto extract_numbers(const std::vector<AVValue>& values) const
        -> std::vector<double>;
    [[nodiscard]] auto extract_timestamps(const std::vector<AVValue>& values) const
        -> std::vector<int64_t>;
};

} // namespace markamp::core::av
