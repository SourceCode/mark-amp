/// @file SurfaceActionAuditor.cpp
/// @brief V21 Phase 07 — SurfaceActionAuditor implementation.

#include "SurfaceActionAuditor.h"
#include <algorithm>

namespace markamp::core
{

void SurfaceActionAuditor::register_control(SurfaceControlBinding binding)
{
    const auto id = binding.action_id;
    if (!controls_.contains(id)) insertion_order_.push_back(id);
    controls_[id] = std::move(binding);
}

void SurfaceActionAuditor::register_controls(std::vector<SurfaceControlBinding> bindings)
{
    for (auto& b : bindings) register_control(std::move(b));
}

auto SurfaceActionAuditor::get_control(const std::string& action_id) const
    -> const SurfaceControlBinding* {
    auto it = controls_.find(action_id);
    return it != controls_.end() ? &it->second : nullptr;
}

auto SurfaceActionAuditor::controls_for_surface(const std::string& surface_id) const
    -> std::vector<const SurfaceControlBinding*> {
    std::vector<const SurfaceControlBinding*> result;
    for (const auto& id : insertion_order_) {
        auto it = controls_.find(id);
        if (it != controls_.end() && it->second.surface_id == surface_id)
            result.push_back(&it->second);
    }
    return result;
}

auto SurfaceActionAuditor::controls_by_kind(SurfaceControlKind kind) const
    -> std::vector<const SurfaceControlBinding*> {
    std::vector<const SurfaceControlBinding*> result;
    for (const auto& id : insertion_order_) {
        auto it = controls_.find(id);
        if (it != controls_.end() && it->second.kind == kind)
            result.push_back(&it->second);
    }
    return result;
}

auto SurfaceActionAuditor::all_surfaces() const -> std::vector<std::string> {
    std::vector<std::string> surfaces;
    for (const auto& id : insertion_order_) {
        auto it = controls_.find(id);
        if (it != controls_.end()) {
            if (std::find(surfaces.begin(), surfaces.end(), it->second.surface_id) == surfaces.end())
                surfaces.push_back(it->second.surface_id);
        }
    }
    return surfaces;
}

auto SurfaceActionAuditor::control_count() const -> std::size_t { return controls_.size(); }

void SurfaceActionAuditor::gate_control(const std::string& action_id, const std::string& reason) {
    auto it = controls_.find(action_id);
    if (it != controls_.end()) { it->second.is_gated = true; it->second.gate_reason = reason; }
}

void SurfaceActionAuditor::ungate_control(const std::string& action_id) {
    auto it = controls_.find(action_id);
    if (it != controls_.end()) { it->second.is_gated = false; it->second.gate_reason.clear(); }
}

auto SurfaceActionAuditor::gated_controls() const -> std::vector<const SurfaceControlBinding*> {
    std::vector<const SurfaceControlBinding*> result;
    for (const auto& [id, c] : controls_) if (c.is_gated) result.push_back(&c);
    return result;
}

void SurfaceActionAuditor::refresh_from_manifest(const ControlActionManifest& manifest) {
    for (auto& [id, binding] : controls_) {
        const auto* action = manifest.get_action(binding.action_id);
        binding.is_bound = action != nullptr && action->has_handler();
        if (!binding.is_bound) binding.is_enabled = false;
    }
}

auto SurfaceActionAuditor::audit(const ControlActionManifest& manifest) const
    -> std::vector<SurfaceAuditEntry> {
    std::vector<SurfaceAuditEntry> entries;
    for (const auto& id : insertion_order_) {
        auto it = controls_.find(id);
        if (it == controls_.end()) continue;
        const auto& c = it->second;
        const auto* action = manifest.get_action(c.action_id);
        if (action == nullptr) {
            entries.push_back({.surface_id = c.surface_id, .action_id = c.action_id,
                .issue = "Control has no manifest action", .is_dead = true});
        } else if (!action->has_handler()) {
            entries.push_back({.surface_id = c.surface_id, .action_id = c.action_id,
                .issue = "Control action has no handler", .is_placeholder = true});
        }
    }
    return entries;
}

auto SurfaceActionAuditor::live_count() const -> std::size_t {
    std::size_t c = 0;
    for (const auto& [id, b] : controls_) if (b.is_bound && b.is_enabled && !b.is_gated) ++c;
    return c;
}

auto SurfaceActionAuditor::dead_count() const -> std::size_t {
    std::size_t c = 0;
    for (const auto& [id, b] : controls_) if (!b.is_bound) ++c;
    return c;
}

auto SurfaceActionAuditor::gated_count() const -> std::size_t {
    return gated_controls().size();
}

} // namespace markamp::core
