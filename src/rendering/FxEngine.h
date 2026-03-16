/// FxEngine.h — Phase 46: FX Compositor Engine
///
/// Manages an ordered pipeline of effect passes with quality tiers,
/// per-surface targeting, and master enable/disable.
///
/// The engine does not perform actual GPU rendering; it orchestrates
/// which passes fire and in what order, providing a hook point for
/// the rendering subsystem.

#pragma once

#include "FxPass.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::rendering
{

/// Result of executing the FX pipeline for a single frame.
struct FxFrameResult
{
    std::size_t passes_executed{0}; ///< Number of passes that ran
    std::size_t passes_skipped{0};  ///< Number of passes skipped (quality/disabled)
    bool master_enabled{true};      ///< Whether master was on
    QualityTier active_tier{QualityTier::kCinematic};
};

/// Orchestrates ordered effect passes for real-time compositing.
///
/// Usage:
///   engine.add_pass("blur", FxPassType::kBlur, config);
///   engine.add_pass("glow", FxPassType::kGlow, config);
///   auto result = engine.execute_pipeline();
class FxEngine
{
public:
    explicit FxEngine(core::EventBus& event_bus);

    // ── Pass management ──

    /// Add a pass to the end of the pipeline.
    void
    add_pass(const std::string& pass_name, FxPassType pass_type, const FxPassConfig& pass_config);

    /// Remove a pass by name. Returns true if found and removed.
    auto remove_pass(const std::string& pass_name) -> bool;

    /// Move a pass to a new index in the pipeline.
    /// Returns true if the pass was found and reordered.
    auto reorder_pass(const std::string& pass_name, std::size_t new_index) -> bool;

    /// Get a pass by name (nullptr if not found).
    [[nodiscard]] auto get_pass(const std::string& pass_name) -> FxPass*;
    [[nodiscard]] auto get_pass(const std::string& pass_name) const -> const FxPass*;

    // ── Pipeline execution ──

    /// Execute all active passes in order. Returns execution summary.
    auto execute_pipeline() -> FxFrameResult;

    /// Execute passes only for a specific surface target.
    auto execute_for_surface(FxSurfaceTarget surface_target) -> FxFrameResult;

    // ── Quality tier ──

    void set_quality_tier(QualityTier tier);
    [[nodiscard]] auto quality_tier() const noexcept -> QualityTier;

    // ── Master toggle ──

    void set_master_enabled(bool enabled);
    [[nodiscard]] auto is_master_enabled() const noexcept -> bool;

    // ── Queries ──

    /// Total passes in the pipeline.
    [[nodiscard]] auto pass_count() const noexcept -> std::size_t;

    /// Passes that would execute at the current quality tier.
    [[nodiscard]] auto active_pass_count() const noexcept -> std::size_t;

    /// Get all pass names in execution order.
    [[nodiscard]] auto pass_names() const -> std::vector<std::string>;

    /// Check if any pass of a given type exists.
    [[nodiscard]] auto has_pass_type(FxPassType pass_type) const noexcept -> bool;

    /// (#67) Get unique pass types in the pipeline.
    [[nodiscard]] auto pass_types() const -> std::vector<FxPassType>;

    /// Number of passes that are disabled or below the current quality tier.
    [[nodiscard]] auto disabled_pass_count() const noexcept -> std::size_t
    {
        return pass_count() - active_pass_count();
    }

    /// Check if a pass with the given name exists.
    [[nodiscard]] auto has_pass(const std::string& pass_name) const -> bool
    {
        return get_pass(pass_name) != nullptr;
    }

    /// Convert quality tier to display string.
    [[nodiscard]] static auto tier_name(QualityTier tier) -> std::string_view;

private:
    core::EventBus& event_bus_;
    std::vector<std::unique_ptr<FxPass>> passes_;
    QualityTier quality_tier_{QualityTier::kBalanced};
    bool master_enabled_{true};
};

} // namespace markamp::rendering
