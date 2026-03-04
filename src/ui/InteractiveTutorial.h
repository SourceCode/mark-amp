// ============================================================================
// File: src/ui/InteractiveTutorial.h
// Phase 48: Welcome and Onboarding — Interactive tutorial engine model
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A single step in an interactive tutorial.
struct TutorialStep
{
    std::string id;
    std::string title;
    std::string description;
    std::string target_element_id; ///< UI element to spotlight
    std::string action_type;       ///< "click", "type", "navigate", etc.
    bool is_completed{false};
};

/// A complete interactive tutorial.
struct Tutorial
{
    std::string id;
    std::string title;
    std::string description;
    std::vector<TutorialStep> steps;

    [[nodiscard]] auto completed_count() const -> int
    {
        int count = 0;
        for (const auto& s : steps)
        {
            if (s.is_completed)
            {
                ++count;
            }
        }
        return count;
    }

    [[nodiscard]] auto is_complete() const -> bool
    {
        return completed_count() == static_cast<int>(steps.size());
    }
};

/// Engine for driving interactive tutorials.
class InteractiveTutorialEngine
{
public:
    InteractiveTutorialEngine() = default;

    /// Register a tutorial.
    void add_tutorial(Tutorial tutorial);

    /// Get all tutorials.
    [[nodiscard]] auto tutorials() const -> const std::vector<Tutorial>&
    {
        return tutorials_;
    }

    /// Start a tutorial by ID.
    void start(const std::string& tutorial_id);

    /// Get the current step (nullptr if none active).
    [[nodiscard]] auto current_step() const -> const TutorialStep*;

    /// Advance to the next step.
    void advance();

    /// Skip the current tutorial.
    void skip();

    /// Whether a tutorial is active.
    [[nodiscard]] auto is_active() const -> bool
    {
        return is_active_;
    }

    /// Get the active tutorial ID.
    [[nodiscard]] auto active_tutorial_id() const -> const std::string&
    {
        return active_tutorial_id_;
    }

    /// Get completion tracking: total completed tutorials.
    [[nodiscard]] auto completed_tutorial_count() const -> int;

private:
    std::vector<Tutorial> tutorials_;
    std::string active_tutorial_id_;
    int current_step_index_{0};
    bool is_active_{false};

    [[nodiscard]] auto find_tutorial(const std::string& id) -> Tutorial*;
};

} // namespace markamp::ui
