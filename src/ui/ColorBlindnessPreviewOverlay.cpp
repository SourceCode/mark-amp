#include "ColorBlindnessPreviewOverlay.h"

#include <wx/sizer.h>
#include <wx/stattext.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(ColorBlindnessPreviewOverlay, wxPanel)
    EVT_CHOICE(wxID_ANY, ColorBlindnessPreviewOverlay::on_simulation_changed) wxEND_EVENT_TABLE()

        ColorBlindnessPreviewOverlay::ColorBlindnessPreviewOverlay(wxWindow* parent,
                                                                   core::ThemeEngine& theme_engine)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
{
    init_ui();
}

ColorBlindnessPreviewOverlay::~ColorBlindnessPreviewOverlay()
{
    // Clean up any active overlay when destroyed
    apply_simulation(core::ColorBlindnessSimulator::VisionType::Normal);
}

void ColorBlindnessPreviewOverlay::init_ui()
{
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Vision Simulator:"),
               0,
               wxALIGN_CENTER_VERTICAL | wxRIGHT,
               5);

    wxArrayString choices;
    choices.Add("None (Normal Vision)");
    choices.Add("Protanopia (No Red)");
    choices.Add("Deuteranopia (No Green)");
    choices.Add("Tritanopia (No Blue)");
    choices.Add("Achromatopsia (No Color)");

    mode_dropdown_ = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
    mode_dropdown_->SetSelection(0);

    sizer->Add(mode_dropdown_, 1, wxEXPAND);
    SetSizer(sizer);
}

void ColorBlindnessPreviewOverlay::on_simulation_changed(wxCommandEvent& event)
{
    int sel = event.GetSelection();
    auto type = static_cast<core::ColorBlindnessSimulator::VisionType>(sel);
    apply_simulation(type);
}

void ColorBlindnessPreviewOverlay::apply_simulation(core::ColorBlindnessSimulator::VisionType type)
{
    if (type == core::ColorBlindnessSimulator::VisionType::Normal)
    {
        theme_engine_.cancel_preview();
        return;
    }

    // Capture the base theme before simulation (if not already previewing, getting current is fine)
    // If already previewing a simulation, we should really grab the base from the registry, but
    // for simplicity, we simulate off the current theme (which applies cumulatively if we don't
    // revert). The exact interaction with regular theme edits will need a unified preview stack,
    // but this suffices for V1.

    // Always cancel the previous preview overlay first so we simulate off the original base
    theme_engine_.cancel_preview();

    core::Theme base_theme = theme_engine_.current_theme();
    core::Theme simulated_theme = base_theme;

    auto sim_color = [](const core::Color& src,
                        core::ColorBlindnessSimulator::VisionType vtype) -> core::Color
    {
        auto wx_col = core::ColorBlindnessSimulator::simulate(src.to_wx_colour(), vtype);
        return core::Color{wx_col.Red(), wx_col.Green(), wx_col.Blue(), wx_col.Alpha()};
    };

    // Simulate syntax tokens
    simulated_theme.syntax.keyword = sim_color(base_theme.syntax.keyword, type);
    simulated_theme.syntax.string_literal = sim_color(base_theme.syntax.string_literal, type);
    simulated_theme.syntax.number = sim_color(base_theme.syntax.number, type);
    simulated_theme.syntax.operator_tok = sim_color(base_theme.syntax.operator_tok, type);
    simulated_theme.syntax.preprocessor = sim_color(base_theme.syntax.preprocessor, type);
    simulated_theme.syntax.comment = sim_color(base_theme.syntax.comment, type);
    simulated_theme.syntax.type_name = sim_color(base_theme.syntax.type_name, type);
    simulated_theme.syntax.function_name = sim_color(base_theme.syntax.function_name, type);

    // Apply the preview
    theme_engine_.preview_theme(simulated_theme);
}

} // namespace markamp::ui
