#pragma once

#include "core/WebviewService.h"

#include <wx/html/htmlwin.h>
#include <wx/panel.h>

#include <string>

namespace markamp::ui
{

/// Webview host panel for rendering extension-provided HTML (#47).
/// Embeds a web view for extension HTML content with sandboxed messaging.
/// Mirrors VS Code's webview panel hosting.
class WebviewHostPanel : public wxPanel
{
public:
    /// Data-only constructor (for tests).
    WebviewHostPanel();

    /// UI constructor with rendering support.
    WebviewHostPanel(wxWindow* parent, core::WebviewService* service);

    /// Set the webview service to read from.
    void set_service(core::WebviewService* service);

    /// Set the active webview panel by view type.
    void set_active_panel(const std::string& view_type);

    /// Get the active panel's view type.
    [[nodiscard]] auto active_panel_type() const -> const std::string&;

    /// Get the HTML content of the active panel.
    [[nodiscard]] auto active_html() -> std::string;

    /// Post a message to the active webview.
    void post_message(const std::string& message_json);

    /// Whether scripting is enabled for the active panel.
    [[nodiscard]] auto scripts_enabled() const -> bool;

    /// Refresh displayed content.
    void RefreshContent();

    /// Apply theme colors.
    void ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour);

private:
    void CreateLayout();

    core::WebviewService* service_{nullptr};
    std::string active_panel_type_;

    // UI controls (null in data-only / test mode)
    wxHtmlWindow* html_window_{nullptr};
};

} // namespace markamp::ui
