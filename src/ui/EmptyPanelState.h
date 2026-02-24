#pragma once

#include "ThemeAwareWindow.h"

#include <wx/button.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>

#include <functional>
#include <string>

namespace markamp::core
{
class ThemeEngine;
}

namespace markamp::ui
{

struct DesignSystemContext;
class IconManager;

class EmptyPanelState : public ThemeAwareWindow
{
public:
    EmptyPanelState(wxWindow* parent, DesignSystemContext& ds, IconManager& im);

    void set_icon(const std::string& icon_id);
    void set_message(const std::string& message);
    void set_action(const std::string& label, std::function<void()> callback);

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    IconManager& im_;

    std::string icon_id_;
    wxStaticBitmap* icon_bmp_{nullptr};
    wxStaticText* message_txt_{nullptr};
    wxButton* action_btn_{nullptr};
    std::function<void()> action_callback_;

    void UpdateLayout();
};

} // namespace markamp::ui
