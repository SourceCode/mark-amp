#include "FileSystemProviderRegistry.h"

namespace markamp::core
{

void FileSystemProviderRegistry::register_provider(const std::string& scheme,
                                                   std::shared_ptr<IFileSystemProvider> provider)
{
    providers_[scheme] = std::move(provider);
}

auto FileSystemProviderRegistry::get_provider(const std::string& scheme) const
    -> std::shared_ptr<IFileSystemProvider>
{
    auto found = providers_.find(scheme);
    return found != providers_.end() ? found->second : nullptr;
}

auto FileSystemProviderRegistry::has_provider(const std::string& scheme) const -> bool
{
    return providers_.contains(scheme);
}

void FileSystemProviderRegistry::unregister_provider(const std::string& scheme)
{
    providers_.erase(scheme);
}

auto FileSystemProviderRegistry::schemes() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    result.reserve(providers_.size());
    for (const auto& [scheme, provider] : providers_)
    {
        result.push_back(scheme);
    }
    return result;
}

} // namespace markamp::core
