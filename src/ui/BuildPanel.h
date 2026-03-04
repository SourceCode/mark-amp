#pragma once

#include "core/BuildService.h"
#include "core/CompilerErrorParser.h"
#include "core/EventBus.h"

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/gauge.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

namespace markamp::ui
{

/// Build Panel — displays build output with progress bar, error/warning counts,
/// action buttons (Build, Clean, Stop), and compiler error highlighting.
class BuildPanel : public wxPanel
{
public:
    explicit BuildPanel(wxWindow* parent);

    /// Full constructor with service and event bus.
    BuildPanel(wxWindow* parent, core::BuildService* service, core::EventBus* event_bus = nullptr);

    ~BuildPanel() override = default;

    /// Set the build service.
    void set_service(core::BuildService* service);

    /// Refresh displayed content.
    void RefreshContent();

    /// Apply theme colors.
    void ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour);

    /// Start a build.
    void StartBuild(const std::string& target = "", const std::string& config = "Debug");

    /// Stop the current build.
    void StopBuild();

    /// Set the compiler (e.g. "gcc", "clang", "msvc").
    void set_compiler(const std::string& compiler);
    [[nodiscard]] auto compiler() const -> const std::string&;

    /// Set the build target.
    void set_build_target(const std::string& target);
    [[nodiscard]] auto build_target() const -> const std::string&;

    /// Set available build targets for the selector.
    void set_available_targets(const std::vector<std::string>& targets);

    /// Enable/disable auto-build on save.
    void set_auto_build(bool enabled);
    [[nodiscard]] auto auto_build() const -> bool;

private:
    void CreateLayout();
    void UpdateProgressBar();
    void UpdateSummary();
    void OnOutputDoubleClick(wxMouseEvent& event);

    core::BuildService* service_{nullptr};
    core::EventBus* event_bus_{nullptr};
    core::CompilerErrorParser error_parser_;

    // UI controls
    wxWindow* toolbar_{nullptr};
    wxTextCtrl* output_area_{nullptr};
    wxGauge* progress_bar_{nullptr};
    wxStaticText* summary_label_{nullptr};
    wxStaticText* time_label_{nullptr};
    wxButton* build_btn_{nullptr};
    wxButton* stop_btn_{nullptr};
    wxButton* clean_btn_{nullptr};
    wxChoice* compiler_choice_{nullptr};
    wxChoice* target_choice_{nullptr};

    std::string compiler_{"clang"};
    std::string build_target_;
    bool auto_build_{false};
};

} // namespace markamp::ui
