#include "TabBar.h"

#include "core/Logger.h"

#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>

#include <algorithm>
#include <cmath>
#include <filesystem>

namespace markamp::ui
{

namespace
{
constexpr int kContextClose = 1;
constexpr int kContextCloseOthers = 2;
constexpr int kContextCloseAll = 3;
constexpr int kContextCloseToLeft = 4;
constexpr int kContextCloseToRight = 5;
constexpr int kContextSave = 6;
constexpr int kContextSaveAs = 7;
constexpr int kContextCopyPath = 8;
} // namespace

TabBar::TabBar(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxSize(-1, kHeight), wxNO_BORDER)
    , event_bus_(event_bus)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, kHeight));
    SetMaxSize(wxSize(-1, kHeight));

    Bind(wxEVT_PAINT, &TabBar::OnPaint, this);
    Bind(wxEVT_MOTION, &TabBar::OnMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &TabBar::OnMouseDown, this);
    Bind(wxEVT_LEAVE_WINDOW, &TabBar::OnMouseLeave, this);
    Bind(wxEVT_RIGHT_DOWN, &TabBar::OnRightDown, this);
    Bind(wxEVT_MOUSEWHEEL, &TabBar::OnMouseWheel, this);
    Bind(wxEVT_SIZE, &TabBar::OnSize, this);
}

// --- Tab management ---

void TabBar::AddTab(const std::string& file_path, const std::string& display_name)
{
    // If already exists, just activate
    int idx = FindTabIndex(file_path);
    if (idx >= 0)
    {
        SetActiveTab(file_path);
        return;
    }

    TabInfo tab;
    tab.file_path = file_path;
    tab.display_name = display_name;
    tab.is_modified = false;
    tab.is_active = false;
    tabs_.push_back(tab);

    SetActiveTab(file_path);
    RecalculateTabRects();
    EnsureTabVisible(static_cast<int>(tabs_.size()) - 1);
    Refresh();
}

void TabBar::RemoveTab(const std::string& file_path)
{
    int idx = FindTabIndex(file_path);
    if (idx < 0)
    {
        return;
    }

    bool was_active = tabs_[static_cast<size_t>(idx)].is_active;
    tabs_.erase(tabs_.begin() + idx);

    // If the closed tab was active, activate an adjacent tab
    if (was_active && !tabs_.empty())
    {
        int new_active = std::min(idx, static_cast<int>(tabs_.size()) - 1);
        tabs_[static_cast<size_t>(new_active)].is_active = true;

        core::events::TabSwitchedEvent evt(tabs_[static_cast<size_t>(new_active)].file_path);
        event_bus_.publish(evt);
    }

    RecalculateTabRects();
    Refresh();
}

void TabBar::SetActiveTab(const std::string& file_path)
{
    bool changed = false;
    for (auto& tab : tabs_)
    {
        bool should_be_active = (tab.file_path == file_path);
        if (tab.is_active != should_be_active)
        {
            tab.is_active = should_be_active;
            changed = true;
        }
    }

    if (changed)
    {
        int idx = FindTabIndex(file_path);
        if (idx >= 0)
        {
            EnsureTabVisible(idx);
        }
        Refresh();
    }
}

void TabBar::SetTabModified(const std::string& file_path, bool modified)
{
    int idx = FindTabIndex(file_path);
    if (idx >= 0 && tabs_[static_cast<size_t>(idx)].is_modified != modified)
    {
        tabs_[static_cast<size_t>(idx)].is_modified = modified;
        Refresh();
    }
}

void TabBar::RenameTab(const std::string& old_path,
                       const std::string& new_path,
                       const std::string& new_display_name)
{
    int idx = FindTabIndex(old_path);
    if (idx >= 0)
    {
        tabs_[static_cast<size_t>(idx)].file_path = new_path;
        tabs_[static_cast<size_t>(idx)].display_name = new_display_name;
        RecalculateTabRects();
        Refresh();
    }
}

// --- Queries ---

auto TabBar::GetActiveTabPath() const -> std::string
{
    for (const auto& tab : tabs_)
    {
        if (tab.is_active)
        {
            return tab.file_path;
        }
    }
    return {};
}

auto TabBar::GetTabCount() const -> int
{
    return static_cast<int>(tabs_.size());
}

auto TabBar::HasTab(const std::string& file_path) const -> bool
{
    return FindTabIndex(file_path) >= 0;
}

