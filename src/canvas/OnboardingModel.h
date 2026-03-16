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

    // ── Round 5 Batch 3 (#26-28) ────────────────────────────────

    /// (#26) Whether a title is set.
    [[nodiscard]] auto has_title() const noexcept -> bool
    {
        return !title.empty();
    }

    /// (#27) Whether an instruction is set.
    [[nodiscard]] auto has_instruction() const noexcept -> bool
    {
        return !instruction.empty();
    }

    /// (#28) Whether this step is completed.
    [[nodiscard]] auto is_done() const noexcept -> bool
    {
        return completed;
    }
};

/// Starter template entry.
struct StarterTemplate
{
    std::string template_id;
    std::string name;
    std::string category; ///< "flowchart", "brainstorm", "roadmap", "diagram"

    // ── Round 5 Batch 3 (#29-30) ────────────────────────────────

    /// (#29) Whether a name is set.
    [[nodiscard]] auto has_name() const noexcept -> bool
    {
        return !name.empty();
    }

    /// (#30) Whether a category is set.
    [[nodiscard]] auto has_category() const noexcept -> bool
    {
        return !category.empty();
    }
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

    // ── Round 5 Batch 4 (#31-33) ────────────────────────────────

    /// (#31) Number of walkthrough steps.
    [[nodiscard]] auto step_count() const noexcept -> size_t
    {
        return steps_.size();
    }

    /// (#32) Whether starter templates are loaded.
    [[nodiscard]] auto has_templates() const noexcept -> bool
    {
        return !starter_templates_.empty();
    }

    /// (#33) Number of starter templates.
    [[nodiscard]] auto template_count() const noexcept -> size_t
    {
        return starter_templates_.size();
    }
};

} // namespace markamp::canvas
