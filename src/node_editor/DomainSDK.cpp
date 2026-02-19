// V11 Phase 19: Domain SDK & Custom Runtime Extensibility

#include "node_editor/DomainSDK.h"

namespace markamp::node_editor
{

auto DomainSDK::register_handler(DomainNodeHandler handler) -> bool
{
    if (handler.type_name.empty())
    {
        return false;
    }

    if (handlers_.contains(handler.type_name))
    {
        return false;
    }

    const auto kHandlerKey = handler.type_name;
    handlers_[kHandlerKey] = std::move(handler);
    return true;
}

auto DomainSDK::find_handler(const std::string& type_name) const -> const DomainNodeHandler*
{
    const auto kHandlerIt = handlers_.find(type_name);
    if (kHandlerIt == handlers_.end())
    {
        return nullptr;
    }
    return &kHandlerIt->second;
}

auto DomainSDK::all_handler_types() const -> std::vector<std::string>
{
    std::vector<std::string> types;
    types.reserve(handlers_.size());
    for (const auto& [key, handler] : handlers_)
    {
        types.push_back(key);
    }
    return types;
}

auto DomainSDK::handler_count() const -> std::size_t
{
    return handlers_.size();
}

auto DomainSDK::remove_handler(const std::string& type_name) -> bool
{
    return handlers_.erase(type_name) > 0;
}

void DomainSDK::clear()
{
    handlers_.clear();
}

} // namespace markamp::node_editor
