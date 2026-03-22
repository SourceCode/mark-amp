/// @file ControlExecutionTracer.h
/// @brief V21 Phase 01 — Control execution tracing and dead-affordance detection.
///
/// Records every control activation event with structured metadata. Detects
/// controls that render but never execute a real workflow (dead affordances)
/// by cross-referencing rendered controls against activation records.
#pragma once

#include "ControlActionManifest.h"

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// ActionActivation — structured activation event
// ============================================================================

/// Records a single control activation.
struct ActionActivation
{
    std::string action_id;                                ///< Canonical action ID
    ControlSurface surface{ControlSurface::kOther};       ///< Surface the action was triggered from
    std::chrono::steady_clock::time_point timestamp;      ///< When the activation occurred
    bool success{false};                                  ///< Whether the handler returned true
    std::string error_message;                            ///< Error detail if handler failed
    int duration_us{0};                                   ///< Handler execution duration in microseconds
};

// ============================================================================
// ControlExecutionTracer — tracing engine
// ============================================================================

/// Traces control activations and detects dead affordances.
class ControlExecutionTracer
{
public:
    ControlExecutionTracer() = default;

    // ── Recording ──

    /// Record an activation event.
    void record(ActionActivation activation);

    /// Record an activation from an action_id and surface with auto-timestamp.
    void record_activation(const std::string& action_id,
                           ControlSurface surface,
                           bool success,
                           int duration_us = 0);

    // ── Query ──

    /// Get all activation records.
    [[nodiscard]] auto all_activations() const -> const std::vector<ActionActivation>&;

    /// Get activations for a specific action.
    [[nodiscard]] auto activations_for(const std::string& action_id) const
        -> std::vector<ActionActivation>;

    /// Get activations from a specific surface.
    [[nodiscard]] auto activations_from_surface(ControlSurface surface) const
        -> std::vector<ActionActivation>;

    /// Get the total count of activation records.
    [[nodiscard]] auto activation_count() const noexcept -> std::size_t;

    /// Get the count of unique action IDs that have been activated.
    [[nodiscard]] auto unique_action_count() const -> std::size_t;

    /// Get the count of failed activations.
    [[nodiscard]] auto failure_count() const -> std::size_t;

    // ── Dead Affordance Detection ──

    /// Register an action ID as "rendered" on a surface (meaning the user can see it).
    void mark_rendered(const std::string& action_id, ControlSurface surface);

    /// Get action IDs that have been rendered but never activated.
    [[nodiscard]] auto never_activated_actions() const -> std::vector<std::string>;

    /// Get action IDs that have been activated but always failed.
    [[nodiscard]] auto always_failing_actions() const -> std::vector<std::string>;

    /// Cross-reference with a manifest to identify dead affordances:
    /// actions in the manifest that have no handler OR are marked Dead/Stub.
    [[nodiscard]] auto detect_dead_affordances(const ControlActionManifest& manifest) const
        -> std::vector<const ActionEntry*>;

    // ── Aggregate Stats ──

    /// Per-action statistics.
    struct ActionStats
    {
        std::string action_id;
        int total_activations{0};
        int success_count{0};
        int failure_count{0};
        int average_duration_us{0};
    };

    /// Get stats for a specific action.
    [[nodiscard]] auto stats_for(const std::string& action_id) const -> ActionStats;

    /// Get stats for all activated actions.
    [[nodiscard]] auto all_stats() const -> std::vector<ActionStats>;

    // ── Management ──

    /// Clear all recorded activations.
    void clear();

    /// Export activation log as JSON string.
    [[nodiscard]] auto export_json() const -> std::string;

private:
    /// All recorded activations in chronological order.
    std::vector<ActionActivation> activations_;

    /// Set of action IDs that have been rendered on at least one surface.
    std::unordered_map<std::string, std::vector<ControlSurface>> rendered_actions_;
};

} // namespace markamp::core
