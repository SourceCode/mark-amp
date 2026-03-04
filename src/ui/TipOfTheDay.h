// ============================================================================
// File: src/ui/TipOfTheDay.h
// Phase 48: Welcome and Onboarding — Tip of the Day model
// ============================================================================
#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A single tip.
struct Tip
{
    std::string id;
    std::string title;
    std::string content;
    std::string category; ///< "editing", "navigation", "productivity", etc.
};

/// Model for the Tip of the Day feature.
class TipOfTheDayModel
{
public:
    TipOfTheDayModel() = default;

    /// Add a tip.
    void add_tip(Tip tip);

    /// Get a tip for today (non-repeating within cooldown).
    [[nodiscard]] auto tip_for_today() const -> const Tip*;

    /// Get total tip count.
    [[nodiscard]] auto tip_count() const -> int
    {
        return static_cast<int>(tips_.size());
    }

    /// Mark a tip as shown (updates cooldown tracking).
    void mark_shown(const std::string& tip_id);

    /// Navigate to next tip.
    void next_tip();

    /// Navigate to previous tip.
    void prev_tip();

    /// Get current tip index.
    [[nodiscard]] auto current_index() const -> int
    {
        return current_index_;
    }

    /// Set the cooldown period in days.
    void set_cooldown_days(int days)
    {
        cooldown_days_ = days;
    }

    /// Get cooldown period.
    [[nodiscard]] auto cooldown_days() const -> int
    {
        return cooldown_days_;
    }

    /// Enable/disable.
    void set_enabled(bool enabled)
    {
        enabled_ = enabled;
    }
    [[nodiscard]] auto is_enabled() const -> bool
    {
        return enabled_;
    }

private:
    std::vector<Tip> tips_;
    std::vector<std::string> shown_tip_ids_; ///< IDs of recently shown tips
    int current_index_{0};
    int cooldown_days_{30};
    bool enabled_{true};
};

} // namespace markamp::ui
