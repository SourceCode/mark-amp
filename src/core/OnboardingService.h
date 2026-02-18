/// @file OnboardingService.h
/// @brief V9 Phase 35 Task 16 — First-launch onboarding flow.
///
/// Manages the multi-step onboarding wizard for new users:
/// Welcome → Choose Theme → Import Settings → Feature Tour → Open Workspace.
/// Persists completion state so onboarding runs only once.

#pragma once

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// OnboardingStep — step identifiers
// ============================================================================

/// Steps in the first-launch onboarding flow.
enum class OnboardingStep
{
    Welcome,        ///< Product overview
    ChooseTheme,    ///< Light/dark + accent color selection
    ImportSettings, ///< Import from VS Code, Obsidian, or skip
    FeatureTour,    ///< 5-slide feature overview
    OpenWorkspace,  ///< Open sample workspace or create new
    Complete,       ///< Onboarding finished
};

/// Convert OnboardingStep to human-readable name.
[[nodiscard]] inline auto onboarding_step_name(OnboardingStep step) -> std::string
{
    switch (step)
    {
        case OnboardingStep::Welcome:
            return "Welcome";
        case OnboardingStep::ChooseTheme:
            return "ChooseTheme";
        case OnboardingStep::ImportSettings:
            return "ImportSettings";
        case OnboardingStep::FeatureTour:
            return "FeatureTour";
        case OnboardingStep::OpenWorkspace:
            return "OpenWorkspace";
        case OnboardingStep::Complete:
            return "Complete";
    }
    return "Unknown";
}

// ============================================================================
// FeatureTourSlide — individual tour slide
// ============================================================================

/// A single slide in the feature tour.
struct FeatureTourSlide
{
    std::string title;       ///< Slide title
    std::string description; ///< Slide body text
    std::string image_ref;   ///< Reference to illustration asset
};

// ============================================================================
// OnboardingService
// ============================================================================

/// Manages the first-launch onboarding wizard.
///
/// Usage:
/// ```cpp
/// OnboardingService onboarding;
/// if (onboarding.is_first_launch())
/// {
///     auto step = onboarding.current_step();
///     // render step UI
///     onboarding.advance();
/// }
/// ```
class OnboardingService
{
public:
    OnboardingService()
    {
        register_tour_slides();
    }

    /// Check if this is the first launch (onboarding not yet completed).
    [[nodiscard]] auto is_first_launch() const -> bool
    {
        return !completed_;
    }

    /// Get the current onboarding step.
    [[nodiscard]] auto current_step() const -> OnboardingStep
    {
        return current_step_;
    }

    /// Get the current step index (0-based).
    [[nodiscard]] auto current_step_index() const -> int
    {
        return static_cast<int>(current_step_);
    }

    /// Get the total number of steps (excluding Complete).
    [[nodiscard]] static auto total_steps() -> int
    {
        return 5;
    }

    /// Advance to the next step.
    void advance()
    {
        switch (current_step_)
        {
            case OnboardingStep::Welcome:
                current_step_ = OnboardingStep::ChooseTheme;
                break;
            case OnboardingStep::ChooseTheme:
                current_step_ = OnboardingStep::ImportSettings;
                break;
            case OnboardingStep::ImportSettings:
                current_step_ = OnboardingStep::FeatureTour;
                break;
            case OnboardingStep::FeatureTour:
                current_step_ = OnboardingStep::OpenWorkspace;
                break;
            case OnboardingStep::OpenWorkspace:
                current_step_ = OnboardingStep::Complete;
                completed_ = true;
                break;
            case OnboardingStep::Complete:
                break;
        }
    }

    /// Skip the entire onboarding flow.
    void skip()
    {
        current_step_ = OnboardingStep::Complete;
        completed_ = true;
    }

    /// Check if onboarding is complete.
    [[nodiscard]] auto is_complete() const -> bool
    {
        return completed_;
    }

    /// Mark onboarding as completed (for loading persisted state).
    void mark_completed()
    {
        completed_ = true;
        current_step_ = OnboardingStep::Complete;
    }

    /// Reset onboarding state (for testing or re-run).
    void reset()
    {
        current_step_ = OnboardingStep::Welcome;
        completed_ = false;
    }

    /// Get the feature tour slides.
    [[nodiscard]] auto tour_slides() const -> const std::vector<FeatureTourSlide>&
    {
        return tour_slides_;
    }

    /// Set the selected theme during onboarding.
    void set_selected_theme(const std::string& theme_name)
    {
        selected_theme_ = theme_name;
    }

    /// Get the selected theme.
    [[nodiscard]] auto selected_theme() const -> const std::string&
    {
        return selected_theme_;
    }

    /// Set the import source during onboarding.
    void set_import_source(const std::string& source)
    {
        import_source_ = source;
    }

    /// Get the import source ("vscode", "obsidian", "none").
    [[nodiscard]] auto import_source() const -> const std::string&
    {
        return import_source_;
    }

private:
    OnboardingStep current_step_{OnboardingStep::Welcome};
    bool completed_{false};
    std::string selected_theme_;
    std::string import_source_{"none"};
    std::vector<FeatureTourSlide> tour_slides_;

    void register_tour_slides()
    {
        tour_slides_.push_back({.title = "Powerful Markdown Editor",
                                .description =
                                    "Write with full syntax highlighting, live preview, "
                                    "and 200+ keyboard shortcuts. Auto-complete, snippets, "
                                    "and multi-cursor editing built in.",
                                .image_ref = "tour_editor"});

        tour_slides_.push_back({.title = "Visual Canvas",
                                .description =
                                    "Think visually with an infinite 2D canvas. "
                                    "Create sticky notes, diagrams, mind maps, and kanban boards. "
                                    "Connect ideas with arrows and connectors.",
                                .image_ref = "tour_canvas"});

        tour_slides_.push_back({.title = "Computational Notebooks",
                                .description = "Combine prose with executable code cells. "
                                               "Supports Python, JavaScript, and more. "
                                               "Run cells inline and see results immediately.",
                                .image_ref = "tour_notebook"});

        tour_slides_.push_back({.title = "Knowledge Graph",
                                .description =
                                    "Visualise connections between your documents with backlinks, "
                                    "tags, and graph filters. Discover hidden connections "
                                    "in your knowledge base.",
                                .image_ref = "tour_graph"});

        tour_slides_.push_back({.title = "Spaced Repetition Flashcards",
                                .description =
                                    "Turn any note into flashcards with FSRS-powered "
                                    "spaced repetition. Study smarter with adaptive scheduling "
                                    "and performance analytics.",
                                .image_ref = "tour_flashcards"});
    }
};

} // namespace markamp::core
