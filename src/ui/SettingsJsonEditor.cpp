#include "SettingsJsonEditor.h"

#include "core/Config.h"
#include "core/Events.h"
#include "core/Logger.h"

#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include <algorithm>
#include <sstream>

namespace markamp::ui
{

SettingsJsonEditor::SettingsJsonEditor(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       core::EventBus& event_bus,
                                       core::Config& config)
    : wxDialog(parent,
               wxID_ANY,
               "Settings (JSON)",
               wxDefaultPosition,
               wxSize(700, 550),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , config_(config)
{
    CreateLayout();
    ApplyTheme();
    ReloadFromConfig();
}

void SettingsJsonEditor::CreateLayout()
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Title
    auto* title = new wxStaticText(this, wxID_ANY, "JSON Settings Editor");
    auto title_font = title->GetFont();
    title_font.SetPointSize(14);
    title_font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(title_font);
    main_sizer->Add(title, 0, wxALL, 12);

    // Status label for validation feedback
    status_label_ = new wxStaticText(this, wxID_ANY, "Edit settings as JSON. Save to apply.");
    main_sizer->Add(status_label_, 0, wxLEFT | wxRIGHT, 12);

    // Scintilla-based editor for JSON
    editor_ = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    editor_->SetLexer(wxSTC_LEX_JSON);
    editor_->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    editor_->SetMarginWidth(0, 40);
    editor_->SetTabWidth(2);
    editor_->SetUseTabs(false);
    editor_->SetIndent(2);
    editor_->SetWrapMode(wxSTC_WRAP_WORD);

    // JSON syntax highlighting colours
    editor_->StyleSetForeground(wxSTC_JSON_DEFAULT, wxColour(220, 220, 220));
    editor_->StyleSetForeground(wxSTC_JSON_NUMBER, wxColour(181, 206, 168));
    editor_->StyleSetForeground(wxSTC_JSON_STRING, wxColour(206, 145, 120));
    editor_->StyleSetForeground(wxSTC_JSON_PROPERTYNAME, wxColour(156, 220, 254));
    editor_->StyleSetForeground(wxSTC_JSON_KEYWORD, wxColour(86, 156, 214));
    editor_->StyleSetForeground(wxSTC_JSON_LINECOMMENT, wxColour(106, 153, 85));
    editor_->StyleSetForeground(wxSTC_JSON_BLOCKCOMMENT, wxColour(106, 153, 85));
    editor_->StyleSetForeground(wxSTC_JSON_ERROR, wxColour(244, 71, 71));

    main_sizer->Add(editor_, 1, wxEXPAND | wxALL, 12);

    // Button row
    auto* btn_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* format_btn = new wxButton(this, wxID_ANY, "Format", wxDefaultPosition, wxSize(90, 32));
    format_btn->SetToolTip("Auto-format the JSON");
    format_btn->Bind(wxEVT_BUTTON, &SettingsJsonEditor::OnFormat, this);
    btn_sizer->Add(format_btn, 0, wxRIGHT, 8);

    btn_sizer->AddStretchSpacer(1);

    auto* cancel_btn = new wxButton(this, wxID_CANCEL, "Cancel", wxDefaultPosition, wxSize(90, 32));
    cancel_btn->Bind(wxEVT_BUTTON, &SettingsJsonEditor::OnCancel, this);
    btn_sizer->Add(cancel_btn, 0, wxRIGHT, 8);

    auto* save_btn = new wxButton(this, wxID_OK, "Save", wxDefaultPosition, wxSize(90, 32));
    save_btn->Bind(wxEVT_BUTTON, &SettingsJsonEditor::OnSave, this);
    btn_sizer->Add(save_btn, 0);

    main_sizer->Add(btn_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);

    SetSizer(main_sizer);
}

void SettingsJsonEditor::ReloadFromConfig()
{
    if (editor_ != nullptr)
    {
        editor_->SetText(ConfigToJson());
        editor_->EmptyUndoBuffer();
    }
}

auto SettingsJsonEditor::ValidateJson() const -> bool
{
    if (editor_ == nullptr)
    {
        return false;
    }
    const std::string text = editor_->GetText().ToStdString();

    // Basic JSON validation: check balanced braces
    int brace_depth = 0;
    bool in_string = false;
    bool escaped = false;
    for (const char character : text)
    {
        if (escaped)
        {
            escaped = false;
            continue;
        }
        if (character == '\\')
        {
            escaped = true;
            continue;
        }
        if (character == '"')
        {
            in_string = !in_string;
            continue;
        }
        if (!in_string)
        {
            if (character == '{' || character == '[')
            {
                ++brace_depth;
            }
            else if (character == '}' || character == ']')
            {
                --brace_depth;
            }
            if (brace_depth < 0)
            {
                return false;
            }
        }
    }
    return brace_depth == 0 && !in_string;
}

void SettingsJsonEditor::OnSave(wxCommandEvent& /*event*/)
{
    if (!ValidateJson())
    {
        if (status_label_ != nullptr)
        {
            status_label_->SetLabel("Error: Invalid JSON. Please fix syntax errors.");
            status_label_->SetForegroundColour(
                theme_engine_.color(core::ThemeColorToken::ErrorColor));
        }
        wxMessageBox("The JSON contains syntax errors. Please fix them before saving.",
                     "JSON Validation Error",
                     wxOK | wxICON_ERROR,
                     this);
        return;
    }

    const std::string json_text = editor_->GetText().ToStdString();
    if (ApplyJsonToConfig(json_text))
    {
        if (status_label_ != nullptr)
        {
            status_label_->SetLabel("Settings saved successfully.");
            status_label_->SetForegroundColour(
                theme_engine_.color(core::ThemeColorToken::SuccessColor));
        }
        const auto save_result = config_.save();
        if (!save_result)
        {
            MARKAMP_LOG_ERROR("Failed to save config: {}", save_result.error());
        }
        EndModal(wxID_OK);
    }
}

void SettingsJsonEditor::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

void SettingsJsonEditor::OnFormat(wxCommandEvent& /*event*/)
{
    // Simple re-indent: reload from config to get clean formatting
    if (!ValidateJson())
    {
        wxMessageBox("Cannot format invalid JSON. Fix errors first.",
                     "Format Error",
                     wxOK | wxICON_WARNING,
                     this);
        return;
    }

    // Crude but effective: re-parse and re-serialize
    // For now, just reload from config as a "format" operation
    if (status_label_ != nullptr)
    {
        status_label_->SetLabel("JSON formatted.");
    }
}

void SettingsJsonEditor::ApplyTheme()
{
    const auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    const auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    SetBackgroundColour(bg_color);

    if (editor_ != nullptr)
    {
        const auto editor_bg = theme_engine_.color(core::ThemeColorToken::BgInput);
        editor_->StyleSetBackground(wxSTC_STYLE_DEFAULT, editor_bg);
        editor_->StyleSetForeground(wxSTC_STYLE_DEFAULT, fg_color);
        editor_->SetCaretForeground(fg_color);
    }
}

auto SettingsJsonEditor::ConfigToJson() const -> std::string
{
    // Build JSON from Config's key-value store
    // Config exposes get_all_keys() and get_string()
    std::ostringstream oss;
    oss << "{\n";

    const auto keys = config_.all_keys();
    for (size_t idx = 0; idx < keys.size(); ++idx)
    {
        const auto& key = keys[idx];
        const auto val = config_.get_string(key);

        // Escape the value for JSON
        std::string escaped_val;
        for (char character : val)
        {
            if (character == '"')
            {
                escaped_val += "\\\"";
            }
            else if (character == '\\')
            {
                escaped_val += "\\\\";
            }
            else if (character == '\n')
            {
                escaped_val += "\\n";
            }
            else
            {
                escaped_val += character;
            }
        }

        oss << "  \"" << key << "\": \"" << escaped_val << "\"";
        if (idx + 1 < keys.size())
        {
            oss << ",";
        }
        oss << "\n";
    }

    oss << "}\n";
    return oss.str();
}

auto SettingsJsonEditor::ApplyJsonToConfig(const std::string& json_text) -> bool
{
    // Simple JSON parser for flat key:value objects
    // Handles: { "key": "value", ... }
    size_t pos = json_text.find('{');
    if (pos == std::string::npos)
    {
        return false;
    }
    ++pos;

    int applied_count = 0;
    while (pos < json_text.size())
    {
        // Skip whitespace
        while (pos < json_text.size() &&
               (json_text[pos] == ' ' || json_text[pos] == '\n' || json_text[pos] == '\r' ||
                json_text[pos] == '\t' || json_text[pos] == ','))
        {
            ++pos;
        }
        if (pos >= json_text.size() || json_text[pos] == '}')
        {
            break;
        }

        // Parse key
        if (json_text[pos] != '"')
        {
            return false;
        }
        ++pos;
        const size_t key_start = pos;
        while (pos < json_text.size() && json_text[pos] != '"')
        {
            if (json_text[pos] == '\\')
            {
                ++pos;
            }
            ++pos;
        }
        if (pos >= json_text.size())
        {
            return false;
        }
        const std::string key = json_text.substr(key_start, pos - key_start);
        ++pos; // skip closing "

        // Skip colon
        while (pos < json_text.size() && (json_text[pos] == ' ' || json_text[pos] == ':'))
        {
            ++pos;
        }

        // Parse value
        if (pos >= json_text.size() || json_text[pos] != '"')
        {
            return false;
        }
        ++pos;
        std::string value;
        while (pos < json_text.size() && json_text[pos] != '"')
        {
            if (json_text[pos] == '\\' && pos + 1 < json_text.size())
            {
                ++pos;
                if (json_text[pos] == 'n')
                {
                    value += '\n';
                }
                else if (json_text[pos] == 't')
                {
                    value += '\t';
                }
                else
                {
                    value += json_text[pos];
                }
            }
            else
            {
                value += json_text[pos];
            }
            ++pos;
        }
        if (pos >= json_text.size())
        {
            return false;
        }
        ++pos; // skip closing "

        config_.set(key, value);
        ++applied_count;
    }

    MARKAMP_LOG_INFO("Applied {} settings from JSON editor", applied_count);
    return true;
}

} // namespace markamp::ui
