/// @file ControlExecutionTracer.cpp
/// @brief V21 Phase 01 — ControlExecutionTracer implementation.

#include "ControlExecutionTracer.h"

#include <algorithm>
#include <set>
#include <sstream>

namespace markamp::core
{

// ── Recording ──

void ControlExecutionTracer::record(ActionActivation activation)
{
    activations_.push_back(std::move(activation));
}

void ControlExecutionTracer::record_activation(const std::string& action_id,
                                                ControlSurface surface,
                                                bool success,
                                                int duration_us)
{
    ActionActivation activation;
    activation.action_id = action_id;
    activation.surface = surface;
    activation.timestamp = std::chrono::steady_clock::now();
    activation.success = success;
    activation.duration_us = duration_us;
    activations_.push_back(std::move(activation));
}

// ── Query ──

auto ControlExecutionTracer::all_activations() const -> const std::vector<ActionActivation>&
{
    return activations_;
}

auto ControlExecutionTracer::activations_for(const std::string& action_id) const
    -> std::vector<ActionActivation>
{
    std::vector<ActionActivation> result;
    for (const auto& activation : activations_)
    {
        if (activation.action_id == action_id)
        {
            result.push_back(activation);
        }
    }
    return result;
}

auto ControlExecutionTracer::activations_from_surface(ControlSurface surface) const
    -> std::vector<ActionActivation>
{
    std::vector<ActionActivation> result;
    for (const auto& activation : activations_)
    {
        if (activation.surface == surface)
        {
            result.push_back(activation);
        }
    }
    return result;
}

auto ControlExecutionTracer::activation_count() const noexcept -> std::size_t
{
    return activations_.size();
}

auto ControlExecutionTracer::unique_action_count() const -> std::size_t
{
    std::set<std::string> unique_ids;
    for (const auto& activation : activations_)
    {
        unique_ids.insert(activation.action_id);
    }
    return unique_ids.size();
}

auto ControlExecutionTracer::failure_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& activation : activations_)
    {
        if (!activation.success)
        {
            ++count;
        }
    }
    return count;
}

// ── Dead Affordance Detection ──

void ControlExecutionTracer::mark_rendered(const std::string& action_id, ControlSurface surface)
{
    auto& surfaces = rendered_actions_[action_id];
    for (const auto& s : surfaces)
    {
        if (s == surface)
        {
            return; // Already marked
        }
    }
    surfaces.push_back(surface);
}

auto ControlExecutionTracer::never_activated_actions() const -> std::vector<std::string>
{
    std::set<std::string> activated_ids;
    for (const auto& activation : activations_)
    {
        activated_ids.insert(activation.action_id);
    }

    std::vector<std::string> result;
    for (const auto& [id, surfaces] : rendered_actions_)
    {
        if (!activated_ids.contains(id))
        {
            result.push_back(id);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

auto ControlExecutionTracer::always_failing_actions() const -> std::vector<std::string>
{
    // Group by action_id, check if all activations failed
    std::unordered_map<std::string, std::pair<int, int>> counts; // total, success
    for (const auto& activation : activations_)
    {
        auto& [total, successes] = counts[activation.action_id];
        ++total;
        if (activation.success)
        {
            ++successes;
        }
    }

    std::vector<std::string> result;
    for (const auto& [id, pair] : counts)
    {
        const auto& [total, successes] = pair;
        if (total > 0 && successes == 0)
        {
            result.push_back(id);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

auto ControlExecutionTracer::detect_dead_affordances(const ControlActionManifest& manifest) const
    -> std::vector<const ActionEntry*>
{
    std::vector<const ActionEntry*> dead;
    for (const auto* action : manifest.all_actions())
    {
        if (!action->has_handler() ||
            action->validation_status == ActionValidationStatus::kDead ||
            action->validation_status == ActionValidationStatus::kStub)
        {
            dead.push_back(action);
        }
    }
    return dead;
}

// ── Aggregate Stats ──

auto ControlExecutionTracer::stats_for(const std::string& action_id) const -> ActionStats
{
    ActionStats stats;
    stats.action_id = action_id;

    int total_duration = 0;
    for (const auto& activation : activations_)
    {
        if (activation.action_id == action_id)
        {
            ++stats.total_activations;
            if (activation.success)
            {
                ++stats.success_count;
            }
            else
            {
                ++stats.failure_count;
            }
            total_duration += activation.duration_us;
        }
    }

    if (stats.total_activations > 0)
    {
        stats.average_duration_us = total_duration / stats.total_activations;
    }

    return stats;
}

auto ControlExecutionTracer::all_stats() const -> std::vector<ActionStats>
{
    std::set<std::string> seen;
    std::vector<ActionStats> result;

    for (const auto& activation : activations_)
    {
        if (seen.insert(activation.action_id).second)
        {
            result.push_back(stats_for(activation.action_id));
        }
    }

    return result;
}

// ── Management ──

void ControlExecutionTracer::clear()
{
    activations_.clear();
    rendered_actions_.clear();
}

auto ControlExecutionTracer::export_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\n  \"activations\": [\n";

    bool first = true;
    for (const auto& activation : activations_)
    {
        if (!first)
        {
            oss << ",\n";
        }
        first = false;
        oss << R"(    {"action_id": ")" << activation.action_id
            << R"(", "surface": ")" << control_surface_label(activation.surface)
            << R"(", "success": )" << (activation.success ? "true" : "false")
            << R"(, "duration_us": )" << activation.duration_us
            << "}";
    }

    oss << "\n  ]\n}";
    return oss.str();
}

} // namespace markamp::core
