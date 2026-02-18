/// @file ChartDataProvider.cpp
/// @brief V9 Phase 48 — ChartDataProvider implementation.

#include "ChartDataProvider.h"

#include <algorithm>

namespace markamp::core
{

auto ChartDataProvider::create_dataset(const std::string& name, ChartType type) -> std::string
{
    ChartDataSet dataset;
    dataset.dataset_id = "chart_" + std::to_string(next_id_++);
    dataset.name = name;
    dataset.type = type;
    datasets_.push_back(std::move(dataset));
    return datasets_.back().dataset_id;
}

auto ChartDataProvider::delete_dataset(const std::string& dataset_id) -> bool
{
    auto iter = std::remove_if(datasets_.begin(),
                               datasets_.end(),
                               [&](const ChartDataSet& ds) { return ds.dataset_id == dataset_id; });
    if (iter == datasets_.end())
    {
        return false;
    }
    datasets_.erase(iter, datasets_.end());
    return true;
}

auto ChartDataProvider::get_dataset(const std::string& dataset_id) const -> const ChartDataSet*
{
    for (const auto& ds : datasets_)
    {
        if (ds.dataset_id == dataset_id)
        {
            return &ds;
        }
    }
    return nullptr;
}

auto ChartDataProvider::list_datasets() const -> std::vector<const ChartDataSet*>
{
    std::vector<const ChartDataSet*> result;
    result.reserve(datasets_.size());
    for (const auto& ds : datasets_)
    {
        result.push_back(&ds);
    }
    return result;
}

auto ChartDataProvider::add_series(const std::string& dataset_id, ChartSeries series_data) -> bool
{
    for (auto& ds : datasets_)
    {
        if (ds.dataset_id == dataset_id)
        {
            ds.series.push_back(std::move(series_data));
            return true;
        }
    }
    return false;
}

auto ChartDataProvider::set_labels(const std::string& dataset_id,
                                   std::vector<std::string> labels_data) -> bool
{
    for (auto& ds : datasets_)
    {
        if (ds.dataset_id == dataset_id)
        {
            ds.labels = std::move(labels_data);
            return true;
        }
    }
    return false;
}

auto ChartDataProvider::dataset_count() const -> int
{
    return static_cast<int>(datasets_.size());
}

void ChartDataProvider::clear()
{
    datasets_.clear();
}

} // namespace markamp::core
