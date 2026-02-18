#include "AVAggregateRow.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Aggregate type string conversion
// ════════════════════════════════════════════════════════════

auto aggregate_type_to_string(AVAggregateType type) -> std::string
{
    switch (type)
    {
        case AVAggregateType::None:
            return "none";
        case AVAggregateType::Count:
            return "count";
        case AVAggregateType::CountValues:
            return "count_values";
        case AVAggregateType::CountEmpty:
            return "count_empty";
        case AVAggregateType::Sum:
            return "sum";
        case AVAggregateType::Average:
            return "average";
        case AVAggregateType::Min:
            return "min";
        case AVAggregateType::Max:
            return "max";
        case AVAggregateType::Median:
            return "median";
        case AVAggregateType::Range:
            return "range";
        case AVAggregateType::PercentEmpty:
            return "percent_empty";
        case AVAggregateType::PercentFilled:
            return "percent_filled";
    }
    return "none";
}

auto string_to_aggregate_type(const std::string& str) -> AVAggregateType
{
    if (str == "count")
    {
        return AVAggregateType::Count;
    }
    if (str == "count_values")
    {
        return AVAggregateType::CountValues;
    }
    if (str == "count_empty")
    {
        return AVAggregateType::CountEmpty;
    }
    if (str == "sum")
    {
        return AVAggregateType::Sum;
    }
    if (str == "average")
    {
        return AVAggregateType::Average;
    }
    if (str == "min")
    {
        return AVAggregateType::Min;
    }
    if (str == "max")
    {
        return AVAggregateType::Max;
    }
    if (str == "median")
    {
        return AVAggregateType::Median;
    }
    if (str == "range")
    {
        return AVAggregateType::Range;
    }
    if (str == "percent_empty")
    {
        return AVAggregateType::PercentEmpty;
    }
    if (str == "percent_filled")
    {
        return AVAggregateType::PercentFilled;
    }
    return AVAggregateType::None;
}

// ════════════════════════════════════════════════════════════
// Configuration
// ════════════════════════════════════════════════════════════

void AVAggregateRow::set_aggregate(const std::string& key_id, AVAggregateType aggregate)
{
    if (aggregate == AVAggregateType::None)
    {
        aggregates_.erase(key_id);
    }
    else
    {
        aggregates_[key_id] = aggregate;
    }
}

auto AVAggregateRow::get_aggregate(const std::string& key_id) const -> AVAggregateType
{
    auto iter = aggregates_.find(key_id);
    if (iter != aggregates_.end())
    {
        return iter->second;
    }
    return AVAggregateType::None;
}

auto AVAggregateRow::configs() const -> std::vector<AVAggregateConfig>
{
    std::vector<AVAggregateConfig> result;
    result.reserve(aggregates_.size());
    for (const auto& [key_id, agg_type] : aggregates_)
    {
        result.push_back({key_id, agg_type});
    }
    return result;
}

void AVAggregateRow::clear()
{
    aggregates_.clear();
}

// ════════════════════════════════════════════════════════════
// Value extraction helpers
// ════════════════════════════════════════════════════════════

auto AVAggregateRow::extract_numbers(const AttributeView& attribute_view, const std::string& key_id)
    -> std::vector<double>
{
    std::vector<double> numbers;
    auto block_ids = attribute_view.row_block_ids();

    for (const auto& bid : block_ids)
    {
        const auto* val = attribute_view.get_value(key_id, bid);
        if (val != nullptr)
        {
            if (const auto* num = val->as_number())
            {
                if (num->is_not_empty)
                {
                    numbers.push_back(num->content);
                }
            }
            else if (const auto* chk = val->as_checkbox())
            {
                numbers.push_back(chk->checked ? 1.0 : 0.0);
            }
        }
    }

    return numbers;
}

auto AVAggregateRow::count_values(const AttributeView& attribute_view, const std::string& key_id)
    -> std::pair<size_t, size_t>
{
    auto block_ids = attribute_view.row_block_ids();
    size_t total = block_ids.size();
    size_t non_empty = 0;

    for (const auto& bid : block_ids)
    {
        const auto* val = attribute_view.get_value(key_id, bid);
        if (val != nullptr && !val->is_empty())
        {
            ++non_empty;
        }
    }

    return {total, non_empty};
}

// ════════════════════════════════════════════════════════════
// Aggregate computation
// ════════════════════════════════════════════════════════════

