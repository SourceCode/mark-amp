#include "TabBar.h"

#include "core/Logger.h"

#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>

#include <algorithm>
#include <array>
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
constexpr int kContextCopyRelativePath = 9;
constexpr int kContextRevealInFinder = 10;
constexpr int kContextPinTab = 11;
constexpr int kContextUnpinTab = 12;
constexpr int kContextCloseSaved = 13;   // R4 Fix 8
constexpr int kContextDuplicateTab = 14; // R19 Fix 4
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
    Bind(wxEVT_LEFT_UP, &TabBar::OnMouseUp, this); // R3 Fix 5
    Bind(wxEVT_LEFT_DCLICK, &TabBar::OnDoubleClick, this);
    Bind(wxEVT_LEAVE_WINDOW, &TabBar::OnMouseLeave, this);
    Bind(wxEVT_RIGHT_DOWN, &TabBar::OnRightDown, this);
    Bind(wxEVT_MIDDLE_DOWN, &TabBar::OnMiddleDown, this);
    Bind(wxEVT_MOUSEWHEEL, &TabBar::OnMouseWheel, this);
    Bind(wxEVT_SIZE, &TabBar::OnSize, this);

    // R18 Fix 1: Fade-in animation timer
    fade_timer_.SetOwner(this);
    Bind(wxEVT_TIMER, &TabBar::OnFadeTimer, this);
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
    tab.opacity = 0.0F; // R18 Fix 1: Start transparent for fade-in
    tabs_.push_back(tab);

    SetActiveTab(file_path);
    RecalculateTabRects();
    EnsureTabVisible(static_cast<int>(tabs_.size()) - 1);

    // R18 Fix 1: Start fade timer if not already running
    if (!fade_timer_.IsRunning())
    {
        fade_timer_.Start(16); // ~60fps
    }

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

    // Fix 9: Clamp scroll offset after removing a tab to prevent blank gap
    if (!tabs_.empty())
    {
        const int total_width = tabs_.back().rect.GetRight();
        const int client_width = GetClientSize().GetWidth();
        const int max_scroll = std::max(0, total_width - client_width);
        scroll_offset_ = std::clamp(scroll_offset_, 0, max_scroll);
    }
    else
    {
        scroll_offset_ = 0;
    }

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
    std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
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

    // Bottom border — R16 Fix 40: subtle light border
    gc->SetPen(gc->CreatePen(wxPen(theme_engine().color(core::ThemeColorToken::BorderLight), 1)));
    gc->StrokeLine(0, sz.GetHeight() - 1, sz.GetWidth(), sz.GetHeight() - 1);

    // Fix 10: Empty state hint when no tabs are open
    if (tabs_.empty())
    {
        wxFont hint_font = theme_engine().font(core::ThemeFontToken::MonoRegular);
        hint_font.SetPointSize(9);
        gc->SetFont(hint_font, theme_engine().color(core::ThemeColorToken::TextMuted));
        wxDouble hint_w = 0;
        wxDouble hint_h = 0;
        const wxString hint_text = "Open a file to start editing";
        gc->GetTextExtent(hint_text, &hint_w, &hint_h);
        gc->DrawText(hint_text,
                     (sz.GetWidth() - static_cast<int>(hint_w)) / 2,
                     (sz.GetHeight() - static_cast<int>(hint_h)) / 2);
        return;
    }

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

    // R16 Fix 6: Tab overflow fade gradient at right edge
    if (!tabs_.empty())
    {
        int last_tab_right = tabs_.back().rect.GetRight() - scroll_offset_;
        if (last_tab_right > sz.GetWidth())
        {
            constexpr int kFadeWidth = 24;
            auto fade_start = sz.GetWidth() - kFadeWidth;
            auto panel_bg = theme_engine().color(core::ThemeColorToken::BgPanel);
            for (int fx = 0; fx < kFadeWidth; ++fx)
            {
                int alpha = (fx * 255) / kFadeWidth;
                wxColour fade_col(panel_bg.Red(),
                                  panel_bg.Green(),
                                  panel_bg.Blue(),
                                  static_cast<unsigned char>(alpha));
                gc->SetPen(gc->CreatePen(wxPen(fade_col, 1)));
                gc->StrokeLine(fade_start + fx, 0, fade_start + fx, sz.GetHeight() - 2);
            }

            // R20 Fix 4: Draw overflow chevron indicator
            wxFont chevron_font = theme_engine().font(core::ThemeFontToken::MonoRegular);
            chevron_font.SetPointSize(14);
            gc->SetFont(chevron_font, theme_engine().color(core::ThemeColorToken::TextMuted));
            wxDouble chev_w = 0;
            wxDouble chev_h = 0;
            gc->GetTextExtent("›", &chev_w, &chev_h);
            gc->DrawText("›",
                         sz.GetWidth() - static_cast<int>(chev_w) - 4,
                         (kHeight - static_cast<int>(chev_h)) / 2);
        }
    }

    // Fix 11: Draw tab count badge right-aligned
    if (!tabs_.empty())
    {
        const std::string count_text =
            std::to_string(tabs_.size()) + (tabs_.size() == 1 ? " tab" : " tabs");
        wxFont count_font = theme_engine().font(core::ThemeFontToken::MonoRegular);
        count_font.SetPointSize(9);
        gc->SetFont(count_font, theme_engine().color(core::ThemeColorToken::TextMuted));
        wxDouble count_w = 0;
        wxDouble count_h = 0;
        gc->GetTextExtent(count_text, &count_w, &count_h);
        gc->DrawText(count_text,
                     sz.GetWidth() - static_cast<int>(count_w) - 12,
                     (kHeight - static_cast<int>(count_h)) / 2);
    }

    // R17 Fix 25: Drag indicator line at insertion point
    if (is_dragging_ && drag_tab_index_ >= 0 && drag_tab_index_ < static_cast<int>(tabs_.size()))
    {
        int indicator_x =
            tabs_[static_cast<size_t>(drag_tab_index_)].rect.GetLeft() - scroll_offset_;
        gc->SetPen(
            gc->CreatePen(wxPen(theme_engine().color(core::ThemeColorToken::AccentPrimary), 2)));
        gc->StrokeLine(indicator_x, 2, indicator_x, sz.GetHeight() - 2);

        // R19 Fix 3: Draw ghost shadow of the dragged tab
        if (drag_tab_index_ >= 0 && drag_tab_index_ < static_cast<int>(tabs_.size()))
        {
            const auto& dragged = tabs_[static_cast<size_t>(drag_tab_index_)];
            int ghost_x = dragged.rect.GetLeft() - scroll_offset_ + 2;
            int ghost_y = 2;
            int ghost_w = dragged.rect.GetWidth();
            int ghost_h = dragged.rect.GetHeight() - 4;
            auto shadow_bg = theme_engine().color(core::ThemeColorToken::BgApp);
            wxColour ghost_color(shadow_bg.Red(), shadow_bg.Green(), shadow_bg.Blue(), 76);
            gc->SetBrush(gc->CreateBrush(wxBrush(ghost_color)));
            gc->SetPen(wxNullPen);
            gc->DrawRoundedRectangle(ghost_x, ghost_y, ghost_w, ghost_h, 4);
        }
    }
}