auto TabBar::IsTabModified(const std::string& file_path) const -> bool
{
    int idx = FindTabIndex(file_path);
    return idx >= 0 && tabs_[static_cast<size_t>(idx)].is_modified;
}

auto TabBar::GetAllTabPaths() const -> std::vector<std::string>
{
    std::vector<std::string> paths;
    paths.reserve(tabs_.size());
    for (const auto& tab : tabs_)
    {
        paths.push_back(tab.file_path);
    }
    return paths;
}

// --- Tab cycling ---

void TabBar::ActivateNextTab()
{
    if (tabs_.size() <= 1)
    {
        return;
    }

    for (size_t idx = 0; idx < tabs_.size(); ++idx)
    {
        if (tabs_[idx].is_active)
        {
            size_t next = (idx + 1) % tabs_.size();
            SetActiveTab(tabs_[next].file_path);

            core::events::TabSwitchedEvent evt(tabs_[next].file_path);
            event_bus_.publish(evt);
            return;
        }
    }
}

void TabBar::ActivatePreviousTab()
{
    if (tabs_.size() <= 1)
    {
        return;
    }

    for (size_t idx = 0; idx < tabs_.size(); ++idx)
    {
        if (tabs_[idx].is_active)
        {
            size_t prev = (idx == 0) ? tabs_.size() - 1 : idx - 1;
            SetActiveTab(tabs_[prev].file_path);

            core::events::TabSwitchedEvent evt(tabs_[prev].file_path);
            event_bus_.publish(evt);
            return;
        }
    }
}

// --- Batch operations ---

void TabBar::CloseAllTabs()
{
    // Collect all paths, then publish close requests
    auto paths = GetAllTabPaths();
    for (const auto& path : paths)
    {
        core::events::TabCloseRequestEvent evt(path);
        event_bus_.publish(evt);
    }
}

void TabBar::CloseOtherTabs(const std::string& keep_path)
{
    auto paths = GetAllTabPaths();
    for (const auto& path : paths)
    {
        if (path != keep_path)
        {
            core::events::TabCloseRequestEvent evt(path);
            event_bus_.publish(evt);
        }
    }
}

void TabBar::CloseTabsToLeft(const std::string& of_path)
{
    int idx = FindTabIndex(of_path);
    if (idx <= 0)
    {
        return;
    }

    for (int tab_idx = 0; tab_idx < idx; ++tab_idx)
    {
        core::events::TabCloseRequestEvent evt(tabs_[static_cast<size_t>(tab_idx)].file_path);
        event_bus_.publish(evt);
    }
}

void TabBar::CloseTabsToRight(const std::string& of_path)
{
    int idx = FindTabIndex(of_path);
    if (idx < 0 || idx >= static_cast<int>(tabs_.size()) - 1)
    {
        return;
    }

    for (int tab_idx = static_cast<int>(tabs_.size()) - 1; tab_idx > idx; --tab_idx)
    {
        core::events::TabCloseRequestEvent evt(tabs_[static_cast<size_t>(tab_idx)].file_path);
        event_bus_.publish(evt);
    }
}

// --- Painting ---

void TabBar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    auto* gc = wxGraphicsContext::Create(dc);
    if (gc == nullptr)
    {
        return;
    }

    auto sz = GetClientSize();
    const auto& theme = theme_engine().current_theme();

    // Background
    gc->SetBrush(gc->CreateBrush(wxBrush(theme_engine().color(core::ThemeColorToken::BgPanel))));
    gc->SetPen(wxNullPen);
    gc->DrawRectangle(0, 0, sz.GetWidth(), sz.GetHeight());

    // Bottom border
    gc->SetPen(gc->CreatePen(wxPen(theme_engine().color(core::ThemeColorToken::BorderDark), 1)));
    gc->StrokeLine(0, sz.GetHeight() - 1, sz.GetWidth(), sz.GetHeight() - 1);

    // Draw tabs with scroll offset
    gc->Clip(0, 0, sz.GetWidth(), sz.GetHeight());

    for (const auto& tab : tabs_)
    {
        // Only draw if visible in the viewport
        if (tab.rect.GetRight() - scroll_offset_ > 0 &&
            tab.rect.GetLeft() - scroll_offset_ < sz.GetWidth())
        {
            DrawTab(*gc, tab, theme);
        }
    }

    delete gc;
}

