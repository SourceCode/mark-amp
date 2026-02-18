/// SubsystemReset.cpp — V7 Phase 37: Per-subsystem reset handlers

#include "SubsystemReset.h"

namespace markamp::core
{

void SubsystemResetCoordinator::register_handler(const std::string& subsystem_name,
                                                 ResetHandler handler)
{
    const std::lock_guard lock(mutex_);
    handlers_[subsystem_name] = std::move(handler);
}

void SubsystemResetCoordinator::unregister_handler(const std::string& subsystem_name)
{
    const std::lock_guard lock(mutex_);
    handlers_.erase(subsystem_name);
}

auto SubsystemResetCoordinator::reset_subsystem(const std::string& subsystem_name) -> Result<void>
{
    const std::lock_guard lock(mutex_);
    auto iter = handlers_.find(subsystem_name);
    if (iter == handlers_.end())
    {
        return std::unexpected(make_error(ErrorCode::NotFound,
                                          SubsystemId::Core,
                                          "No reset handler for subsystem: " + subsystem_name));
    }
    return iter->second();
}

auto SubsystemResetCoordinator::reset_all() -> std::vector<ResetResult>
{
    const std::lock_guard lock(mutex_);
    std::vector<ResetResult> results;
    results.reserve(handlers_.size());

    for (const auto& [name, handler] : handlers_)
    {
        auto result = handler();
        results.push_back(ResetResult{
            .subsystem_name = name,
            .success = result.has_value(),
            .error_message = result.has_value() ? "" : result.error().message,
        });
    }
    return results;
}

auto SubsystemResetCoordinator::has_handler(const std::string& subsystem_name) const -> bool
{
    const std::lock_guard lock(mutex_);
    return handlers_.contains(subsystem_name);
}

auto SubsystemResetCoordinator::registered_subsystems() const -> std::vector<std::string>
{
    const std::lock_guard lock(mutex_);
    std::vector<std::string> names;
    names.reserve(handlers_.size());
    for (const auto& [name, _] : handlers_)
    {
        names.push_back(name);
    }
    return names;
}

auto SubsystemResetCoordinator::handler_count() const -> size_t
{
    const std::lock_guard lock(mutex_);
    return handlers_.size();
}

} // namespace markamp::core
