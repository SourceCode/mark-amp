// ============================================================================
// File: src/core/SyncConflictManager.h
// Phase 27: Cloud Sync & Collaboration — Conflict resolution management
// ============================================================================
#pragma once

#include "SyncTypes.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class SyncEngine;

// Conflict resolution record — tracks how a conflict was resolved.
struct ConflictResolution
{
    std::string file_path;
    SyncConflictResolution strategy{SyncConflictResolution::AskUser};
    int64_t resolved_at{0};
    std::string kept_version; // "local", "remote", "both", "merged"
};

// SyncConflictManager — manages conflict detection, resolution, and history.
class SyncConflictManager
{
public:
    SyncConflictManager(EventBus& event_bus, SyncEngine& sync_engine);

    // Set pending conflicts from a sync operation.
    auto set_pending_conflicts(const std::vector<SyncConflict>& conflicts) -> void;

    // Get all unresolved conflicts.
    [[nodiscard]] auto pending_conflicts() const -> const std::vector<SyncConflict>&;
    [[nodiscard]] auto pending_count() const -> int32_t;
    [[nodiscard]] auto has_conflicts() const -> bool;

    // Resolve a single conflict by file path.
    auto resolve_conflict(const std::string& file_path, SyncConflictResolution strategy) -> bool;

    // Resolve all pending conflicts with the same strategy.
    auto resolve_all(SyncConflictResolution strategy) -> int32_t;

    // Auto-resolve using default strategy (KeepNewer).
    auto auto_resolve() -> int32_t;

    // Set the default resolution strategy.
    auto set_default_strategy(SyncConflictResolution strategy) -> void;
    [[nodiscard]] auto default_strategy() const -> SyncConflictResolution;

    // Find a specific conflict by file path.
    [[nodiscard]] auto find_conflict(const std::string& file_path) const
        -> std::optional<SyncConflict>;

    // Resolution history.
    [[nodiscard]] auto resolution_history() const -> const std::vector<ConflictResolution>&;
    [[nodiscard]] auto resolution_count() const -> int32_t;
    auto clear_history() -> void;

    // Clear all pending conflicts (after sync completes).
    auto clear_pending() -> void;

private:
    EventBus& event_bus_;
    [[maybe_unused]] SyncEngine& sync_engine_;

    std::vector<SyncConflict> pending_conflicts_;
    std::vector<ConflictResolution> resolution_history_;
    SyncConflictResolution default_strategy_{SyncConflictResolution::KeepNewer};

    // Record a resolution in history.
    auto record_resolution(const std::string& file_path, SyncConflictResolution strategy) -> void;
};

} // namespace markamp::core
