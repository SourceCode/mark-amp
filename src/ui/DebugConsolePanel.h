#pragma once

#include "core/DebugConsoleService.h"
#include "core/ExpressionEvaluator.h"
#include "core/LogLevel.h"

#include <wx/clipbrd.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

namespace markamp::ui
{

/// Debug Console Panel — interactive REPL with command history,
/// colored output, log level filter, and expression evaluation.
/// Replaces the previous stub implementation.
class DebugConsolePanel : public wxPanel
{
public:
    explicit DebugConsolePanel(wxWindow* parent);

    /// Full constructor with services.
    DebugConsolePanel(wxWindow* parent, core::DebugConsoleService* service);

    ~DebugConsolePanel() override = default;

    /// Set the debug console service.
    void set_service(core::DebugConsoleService* service);

    /// Get the expression evaluator.
    [[nodiscard]] auto evaluator() -> core::ExpressionEvaluator&;

    /// Refresh the output display.
    void RefreshContent();

    /// Apply theme colors.
    void ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour);

    /// Submit a command programmatically.
    void SubmitCommand(const std::string& command);

    /// Log level filter — entries below this level are hidden.
    void set_log_level_filter(core::LogLevel level);
    [[nodiscard]] auto log_level_filter() const -> core::LogLevel;

    /// Configurable font settings.
    void set_font_size(int size);
    void set_font_family(const std::string& family);

private:
    void CreateLayout();
    void OnInputEnter(wxCommandEvent& event);
    void OnInputKey(wxKeyEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void CopySelectedToClipboard();
    void RenderEntries();

    core::DebugConsoleService* service_{nullptr};
    core::DebugConsoleService owned_service_; ///< Fallback if no service injected
    core::ExpressionEvaluator evaluator_;
    core::LogLevel log_level_filter_{core::LogLevel::kTrace};
    int font_size_{11};
    std::string font_family_{"Menlo"};
    std::size_t listener_id_{0};

    // UI controls
    wxListCtrl* output_list_{nullptr};
    wxTextCtrl* input_ctrl_{nullptr};
    wxStaticText* prompt_label_{nullptr};
};

} // namespace markamp::ui