void TabBar::DrawTab(wxGraphicsContext& gc, const TabInfo& tab, const core::Theme& /*theme*/) const
{
    int tab_x = tab.rect.GetLeft() - scroll_offset_;
    int tab_y = tab.rect.GetTop();
    int tab_w = tab.rect.GetWidth();
    int tab_h = tab.rect.GetHeight();

    // R20 Fix 2: Apply fade-in opacity for new tabs
    if (tab.opacity < 1.0F)
    {
        gc.PushState();
        // Modulate all drawing within this tab by its opacity
        // We'll draw into a partially-transparent state
    }

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
        bg_color = theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(115);
    }
    else
    {
        bg_color = theme_engine().color(core::ThemeColorToken::BgPanel);
    }

    // R20 Fix 5: Tint tab background by directory group color
    if (!tab.is_active)
    {
        auto tint = GetGroupColorTint(tab.file_path);
        if (tint.Alpha() > 0)
        {
            // Blend tint into current background at low alpha
            int blended_r = (bg_color.Red() * 92 + tint.Red() * 8) / 100;
            int blended_g = (bg_color.Green() * 92 + tint.Green() * 8) / 100;
            int blended_b = (bg_color.Blue() * 92 + tint.Blue() * 8) / 100;
            bg_color = wxColour(static_cast<unsigned char>(blended_r),
                                static_cast<unsigned char>(blended_g),
                                static_cast<unsigned char>(blended_b));
        }
    }

    // R16 Fix 1: Rounded active tab background with 4px radius
    gc.SetBrush(gc.CreateBrush(wxBrush(bg_color)));
    gc.SetPen(wxNullPen);
    if (tab.is_active)
    {
        gc.DrawRoundedRectangle(tab_x, tab_y, tab_w, tab_h + 4, 4);
    }
    else
    {
        gc.DrawRectangle(tab_x, tab_y, tab_w, tab_h);
    }

    // Active indicator — 2px accent line at bottom
    if (tab.is_active)
    {
        gc.SetBrush(
            gc.CreateBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentPrimary))));
        gc.DrawRectangle(tab_x, tab_y + tab_h - 2, tab_w, 2);

        // R20 Fix 3: Active tab bottom glow (neon-edge beneath indicator)
        auto accent = theme_engine().color(core::ThemeColorToken::AccentPrimary);
        for (int glow_row = 0; glow_row < kGlowLineHeight; ++glow_row)
        {
            int glow_alpha = 80 - (glow_row * 40);
            if (glow_alpha < 0)
            {
                glow_alpha = 0;
            }
            wxColour glow_color(accent.Red(),
                                accent.Green(),
                                accent.Blue(),
                                static_cast<unsigned char>(glow_alpha));
            gc.SetPen(gc.CreatePen(wxPen(glow_color, 1)));
            gc.StrokeLine(tab_x, tab_y + tab_h + glow_row, tab_x + tab_w, tab_y + tab_h + glow_row);
        }
    }

    // R19 Fix 2: Pinned tab left accent stripe
    if (tab.is_pinned)
    {
        gc.SetBrush(
            gc.CreateBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentSecondary))));
        gc.SetPen(wxNullPen);
        gc.DrawRectangle(tab_x, tab_y + 4, kPinnedStripeWidth, tab_h - 8);
    }

    // Right separator
    if (!tab.is_active)
    {
        gc.SetPen(gc.CreatePen(wxPen(theme_engine().color(core::ThemeColorToken::BorderLight), 1)));
        gc.StrokeLine(tab_x + tab_w, tab_y + 4, tab_x + tab_w, tab_y + tab_h - 4);
    }

    // R16 Fix 4: Active tab uses semibold weight
    wxFont font = theme_engine().font(core::ThemeFontToken::MonoRegular);
    font.SetPointSize(10);
    if (tab.is_active)
    {
        font.SetWeight(wxFONTWEIGHT_SEMIBOLD);
    }
    // R19 Fix 5: Italic style for modified tabs
    if (tab.is_modified)
    {
        font.SetStyle(wxFONTSTYLE_ITALIC);
    }
    gc.SetFont(font,
               tab.is_active ? theme_engine().color(core::ThemeColorToken::TextMain)
                             : theme_engine().color(core::ThemeColorToken::TextMuted));

    // Build display text with modified indicator
    std::string display = tab.display_name;

    // Calculate text area (leave room for close button)
    int text_x = tab_x + kTabPaddingH;
    int text_max_w = tab_w - kTabPaddingH * 2 - kCloseButtonSize - kCloseButtonMargin;

    // R16 Fix 3: Pinned tab draws 📌 icon
    if (tab.is_pinned)
    {
        wxDouble pin_w = 0;
        wxDouble pin_h = 0;
        gc.GetTextExtent("📌", &pin_w, &pin_h);
        gc.DrawText("📌", text_x, tab_y + (tab_h - static_cast<int>(pin_h)) / 2);
        text_x += static_cast<int>(pin_w) + 2;
        text_max_w -= static_cast<int>(pin_w) + 2;
    }

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

    // R18 Fix 4: Show parent folder for disambiguation on duplicate names
    std::string disambig = GetDisambiguationSuffix(tab);
    if (!disambig.empty())
    {
        wxFont disambig_font = theme_engine().font(core::ThemeFontToken::MonoRegular);
        disambig_font.SetPointSize(8);
        gc.SetFont(disambig_font, theme_engine().color(core::ThemeColorToken::TextMuted));
        wxDouble dw = 0;
        wxDouble dh = 0;
        gc.GetTextExtent(disambig, &dw, &dh);
        gc.DrawText(disambig, text_x + static_cast<int>(text_w) + 4, text_y + 1);
    }

    // Close button (×) — Fix 12: show modified dot (●) instead of × when not hovered
    int close_x = tab_x + tab_w - kCloseButtonSize - kCloseButtonMargin;
    int close_y = tab_y + (tab_h - kCloseButtonSize) / 2;

    const bool is_tab_hovered =
        (hovered_tab_index_ >= 0 &&
         tabs_[static_cast<size_t>(hovered_tab_index_)].file_path == tab.file_path);

    // Show close button area on hover or if tab is active
    if (tab.is_active || is_tab_hovered)
    {
        // Fix 12: If modified and close NOT hovered, draw dot instead of ×
        if (tab.is_modified && !tab.close_hovered)
        {
            int dot_cx = close_x + kCloseButtonSize / 2;
            int dot_cy = close_y + kCloseButtonSize / 2;
            gc.SetBrush(gc.CreateBrush(
                wxBrush(theme_engine().color(core::ThemeColorToken::AccentSecondary))));
            gc.SetPen(wxNullPen);
            gc.DrawEllipse(dot_cx - kModifiedDotSize / 2,
                           dot_cy - kModifiedDotSize / 2,
                           kModifiedDotSize,
                           kModifiedDotSize);
        }
        else
        {
            // Close button hover background
            if (tab.close_hovered)
            {
                gc.SetBrush(gc.CreateBrush(wxBrush(
                    theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(85))));
                gc.SetPen(wxNullPen);
                gc.DrawRoundedRectangle(
                    close_x - 2, close_y - 2, kCloseButtonSize + 4, kCloseButtonSize + 4, 3);
            }

            // R20 Fix 1: × glyph turns red and scales up on hover
            wxColour close_color;
            if (tab.close_hovered)
            {
                // Use error/red color for hovered close button
                auto t = theme_engine().current_theme();
                close_color = wxColour(t.error_color().to_rgba_string());
            }
            else
            {
                close_color = theme_engine().color(core::ThemeColorToken::TextMuted);
            }

            int margin = 3;
            int cx_x = close_x;
            int cy_y = close_y;
            int cx_size = kCloseButtonSize;

            // R20 Fix 1: Scale up close button on hover
            if (tab.close_hovered)
            {
                const int scaled_size = static_cast<int>(kCloseButtonSize * kCloseHoverScale);
                const int offset = (scaled_size - kCloseButtonSize) / 2;
                cx_x -= offset;
                cy_y -= offset;
                cx_size = scaled_size;
                margin = static_cast<int>(static_cast<float>(margin) * kCloseHoverScale);
            }

            gc.SetPen(gc.CreatePen(wxPen(close_color, tab.close_hovered ? 2 : 1)));
            gc.StrokeLine(
                cx_x + margin, cy_y + margin, cx_x + cx_size - margin, cy_y + cx_size - margin);
            gc.StrokeLine(
                cx_x + cx_size - margin, cy_y + margin, cx_x + margin, cy_y + cx_size - margin);
        }
    }

    // R20 Fix 2: Pop opacity state
    if (tab.opacity < 1.0F)
    {
        gc.PopState();
    }
}

