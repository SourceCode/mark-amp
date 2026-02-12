#include "ImagePreviewPopover.h"

#include "core/Color.h"

#include <wx/image.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>

namespace markamp::ui
{

ImagePreviewPopover::ImagePreviewPopover(wxWindow* parent, core::ThemeEngine& theme_engine)
    : wxPopupTransientWindow(parent, wxBORDER_SIMPLE)
    , theme_engine_(theme_engine)
{
    CreateLayout();
    ApplyTheme();
}

void ImagePreviewPopover::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    thumbnail_ = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize);

    alt_label_ = new wxStaticText(this, wxID_ANY, "");
    auto alt_font = alt_label_->GetFont();
    alt_font.SetWeight(wxFONTWEIGHT_BOLD);
    alt_label_->SetFont(alt_font);

    path_label_ = new wxStaticText(this, wxID_ANY, "");
    auto path_font = path_label_->GetFont();
    path_font.SetPointSize(path_font.GetPointSize() - 1);
    path_label_->SetFont(path_font);

    error_label_ = new wxStaticText(this, wxID_ANY, "");
    error_label_->Hide();

    sizer->AddSpacer(kPadding);
    sizer->Add(thumbnail_, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_HORIZONTAL, kPadding);
    sizer->AddSpacer(4);
    sizer->Add(alt_label_, 0, wxLEFT | wxRIGHT, kPadding);
    sizer->Add(path_label_, 0, wxLEFT | wxRIGHT, kPadding);
    sizer->Add(error_label_, 0, wxLEFT | wxRIGHT, kPadding);
    sizer->AddSpacer(kPadding);

    SetSizer(sizer);
}

auto ImagePreviewPopover::SetImage(const std::filesystem::path& image_path,
                                   const std::string& alt_text) -> bool
{
    error_label_->Hide();
    thumbnail_->Show();

    if (!std::filesystem::exists(image_path))
    {
        ShowError("Image not found: " + image_path.filename().string());
        return false;
    }

    wxImage image;
    if (!image.LoadFile(wxString::FromUTF8(image_path.string())))
    {
        ShowError("Failed to load: " + image_path.filename().string());
        return false;
    }

    // Scale to fit within max thumbnail size, preserving aspect ratio
    int img_width = image.GetWidth();
    int img_height = image.GetHeight();

    if (img_width > kMaxThumbWidth || img_height > kMaxThumbHeight)
    {
        double scale_x = static_cast<double>(kMaxThumbWidth) / img_width;
        double scale_y = static_cast<double>(kMaxThumbHeight) / img_height;
        double scale = std::min(scale_x, scale_y);

        img_width = static_cast<int>(img_width * scale);
        img_height = static_cast<int>(img_height * scale);
        image.Rescale(img_width, img_height, wxIMAGE_QUALITY_BICUBIC);
    }

    thumbnail_->SetBitmap(wxBitmap(image));

    alt_label_->SetLabel(wxString::FromUTF8(alt_text.empty() ? "(no alt text)" : alt_text));
    path_label_->SetLabel(wxString::FromUTF8(image_path.filename().string()));

    GetSizer()->Fit(this);
    return true;
}

void ImagePreviewPopover::ApplyTheme()
{
    auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    auto muted = theme_engine_.color(core::ThemeColorToken::TextMuted);

    SetBackgroundColour(bg_color);
    alt_label_->SetForegroundColour(fg_color);
    path_label_->SetForegroundColour(muted);
    error_label_->SetForegroundColour(wxColour(200, 80, 80));

    Refresh();
}

void ImagePreviewPopover::ShowError(const std::string& message)
{
    thumbnail_->Hide();
    error_label_->SetLabel(wxString::FromUTF8(message));
    error_label_->Show();
    alt_label_->SetLabel("");
    path_label_->SetLabel("");

    GetSizer()->Fit(this);
}

} // namespace markamp::ui
