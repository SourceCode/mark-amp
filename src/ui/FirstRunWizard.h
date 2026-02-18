#pragma once

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/simplebook.h>
#include <wx/stattext.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{
class Config;
class ConfigProfileManager;
class ThemeRegistry;
} // namespace markamp::core

namespace markamp::ui
{

/// First-run setup wizard shown on first launch.
/// Pages: Welcome → Theme Selection → Key Profile → Extension Recommendations
class FirstRunWizard : public wxDialog
{
public:
    FirstRunWizard(wxWindow* parent,
                   core::ThemeEngine& theme_engine,
                   core::EventBus& event_bus,
                   core::Config& config,
                   core::ThemeRegistry& theme_registry,
                   core::ConfigProfileManager& profile_manager);

    /// Check whether the wizard should be shown (first_run_completed == false).
    [[nodiscard]] static inline auto ShouldShow(const core::Config& config) -> bool
    {
        return config.get_string("app.first_run_completed", "false") != "true";
    }

private:
    core::ThemeEngine& theme_engine_;
    [[maybe_unused]] core::EventBus& event_bus_;
    core::Config& config_;
    core::ThemeRegistry& theme_registry_;
    core::ConfigProfileManager& profile_manager_;

    wxSimplebook* pages_{nullptr};
    int current_page_{0};
    static constexpr int kPageCount = 4;

    // Page creation
    auto CreateWelcomePage() -> wxPanel*;
    auto CreateThemePage() -> wxPanel*;
    auto CreateProfilePage() -> wxPanel*;
    auto CreateExtensionsPage() -> wxPanel*;

    // Navigation
    void OnNext(wxCommandEvent& event);
    void OnBack(wxCommandEvent& event);
    void OnFinish(wxCommandEvent& event);
    void UpdateNavButtons();

    wxButton* back_btn_{nullptr};
    wxButton* next_btn_{nullptr};
    wxButton* finish_btn_{nullptr};

    // Selected values
    std::string selected_theme_{"Dark Modern"};
    std::string selected_profile_{"(Default)"};
};

} // namespace markamp::ui