void TabBar::DrawTab(wxGraphicsContext& gc, const TabInfo& tab, const core::Theme& /*theme*/) const
{
    int tab_x = tab.rect.GetLeft() - scroll_offset_;
    int tab_y = tab.rect.GetTop();
    int tab_w = tab.rect.GetWidth();
    int tab_h = tab.rect.GetHeight();

    // Tab background
    wxColour bg_color;
    if (tab.is_active)
    {
        bg_color = theme_engine().color(core::ThemeColorToken::BgApp);
    }
    else if (tab.close_hovered ||
             (hovered_tab_index_ >= 0 &&
              tabs_[static_cast<size_t>(hovered_tab_index_)].file_path == tab.file_path))
    {
        bg_color = theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(110);
    }
    else
    {
        bg_color = theme_engine().color(core::ThemeColorToken::BgPanel);
    }

    gc.SetBrush(gc.CreateBrush(wxBrush(bg_color)));
    gc.SetPen(wxNullPen);
    gc.DrawRectangle(tab_x, tab_y, tab_w, tab_h);

    // Active indicator — 2px accent line at bottom
    if (tab.is_active)
    {
        gc.SetBrush(
            gc.CreateBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentPrimary))));
        gc.DrawRectangle(tab_x, tab_y + tab_h - 2, tab_w, 2);
    }

    // Right separator
    if (!tab.is_active)
    {
        gc.SetPen(gc.CreatePen(wxPen(theme_engine().color(core::ThemeColorToken::BorderLight), 1)));
        gc.StrokeLine(tab_x + tab_w, tab_y + 4, tab_x + tab_w, tab_y + tab_h - 4);
    }

    // Text
    wxFont font = theme_engine().font(core::ThemeFontToken::MonoRegular);
    font.SetPointSize(10);
    gc.SetFont(font,
               tab.is_active ? theme_engine().color(core::ThemeColorToken::TextMain)
                             : theme_engine().color(core::ThemeColorToken::TextMuted));

    // Build display text with modified indicator
    std::string display = tab.display_name;

    // Calculate text area (leave room for close button)
    int text_x = tab_x + kTabPaddingH;
    int text_max_w = tab_w - kTabPaddingH * 2 - kCloseButtonSize - kCloseButtonMargin;

    // Modified dot (●) before filename
    if (tab.is_modified)
    {
        int dot_y = tab_y + (tab_h - kModifiedDotSize) / 2;
        gc.SetBrush(
            gc.CreateBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentSecondary))));
        gc.SetPen(wxNullPen);
        gc.DrawEllipse(text_x, dot_y, kModifiedDotSize, kModifiedDotSize);
        text_x += kModifiedDotSize + 4;
        text_max_w -= kModifiedDotSize + 4;
    }

    // Truncate text if necessary
    wxDouble text_w = 0;
    wxDouble text_h = 0;
    gc.GetTextExtent(display, &text_w, &text_h);

    if (text_w > text_max_w && text_max_w > 0)
    {
        // Truncate with ellipsis
        while (display.length() > 1)
        {
            display.pop_back();
            gc.GetTextExtent(display + "…", &text_w, &text_h);
            if (text_w <= text_max_w)
            {
                display += "…";
                break;
            }
        }
    }

    int text_y = tab_y + (tab_h - static_cast<int>(text_h)) / 2;
    gc.DrawText(display, text_x, text_y);

    // Close button (×)
    int close_x = tab_x + tab_w - kCloseButtonSize - kCloseButtonMargin;
    int close_y = tab_y + (tab_h - kCloseButtonSize) / 2;

    // Show close button on hover or if tab is active
    if (tab.is_active ||
        (hovered_tab_index_ >= 0 &&
         tabs_[static_cast<size_t>(hovered_tab_index_)].file_path == tab.file_path))
    {
        // Close button hover background
        if (tab.close_hovered)
        {
            gc.SetBrush(gc.CreateBrush(
                wxBrush(theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(85))));
            gc.SetPen(wxNullPen);
            gc.DrawRoundedRectangle(
                close_x - 2, close_y - 2, kCloseButtonSize + 4, kCloseButtonSize + 4, 3);
        }

        // Draw × glyph
        gc.SetPen(gc.CreatePen(wxPen(tab.close_hovered
                                         ? theme_engine().color(core::ThemeColorToken::TextMain)
                                         : theme_engine().color(core::ThemeColorToken::TextMuted),
                                     1)));
        int margin = 3;
        gc.StrokeLine(close_x + margin,
                      close_y + margin,
                      close_x + kCloseButtonSize - margin,
                      close_y + kCloseButtonSize - margin);
        gc.StrokeLine(close_x + kCloseButtonSize - margin,
                      close_y + margin,
                      close_x + margin,
                      close_y + kCloseButtonSize - margin);
    }
}

