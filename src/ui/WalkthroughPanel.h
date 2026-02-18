#pragma once

#include "core/ExtensionManifest.h"

#include <wx/gauge.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::ui
{

/// Interactive step within a walkthrough tutorial.
struct WalkthroughStep
{
    std::string step_id;          ///< Unique step identifier
    std::string title;            ///< Display title
    std::string description;      ///< Instructional text
    std::string action_label;     ///< Button label (e.g. "Try It")
    std::string target_element;   ///< UI element ID to spotlight
    std::function<void()> action; ///< Action to perform on "Try It"
};

/// Built-in walkthrough definition.
struct BuiltInWalkthrough
{
    std::string walkthrough_id;         ///< Unique walkthrough ID
    std::string title;                  ///< Display title
    std::string description;            ///< Brief description
    std::vector<WalkthroughStep> steps; ///< Ordered steps
};

/// Getting-started walkthrough panel (#48).
/// Renders walkthrough steps with checkmarks, descriptions, media,
/// and completion tracking. Mirrors VS Code's "Getting Started" experience.
///
/// Phase 35 extensions: interactive tutorial steps with spotlight targeting,
/// "Try It" actions, and built-in walkthroughs for Getting Started, Editor
/// Basics, Canvas, Notebooks, and Graph.
class WalkthroughPanel : public wxPanel
{
public:
    /// Data-only constructor (for tests).
    WalkthroughPanel();

    /// UI constructor with rendering support.
    explicit WalkthroughPanel(wxWindow* parent);

    /// Set walkthroughs from extension contributions.
    void set_walkthroughs(std::vector<core::ExtensionWalkthrough> walkthroughs);

    /// Get all walkthroughs.
    [[nodiscard]] auto walkthroughs() const -> const std::vector<core::ExtensionWalkthrough>&;

    /// Mark a step as completed.
    void complete_step(const std::string& walkthrough_id, const std::string& step_id);

    /// Check if a step is completed.
    [[nodiscard]] auto is_step_completed(const std::string& walkthrough_id,
                                         const std::string& step_id) const -> bool;

    /// Get the completion percentage for a walkthrough (0.0 – 1.0).
    [[nodiscard]] auto completion_progress(const std::string& walkthrough_id) const -> double;

    /// Get the currently active walkthrough ID.
    [[nodiscard]] auto active_walkthrough() const -> const std::string&;
    void set_active_walkthrough(const std::string& walkthrough_id);

    /// Reset completion for a walkthrough.
    void reset_walkthrough(const std::string& walkthrough_id);

    // --- Phase 35: Interactive tutorial support ---

    /// Register a built-in walkthrough.
    void register_built_in(BuiltInWalkthrough walkthrough);

    /// Get all registered built-in walkthroughs.
    [[nodiscard]] auto built_in_walkthroughs() const -> const std::vector<BuiltInWalkthrough>&;

    /// Get a specific built-in walkthrough by ID.
    [[nodiscard]] auto get_built_in(const std::string& walkthrough_id) const
        -> const BuiltInWalkthrough*;

    /// Set the spotlight target for the current step.
    void set_spotlight_target(const std::string& element_id);

    /// Get the current spotlight target element ID.
    [[nodiscard]] auto spotlight_target() const -> const std::string&;

    /// Execute the "Try It" action for a specific step.
    void try_action(const std::string& walkthrough_id, const std::string& step_id);

    /// Get the total number of built-in walkthroughs.
    [[nodiscard]] auto built_in_count() const -> std::size_t;

    /// Load built-in walkthroughs (Getting Started, Editor, Canvas, Notebooks, Graph).
    void load_built_in_walkthroughs();

    /// Refresh displayed content.
    void RefreshContent();

    /// Apply theme colors.
    void ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour);

private:
    void CreateLayout();

    std::vector<core::ExtensionWalkthrough> walkthroughs_;
    std::string active_walkthrough_;
    std::unordered_set<std::string> completed_steps_;
    std::vector<BuiltInWalkthrough> built_in_walkthroughs_;
    std::string spotlight_target_;

    // UI controls (null in data-only / test mode)
    wxScrolledWindow* scroll_area_{nullptr};
    wxGauge* progress_gauge_{nullptr};
};

} // namespace markamp::ui
