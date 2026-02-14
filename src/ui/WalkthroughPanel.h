#pragma once

#include "core/ExtensionManifest.h"

#include <wx/gauge.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>

#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::ui
{

/// Getting-started walkthrough panel (#48).
/// Renders walkthrough steps with checkmarks, descriptions, media,
/// and completion tracking. Mirrors VS Code's "Getting Started" experience.
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

    /// Refresh displayed content.
    void RefreshContent();

    /// Apply theme colors.
    void ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour);

private:
    void CreateLayout();

    std::vector<core::ExtensionWalkthrough> walkthroughs_;
    std::string active_walkthrough_;
    std::unordered_set<std::string> completed_steps_;

    // UI controls (null in data-only / test mode)
    wxScrolledWindow* scroll_area_{nullptr};
    wxGauge* progress_gauge_{nullptr};
};

} // namespace markamp::ui
