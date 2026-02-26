#pragma once

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

namespace markamp::ui
{

/**
 * @brief Stub implementation for the Debug Console Panel.
 * Will eventually host an interactive debug REPL or log view.
 */
class DebugConsolePanel : public wxPanel
{
public:
    explicit DebugConsolePanel(wxWindow* parent);
    ~DebugConsolePanel() override = default;

private:
    wxStaticText* placeholder_text_;
};

} // namespace markamp::ui
