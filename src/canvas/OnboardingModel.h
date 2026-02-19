#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Walkthrough step.
struct WalkthroughStep
{
    std::string step_id;
    std::string title;
    std::string instruction;
    bool completed{false};
};

/// Starter template entry.
struct StarterTemplate
{
    std::string template_id;
    std::string name;
    std::string category; ///< "flowchart", "brainstorm", "roadmap", "diagram"
};

/// Testable model for Canvas Onboarding / Tutorials (Phase 78).
///
/// Encapsulates:
/// - Guided walkthrough steps with completion tracking
/// - Contextual hint dismissal state
/// - Starter board template catalog
/// - Quick reference visibility
class OnboardingModel
{
public:
    // ── Walkthrough ─────────────────────────────────────────────────

    void set_steps(std::vector<WalkthroughStep> steps);
    [[nodiscard]] auto steps() const -> const std::vector<WalkthroughStep>&;
    void complete_step(const std::string& step_id);
    [[nodiscard]] auto progress_percent() const -> int;
    [[nodiscard]] auto is_complete() const -> bool;

    // ── Hints ───────────────────────────────────────────────────────

    void dismiss_hint(const std::string& hint_id);
    [[nodiscard]] auto is_hint_dismissed(const std::string& hint_id) const -> bool;

    // ── Starter templates ───────────────────────────────────────────

    void set_starter_templates(std::vector<StarterTemplate> templates);
    [[nodiscard]] auto starter_templates() const -> const std::vector<StarterTemplate>&;

    // ── Quick reference ─────────────────────────────────────────────

    void set_reference_visible(bool visible);
    [[nodiscard]] auto reference_visible() const -> bool;

private:
    std::vector<WalkthroughStep> steps_;
    std::vector<std::string> dismissed_hints_;
    std::vector<StarterTemplate> starter_templates_;
    bool reference_visible_{false};
};

} // namespace markamp::canvas