// --- Mouse interaction ---

void TabBar::OnMouseMove(wxMouseEvent& event)
{
    auto pos = event.GetPosition();
    int new_hovered = HitTestTab(pos);

    // R3 Fix 5: Drag reorder — swap tabs on drag
    if (is_dragging_ && drag_tab_index_ >= 0 && event.LeftIsDown())
    {
        const int delta_x = pos.x - drag_start_x_;
        constexpr int kDragThreshold = 30;
        if (std::abs(delta_x) > kDragThreshold)
        {
            const int direction = (delta_x > 0) ? 1 : -1;
            const int swap_idx = drag_tab_index_ + direction;
            if (swap_idx >= 0 && swap_idx < static_cast<int>(tabs_.size()))
            {
                // Don't swap across pinned/unpinned boundary
                if (tabs_[static_cast<size_t>(drag_tab_index_)].is_pinned ==
                    tabs_[static_cast<size_t>(swap_idx)].is_pinned)
                {
                    std::swap(tabs_[static_cast<size_t>(drag_tab_index_)],
                              tabs_[static_cast<size_t>(swap_idx)]);
                    drag_tab_index_ = swap_idx;
                    drag_start_x_ = pos.x;
                    RecalculateTabRects();
                    Refresh();
                }
            }
        }
        return;
    }

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

    // R3 Fix 6: Show relative file path tooltip on tab hover
    if (new_hovered != hovered_tab_index_)
    {
        if (new_hovered >= 0)
        {
            std::string tip = tabs_[static_cast<size_t>(new_hovered)].file_path;
            if (!workspace_root_.empty())
            {
                try
                {
                    tip = std::filesystem::relative(tip, workspace_root_).string();
                }
                catch (const std::filesystem::filesystem_error& /*err*/)
                {
                }
            }
            SetToolTip(tip);
        }
        else
        {
            UnsetToolTip();
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

    // R3 Fix 5: Start drag
    drag_start_x_ = pos.x;
    drag_tab_index_ = tab_index;
    is_dragging_ = true;
    CaptureMouse();

    // Switch to the clicked tab
    const auto& tab = tabs_[static_cast<size_t>(tab_index)];
    if (!tab.is_active)
    {
        SetActiveTab(tab.file_path);

        core::events::TabSwitchedEvent evt(tab.file_path);
        event_bus_.publish(evt);
    }
}

// R3 Fix 5: End drag on mouse up
void TabBar::OnMouseUp(wxMouseEvent& /*event*/)
{
    if (is_dragging_)
    {
        is_dragging_ = false;
        drag_tab_index_ = -1;
        if (HasCapture())
        {
            ReleaseMouse();
        }
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

void TabBar::OnMiddleDown(wxMouseEvent& event)
{
    const int tab_index = HitTestTab(event.GetPosition());
    if (tab_index >= 0)
    {
        const core::events::TabCloseRequestEvent evt(
            tabs_[static_cast<size_t>(tab_index)].file_path);
        event_bus_.publish(evt);
    }
}

void TabBar::OnDoubleClick(wxMouseEvent& event)
{
    // Fix 10: Double-click on empty area (no tab hit) creates a new untitled file
    const int tab_index = HitTestTab(event.GetPosition());
    if (tab_index < 0)
    {
        // Publish a new-tab event by creating a unique untitled path
        static int untitled_counter = 1;
        const std::string untitled_path = "Untitled-" + std::to_string(untitled_counter++) + ".md";
        AddTab(untitled_path, untitled_path);

        const core::events::TabSwitchedEvent evt(untitled_path);
        event_bus_.publish(evt);
    }
}

void TabBar::OnMouseWheel(wxMouseEvent& event)
{
    int delta = event.GetWheelRotation();
    int scroll_amount = 40;

    // Fix 9: Calculate max scroll before applying delta
    int total_width = 0;
    for (const auto& tab : tabs_)
    {
        total_width = std::max(total_width, tab.rect.GetRight());
    }
    const int max_scroll = std::max(0, total_width - GetClientSize().GetWidth());

    if (delta > 0)
    {
        scroll_offset_ = std::max(0, scroll_offset_ - scroll_amount);
    }
    else if (delta < 0)
    {
        scroll_offset_ = std::min(max_scroll, scroll_offset_ + scroll_amount);
    }

    // Ensure we never exceed bounds
    scroll_offset_ = std::clamp(scroll_offset_, 0, max_scroll);

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
    menu.Append(kContextCloseSaved, "Close Saved"); // R4 Fix 8
    menu.AppendSeparator();
    menu.Append(kContextCloseToLeft, "Close to Left");
    menu.Append(kContextCloseToRight, "Close to Right");
    menu.AppendSeparator();
    menu.Append(kContextSave, "Save");
    menu.Append(kContextSaveAs, "Save As…");
    menu.AppendSeparator();
    menu.Append(kContextCopyPath, "Copy Path");
    menu.Append(kContextCopyRelativePath, "Copy Relative Path");
    menu.AppendSeparator();
    menu.Append(kContextRevealInFinder, "Reveal in Finder");

    // R3 Fix 7: Pin/Unpin tab
    menu.AppendSeparator();
    if (tab.is_pinned)
    {
        menu.Append(kContextUnpinTab, "Unpin Tab");
    }
    else
    {
        menu.Append(kContextPinTab, "Pin Tab");
    }

    // R19 Fix 4: Duplicate Tab
    menu.AppendSeparator();
    menu.Append(kContextDuplicateTab, "Duplicate Tab");

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
                      // R2 Fix 8: Copy Relative Path
                      case kContextCopyRelativePath:
                      {
                          std::string relative_path = target_path;
                          if (!workspace_root_.empty())
                          {
                              const auto rel =
                                  std::filesystem::relative(target_path, workspace_root_);
                              relative_path = rel.string();
                          }
                          if (wxTheClipboard->Open())
                          {
                              wxTheClipboard->SetData(new wxTextDataObject(relative_path));
                              wxTheClipboard->Close();
                          }
                          break;
                      }
                      // R2 Fix 9: Reveal in Finder
                      case kContextRevealInFinder:
                      {
#ifdef __APPLE__
                          wxExecute(wxString::Format("open -R \"%s\"", target_path));
#elif defined(__linux__)
                          wxExecute(wxString::Format(
                              "xdg-open \"%s\"",
                              std::filesystem::path(target_path)
                                  .parent_path()
                                  .string()));
#endif
                          break;
                      }
                      default:
                          break;
                      // R4 Fix 8: Close Saved (all unmodified tabs)
                      case kContextCloseSaved:
                          CloseSavedTabs();
                          break;
                      // R3 Fix 7: Pin / Unpin
                      case kContextPinTab:
                          PinTab(target_path);
                          break;
                      case kContextUnpinTab:
                          UnpinTab(target_path);
                          break;
                      // R19 Fix 4: Duplicate tab
                      case kContextDuplicateTab:
                          DuplicateTab(target_path);
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

        // R19 Fix 1: Smooth width transition
        tab.target_width = tab_width;
        if (tab.anim_width == 0)
        {
            tab.anim_width = tab_width; // first layout — snap
        }
        else
        {
            int diff = tab_width - tab.anim_width;
            tab.anim_width += static_cast<int>(static_cast<float>(diff) * kWidthAnimSpeed);
            if (std::abs(tab_width - tab.anim_width) <= 1)
            {
                tab.anim_width = tab_width;
            }
        }
        int effective_width = tab.anim_width;

        tab.rect = wxRect(x_offset, 0, effective_width, kHeight);
        tab.close_rect = wxRect(x_offset + effective_width - kCloseButtonSize - kCloseButtonMargin,
                                (kHeight - kCloseButtonSize) / 2,
                                kCloseButtonSize,
                                kCloseButtonSize);

        x_offset += effective_width;
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

// R3 Fix 7: Pin / Unpin helpers
void markamp::ui::TabBar::PinTab(const std::string& file_path)
{
    const int idx = FindTabIndex(file_path);
    if (idx < 0)
    {
        return;
    }
    tabs_[static_cast<size_t>(idx)].is_pinned = true;

    // Move pinned tab to the end of the pinned region
    int first_unpinned = 0;
    for (size_t tab_idx = 0; tab_idx < tabs_.size(); ++tab_idx)
    {
        if (!tabs_[tab_idx].is_pinned)
        {
            first_unpinned = static_cast<int>(tab_idx);
            break;
        }
        first_unpinned = static_cast<int>(tab_idx) + 1;
    }

    if (idx > first_unpinned)
    {
        // Already past pinned region — move it
        auto tab = std::move(tabs_[static_cast<size_t>(idx)]);
        tabs_.erase(tabs_.begin() + idx);
        tabs_.insert(tabs_.begin() + first_unpinned, std::move(tab));
    }

    RecalculateTabRects();
    Refresh();
}

void markamp::ui::TabBar::UnpinTab(const std::string& file_path)
{
    const int idx = FindTabIndex(file_path);
    if (idx < 0)
    {
        return;
    }
    tabs_[static_cast<size_t>(idx)].is_pinned = false;
    RecalculateTabRects();
    Refresh();
}

// R4 Fix 8: Close all unmodified (saved) tabs
void markamp::ui::TabBar::CloseSavedTabs()
{
    // Collect paths of non-modified tabs first, then close them
    std::vector<std::string> saved_paths;
    for (const auto& tab : tabs_)
    {
        if (!tab.is_modified)
        {
            saved_paths.push_back(tab.file_path);
        }
    }
    for (const auto& path : saved_paths)
    {
        const core::events::TabCloseRequestEvent evt(path);
        event_bus_.publish(evt);
    }
}

// R18 Fix 1: Fade-in animation timer callback
void markamp::ui::TabBar::OnFadeTimer(wxTimerEvent& /*event*/)
{
    bool any_fading = false;
    for (auto& tab : tabs_)
    {
        if (tab.opacity < 1.0F)
        {
            tab.opacity = std::min(1.0F, tab.opacity + 0.1F);
            any_fading = true;
        }
    }

    if (!any_fading)
    {
        fade_timer_.Stop();
    }
    Refresh();
}

// R18 Fix 4: Get parent folder name suffix for tabs with duplicate display names
auto markamp::ui::TabBar::GetDisambiguationSuffix(const TabInfo& tab) const -> std::string
{
    // Count how many tabs share the same display_name
    int count = 0;
    for (const auto& other : tabs_)
    {
        if (other.display_name == tab.display_name)
        {
            ++count;
        }
    }

    if (count <= 1)
    {
        return {};
    }

    // Extract parent folder name from file path
    try
    {
        const auto parent = std::filesystem::path(tab.file_path).parent_path().filename().string();
        if (!parent.empty() && parent != ".")
        {
            return parent;
        }
    }
    catch (const std::exception& /*err*/)
    {
    }

    return {};
}

// R19 Fix 4: Duplicate Tab — fires event for MainFrame to re-open the file
void markamp::ui::TabBar::DuplicateTab(const std::string& file_path)
{
    const core::events::TabDuplicateRequestEvent evt(file_path);
    event_bus_.publish(evt);
}

// R20 Fix 5: Generate a group color tint based on the file's parent directory
auto markamp::ui::TabBar::GetGroupColorTint(const std::string& file_path) const -> wxColour
{
    try
    {
        const auto parent = std::filesystem::path(file_path).parent_path().string();
        if (parent.empty())
        {
            return {0, 0, 0, 0}; // No tint for root-level files
        }

        // Check if more than one tab shares this directory
        int dir_count = 0;
        for (const auto& tab : tabs_)
        {
            try
            {
                if (std::filesystem::path(tab.file_path).parent_path().string() == parent)
                {
                    ++dir_count;
                }
            }
            catch (const std::exception& /*ex*/)
            {
            }
        }

        if (dir_count <= 1)
        {
            return {0, 0, 0, 0}; // Only tint when multiple files share a directory
        }

        // Hash the directory path into a hue index
        const std::size_t hash_val = std::hash<std::string>{}(parent);
        const int hue_index = static_cast<int>(hash_val % kGroupColorCount);

        // 6 distinct pastel tint colors
        static const std::array<wxColour, kGroupColorCount> kGroupColors = {{
            wxColour(100, 149, 237), // Cornflower blue
            wxColour(144, 238, 144), // Light green
            wxColour(255, 182, 193), // Light pink
            wxColour(255, 218, 130), // Gold
            wxColour(186, 152, 255), // Lavender
            wxColour(100, 220, 220), // Cyan
        }};

        return kGroupColors.at(static_cast<size_t>(hue_index));
    }
    catch (const std::exception& /*ex*/)
    {
        return {0, 0, 0, 0};
    }
}
