#pragma once

#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <wx/popupwin.h>
#include <wx/wx.h>

#include <string>

namespace markamp::ui
{

/// Popover that appears when the cursor dwells on a markdown link [text](url).
/// Shows the URL text, link label, and an "Open in browser" button.
class LinkPreviewPopover : public wxPopupTransientWindow
{
public:
    LinkPreviewPopover(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       core::EventBus& event_bus);

    /// Set the link content to display.
    void SetLink(const std::string& link_text, const std::string& url);

    /// Update styling from theme.
    void ApplyTheme();

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::Subscription theme_sub_;

    wxStaticText* link_label_{nullptr};
    wxStaticText* url_label_{nullptr};
    wxButton* open_btn_{nullptr};
    wxButton* copy_btn_{nullptr}; // R21 Fix 20

    std::string current_url_;

    static constexpr int kMaxWidth = 400;
    static constexpr int kPadding = 8;

    void CreateLayout();
    void OnOpenInBrowser(wxCommandEvent& event);
};

} // namespace markamp::ui
