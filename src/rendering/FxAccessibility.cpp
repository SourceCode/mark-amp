// ============================================================================
// File: src/rendering/FxAccessibility.cpp
// Phase 28: FX Visual Effects System — Accessibility guards
// ============================================================================

#include "FxAccessibility.h"

#include <sstream>

namespace markamp::rendering
{

auto FxAccessibility::announce_preset_change(const std::string& from_preset,
                                             const std::string& to_preset) const -> std::string
{
    std::ostringstream msg;
    msg << "Visual effects preset changed from " << from_preset << " to " << to_preset;
    return msg.str();
}

auto FxAccessibility::announce_quality_change(QualityTier tier) const -> std::string
{
    std::string tier_name;
    switch (tier)
    {
        case QualityTier::kCinematic:
            tier_name = "Cinematic";
            break;
        case QualityTier::kBalanced:
            tier_name = "Balanced";
            break;
        case QualityTier::kEfficient:
            tier_name = "Efficient";
            break;
        case QualityTier::kMinimal:
            tier_name = "Minimal";
            break;
    }
    return "Visual effects quality set to " + tier_name;
}

auto FxAccessibility::announce_safety_mode(const std::string& mode_name, bool enabled) const
    -> std::string
{
    return mode_name + (enabled ? " enabled" : " disabled");
}

auto FxAccessibility::announce_master_toggle(bool enabled) const -> std::string
{
    return enabled ? "Visual effects enabled" : "Visual effects disabled";
}

auto FxAccessibility::prefers_reduced_motion() const -> bool
{
    // Platform-specific detection.
#ifdef __APPLE__
    // On macOS, this would check:
    //   NSWorkspace.shared.accessibilityDisplayShouldReduceMotion
    // For now, return false as a default.
    return false;
#else
    return false;
#endif
}

auto FxAccessibility::prefers_high_contrast() const -> bool
{
#ifdef __APPLE__
    // On macOS: NSWorkspace.shared.accessibilityDisplayShouldIncreaseContrast
    return false;
#else
    return false;
#endif
}

auto FxAccessibility::get_accessibility_summary(bool fx_enabled,
                                                bool reduced_motion,
                                                bool low_power,
                                                bool text_safety,
                                                QualityTier tier) const -> std::string
{
    std::ostringstream summary;

    summary << "FX Status: " << (fx_enabled ? "Enabled" : "Disabled");

    if (fx_enabled)
    {
        summary << " | Quality: ";
        switch (tier)
        {
            case QualityTier::kCinematic:
                summary << "Cinematic";
                break;
            case QualityTier::kBalanced:
                summary << "Balanced";
                break;
            case QualityTier::kEfficient:
                summary << "Efficient";
                break;
            case QualityTier::kMinimal:
                summary << "Minimal";
                break;
        }
    }

    if (reduced_motion)
    {
        summary << " | Reduced Motion: On";
    }
    if (low_power)
    {
        summary << " | Low Power: On";
    }
    if (text_safety)
    {
        summary << " | Text Safety: On";
    }

    return summary.str();
}

auto FxAccessibility::get_recommendations(bool fx_enabled, bool reduced_motion) const
    -> std::vector<std::string>
{
    std::vector<std::string> recs;

    if (prefers_reduced_motion() && !reduced_motion && fx_enabled)
    {
        recs.emplace_back("OS prefers reduced motion — consider enabling reduced motion mode");
    }

    if (prefers_high_contrast() && fx_enabled)
    {
        recs.emplace_back("OS prefers high contrast — consider enabling text safety mode");
    }

    if (fx_enabled && !reduced_motion)
    {
        recs.emplace_back("Consider enabling reduced motion for better accessibility");
    }

    return recs;
}

auto FxAccessibility::record_announcement(const std::string& message) -> void
{
    history_.push_back(message);
    if (history_.size() > kMaxHistory)
    {
        history_.erase(history_.begin());
    }
}

auto FxAccessibility::announcement_history() const -> const std::vector<std::string>&
{
    return history_;
}

auto FxAccessibility::clear_history() -> void
{
    history_.clear();
}

auto FxAccessibility::announcement_count() const noexcept -> std::size_t
{
    return history_.size();
}

} // namespace markamp::rendering
