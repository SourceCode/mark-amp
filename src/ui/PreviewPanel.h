#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/HtmlSanitizer.h"
#include "core/MarkdownParser.h"
#include "core/Types.h"
#include "rendering/HtmlRenderer.h"

#include <wx/html/htmlwin.h>
#include <wx/timer.h>

#include <filesystem>
#include <string>

namespace markamp::core
{
class Config;
}

namespace markamp::ui
{

class BevelPanel;

/// Preview pane that renders parsed markdown as themed HTML.
/// Uses wxHtmlWindow for display, with debounced rendering,
/// link handling, and scroll position management.
class PreviewPanel : public ThemeAwareWindow
{
public:
    PreviewPanel(wxWindow* parent,
                 core::ThemeEngine& theme_engine,
                 core::EventBus& event_bus,
                 core::Config* config = nullptr);
    ~PreviewPanel() override;

    // Non-copyable, non-movable (wxWidgets panel)
    PreviewPanel(const PreviewPanel&) = delete;
    PreviewPanel& operator=(const PreviewPanel&) = delete;
    PreviewPanel(PreviewPanel&&) = delete;
    PreviewPanel& operator=(PreviewPanel&&) = delete;

    // Content
    void SetMarkdownContent(const std::string& markdown);
    void Clear();

    // Scrolling
    void ScrollToTop();
    void SetScrollFraction(double fraction);

    // Scroll sync control
    void set_scroll_sync_enabled(bool enabled);

    // Export
    [[nodiscard]] auto ExportHtml(const std::filesystem::path& output_path) const -> bool;

    // CSS/HTML generation (public for testing)
    [[nodiscard]] auto GenerateCSS() const -> std::string;
    [[nodiscard]] auto GenerateFullHtml(const std::string& body_html) const -> std::string;

    // Base path for image resolution
    void set_base_path(const std::filesystem::path& base_path);

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    wxHtmlWindow* html_view_{nullptr};
    BevelPanel* bevel_overlay_{nullptr};
    core::MarkdownParser parser_;
    core::HtmlSanitizer sanitizer_;

    // Base path for relative image resolution
    std::filesystem::path base_path_;

    // Rendering pipeline
    void RenderContent(const std::string& markdown);
    void DisplayError(const std::string& error_message);

    // Debouncing
    int render_debounce_ms_{300};
    wxTimer render_timer_;
    std::string pending_content_;
    std::string last_rendered_content_;
    mutable std::string cached_css_;
    std::string last_rendered_html_;   // Improvement 25: cached HTML body for DisplayError
    rendering::HtmlRenderer renderer_; // Improvement 11: reused across renders
    void OnRenderTimer(wxTimerEvent& event);

    // Improvement 24: resize debounce
    static constexpr int kResizeDebounceMs = 150;
    wxTimer resize_timer_;
    void OnResizeTimer(wxTimerEvent& event);

    // Scroll synchronization
    static constexpr int kScrollSyncDebounceMs = 50;
    wxTimer scroll_sync_timer_;
    double pending_scroll_fraction_{0.0};
    bool scroll_sync_enabled_{true};
    void OnScrollSyncTimer(wxTimerEvent& event);

    // Event handling
    void OnLinkClicked(wxHtmlLinkEvent& event);
    void OnSize(wxSizeEvent& event);

    // Event subscriptions
    core::Subscription content_changed_sub_;
    core::Subscription view_mode_sub_;
    core::Subscription active_file_sub_;
    core::Subscription scroll_sync_sub_;

    // Zoom support
    int zoom_level_{0};
    void SetZoomLevel(int level);
    void OnMouseWheel(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // R21 Fix 32: Scroll-to-top button
    wxButton* scroll_to_top_btn_{nullptr};
    void UpdateScrollToTopButton();
    void PositionScrollToTopButton();
};

} // namespace markamp::ui
