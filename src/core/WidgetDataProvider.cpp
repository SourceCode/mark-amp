/// @file WidgetDataProvider.cpp
/// @brief V9 Phase 41 — WidgetDataProviderManager implementation.

#include "WidgetDataProvider.h"

#include <algorithm>

namespace markamp::core
{

const std::vector<WidgetDataItem> WidgetDataProviderManager::empty_data_{};

void WidgetDataProviderManager::register_provider(DataProviderRegistration provider)
{
    if (provider.provider_id.empty())
    {
        provider.provider_id = "dp_" + std::to_string(providers_.size());
    }
    providers_.push_back(std::move(provider));
    cached_results_.emplace_back();
}

auto WidgetDataProviderManager::unregister_provider(const std::string& provider_id) -> bool
{
    auto idx = find_index(provider_id);
    if (idx < 0)
    {
        return false;
    }
    providers_.erase(providers_.begin() + idx);
    cached_results_.erase(cached_results_.begin() + idx);
    return true;
}

auto WidgetDataProviderManager::find_provider(const std::string& provider_id) const
    -> const DataProviderRegistration*
{
    for (const auto& provider : providers_)
    {
        if (provider.provider_id == provider_id)
        {
            return &provider;
        }
    }
    return nullptr;
}

auto WidgetDataProviderManager::provider_count() const -> int
{
    return static_cast<int>(providers_.size());
}

auto WidgetDataProviderManager::refresh(const std::string& provider_id)
    -> std::vector<WidgetDataItem>
{
    auto idx = find_index(provider_id);
    if (idx < 0)
    {
        return {};
    }

    auto& provider = providers_[static_cast<size_t>(idx)];
    if (provider.fetch_fn)
    {
        auto result = provider.fetch_fn();
        cached_results_[static_cast<size_t>(idx)] = result;
        provider.cached = true;
        provider.last_refresh = std::chrono::system_clock::now();
        return result;
    }
    return {};
}

auto WidgetDataProviderManager::cached_data(const std::string& provider_id) const
    -> const std::vector<WidgetDataItem>&
{
    auto idx = find_index(provider_id);
    if (idx < 0)
    {
        return empty_data_;
    }
    return cached_results_[static_cast<size_t>(idx)];
}

auto WidgetDataProviderManager::providers_for_widget(const std::string& widget_id) const
    -> std::vector<const DataProviderRegistration*>
{
    std::vector<const DataProviderRegistration*> result;
    for (const auto& provider : providers_)
    {
        if (provider.widget_id == widget_id)
        {
            result.push_back(&provider);
        }
    }
    return result;
}

auto WidgetDataProviderManager::all_providers() const
    -> std::vector<const DataProviderRegistration*>
{
    std::vector<const DataProviderRegistration*> result;
    result.reserve(providers_.size());
    for (const auto& provider : providers_)
    {
        result.push_back(&provider);
    }
    return result;
}

void WidgetDataProviderManager::clear_all()
{
    providers_.clear();
    cached_results_.clear();
}

auto WidgetDataProviderManager::find_index(const std::string& provider_id) const -> int
{
    for (int idx = 0; idx < static_cast<int>(providers_.size()); ++idx)
    {
        if (providers_[static_cast<size_t>(idx)].provider_id == provider_id)
        {
            return idx;
        }
    }
    return -1;
}

} // namespace markamp::core
