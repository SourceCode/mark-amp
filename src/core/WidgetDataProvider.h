/// @file WidgetDataProvider.h
/// @brief V9 Phase 41 — Data binding for sidebar widgets with refresh and caching.
#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Types of data a widget can display.
enum class WidgetDataType : uint8_t
{
    kList = 0,   ///< List of items
    kTree = 1,   ///< Hierarchical tree
    kChart = 2,  ///< Chart/graph data
    kText = 3,   ///< Plain text content
    kTable = 4,  ///< Tabular data
    kCustom = 5, ///< Custom rendered content
};

/// A single data item for a widget.
struct WidgetDataItem
{
    std::string item_id;
    std::string label;
    std::string icon;
    std::string tooltip;
    std::string value;
    int indent_level{0}; ///< Nesting level for tree views
    bool selectable{true};
    bool expandable{false};
    bool expanded{false};
};

/// A registered data provider for a widget.
struct DataProviderRegistration
{
    std::string provider_id; ///< Unique identifier
    std::string widget_id;   ///< Widget this provides data for
    WidgetDataType data_type{WidgetDataType::kList};
    std::function<std::vector<WidgetDataItem>()> fetch_fn; ///< Data fetcher
    int refresh_interval_ms{0}; ///< Auto-refresh interval (0 = manual only)
    bool cached{false};         ///< Whether last result is cached
    std::chrono::system_clock::time_point last_refresh;
};

/// Manages data providers for sidebar widgets.
///
/// Widgets bind to data providers that supply their content. Providers
/// support manual and auto-refresh, caching of results, and typed data.
class WidgetDataProviderManager
{
public:
    WidgetDataProviderManager() = default;

    // ── Provider management ───────────────────────────────────────────
    void register_provider(DataProviderRegistration provider);
    auto unregister_provider(const std::string& provider_id) -> bool;
    [[nodiscard]] auto find_provider(const std::string& provider_id) const
        -> const DataProviderRegistration*;
    [[nodiscard]] auto provider_count() const -> int;

    // ── Data fetching ─────────────────────────────────────────────────
    auto refresh(const std::string& provider_id) -> std::vector<WidgetDataItem>;
    [[nodiscard]] auto cached_data(const std::string& provider_id) const
        -> const std::vector<WidgetDataItem>&;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto providers_for_widget(const std::string& widget_id) const
        -> std::vector<const DataProviderRegistration*>;
    [[nodiscard]] auto all_providers() const -> std::vector<const DataProviderRegistration*>;

    void clear_all();

private:
    std::vector<DataProviderRegistration> providers_;
    std::vector<std::vector<WidgetDataItem>> cached_results_;
    static const std::vector<WidgetDataItem> empty_data_;

    auto find_index(const std::string& provider_id) const -> int;
};

} // namespace markamp::core
