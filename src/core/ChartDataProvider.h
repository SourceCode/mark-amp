/// @file ChartDataProvider.h
/// @brief V9 Phase 48 — Chart dataset management for data visualization.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Supported chart types.
enum class ChartType : uint8_t
{
    kBar = 0,
    kLine = 1,
    kPie = 2,
    kScatter = 3,
    kHistogram = 4,
};

/// A single data series within a chart.
struct ChartSeries
{
    std::string label;
    std::vector<double> values;
    std::string color; ///< CSS color string
};

/// A complete chart dataset.
struct ChartDataSet
{
    std::string dataset_id;
    std::string name;
    ChartType type{ChartType::kBar};
    std::vector<std::string> labels; ///< X-axis labels
    std::vector<ChartSeries> series;
};

/// Manages chart datasets for data visualization.
class ChartDataProvider
{
public:
    ChartDataProvider() = default;

    // ── Dataset lifecycle ─────────────────────────────────────────────
    auto create_dataset(const std::string& name, ChartType type) -> std::string;
    auto delete_dataset(const std::string& dataset_id) -> bool;
    [[nodiscard]] auto get_dataset(const std::string& dataset_id) const -> const ChartDataSet*;
    [[nodiscard]] auto list_datasets() const -> std::vector<const ChartDataSet*>;

    // ── Series management ─────────────────────────────────────────────
    auto add_series(const std::string& dataset_id, ChartSeries series_data) -> bool;
    auto set_labels(const std::string& dataset_id, std::vector<std::string> labels_data) -> bool;

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto dataset_count() const -> int;
    void clear();

private:
    std::vector<ChartDataSet> datasets_;
    int next_id_{1};
};

} // namespace markamp::core
