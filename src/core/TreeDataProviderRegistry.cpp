#include "TreeDataProviderRegistry.h"

namespace markamp::core
{

void TreeDataProviderRegistry::register_provider(const std::string& view_id,
                                                 std::shared_ptr<ITreeDataProvider> provider)
{
    providers_[view_id] = std::move(provider);
}

auto TreeDataProviderRegistry::get_provider(const std::string& view_id) const
    -> std::shared_ptr<ITreeDataProvider>
{
    auto found = providers_.find(view_id);
    return found != providers_.end() ? found->second : nullptr;
}

auto TreeDataProviderRegistry::has_provider(const std::string& view_id) const -> bool
{
    return providers_.contains(view_id);
}

void TreeDataProviderRegistry::unregister_provider(const std::string& view_id)
{
    providers_.erase(view_id);
}

auto TreeDataProviderRegistry::view_ids() const -> std::vector<std::string>
{
    std::vector<std::string> ids;
    ids.reserve(providers_.size());
    for (const auto& [id, provider] : providers_)
    {
        ids.push_back(id);
    }
    return ids;
}

} // namespace markamp::core
