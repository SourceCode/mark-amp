#pragma once

#include "core/ThemeEngine.h"

#include <wx/popupwin.h>
#include <wx/wx.h>

#include <filesystem>
#include <string>

namespace markamp::ui
{

/// Popover that renders a thumbnail of a local image when the cursor
/// hovers over a markdown image reference ![alt](path).
class ImagePreviewPopover : public wxPopupTransientWindow
{
public:
    ImagePreviewPopover(wxWindow* parent, core::ThemeEngine& theme_engine);

    /// Load and display an image from the given path.
    /// Returns false if the image could not be loaded.
    auto SetImage(const std::filesystem::path& image_path, const std::string& alt_text) -> bool;

    /// Update styling from theme.
    void ApplyTheme();

private:
    core::ThemeEngine& theme_engine_;

    wxStaticBitmap* thumbnail_{nullptr};
    wxStaticText* alt_label_{nullptr};
    wxStaticText* path_label_{nullptr};
    wxStaticText* error_label_{nullptr};

    static constexpr int kMaxThumbWidth = 300;
    static constexpr int kMaxThumbHeight = 200;
    static constexpr int kPadding = 8;

    void CreateLayout();
    void ShowError(const std::string& message);
};

} // namespace markamp::ui
