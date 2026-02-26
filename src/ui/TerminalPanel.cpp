#include "ui/TerminalPanel.h"

namespace markamp::ui
{

TerminalPanel::TerminalPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // Add a placeholder text centered in the panel
    placeholder_text_ =
        new wxStaticText(this, wxID_ANY, "Terminal (Stub) - Awaiting Implementation");

    // Give it a monospaced font feel just for the stub
    wxFont font(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    placeholder_text_->SetFont(font);

    sizer->AddStretchSpacer(1);
    sizer->Add(placeholder_text_, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 20);
    sizer->AddStretchSpacer(1);

    SetSizer(sizer);
}

} // namespace markamp::ui