// --- Mouse interaction ---

void TabBar::OnMouseMove(wxMouseEvent& event)
{
    auto pos = event.GetPosition();
    int new_hovered = HitTestTab(pos);

    bool close_state_changed = false;

    // Update close button hover states
    for (size_t idx = 0; idx < tabs_.size(); ++idx)
    {
        bool was_hovered = tabs_[idx].close_hovered;
        tabs_[idx].close_hovered = (static_cast<int>(idx) == new_hovered) &&
                                   HitTestCloseButton(pos, static_cast<int>(idx));
        if (tabs_[idx].close_hovered != was_hovered)
        {
            close_state_changed = true;
        }
    }

    if (new_hovered != hovered_tab_index_ || close_state_changed)
    {
        hovered_tab_index_ = new_hovered;
        Refresh();
    }
}

void TabBar::OnMouseDown(wxMouseEvent& event)
{
    auto pos = event.GetPosition();
    int tab_index = HitTestTab(pos);

    if (tab_index < 0)
    {
        return;
    }

    // Check if close button was clicked
    if (HitTestCloseButton(pos, tab_index))
    {
        core::events::TabCloseRequestEvent evt(tabs_[static_cast<size_t>(tab_index)].file_path);
        event_bus_.publish(evt);
        return;
    }

    // Switch to the clicked tab
    const auto& tab = tabs_[static_cast<size_t>(tab_index)];
    if (!tab.is_active)
    {
        SetActiveTab(tab.file_path);

        core::events::TabSwitchedEvent evt(tab.file_path);
        event_bus_.publish(evt);
    }
}

void TabBar::OnMouseLeave(wxMouseEvent& /*event*/)
{
    if (hovered_tab_index_ >= 0)
    {
        for (auto& tab : tabs_)
        {
            tab.close_hovered = false;
        }
        hovered_tab_index_ = -1;
        Refresh();
    }
}

void TabBar::OnRightDown(wxMouseEvent& event)
{
    int tab_index = HitTestTab(event.GetPosition());
    if (tab_index >= 0)
    {
        ShowTabContextMenu(tab_index);
    }
}

void TabBar::OnMouseWheel(wxMouseEvent& event)
{
    int delta = event.GetWheelRotation();
    int scroll_amount = 40;

    if (delta > 0)
    {
        scroll_offset_ = std::max(0, scroll_offset_ - scroll_amount);
    }
    else if (delta < 0)
    {
        // Calculate max scroll
        int total_width = 0;
        for (const auto& tab : tabs_)
        {
            total_width = std::max(total_width, tab.rect.GetRight());
        }
        int max_scroll = std::max(0, total_width - GetClientSize().GetWidth());
        scroll_offset_ = std::min(max_scroll, scroll_offset_ + scroll_amount);
    }

    Refresh();
}

// --- Context menu ---

void TabBar::ShowTabContextMenu(int tab_index)
{
    if (tab_index < 0 || tab_index >= static_cast<int>(tabs_.size()))
    {
        return;
    }

    const auto& tab = tabs_[static_cast<size_t>(tab_index)];
    std::string target_path = tab.file_path;

    wxMenu menu;
    menu.Append(kContextClose, "Close");
    menu.Append(kContextCloseOthers, "Close Others");
    menu.Append(kContextCloseAll, "Close All");
    menu.AppendSeparator();
    menu.Append(kContextCloseToLeft, "Close to Left");
    menu.Append(kContextCloseToRight, "Close to Right");
    menu.AppendSeparator();
    menu.Append(kContextSave, "Save");
    menu.Append(kContextSaveAs, "Save As…");
    menu.AppendSeparator();
    menu.Append(kContextCopyPath, "Copy Path");

    // Disable close to left/right if not applicable
    menu.Enable(kContextCloseToLeft, tab_index > 0);
    menu.Enable(kContextCloseToRight, tab_index < static_cast<int>(tabs_.size()) - 1);
    menu.Enable(kContextCloseOthers, tabs_.size() > 1);

    menu.Bind(wxEVT_MENU,
              [this, target_path](wxCommandEvent& cmd_event)
              {
                  switch (cmd_event.GetId())
                  {
                      case kContextClose:
                      {
                          core::events::TabCloseRequestEvent evt(target_path);
                          event_bus_.publish(evt);
                          break;
                      }
                      case kContextCloseOthers:
                          CloseOtherTabs(target_path);
                          break;
                      case kContextCloseAll:
                          CloseAllTabs();
                          break;
                      case kContextCloseToLeft:
                          CloseTabsToLeft(target_path);
                          break;
                      case kContextCloseToRight:
                          CloseTabsToRight(target_path);
                          break;
                      case kContextSave:
                      {
                          core::events::TabSaveRequestEvent evt(target_path);
                          event_bus_.publish(evt);
                          break;
                      }
                      case kContextSaveAs:
                      {
                          core::events::TabSaveAsRequestEvent evt(target_path);
                          event_bus_.publish(evt);
                          break;
                      }
                      case kContextCopyPath:
                      {
                          if (wxTheClipboard->Open())
                          {
                              wxTheClipboard->SetData(new wxTextDataObject(target_path));
                              wxTheClipboard->Close();
                          }
                          break;
                      }
                      default:
                          break;
                  }
              });

    PopupMenu(&menu);
}

