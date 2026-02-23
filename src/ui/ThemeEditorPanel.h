#pragma once

#include "../core/EventBus.h"
#include "../core/ThemeEngine.h"

#include <wx/button.h>
#include <wx/clrpicker.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <wx/treelist.h>

#include <map>
#include <string>
#include <vector>

namespace markamp::ui
{

class ThemeEditorPanel : public wxPanel
{
public:
    ThemeEditorPanel(wxWindow* parent, core::ThemeEngine& theme_engine);
    ~ThemeEditorPanel() override;

private:
    void init_ui();
    void populate_token_list();
    void update_preview();
    void update_validation_warnings();

    // Event Handlers
    void on_token_selected(wxTreeListEvent& event);
    void on_color_changed(wxColourPickerEvent& event);
    void on_export_clicked(wxCommandEvent& event);
    void on_reset_clicked(wxCommandEvent& event);
    void on_preview_timer(wxTimerEvent& event);

    // Core references
    core::ThemeEngine& theme_engine_;

    core::Subscription theme_sub_;

    // UI Components
    wxTreeListCtrl* token_tree_ = nullptr;
    wxColourPickerCtrl* color_picker_ = nullptr;
    wxStaticText* token_name_label_ = nullptr;
    wxStaticText* token_info_label_ = nullptr;
    wxStaticText* validation_warning_label_ = nullptr;
    wxButton* export_btn_ = nullptr;
    wxButton* reset_btn_ = nullptr;

    // State
    std::string selected_token_;
    std::string preview_token_buffer_;
    wxColour preview_color_buffer_;
    wxTimer preview_timer_;

    // Modified tokens tracked for export
    std::map<std::string, wxColour> modified_tokens_;

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
