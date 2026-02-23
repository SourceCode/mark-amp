#pragma once

#include "../core/ColorBlindnessSimulator.h"
#include "../core/ThemeEngine.h"

#include <wx/choice.h>
#include <wx/panel.h>

namespace markamp::ui
{

class ColorBlindnessPreviewOverlay : public wxPanel
{
public:
    ColorBlindnessPreviewOverlay(wxWindow* parent, core::ThemeEngine& theme_engine);
    ~ColorBlindnessPreviewOverlay() override;

private:
    void init_ui();
    void on_simulation_changed(wxCommandEvent& event);
    void apply_simulation(core::ColorBlindnessSimulator::VisionType type);

    core::ThemeEngine& theme_engine_;
    wxChoice* mode_dropdown_ = nullptr;

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
