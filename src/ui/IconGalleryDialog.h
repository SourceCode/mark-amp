#pragma once

#include "core/ThemeEngine.h"

#include <wx/dialog.h>
#include <wx/scrolwin.h>

namespace markamp::ui
{

class IconGalleryDialog : public wxDialog
{
public:
    IconGalleryDialog(wxWindow* parent, core::ThemeEngine& theme_engine);
    ~IconGalleryDialog() override = default;

private:
    core::ThemeEngine& theme_engine_;
    wxScrolledWindow* scroll_panel_{nullptr};

    void setupLayout();
    void renderIcons();
};

} // namespace markamp::ui
