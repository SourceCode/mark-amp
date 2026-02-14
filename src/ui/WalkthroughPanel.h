#pragma once

#include "core/ExtensionManifest.h"

#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::ui
{

/// Getting-started walkthrough panel (#48).
/// Renders walkthrough steps with checkmarks, descriptions, media,
/// and completion tracking. Mirrors VS Code's "Getting Started" experience.
class WalkthroughPanel
{
public:
    WalkthroughPanel() = default;

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

private:
    std::vector<core::ExtensionWalkthrough> walkthroughs_;
    std::string active_walkthrough_;
    // Completed step keys: "walkthrough_id:step_id"
    std::unordered_set<std::string> completed_steps_;
};

} // namespace markamp::ui
