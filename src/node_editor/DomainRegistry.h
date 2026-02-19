#pragma once

// V11 Phase 11: Domain Abstraction Layer & Runtime Contracts
// Registration and discovery of domain runtimes.

#include "node_editor/IDomainRuntime.h"
#include "node_editor/NodeEditorTypes.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

// ============================================================================
// Domain Registry
// ============================================================================

/// Thread-safe registry for domain runtime discovery.
///
/// Maps GraphMode values to their corresponding IDomainRuntime implementations.
/// Runtimes are registered at startup and remain for the application lifetime.
class DomainRegistry
{
public:
    DomainRegistry() = default;

    /// Register a domain runtime. Returns false if a runtime for that GraphMode
    /// is already registered.
    auto register_runtime(std::unique_ptr<IDomainRuntime> runtime) -> bool;

    /// Find the runtime for a given GraphMode. Returns nullptr if none registered.
    [[nodiscard]] auto find(GraphMode mode) const -> IDomainRuntime*;

    /// Find by domain ID string. Returns nullptr if not found.
    [[nodiscard]] auto find_by_id(const std::string& domain_id) const -> IDomainRuntime*;

    /// List all registered domain IDs.
    [[nodiscard]] auto all_domain_ids() const -> std::vector<std::string>;

    /// Get the number of registered runtimes.
    [[nodiscard]] auto count() const -> std::size_t;

    /// Check if a runtime is registered for the given mode.
    [[nodiscard]] auto has_runtime(GraphMode mode) const -> bool;

    /// Remove all registered runtimes.
    void clear();

private:
    mutable std::mutex mutex_;
    std::unordered_map<int, std::unique_ptr<IDomainRuntime>> runtimes_by_mode_;
};

} // namespace markamp::node_editor
