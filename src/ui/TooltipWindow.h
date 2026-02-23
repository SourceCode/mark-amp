#pragma once

#include "ui/DesignSystemContext.h"
#include "ui/animation/TransitionManager.h"

#include <wx/frame.h>
#include <wx/stattext.h>

namespace markamp::ui
{

class TooltipWindow : public wxFrame
{
public:
    TooltipWindow(wxWindow* parent, DesignSystemContext& ds);
    ~TooltipWindow() override;

    void ShowTooltip(const wxString& text, const wxPoint& screen_pos);
    void HideTooltip();

    static TooltipWindow* GetOrCreate(wxWindow* parent, DesignSystemContext& ds);

private:
    DesignSystemContext& ds_;
    animation::TransitionManager transition_manager_;
    wxStaticText* label_{nullptr};
    bool is_showing_{false};

    void ApplyTheme();
    void OnPaint(wxPaintEvent& event);
};

} // namespace markamp::ui
