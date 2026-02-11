#pragma once

#include "core/Theme.h"

#include <wx/panel.h>

#include <functional>
#include <string>

namespace markamp::ui
{

/// A custom-drawn card that renders a miniature MarkAmp UI preview using a
/// theme's own colors, with an active checkmark badge, hover border
/// highlight, and click/export/delete callbacks.
class ThemePreviewCard : public wxPanel
{
public:
    ThemePreviewCard(wxWindow* parent, const core::Theme& theme, bool is_active, bool is_builtin);

    void SetActive(bool active);
    [[nodiscard]] auto IsActive() const -> bool;
    [[nodiscard]] auto GetThemeId() const -> std::string;
    [[nodiscard]] auto GetThemeName() const -> std::string;
    [[nodiscard]] auto IsBuiltin() const -> bool;

    using ClickCallback = std::function<void(const std::string&)>;
    using ExportCallback = std::function<void(const std::string&)>;
    using DeleteCallback = std::function<void(const std::string&)>;
    void SetOnClick(ClickCallback callback);
    void SetOnExport(ExportCallback callback);
    void SetOnDelete(DeleteCallback callback);

    static constexpr int kCardWidth = 280;
    static constexpr int kCardHeight = 160;
    static constexpr int kPreviewHeight = 112;
    static constexpr int kFooterHeight = 48;
    static constexpr int kCornerRadius = 8;
    static constexpr int kBorderWidth = 2;
    static constexpr int kBadgeSize = 20;

private:
    core::Theme theme_;
    bool is_active_{false};
    bool is_builtin_{false};
    bool is_hovered_{false};
    wxRect export_btn_rect_;
    wxRect delete_btn_rect_;

    // Painting
    void OnPaint(wxPaintEvent& event);
    void DrawMiniPreview(wxDC& dc, const wxRect& area);
    void DrawFooter(wxDC& dc, const wxRect& area);
    void DrawActiveIndicator(wxDC& dc);
    void DrawExportButton(wxDC& dc, const wxRect& area);
    void DrawDeleteButton(wxDC& dc, const wxRect& area);

    // Mouse
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);

    ClickCallback on_click_;
    ExportCallback on_export_;
    DeleteCallback on_delete_;
};

} // namespace markamp::ui
