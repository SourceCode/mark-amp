#pragma once

#include <concepts>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace markamp::core
{

/// Lightweight service locator for wiring components without hard dependencies.
/// Thread-safe for read access after initial registration.
class ServiceRegistry
{
public:
    /// Register a service implementation under its interface type.
    template <typename Interface, typename Implementation>
        requires std::derived_from<Implementation, Interface>
    void register_service(std::shared_ptr<Implementation> impl)
    {
        std::lock_guard lock(mutex_);
        services_[std::type_index(typeid(Interface))] = std::shared_ptr<Interface>(std::move(impl));
    }

    /// Retrieve a service by interface type. Returns nullptr if not registered.
    template <typename Interface>
    [[nodiscard]] auto get() const -> std::shared_ptr<Interface>
    {
        std::lock_guard lock(mutex_);
        auto it = services_.find(std::type_index(typeid(Interface)));
        if (it == services_.end())
        {
            return nullptr;
        }
        return std::static_pointer_cast<Interface>(it->second);
    }

    /// Check if a service is registered.
    template <typename Interface>
    [[nodiscard]] auto has() const -> bool
    {
        std::lock_guard lock(mutex_);
        return services_.contains(std::type_index(typeid(Interface)));
    }

    /// Retrieve a service, throwing if not found.
    template <typename Interface>
    [[nodiscard]] auto require() const -> std::shared_ptr<Interface>
    {
        auto svc = get<Interface>();
        if (!svc)
        {
            throw std::runtime_error(std::string("Required service not registered: ") +
                                     typeid(Interface).name());
        }
        return svc;
    }

    /// Singleton access for the application-scoped registry.
    /// R20 Fix 40: Thread-safe — Meyers singleton is guaranteed safe in C++11+
    /// (static local initialization is thread-safe per [stmt.dcl]§6.7.4).
    [[nodiscard]] static auto instance() -> ServiceRegistry&
    {
        static ServiceRegistry registry;
        return registry;
    }

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::type_index, std::shared_ptr<void>> services_;
};

} // namespace markamp::core
