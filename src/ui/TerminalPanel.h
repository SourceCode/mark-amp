#pragma once

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

namespace markamp::ui
{

/**
 * @brief Stub implementation for the Terminal Panel.
 * Will eventually host an embedded terminal emulator.
 */
class TerminalPanel : public wxPanel
{
public:
    explicit TerminalPanel(wxWindow* parent);
    ~TerminalPanel() override = default;

private:
    wxStaticText* placeholder_text_;
};

} // namespace markamp::ui
