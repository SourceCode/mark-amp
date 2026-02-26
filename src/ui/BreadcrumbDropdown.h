#pragma once

#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include "ui/DesignSystemContext.h"
#include "ui/ThemeAwareWindow.h"

#include <wx/graphics.h>
#include <wx/popupwin.h>

#include <string>
#include <vector>

namespace markamp::ui
{

/// Structure representing a single item within the Breadcrumb dropdown picker
struct DropdownItem
{
    std::string text;
    std::string icon_name;
    std::string full_path;
    bool is_folder = false;
    wxRect bounds;
};

/// A transient popup window that displays a scrollable list of siblings
/// for a given breadcrumb segment (files/folders or heading symbols).
class BreadcrumbDropdown : public wxPopupTransientWindow
{
public:
    BreadcrumbDropdown(wxWindow* parent,
                       DesignSystemContext& ds,
                       core::EventBus& event_bus,
                       const std::vector<DropdownItem>& items);

    ~BreadcrumbDropdown() override = default;

    /// Called when the popup is dismissed or loses focus externally
    void OnDismiss() override;

private:
    void OnPaint(wxPaintEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // Virtual focus overrides
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    void RenderBackground(wxGraphicsContext* gc, int width, int height) const;
    void RenderItems(wxPaintDC& dc, wxGraphicsContext* gc, int width);

    void SelectItem(int index);

    DesignSystemContext& ds_;
    core::EventBus& event_bus_;
    std::vector<DropdownItem> items_;

    int hovered_index_ = -1;
    int focused_index_ = 0; // The active list item via keyboard

    const int kItemHeight = 28;
    const int kMaxVisibleItems = 10;

    // Internal state cache for accurate rendering
    static const core::ThemeColorToken kBgColorToken;
    static const core::ThemeColorToken kHoverColorToken;
    static const core::ThemeColorToken kBorderColorToken;
    static const core::ThemeColorToken kTextColorToken;

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
