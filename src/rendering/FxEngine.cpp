/// FxEngine.cpp — Phase 46: FX Compositor Engine Implementation

#include "FxEngine.h"

#include "core/EventBus.h"

#include <algorithm>
#include <iterator>
#include <spdlog/spdlog.h>

namespace markamp::rendering
{

FxEngine::FxEngine(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
    static_cast<void>(event_bus_); // Reserved for future event publishing.
}

// ── Pass management ──

void FxEngine::add_pass(const std::string& pass_name,
                        FxPassType pass_type,
                        const FxPassConfig& pass_config)
{
    passes_.push_back(std::make_unique<FxPass>(pass_name, pass_type, pass_config));
    spdlog::debug("FxEngine: added pass '{}' (type={})", pass_name, FxPass::type_name(pass_type));
}

auto FxEngine::remove_pass(const std::string& pass_name) -> bool
{
    auto iter = std::ranges::find_if(
        passes_, [&](const auto& pass_ptr) { return pass_ptr->name() == pass_name; });
    if (iter == passes_.end())
    {
        return false;
    }
    spdlog::debug("FxEngine: removed pass '{}'", pass_name);
    passes_.erase(iter);
    return true;
}

auto FxEngine::reorder_pass(const std::string& pass_name, std::size_t new_index) -> bool
{
    auto iter = std::ranges::find_if(
        passes_, [&](const auto& pass_ptr) { return pass_ptr->name() == pass_name; });
    if (iter == passes_.end())
    {
        return false;
    }

    const auto kClampedIndex = std::min(new_index, passes_.size() - 1);
    const auto kCurrentIndex = static_cast<std::size_t>(std::distance(passes_.begin(), iter));

    if (kCurrentIndex == kClampedIndex)
    {
        return true; // Already at target
    }

    auto pass_ptr = std::move(*iter);
    passes_.erase(iter);
    passes_.insert(passes_.begin() + static_cast<std::ptrdiff_t>(kClampedIndex),
                   std::move(pass_ptr));

    spdlog::debug("FxEngine: reordered '{}' to index {}", pass_name, kClampedIndex);
    return true;
}

auto FxEngine::get_pass(const std::string& pass_name) -> FxPass*
{
    auto iter = std::ranges::find_if(
        passes_, [&](const auto& pass_ptr) { return pass_ptr->name() == pass_name; });
    return (iter != passes_.end()) ? iter->get() : nullptr;
}

auto FxEngine::get_pass(const std::string& pass_name) const -> const FxPass*
{
    auto iter = std::ranges::find_if(
        passes_, [&](const auto& pass_ptr) { return pass_ptr->name() == pass_name; });
    return (iter != passes_.end()) ? iter->get() : nullptr;
}

// ── Pipeline execution ──

auto FxEngine::execute_pipeline() -> FxFrameResult
{
    FxFrameResult result;
    result.master_enabled = master_enabled_;
    result.active_tier = quality_tier_;

    if (!master_enabled_)
    {
        result.passes_skipped = passes_.size();
        return result;
    }

    for (const auto& pass : passes_)
    {
        if (pass->is_active_at(quality_tier_))
        {
            static_cast<void>(pass->execute());
            ++result.passes_executed;
        }
        else
        {
            ++result.passes_skipped;
        }
    }

    return result;
}

auto FxEngine::execute_for_surface(FxSurfaceTarget surface_target) -> FxFrameResult
{
    FxFrameResult result;
    result.master_enabled = master_enabled_;
    result.active_tier = quality_tier_;

    if (!master_enabled_)
    {
        result.passes_skipped = passes_.size();
        return result;
    }

    for (const auto& pass : passes_)
    {
        const auto kPassTarget = pass->target();
        const bool kSurfaceMatch =
            (kPassTarget == FxSurfaceTarget::kAll || kPassTarget == surface_target);

        if (kSurfaceMatch && pass->is_active_at(quality_tier_))
        {
            static_cast<void>(pass->execute());
            ++result.passes_executed;
        }
        else
        {
            ++result.passes_skipped;
        }
    }

    return result;
}

// ── Quality tier ──

void FxEngine::set_quality_tier(QualityTier tier)
{
    quality_tier_ = tier;
    spdlog::info("FxEngine: quality tier set to '{}'", tier_name(tier));
}

auto FxEngine::quality_tier() const noexcept -> QualityTier
{
    return quality_tier_;
}

// ── Master toggle ──

void FxEngine::set_master_enabled(bool enabled)
{
    master_enabled_ = enabled;
    spdlog::info("FxEngine: master {}", enabled ? "enabled" : "disabled");
}

auto FxEngine::is_master_enabled() const noexcept -> bool
{
    return master_enabled_;
}

// ── Queries ──

auto FxEngine::pass_count() const noexcept -> std::size_t
{
    return passes_.size();
}

auto FxEngine::active_pass_count() const noexcept -> std::size_t
{
    if (!master_enabled_)
    {
        return 0;
    }
    return static_cast<std::size_t>(std::ranges::count_if(
        passes_, [this](const auto& pass_ptr) { return pass_ptr->is_active_at(quality_tier_); }));
}

auto FxEngine::pass_names() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(passes_.size());
    for (const auto& pass : passes_)
    {
        names.emplace_back(pass->name());
    }
    return names;
}

auto FxEngine::has_pass_type(FxPassType pass_type) const noexcept -> bool
{
    return std::ranges::any_of(
        passes_, [pass_type](const auto& pass_ptr) { return pass_ptr->type() == pass_type; });
}

auto FxEngine::tier_name(QualityTier tier) -> std::string_view
{
    switch (tier)
    {
        case QualityTier::kCinematic:
            return "Cinematic";
        case QualityTier::kBalanced:
            return "Balanced";
        case QualityTier::kEfficient:
            return "Efficient";
        case QualityTier::kMinimal:
            return "Minimal";
    }
    return "Unknown";
}

} // namespace markamp::rendering