// --- Hit testing ---

auto TabBar::HitTestTab(const wxPoint& point) const -> int
{
    for (size_t idx = 0; idx < tabs_.size(); ++idx)
    {
        wxRect adjusted = tabs_[idx].rect;
        adjusted.Offset(-scroll_offset_, 0);
        if (adjusted.Contains(point))
        {
            return static_cast<int>(idx);
        }
    }
    return -1;
}

auto TabBar::HitTestCloseButton(const wxPoint& point, int tab_index) const -> bool
{
    if (tab_index < 0 || tab_index >= static_cast<int>(tabs_.size()))
    {
        return false;
    }

    const auto& tab = tabs_[static_cast<size_t>(tab_index)];
    int close_x = tab.rect.GetLeft() - scroll_offset_ + tab.rect.GetWidth() - kCloseButtonSize -
                  kCloseButtonMargin;
    int close_y = tab.rect.GetTop() + (tab.rect.GetHeight() - kCloseButtonSize) / 2;

    wxRect close_rect(close_x - 2, close_y - 2, kCloseButtonSize + 4, kCloseButtonSize + 4);
    return close_rect.Contains(point);
}

// --- Layout ---

void TabBar::RecalculateTabRects()
{
    wxClientDC dc(this);
    wxFont font = theme_engine().font(core::ThemeFontToken::MonoRegular);
    font.SetPointSize(10);
    dc.SetFont(font);

    int x_offset = 0;
    for (auto& tab : tabs_)
    {
        auto text_extent = dc.GetTextExtent(tab.display_name);
        int tab_width =
            text_extent.GetWidth() + kTabPaddingH * 2 + kCloseButtonSize + kCloseButtonMargin + 4;

        // Add space for modified dot
        tab_width += kModifiedDotSize + 4;

        // Clamp to min/max
        tab_width = std::clamp(tab_width, kMinTabWidth, kMaxTabWidth);

        tab.rect = wxRect(x_offset, 0, tab_width, kHeight);
        tab.close_rect = wxRect(x_offset + tab_width - kCloseButtonSize - kCloseButtonMargin,
                                (kHeight - kCloseButtonSize) / 2,
                                kCloseButtonSize,
                                kCloseButtonSize);

        x_offset += tab_width;
    }
}

void TabBar::OnSize(wxSizeEvent& event)
{
    RecalculateTabRects();
    Refresh();
    event.Skip();
}

void TabBar::EnsureTabVisible(int tab_index)
{
    if (tab_index < 0 || tab_index >= static_cast<int>(tabs_.size()))
    {
        return;
    }

    const auto& tab = tabs_[static_cast<size_t>(tab_index)];
    int view_width = GetClientSize().GetWidth();

    if (tab.rect.GetLeft() < scroll_offset_)
    {
        scroll_offset_ = tab.rect.GetLeft();
    }
    else if (tab.rect.GetRight() > scroll_offset_ + view_width)
    {
        scroll_offset_ = tab.rect.GetRight() - view_width;
    }
}

// --- Helpers ---

auto TabBar::FindTabIndex(const std::string& file_path) const -> int
{
    for (size_t idx = 0; idx < tabs_.size(); ++idx)
    {
        if (tabs_[idx].file_path == file_path)
        {
            return static_cast<int>(idx);
        }
    }
    return -1;
}

// --- Theme ---

void TabBar::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    RecalculateTabRects();
    Refresh();
}

} // namespace markamp::ui
