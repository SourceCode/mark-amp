/// FaultDomain.cpp — V7 Phase 38: Subsystem fault domains

#include "FaultDomain.h"

#include <chrono>

namespace markamp::core
{

auto health_name(DomainHealth health) -> std::string
{
    switch (health)
    {
        case DomainHealth::kHealthy:
            return "Healthy";
        case DomainHealth::kDegraded:
            return "Degraded";
        case DomainHealth::kFaulted:
            return "Faulted";
        case DomainHealth::kIsolated:
            return "Isolated";
    }
    return "Unknown";
}

// ══════════════════════════════════════════════════════════════════════════════
// Fault Domain Registry
// ══════════════════════════════════════════════════════════════════════════════

FaultDomainRegistry::FaultDomainRegistry()
{
    // Pre-register 8 standard domains
    const std::vector<std::string> default_domains = {"Rendering",
                                                      "ExtensionHost",
                                                      "MarkdownEngine",
                                                      "YAMLParsing",
                                                      "Workspace",
                                                      "Network",
                                                      "Editor",
                                                      "Search"};

    for (const auto& name : default_domains)
    {
        domains_[name] = FaultDomain{.name = name};
    }
}

void FaultDomainRegistry::register_domain(const std::string& name)
{
    const std::lock_guard lock(mutex_);
    if (!domains_.contains(name))
    {
        domains_[name] = FaultDomain{.name = name};
    }
}

void FaultDomainRegistry::record_error(const std::string& domain_name,
                                       const std::string& error_message)
{
    const std::lock_guard lock(mutex_);
    auto iter = domains_.find(domain_name);
    if (iter == domains_.end())
    {
        return;
    }

    auto& domain = iter->second;
    domain.error_count++;
    domain.last_error_message = error_message;
    domain.last_error_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                               std::chrono::system_clock::now().time_since_epoch())
                               .count();

    // Transition health based on error count
    if (domain.health == DomainHealth::kIsolated)
    {
        return; // Don't change isolated domains
    }

    if (domain.error_count >= faulted_threshold_)
    {
        domain.health = DomainHealth::kFaulted;
    }
    else if (domain.error_count >= degraded_threshold_)
    {
        domain.health = DomainHealth::kDegraded;
    }
}

auto FaultDomainRegistry::health(const std::string& domain_name) const -> DomainHealth
{
    const std::lock_guard lock(mutex_);
    auto iter = domains_.find(domain_name);
    if (iter == domains_.end())
    {
        return DomainHealth::kHealthy;
    }
    return iter->second.health;
}

void FaultDomainRegistry::set_health(const std::string& domain_name, DomainHealth health_state)
{
    const std::lock_guard lock(mutex_);
    auto iter = domains_.find(domain_name);
    if (iter != domains_.end())
    {
        iter->second.health = health_state;
    }
}

void FaultDomainRegistry::isolate(const std::string& domain_name)
{
    set_health(domain_name, DomainHealth::kIsolated);
}

void FaultDomainRegistry::recover(const std::string& domain_name)
{
    const std::lock_guard lock(mutex_);
    auto iter = domains_.find(domain_name);
    if (iter != domains_.end())
    {
        iter->second.health = DomainHealth::kHealthy;
        iter->second.error_count = 0;
        iter->second.last_error_message.clear();
    }
}

auto FaultDomainRegistry::is_operational(const std::string& domain_name) const -> bool
{
    const std::lock_guard lock(mutex_);
    auto iter = domains_.find(domain_name);
    if (iter == domains_.end())
    {
        return true;
    }
    return iter->second.health <= DomainHealth::kDegraded;
}

auto FaultDomainRegistry::domain_names() const -> std::vector<std::string>
{
    const std::lock_guard lock(mutex_);
    std::vector<std::string> names;
    names.reserve(domains_.size());
    for (const auto& [name, _] : domains_)
    {
        names.push_back(name);
    }
    return names;
}

auto FaultDomainRegistry::all_domains() const -> std::vector<FaultDomain>
{
    const std::lock_guard lock(mutex_);
    std::vector<FaultDomain> result;
    result.reserve(domains_.size());
    for (const auto& [_, domain] : domains_)
    {
        result.push_back(domain);
    }
    return result;
}

auto FaultDomainRegistry::domain_count() const -> size_t
{
    const std::lock_guard lock(mutex_);
    return domains_.size();
}

void FaultDomainRegistry::set_thresholds(size_t degraded_threshold, size_t faulted_threshold)
{
    const std::lock_guard lock(mutex_);
    degraded_threshold_ = degraded_threshold;
    faulted_threshold_ = faulted_threshold;
}

} // namespace markamp::core