auto AVAggregateRow::compute_aggregate(const AttributeView& attribute_view,
                                       const std::string& key_id,
                                       AVAggregateType aggregate) -> double
{
    switch (aggregate)
    {
        case AVAggregateType::None:
            return 0.0;

        case AVAggregateType::Count:
        {
            const auto [total, _] = count_values(attribute_view, key_id);
            return static_cast<double>(total);
        }

        case AVAggregateType::CountValues:
        {
            auto [_, non_empty] = count_values(attribute_view, key_id);
            return static_cast<double>(non_empty);
        }

        case AVAggregateType::CountEmpty:
        {
            auto [total, non_empty] = count_values(attribute_view, key_id);
            return static_cast<double>(total - non_empty);
        }

        case AVAggregateType::Sum:
        {
            auto nums = extract_numbers(attribute_view, key_id);
            return std::accumulate(nums.begin(), nums.end(), 0.0);
        }

        case AVAggregateType::Average:
        {
            auto nums = extract_numbers(attribute_view, key_id);
            if (nums.empty())
            {
                return 0.0;
            }
            return std::accumulate(nums.begin(), nums.end(), 0.0) /
                   static_cast<double>(nums.size());
        }

        case AVAggregateType::Min:
        {
            auto nums = extract_numbers(attribute_view, key_id);
            if (nums.empty())
            {
                return 0.0;
            }
            return *std::min_element(nums.begin(), nums.end());
        }

        case AVAggregateType::Max:
        {
            auto nums = extract_numbers(attribute_view, key_id);
            if (nums.empty())
            {
                return 0.0;
            }
            return *std::max_element(nums.begin(), nums.end());
        }

        case AVAggregateType::Median:
        {
            auto nums = extract_numbers(attribute_view, key_id);
            if (nums.empty())
            {
                return 0.0;
            }
            std::sort(nums.begin(), nums.end());
            const size_t mid = nums.size() / 2;
            if (nums.size() % 2 == 0)
            {
                return (nums[mid - 1] + nums[mid]) / 2.0;
            }
            return nums[mid];
        }

        case AVAggregateType::Range:
        {
            auto nums = extract_numbers(attribute_view, key_id);
            if (nums.empty())
            {
                return 0.0;
            }
            auto [min_it, max_it] = std::minmax_element(nums.begin(), nums.end());
            return *max_it - *min_it;
        }

        case AVAggregateType::PercentEmpty:
        {
            auto [total, non_empty] = count_values(attribute_view, key_id);
            if (total == 0)
            {
                return 0.0;
            }
            return static_cast<double>(total - non_empty) / static_cast<double>(total) * 100.0;
        }

        case AVAggregateType::PercentFilled:
        {
            auto [total, non_empty] = count_values(attribute_view, key_id);
            if (total == 0)
            {
                return 0.0;
            }
            return static_cast<double>(non_empty) / static_cast<double>(total) * 100.0;
        }
    }

    return 0.0;
}

// ════════════════════════════════════════════════════════════
// Public compute methods
// ════════════════════════════════════════════════════════════

auto AVAggregateRow::compute(const AttributeView& attribute_view) const
    -> std::vector<AVAggregateResult>
{
    std::vector<AVAggregateResult> results;

    for (const auto& [key_id, agg_type] : aggregates_)
    {
        AVAggregateResult result;
        result.key_id = key_id;
        result.aggregate = agg_type;
        result.numeric_result = compute_aggregate(attribute_view, key_id, agg_type);
        result.formatted_result = format_result(result);
        results.push_back(result);
    }

    return results;
}

auto AVAggregateRow::compute_single(const AttributeView& attribute_view,
                                    const std::string& key_id) const -> AVAggregateResult
{
    AVAggregateResult result;
    result.key_id = key_id;

    auto iter = aggregates_.find(key_id);
    if (iter == aggregates_.end())
    {
        return result;
    }

    result.aggregate = iter->second;
    result.numeric_result = compute_aggregate(attribute_view, key_id, iter->second);
    result.formatted_result = format_result(result);
    return result;
}

auto AVAggregateRow::format_result(const AVAggregateResult& result) -> std::string
{
    std::ostringstream oss;

    switch (result.aggregate)
    {
        case AVAggregateType::None:
            return "";

        case AVAggregateType::Count:
        case AVAggregateType::CountValues:
        case AVAggregateType::CountEmpty:
            oss << static_cast<int>(result.numeric_result);
            break;

        case AVAggregateType::PercentEmpty:
        case AVAggregateType::PercentFilled:
            oss << std::round(result.numeric_result * 10.0) / 10.0 << "%";
            break;

        default:
            // For Sum, Average, Min, Max, Median, Range
            if (result.numeric_result == std::floor(result.numeric_result))
            {
                oss << static_cast<int64_t>(result.numeric_result);
            }
            else
            {
                oss << result.numeric_result;
            }
            break;
    }

    return oss.str();
}

} // namespace markamp::core::av
