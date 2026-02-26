#pragma once

#include "core/ThemeEngine.h"
#include "ui/ThemeAwareWindow.h"

#include <wx/bitmap.h>
#include <wx/panel.h>

#include <string>
#include <vector>

class wxStyledTextCtrl;

namespace markamp::ui
{

struct SyntaxToken
{
    int position;
    int length;
    int style;
};

class MinimapPanel : public ThemeAwareWindow
{
public:
    MinimapPanel(wxWindow* parent, core::ThemeEngine& theme_engine, wxStyledTextCtrl* editor);
    ~MinimapPanel() override = default;

    MinimapPanel(const MinimapPanel&) = delete;
    auto operator=(const MinimapPanel&) -> MinimapPanel& = delete;
    MinimapPanel(MinimapPanel&&) = delete;
    auto operator=(MinimapPanel&&) -> MinimapPanel& = delete;

    // Content Sync
    void SetContent(const std::string& content, const std::vector<SyntaxToken>& tokens);
    void SetViewportRange(int first_line, int lines_on_screen, int total_lines);

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);

    void RenderToBuffer();
    void InvalidateCache();
    void ScrollEditorToCoordinate(int y_pos);

    wxStyledTextCtrl* editor_; // Reference back to the main document

    // State
    std::string content_;
    std::vector<SyntaxToken> tokens_;
    int first_visible_line_{0};
    int lines_on_screen_{0};
    int total_lines_{0};

    // Rendering
    wxBitmap render_buffer_;
    bool cache_dirty_{true};

    // Interaction
    bool is_dragging_{false};
};

} // namespace markamp::ui
