// V11 Phase 11: Domain Abstraction Layer & Runtime Contracts

#include "node_editor/DomainRegistry.h"

#include <algorithm>

namespace markamp::node_editor
{

auto DomainRegistry::register_runtime(std::unique_ptr<IDomainRuntime> runtime) -> bool
{
    if (!runtime)
    {
        return false;
    }

    std::lock_guard lock(mutex_);
    const int mode_key = static_cast<int>(runtime->graph_mode());

    if (runtimes_by_mode_.contains(mode_key))
    {
        return false;
    }

    runtimes_by_mode_[mode_key] = std::move(runtime);
    return true;
}

auto DomainRegistry::find(GraphMode mode) const -> IDomainRuntime*
{
    std::lock_guard lock(mutex_);
    const auto it = runtimes_by_mode_.find(static_cast<int>(mode));
    if (it == runtimes_by_mode_.end())
    {
        return nullptr;
    }
    return it->second.get();
}

auto DomainRegistry::find_by_id(const std::string& domain_id) const -> IDomainRuntime*
{
    std::lock_guard lock(mutex_);
    for (const auto& [mode_key, rt] : runtimes_by_mode_)
    {
        if (rt->domain_id() == domain_id)
        {
            return rt.get();
        }
    }
    return nullptr;
}

auto DomainRegistry::all_domain_ids() const -> std::vector<std::string>
{
    std::lock_guard lock(mutex_);
    std::vector<std::string> ids;
    ids.reserve(runtimes_by_mode_.size());
    for (const auto& [mode_key, rt] : runtimes_by_mode_)
    {
        ids.push_back(rt->domain_id());
    }
    return ids;
}

auto DomainRegistry::count() const -> std::size_t
{
    std::lock_guard lock(mutex_);
    return runtimes_by_mode_.size();
}

auto DomainRegistry::has_runtime(GraphMode mode) const -> bool
{
    std::lock_guard lock(mutex_);
    return runtimes_by_mode_.contains(static_cast<int>(mode));
}

void DomainRegistry::clear()
{
    std::lock_guard lock(mutex_);
    runtimes_by_mode_.clear();
}

} // namespace markamp::node_editor
