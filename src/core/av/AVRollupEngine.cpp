#include "AVRollupEngine.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>

namespace markamp::core::av
{

auto AVRollupEngine::extract_numbers(const std::vector<AVValue>& values) const
    -> std::vector<double>
{
    std::vector<double> result;
    result.reserve(values.size());
    for (const auto& val : values)
    {
        const auto* num_val = val.as_number();
        if (num_val && num_val->is_not_empty)
        {
            result.push_back(num_val->content);
        }
    }
    return result;
}

auto AVRollupEngine::extract_timestamps(const std::vector<AVValue>& values) const
    -> std::vector<int64_t>
{
    std::vector<int64_t> result;
    result.reserve(values.size());
    for (const auto& val : values)
    {
        const auto* date_val = val.as_date();
        if (date_val && date_val->is_not_empty)
        {
            result.push_back(date_val->content);
        }
        const auto* created_val = val.as_created();
        if (created_val && created_val->is_not_empty)
        {
            result.push_back(created_val->content);
        }
        const auto* updated_val = val.as_updated();
        if (updated_val && updated_val->is_not_empty)
        {
            result.push_back(updated_val->content);
        }
    }
    return result;
}

auto AVRollupEngine::calc_count_all(const std::vector<AVValue>& values) const -> double
{
    return static_cast<double>(values.size());
}

auto AVRollupEngine::calc_count_values(const std::vector<AVValue>& values) const -> double
{
    return static_cast<double>(std::count_if(
        values.begin(), values.end(), [](const AVValue& val) { return !val.is_empty(); }));
}

auto AVRollupEngine::calc_count_unique(const std::vector<AVValue>& values) const -> double
{
    std::set<std::string> unique_strs;
    for (const auto& val : values)
    {
        if (!val.is_empty())
        {
            unique_strs.insert(val.to_display_string());
        }
    }
    return static_cast<double>(unique_strs.size());
}

auto AVRollupEngine::calc_count_empty(const std::vector<AVValue>& values) const -> double
{
    return static_cast<double>(std::count_if(
        values.begin(), values.end(), [](const AVValue& val) { return val.is_empty(); }));
}

auto AVRollupEngine::calc_sum(const std::vector<AVValue>& values) const -> double
{
    const auto numbers = extract_numbers(values);
    return std::accumulate(numbers.begin(), numbers.end(), 0.0);
}

auto AVRollupEngine::calc_average(const std::vector<AVValue>& values) const -> double
{
    const auto numbers = extract_numbers(values);
    if (numbers.empty())
    {
        return 0.0;
    }
    return std::accumulate(numbers.begin(), numbers.end(), 0.0) /
           static_cast<double>(numbers.size());
}

auto AVRollupEngine::calc_median(const std::vector<AVValue>& values) const -> double
{
    auto numbers = extract_numbers(values);
    if (numbers.empty())
    {
        return 0.0;
    }
    std::sort(numbers.begin(), numbers.end());
    const auto mid = numbers.size() / 2;
    if (numbers.size() % 2 == 0)
    {
        return (numbers[mid - 1] + numbers[mid]) / 2.0;
    }
    return numbers[mid];
}

auto AVRollupEngine::calc_min(const std::vector<AVValue>& values) const -> double
{
    const auto numbers = extract_numbers(values);
    if (numbers.empty())
    {
        return 0.0;
    }
    return *std::min_element(numbers.begin(), numbers.end());
}

auto AVRollupEngine::calc_max(const std::vector<AVValue>& values) const -> double
{
    const auto numbers = extract_numbers(values);
    if (numbers.empty())
    {
        return 0.0;
    }
    return *std::max_element(numbers.begin(), numbers.end());
}

auto AVRollupEngine::calc_range(const std::vector<AVValue>& values) const -> double
{
    const auto numbers = extract_numbers(values);
    if (numbers.empty())
    {
        return 0.0;
    }
    const auto [min_it, max_it] = std::minmax_element(numbers.begin(), numbers.end());
    return *max_it - *min_it;
}

auto AVRollupEngine::calc_date_earliest(const std::vector<AVValue>& values) const -> int64_t
{
    const auto timestamps = extract_timestamps(values);
    if (timestamps.empty())
    {
        return 0;
    }
    return *std::min_element(timestamps.begin(), timestamps.end());
}

auto AVRollupEngine::calc_date_latest(const std::vector<AVValue>& values) const -> int64_t
{
    const auto timestamps = extract_timestamps(values);
    if (timestamps.empty())
    {
        return 0;
    }
    return *std::max_element(timestamps.begin(), timestamps.end());
}

auto AVRollupEngine::calc_checked_count(const std::vector<AVValue>& values) const -> double
{
    return static_cast<double>(std::count_if(values.begin(),
                                             values.end(),
                                             [](const AVValue& val)
                                             {
                                                 const auto* cb = val.as_checkbox();
                                                 return cb && cb->checked;
                                             }));
}

auto AVRollupEngine::calc_unchecked_count(const std::vector<AVValue>& values) const -> double
{
    return static_cast<double>(std::count_if(values.begin(),
                                             values.end(),
                                             [](const AVValue& val)
                                             {
                                                 const auto* cb = val.as_checkbox();
                                                 return !cb || !cb->checked;
                                             }));
}

auto AVRollupEngine::calculate(const std::vector<AVValue>& values,
                               AVRollupCalc calc,
                               AVKeyType /*target_type*/) const -> AVValueVariant
{
    double result_num = 0.0;

    switch (calc)
    {
        case AVRollupCalc::None:
            return {std::monostate{}};
        case AVRollupCalc::CountAll:
            result_num = calc_count_all(values);
            break;
        case AVRollupCalc::CountValues:
            result_num = calc_count_values(values);
            break;
        case AVRollupCalc::CountUniqueValues:
            result_num = calc_count_unique(values);
            break;
        case AVRollupCalc::CountEmpty:
            result_num = calc_count_empty(values);
            break;
        case AVRollupCalc::CountNotEmpty:
            result_num = calc_count_values(values);
            break;
        case AVRollupCalc::PercentEmpty:
        {
            const double total = calc_count_all(values);
            result_num = total > 0.0 ? calc_count_empty(values) / total : 0.0;
            break;
        }
        case AVRollupCalc::PercentNotEmpty:
        {
            const double total = calc_count_all(values);
            result_num = total > 0.0 ? calc_count_values(values) / total : 0.0;
            break;
        }
        case AVRollupCalc::Sum:
            result_num = calc_sum(values);
            break;
        case AVRollupCalc::Average:
            result_num = calc_average(values);
            break;
        case AVRollupCalc::Median:
            result_num = calc_median(values);
            break;
        case AVRollupCalc::Min:
            result_num = calc_min(values);
            break;
        case AVRollupCalc::Max:
            result_num = calc_max(values);
            break;
        case AVRollupCalc::Range:
            result_num = calc_range(values);
            break;
        case AVRollupCalc::DateEarliest:
        {
            const auto ts = calc_date_earliest(values);
            return AVValueVariant{AVValueDate{ts, 0, ts != 0, false, false, {}}};
        }
        case AVRollupCalc::DateLatest:
        {
            const auto ts = calc_date_latest(values);
            return AVValueVariant{AVValueDate{ts, 0, ts != 0, false, false, {}}};
        }
        case AVRollupCalc::Checked:
            result_num = calc_checked_count(values);
            break;
        case AVRollupCalc::Unchecked:
            result_num = calc_unchecked_count(values);
            break;
        case AVRollupCalc::PercentChecked:
        {
            const double total = calc_count_all(values);
            result_num = total > 0.0 ? calc_checked_count(values) / total : 0.0;
            break;
        }
        case AVRollupCalc::PercentUnchecked:
        {
            const double total = calc_count_all(values);
            result_num = total > 0.0 ? calc_unchecked_count(values) / total : 0.0;
            break;
        }
    }

    return AVValueVariant{AVValueNumber{result_num, true, AVNumberFormat::None, {}}};
}

} // namespace markamp::core::av
