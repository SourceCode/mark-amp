#include "TabBar.h"

#include "LayoutMetrics.h"
#include "core/Logger.h"
#include "ui/FileTypeIconRegistry.h"
#include "ui/FocusManager.h"
#include "ui/FocusRingRenderer.h"
#include "ui/IconManager.h"
#include "ui/accessibility/AccessibilityController.h"

#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>
#include <wx/dnd.h>
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
constexpr int kContextSortByName = 15;   // Phase 11 Task 16
constexpr int kContextSortByPath = 16;   // Phase 11 Task 16

// Phase 11 Task 10: Drop Target for tab dragging
class TabDropTarget : public wxTextDropTarget
{
public:
    explicit TabDropTarget(TabBar* tab_bar)
        : tab_bar_(tab_bar)
    {
    }

    bool OnDropText(wxCoord /*x*/, wxCoord /*y*/, const wxString& data) override
    {
        if (data.StartsWith("markamp_tab:"))
        {
            wxString remains = data.Mid(12);
            long source_id = -1;
            int last_colon = remains.Find(':', true);
            wxString path = remains;
            if (last_colon != wxNOT_FOUND)
            {
                path = remains.Left(static_cast<size_t>(last_colon));
                remains.Mid(static_cast<size_t>(last_colon) + 1).ToLong(&source_id);
            }

            if (static_cast<int>(source_id) != tab_bar_->GetId())
            {
                std::filesystem::path p(path.ToStdString());
                tab_bar_->AddTab(path.ToStdString(), p.filename().string());

                const core::events::TabSwitchedEvent evt(path.ToStdString());
                tab_bar_->GetEventBus().publish(evt);
            }
            return true;
        }
        return false;
    }

private:
    TabBar* tab_bar_;
};
} // namespace

TabBar::TabBar(wxWindow* parent, DesignSystemContext& ds, core::EventBus& event_bus)
    : ThemeAwareWindow(parent,
                       ds.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxSize(-1, ds.metrics.tab_height()),
                       wxNO_BORDER | wxTAB_TRAVERSAL | wxWANTS_CHARS)
    , ds_(ds)
    , event_bus_(event_bus)
{
    const int kHeight = ds_.metrics.tab_height();
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, kHeight));
    SetMaxSize(wxSize(-1, kHeight));

    SetCanFocus(true);

    // Phase 11 Task 10: Enable drag and drop across tab bars
    SetDropTarget(new TabDropTarget(this));

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
    Bind(wxEVT_SET_FOCUS, &TabBar::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &TabBar::OnKillFocus, this);
    Bind(wxEVT_KEY_DOWN, &TabBar::OnKeyDown, this);

    // Phase 11 Task 11: Bind pulse timer
    Bind(wxEVT_TIMER, &TabBar::OnPulseTimer, this);

    keyboard_mode_sub_ = event_bus_.subscribe<core::events::KeyboardModeChangedEvent>(
        [this](const core::events::KeyboardModeChangedEvent& /*evt*/) { Refresh(); });

    // Phase 11 Task 12: Saving spinner subscriptions
    tab_save_req_sub_ = event_bus_.subscribe<core::events::TabSaveRequestEvent>(
        [this](const core::events::TabSaveRequestEvent& evt)
        {
            int t_idx = FindTabIndex(evt.file_path);
            if (t_idx >= 0)
            {
                tabs_[static_cast<size_t>(t_idx)].is_saving = true;
                UpdatePulseTimer();
                Refresh();
            }
        });

    file_saved_sub_ = event_bus_.subscribe<core::events::FileSavedEvent>(
        [this](const core::events::FileSavedEvent& evt)
        {
            int t_idx = FindTabIndex(evt.file_path);
            if (t_idx >= 0)
            {
                tabs_[static_cast<size_t>(t_idx)].is_saving = false;
                tabs_[static_cast<size_t>(t_idx)].is_modified = false;
                UpdatePulseTimer();
                Refresh();
            }
        });
}

// --- Tab management ---

void TabBar::SetTabSizeMode(TabSizeMode mode)
{
    if (size_mode_ != mode)
    {
        size_mode_ = mode;
        RecalculateTabRects();
        Refresh();
    }
}

void TabBar::AddTab(const std::string& file_path, const std::string& display_name, bool is_preview)
{
    // If already exists, just activate
    int idx = FindTabIndex(file_path);
    if (idx >= 0)
    {
        if (!is_preview)
        {
            tabs_[static_cast<size_t>(idx)].is_preview = false;
        }
        SetActiveTab(file_path);
        return;
    }

    // Phase 11 Task 3: Replace existing preview tab if we are opening a new preview
    if (is_preview)
    {
        for (size_t i = 0; i < tabs_.size(); ++i)
        {
            if (tabs_[i].is_preview && !tabs_[i].is_modified && !tabs_[i].is_pinned)
            {
                tabs_[i].file_path = file_path;
                tabs_[i].display_name = display_name;
                SetActiveTab(file_path);
                RecalculateTabRects();
                Refresh();

                // Animate fade-in for replaced preview tab
                animation::AnimationConfig config;
                config.duration = std::chrono::milliseconds(160);
                config.easing_type = animation::EasingType::Linear;
                std::string anim_name = "tab_fade_" + file_path;
                transition_manager_.register_transition(anim_name, config);
                transition_manager_.start<float>(anim_name,
                                                 0.0F,
                                                 1.0F,
                                                 [this, file_path](float op)
                                                 {
                                                     int t_idx = FindTabIndex(file_path);
                                                     if (t_idx >= 0)
                                                     {
                                                         tabs_[static_cast<size_t>(t_idx)].opacity =
                                                             op;
                                                         Refresh();
                                                     }
                                                 });

                return;
            }
        }
    }

    TabInfo tab;
    tab.file_path = file_path;
    tab.display_name = display_name;
    tab.is_modified = false;
    tab.is_active = false;
    tab.is_preview = is_preview;
    tab.opacity = 0.0F; // R18 Fix 1: Start transparent for fade-in
    tabs_.push_back(tab);

    UpdateTabGroups();  // Update group assignments
    UpdatePulseTimer(); // Phase 11 Task 11

    SetActiveTab(file_path);
    RecalculateTabRects();
    EnsureTabVisible(static_cast<int>(tabs_.size()) - 1);

    animation::AnimationConfig config;
    config.duration = std::chrono::milliseconds(160);
    config.easing_type = animation::EasingType::Linear;

    std::string anim_name = "tab_fade_" + file_path;
    transition_manager_.register_transition(anim_name, config);
    transition_manager_.start<float>(anim_name,
                                     0.0F,
                                     1.0F,
                                     [this, file_path](float op)
                                     {
                                         int t_idx = FindTabIndex(file_path);
                                         if (t_idx >= 0)
                                         {
                                             tabs_[static_cast<size_t>(t_idx)].opacity = op;
                                             Refresh();
                                         }
                                     });

    Refresh();
}

void TabBar::RemoveTab(const std::string& file_path)
{
    int idx = FindTabIndex(file_path);
    if (idx < 0)
        return;

    auto& tab_to_close = tabs_[static_cast<size_t>(idx)];
    if (tab_to_close.is_closing)
        return;

    bool was_active = tab_to_close.is_active;
    tab_to_close.is_closing = true;
    tab_to_close.is_active = false;

    // If the closed tab was active, activate an adjacent tab
    if (was_active)
    {
        int new_active = -1;
        // Search left
        for (int i = idx - 1; i >= 0; --i)
        {
            if (!tabs_[static_cast<size_t>(i)].is_closing)
            {
                new_active = i;
                break;
            }
        }
        // Search right
        if (new_active < 0)
        {
            for (size_t i = static_cast<size_t>(idx + 1); i < tabs_.size(); ++i)
            {
                if (!tabs_[i].is_closing)
                {
                    new_active = static_cast<int>(i);
                    break;
                }
            }
        }

        if (new_active >= 0)
        {
            tabs_[static_cast<size_t>(new_active)].is_active = true;
            core::events::TabSwitchedEvent evt(tabs_[static_cast<size_t>(new_active)].file_path);
            event_bus_.publish(evt);
        }
    }

    // Phase 11 Task 17: Tab close animation fade-out
    animation::AnimationConfig config;
    config.duration = std::chrono::milliseconds(180);
    config.easing_type = animation::EasingType::EaseOutCubic;
    std::string aname = "tab_close_" + file_path;
    transition_manager_.register_transition(aname, config);

    transition_manager_.start<float>(
        aname,
        tab_to_close.opacity,
        0.0F,
        [this, file_path](float op)
        {
            int t_idx = -1;
            for (size_t i = 0; i < tabs_.size(); ++i)
            {
                if (tabs_[i].file_path == file_path && tabs_[i].is_closing)
                {
                    t_idx = static_cast<int>(i);
                    break;
                }
            }

            if (t_idx >= 0)
            {
                tabs_[static_cast<size_t>(t_idx)].opacity = op;
                if (op <= 0.01F)
                {
                    // Animation finished - totally erase
                    tabs_.erase(tabs_.begin() + static_cast<std::ptrdiff_t>(t_idx));
                    UpdateTabGroups();
                    UpdatePulseTimer();
                    RecalculateTabRects();
                    hovered_tab_index_ = -1;

                    if (focused_tab_index_ >= static_cast<int>(tabs_.size()))
                    {
                        focused_tab_index_ = static_cast<int>(tabs_.size()) - 1;
                        if (focused_tab_index_ >= 0)
                        {
                            FocusManager::get().set_item(focused_tab_index_);
                        }
                    }

                    // Clamp scroll offset
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
                }
                Refresh();
            }
        });

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
        UpdatePulseTimer();
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
        UpdateTabGroups(); // Update group assignments
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

// Phase 11 Task 11: Update modified dot pulse timer
void TabBar::UpdatePulseTimer()
{
    bool any_animating = false;
    for (const auto& tab : tabs_)
    {
        if (tab.is_modified || tab.is_saving)
        {
            any_animating = true;
            break;
        }
    }

    if (any_animating && !pulse_timer_.IsRunning())
    {
        pulse_timer_.Start(32); // ~30fps
    }
    else if (!any_animating && pulse_timer_.IsRunning())
    {
        pulse_timer_.Stop();
    }
}

void TabBar::OnPulseTimer(wxTimerEvent& /*event*/)
{
    Refresh();
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
    gc->SetBrush(gc->CreateBrush(
        wxBrush(theme_engine()
                    .resolve_token("tab.inactive_bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)))));
    gc->SetPen(wxNullPen);
    gc->DrawRectangle(0, 0, sz.GetWidth(), sz.GetHeight());

    // Phase 12 Task 7: Group Focus Indicator
    if (is_group_focused_)
    {
        gc->SetPen(gc->CreatePen(
            wxPen(theme_engine()
                      .resolve_token("tab.group_focus_border")
                      .value_or(theme_engine().color(core::ThemeColorToken::AccentPrimary)),
                  2)));
        gc->StrokeLine(0, 0, sz.GetWidth(), 0);
    }

    // Bottom border — R16 Fix 40: subtle light border
    gc->SetPen(
        gc->CreatePen(wxPen(theme_engine()
                                .resolve_token("tab.border")
                                .value_or(theme_engine().color(core::ThemeColorToken::BorderLight)),
                            1)));
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

    for (size_t i = 0; i < tabs_.size(); ++i)
    {
        const auto& tab = tabs_[i];
        // Only draw if visible in the viewport
        if (tab.rect.GetRight() - scroll_offset_ > 0 &&
            tab.rect.GetLeft() - scroll_offset_ < sz.GetWidth())
        {
            DrawTab(*gc, tab, theme);

            // Phase 06 Task 6: Register bounds for global focus ring
            wxRect adjusted_rect = tab.rect;
            adjusted_rect.SetLeft(adjusted_rect.GetLeft() - scroll_offset_);
            FocusRingRenderer::get().register_item_bounds(
                FocusZoneId::kEditorArea, static_cast<int>(i), this, adjusted_rect);
        }
    }

    // R16 Fix 6: Tab overflow fade gradient at right edge
    if (!tabs_.empty())
    {
        const int last_tab_right = tabs_.back().rect.GetRight() - scroll_offset_;
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
            const int kHeight = ds_.metrics.tab_height();
            gc->DrawText("›",
                         sz.GetWidth() - static_cast<int>(chev_w) - 4,
                         (kHeight - static_cast<int>(chev_h)) / 2);
        }
    }

    // Phase 11 Task 15: Background action area '+' button if there is space
    int last_tab_right_safe = tabs_.empty() ? 0 : tabs_.back().rect.GetRight() - scroll_offset_;
    if (last_tab_right_safe + 32 <= sz.GetWidth())
    {
        int add_x = last_tab_right_safe + 4;
        const int kHeight = ds_.metrics.tab_height();
        int add_y = (kHeight - 24) / 2;
        wxRect r(add_x, add_y, 24, 24);

        if (new_file_hovered_)
        {
            gc->SetBrush(gc->CreateBrush(wxBrush(wxColour(128, 128, 128, 40))));
            gc->SetPen(wxNullPen);
            gc->DrawRoundedRectangle(r.x, r.y, r.width, r.height, 4);
        }

        gc->SetPen(gc->CreatePen(wxPen(theme_engine().color(core::ThemeColorToken::TextMuted), 1)));
        gc->StrokeLine(add_x + 12, add_y + 6, add_x + 12, add_y + 18);
        gc->StrokeLine(add_x + 6, add_y + 12, add_x + 18, add_y + 12);
    }

    // Phase 12 Task 8: Group Action Buttons (Right-aligned)
    const int kHeight = ds_.metrics.tab_height();
    int actions_x = sz.GetWidth() - 8;

    // 1. "More Actions" (...) Button
    actions_x -= 24;
    wxRect more_actions_rect(actions_x, (kHeight - 24) / 2, 24, 24);
    if (more_actions_hovered_)
    {
        gc->SetBrush(gc->CreateBrush(wxBrush(wxColour(128, 128, 128, 40))));
        gc->SetPen(wxNullPen);
        gc->DrawRoundedRectangle(more_actions_rect.x, more_actions_rect.y, 24, 24, 4);
    }

    gc->SetBrush(gc->CreateBrush(wxBrush(theme_engine().color(core::ThemeColorToken::TextMuted))));
    gc->SetPen(wxNullPen);
    int dot_x = more_actions_rect.x + 12;
    int dot_y = more_actions_rect.y + 12;
    gc->DrawEllipse(dot_x - 5, dot_y - 1, 2, 2);
    gc->DrawEllipse(dot_x - 1, dot_y - 1, 2, 2);
    gc->DrawEllipse(dot_x + 3, dot_y - 1, 2, 2);

    // 2. "Split Right" Button
    actions_x -= 24;
    wxRect split_right_rect(actions_x, (kHeight - 24) / 2, 24, 24);
    if (split_right_hovered_)
    {
        gc->SetBrush(gc->CreateBrush(wxBrush(wxColour(128, 128, 128, 40))));
        gc->SetPen(wxNullPen);
        gc->DrawRoundedRectangle(split_right_rect.x, split_right_rect.y, 24, 24, 4);
    }

    gc->SetPen(gc->CreatePen(wxPen(theme_engine().color(core::ThemeColorToken::TextMuted), 1)));
    gc->StrokeLine(split_right_rect.x + 6,
                   split_right_rect.y + 6,
                   split_right_rect.x + 18,
                   split_right_rect.y + 6);
    gc->StrokeLine(split_right_rect.x + 6,
                   split_right_rect.y + 18,
                   split_right_rect.x + 18,
                   split_right_rect.y + 18);
    gc->StrokeLine(split_right_rect.x + 6,
                   split_right_rect.y + 6,
                   split_right_rect.x + 6,
                   split_right_rect.y + 18);
    gc->StrokeLine(split_right_rect.x + 18,
                   split_right_rect.y + 6,
                   split_right_rect.x + 18,
                   split_right_rect.y + 18);
    gc->StrokeLine(split_right_rect.x + 12,
                   split_right_rect.y + 6,
                   split_right_rect.x + 12,
                   split_right_rect.y + 18);

    actions_x -= 8; // Padding before the text

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
                     actions_x - static_cast<int>(count_w),
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

    // Draw the global animated focus ring over the top
    FocusRingRenderer::get().draw(dc, this, theme_engine());
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
        bg_color = theme_engine()
                       .resolve_token("tab.active_bg")
                       .value_or(theme_engine().color(core::ThemeColorToken::BgApp));
    }
    else if (tab.close_hovered ||
             (hovered_tab_index_ >= 0 &&
              tabs_[static_cast<size_t>(hovered_tab_index_)].file_path == tab.file_path))
    {
        bg_color =
            theme_engine()
                .resolve_token("tab.hover_bg")
                .value_or(
                    theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(115));
    }
    else
    {
        bg_color = theme_engine()
                       .resolve_token("tab.inactive_bg")
                       .value_or(theme_engine().color(core::ThemeColorToken::BgPanel));
    }

    // R20 Fix 5: Tint tab background by directory group color
    if (!tab.is_active && tab.group_color.IsOk() && tab.group_color.Alpha() > 0)
    {
        auto tint = tab.group_color.ChangeLightness(160);
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

        // Phase 11 Task 7: Explicit Tab Groups 2px top border matching the group color
        if (tab.group_color.IsOk() && tab.group_color.Alpha() > 0)
        {
            gc.SetBrush(gc.CreateBrush(wxBrush(tab.group_color)));
            gc.DrawRectangle(tab_x, tab_y, tab_w, 2);
        }
    }

    // 17. Active indicator — Phase 06 Task 50: Prominent top/bottom borders
    if (tab.is_active)
    {
        auto top_border_color =
            theme_engine()
                .resolve_token("tab.active_border_top")
                .value_or(theme_engine().color(core::ThemeColorToken::AccentPrimary));

        // Phase 11 Task 14: Tab Group Header Bar
        if (tab.group_color.IsOk() && tab.group_color.Alpha() > 0)
        {
            top_border_color = tab.group_color;
        }

        gc.SetBrush(gc.CreateBrush(wxBrush(top_border_color)));
        gc.DrawRectangle(tab_x, tab_y, tab_w, 3); // Top border

        gc.SetBrush(gc.CreateBrush(
            wxBrush(theme_engine()
                        .resolve_token("tab.active_border_bottom")
                        .value_or(theme_engine().color(core::ThemeColorToken::AccentPrimary)))));
        gc.DrawRectangle(tab_x, tab_y + tab_h - 3, tab_w, 3); // Bottom border keeping accent

        // R20 Fix 3: Active tab top glow (neon-edge beneath indicator)
        auto accent = top_border_color;
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
            gc.StrokeLine(tab_x, tab_y + 2 + glow_row, tab_x + tab_w, tab_y + 2 + glow_row);
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

    // 21. Right separator
    if (!tab.is_active)
    {
        gc.SetPen(gc.CreatePen(wxPen(
            theme_engine()
                .resolve_token("tab.border")
                .value_or(
                    theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(120)),
            1)));
        gc.StrokeLine(tab_x + tab_w, tab_y + 8, tab_x + tab_w, tab_y + tab_h - 8);
    }

    // R16 Fix 4: Active tab uses semibold weight
    wxFont font = theme_engine().font(core::ThemeFontToken::MonoRegular);
    font.SetPointSize(10);
    if (tab.is_active)
    {
        font.SetWeight(wxFONTWEIGHT_SEMIBOLD);
    }
    // R19 Fix 5: Italic style for modified tabs and preview tabs
    if (tab.is_modified || tab.is_preview)
    {
        font.SetStyle(wxFONTSTYLE_ITALIC);
    }
    gc.SetFont(font,
               tab.is_active
                   ? theme_engine()
                         .resolve_token("tab.active_fg")
                         .value_or(theme_engine().color(core::ThemeColorToken::TabActiveFg))
                   : theme_engine()
                         .resolve_token("tab.inactive_fg")
                         .value_or(theme_engine().color(core::ThemeColorToken::TabInactiveFg)));

    // Build display text with modified indicator
    std::string display = tab.is_pinned ? "" : tab.display_name;

    // Calculate text area (leave room for close button)
    const int kTabPaddingH = ds_.metrics.control_padding_h();
    const int kCloseButtonSize = ds_.metrics.icon_size_small();
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

    // (Redundant indicator before filename removed; now rendered over the X button instead)

    // --- Draw File Type Icon ---
    if (tab_w >= 36)
    {
        const int kIconSize = 14;
        int icon_y = tab_y + (tab_h - kIconSize) / 2;
        static const FileTypeIconRegistry icon_registry;
        icon_registry.DrawFileIcon(gc, tab.file_path, text_x, icon_y, kIconSize, theme_engine());

        text_x += kIconSize + 6;
        text_max_w -= kIconSize + 6;
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

    // Close button (×) — Fix 12: show modified dot (●) or saving spinner instead of × when not
    // hovered
    int close_x = tab_x + tab_w - kCloseButtonSize - kCloseButtonMargin;
    int close_y = tab_y + (tab_h - kCloseButtonSize) / 2;

    const bool is_tab_hovered =
        (hovered_tab_index_ >= 0 &&
         tabs_[static_cast<size_t>(hovered_tab_index_)].file_path == tab.file_path);

    // Show close button area on hover or if tab is active
    if (tab.is_active || is_tab_hovered)
    {
        // Fix 12: If modified/saving and close NOT hovered, draw corresponding indicator instead of
        // ×
        if ((tab.is_modified || tab.is_saving) && !tab.close_hovered)
        {
            if (tab.is_saving)
            {
                double time_ms = static_cast<double>(wxGetLocalTimeMillis().GetValue());
                double angle = (std::fmod(time_ms, 1200.0) / 1200.0) * 2.0 * M_PI;
                int dot_cx = close_x + kCloseButtonSize / 2;
                int dot_cy = close_y + kCloseButtonSize / 2;

                gc.SetPen(gc.CreatePen(
                    wxPen(theme_engine().color(core::ThemeColorToken::AccentPrimary), 2)));
                gc.SetBrush(wxNullBrush);

                wxGraphicsPath path = gc.CreatePath();
                path.AddArc(
                    dot_cx, dot_cy, kModifiedDotSize / 2.0 + 1.0, angle, angle + M_PI * 1.3, true);
                gc.StrokePath(path);
            }
            else
            {
                double time_ms = static_cast<double>(wxGetLocalTimeMillis().GetValue());
                double pulse_alpha = 0.5 + 0.5 * std::abs(std::sin(time_ms / 400.0));
                wxColour dot_color = theme_engine().color(core::ThemeColorToken::AccentSecondary);
                dot_color = wxColour(dot_color.Red(),
                                     dot_color.Green(),
                                     dot_color.Blue(),
                                     static_cast<unsigned char>(255.0 * pulse_alpha));

                int dot_cx = close_x + kCloseButtonSize / 2;
                int dot_cy = close_y + kCloseButtonSize / 2;
                gc.SetBrush(gc.CreateBrush(wxBrush(dot_color)));
                gc.SetPen(wxNullPen);
                gc.DrawEllipse(dot_cx - kModifiedDotSize / 2,
                               dot_cy - kModifiedDotSize / 2,
                               kModifiedDotSize,
                               kModifiedDotSize);
            }
        }
        else
        {
            // 19. Close button hover background (soft red)
            if (tab.close_hovered)
            {
                auto error_color = theme_engine().color(core::ThemeColorToken::ErrorColor);
                wxColour hover_bg(error_color.Red(), error_color.Green(), error_color.Blue(), 40);
                gc.SetBrush(gc.CreateBrush(wxBrush(hover_bg)));
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
        const int delta_y = pos.y - drag_start_y_;

        // Phase 11 Task 10: Tear-off / Cross-group drag
        constexpr int kDropThreshold = 20;
        if (std::abs(delta_y) > kDropThreshold)
        {
            std::string path = tabs_[static_cast<size_t>(drag_tab_index_)].file_path;

            is_dragging_ = false;
            if (HasCapture())
            {
                ReleaseMouse();
            }

            wxTextDataObject drag_data("markamp_tab:" + path + ":" + std::to_string(GetId()));
            wxDropSource drag_source(this);
            drag_source.SetData(drag_data);

            wxDragResult result = drag_source.DoDragDrop(wxDrag_AllowMove);
            if (result == wxDragMove)
            {
                RemoveTab(path);
            }
            return;
        }

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

    // 18. Show absolute file path tooltip on tab hover
    if (new_hovered != hovered_tab_index_ || close_state_changed)
    {
        if (new_hovered >= 0)
        {
            const auto& hover_tab = tabs_[static_cast<size_t>(new_hovered)];
            std::string tip;

            // Provide specific tooltip if hovering the close button
            if (hover_tab.close_hovered)
            {
                tip = "Close " + hover_tab.display_name;
            }
            else
            {
                tip = hover_tab.display_name;

                if (hover_tab.is_preview)
                {
                    tip += " [Preview]";
                }
                if (hover_tab.is_pinned)
                {
                    tip += " [Pinned]";
                }
                if (hover_tab.is_saving)
                {
                    tip += " (Saving...)";
                }
                else if (hover_tab.is_modified)
                {
                    tip += " (Modified)";
                }

                if (!hover_tab.group_id.empty())
                {
                    tip += "\nGroup: " + hover_tab.group_id;
                }

                tip += "\nPath: " + hover_tab.file_path;
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

    // Phase 11 Task 15: Background action area (+) button hover state
    bool new_file_btn_hovered = HitTestNewFileButton(pos);
    if (new_file_btn_hovered != new_file_hovered_)
    {
        new_file_hovered_ = new_file_btn_hovered;
        if (new_file_hovered_)
        {
            SetToolTip("New File");
        }
        else if (new_hovered < 0)
        {
            UnsetToolTip();
        }
        Refresh();
    }
}

void TabBar::OnMouseDown(wxMouseEvent& event)
{
    auto pos = event.GetPosition();

    // Phase 11 Task 9: Check overflow chevron click
    if (HitTestOverflowChevron(pos))
    {
        ShowOverflowMenu();
        return;
    }

    // Phase 12 Task 8: Group Actions click
    if (HitTestSplitRightButton(pos))
    {
        core::events::EditorGroupSplitRequestEvent evt;
        evt.source_tabbar_id = GetId();
        evt.is_horizontal_split = true;
        event_bus_.publish(evt);
        return;
    }

    if (HitTestMoreActionsButton(pos))
    {
        core::events::EditorGroupMoreActionsEvent evt;
        evt.source_tabbar_id = GetId();
        wxPoint screen_pos = ClientToScreen(pos);
        evt.screen_x = screen_pos.x;
        evt.screen_y = screen_pos.y;
        event_bus_.publish(evt);
        return;
    }

    // Phase 11 Task 15: Background action area click
    if (HitTestNewFileButton(pos))
    {
        static int untitled_counter = 1;
        const std::string untitled_path = "Untitled-" + std::to_string(untitled_counter++) + ".md";
        AddTab(untitled_path, untitled_path);

        const core::events::TabSwitchedEvent evt(untitled_path);
        event_bus_.publish(evt);
        return;
    }

    // Phase 11 Task 15: Background action area click
    if (HitTestNewFileButton(pos))
    {
        static int untitled_counter = 1;
        const std::string untitled_path = "Untitled-" + std::to_string(untitled_counter++) + ".md";
        AddTab(untitled_path, untitled_path);

        const core::events::TabSwitchedEvent evt(untitled_path);
        event_bus_.publish(evt);
        return;
    }

    int tab_index = HitTestTab(pos);

    if (tab_index < 0)
    {
        return;
    }

    focused_tab_index_ = tab_index;
    FocusManager::get().set_focus(FocusZoneId::kEditorArea, tab_index);
    Refresh();

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
    const int tab_index = HitTestTab(event.GetPosition());
    if (tab_index >= 0)
    {
        // Phase 11 Task 3: Lock preview tab on double click
        if (tabs_[static_cast<size_t>(tab_index)].is_preview)
        {
            tabs_[static_cast<size_t>(tab_index)].is_preview = false;
            Refresh();
        }
    }
    else
    {
        // Fix 10: Double-click on empty area (no tab hit) creates a new untitled file
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

    // Phase 11 Task 16: Sort Tabs
    menu.AppendSeparator();
    menu.Append(kContextSortByName, "Sort Tabs by Name");
    menu.Append(kContextSortByPath, "Sort Tabs by Path");

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
                      // Phase 11 Task 16: Sort Tabs
                      case kContextSortByName:
                      {
                          std::stable_sort(tabs_.begin(),
                                           tabs_.end(),
                                           [](const TabInfo& a, const TabInfo& b)
                                           {
                                               if (a.is_pinned != b.is_pinned)
                                                   return a.is_pinned;
                                               return a.display_name < b.display_name;
                                           });
                          for (size_t i = 0; i < tabs_.size(); ++i)
                          {
                              if (tabs_[i].is_active)
                              {
                                  EnsureTabVisible(static_cast<int>(i));
                                  break;
                              }
                          }
                          UpdateTabGroups();
                          RecalculateTabRects();
                          Refresh();
                          break;
                      }
                      case kContextSortByPath:
                      {
                          std::stable_sort(tabs_.begin(),
                                           tabs_.end(),
                                           [](const TabInfo& a, const TabInfo& b)
                                           {
                                               if (a.is_pinned != b.is_pinned)
                                                   return a.is_pinned;
                                               return a.file_path < b.file_path;
                                           });
                          for (size_t i = 0; i < tabs_.size(); ++i)
                          {
                              if (tabs_[i].is_active)
                              {
                                  EnsureTabVisible(static_cast<int>(i));
                                  break;
                              }
                          }
                          UpdateTabGroups();
                          RecalculateTabRects();
                          Refresh();
                          break;
                      }
                  }
              });

    PopupMenu(&menu);
}

// Phase 11 Task 9: Show Overflow Menu
void TabBar::ShowOverflowMenu()
{
    wxMenu menu;
    const auto sz = GetClientSize();
    int menu_id = 10000;
    std::unordered_map<int, std::string> id_to_path;

    for (const auto& tab : tabs_)
    {
        if (tab.rect.GetRight() - scroll_offset_ > sz.GetWidth() ||
            tab.rect.GetLeft() - scroll_offset_ < 0)
        {
            menu.Append(menu_id, tab.display_name);
            id_to_path[menu_id] = tab.file_path;
            menu_id++;
        }
    }

    if (id_to_path.empty())
    {
        return;
    }

    menu.Bind(wxEVT_MENU,
              [this, id_to_path](wxCommandEvent& cmd_event)
              {
                  auto it = id_to_path.find(cmd_event.GetId());
                  if (it != id_to_path.end())
                  {
                      SetActiveTab(it->second);
                  }
              });

    wxPoint popup_pos(sz.GetWidth() - 24, sz.GetHeight());
    PopupMenu(&menu, popup_pos);
}

// --- Hit testing ---

auto TabBar::HitTestTab(const wxPoint& point) const -> int
{
    for (size_t idx = 0; idx < tabs_.size(); ++idx)
    {
        if (tabs_[idx].is_closing)
            continue;

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

    const int kCloseButtonSize = ds_.metrics.icon_size_small();
    const auto& tab = tabs_[static_cast<size_t>(tab_index)];
    int close_x = tab.rect.GetLeft() - scroll_offset_ + tab.rect.GetWidth() - kCloseButtonSize -
                  kCloseButtonMargin;
    int close_y = tab.rect.GetTop() + (tab.rect.GetHeight() - kCloseButtonSize) / 2;

    wxRect close_rect(close_x - 2, close_y - 2, kCloseButtonSize + 4, kCloseButtonSize + 4);
    return close_rect.Contains(point);
}

// Phase 11 Task 9: Hit test for overflow chevron
auto TabBar::HitTestOverflowChevron(const wxPoint& point) const -> bool
{
    if (tabs_.empty())
    {
        return false;
    }

    int last_tab_right = tabs_.back().rect.GetRight() - scroll_offset_;
    const auto sz = GetClientSize();
    if (last_tab_right > sz.GetWidth())
    {
        constexpr int kFadeWidth = 24;
        wxRect chevron_rect(sz.GetWidth() - kFadeWidth, 0, kFadeWidth, sz.GetHeight());
        return chevron_rect.Contains(point);
    }
    return false;
}

// Phase 11 Task 15: Background Add Button
auto TabBar::HitTestNewFileButton(const wxPoint& point) const -> bool
{
    const auto sz = GetClientSize();
    int last_tab_right = tabs_.empty() ? 0 : tabs_.back().rect.GetRight() - scroll_offset_;
    if (last_tab_right + 32 <= sz.GetWidth())
    {
        const int kHeight = ds_.metrics.tab_height();
        wxRect r(last_tab_right + 4, (kHeight - 24) / 2, 24, 24);
        return r.Contains(point);
    }
    return false;
}

// Phase 12 Task 8: Split Right HitTest
auto TabBar::HitTestSplitRightButton(const wxPoint& point) const -> bool
{
    const auto sz = GetClientSize();
    int actions_x = sz.GetWidth() - 8 - 24 - 24;
    const int kHeight = ds_.metrics.tab_height();
    wxRect r(actions_x, (kHeight - 24) / 2, 24, 24);
    return r.Contains(point);
}

// Phase 12 Task 8: More Actions HitTest
auto TabBar::HitTestMoreActionsButton(const wxPoint& point) const -> bool
{
    const auto sz = GetClientSize();
    int actions_x = sz.GetWidth() - 8 - 24;
    const int kHeight = ds_.metrics.tab_height();
    wxRect r(actions_x, (kHeight - 24) / 2, 24, 24);
    return r.Contains(point);
}

// --- Layout ---

void TabBar::RecalculateTabRects()
{
    wxClientDC dc(this);
    wxFont font = theme_engine().font(core::ThemeFontToken::MonoRegular);
    font.SetPointSize(10);
    dc.SetFont(font);

    const int kTabPaddingH = ds_.metrics.control_padding_h();
    const int kCloseButtonSize = ds_.metrics.icon_size_small();
    const int kHeight = ds_.metrics.tab_height();

    int available_width = GetClientSize().GetWidth();
    int shrink_width = kMaxTabWidth;
    if (size_mode_ == TabSizeMode::kShrink && !tabs_.empty())
    {
        shrink_width = std::max(kMinTabWidth, available_width / static_cast<int>(tabs_.size()));
    }

    int x_offset = 0;
    for (auto& tab : tabs_)
    {
        int tab_width = 0;

        if (tab.is_closing)
        {
            tab_width = 0;
        }
        else if (tab.is_pinned)
        {
            tab_width = 40; // Phase 11 Task 20: Compact Pinned Tabs
        }
        else if (size_mode_ == TabSizeMode::kFixedWidth)
        {
            tab_width = kFixedWidth;
        }
        else
        {
            auto text_extent = dc.GetTextExtent(tab.display_name);
            tab_width = text_extent.GetWidth() + kTabPaddingH * 2 + kCloseButtonSize +
                        kCloseButtonMargin + 4;

            // Add space for modified dot
            tab_width += kModifiedDotSize + 4;

            if (!tab.is_pinned && size_mode_ == TabSizeMode::kShrink)
            {
                tab_width = std::max(kMinTabWidth, std::min(tab_width, shrink_width));
            }
        }

        // Clamp to min/max globally just to be safe, except when closing
        if (!tab.is_closing)
        {
            tab_width = std::clamp(tab_width, kMinTabWidth, kMaxTabWidth);
        }

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

void TabBar::UpdateLayoutMetrics()
{
    const int kHeight = ds_.metrics.tab_height();
    SetMinSize(wxSize(-1, kHeight));
    SetMaxSize(wxSize(-1, kHeight));
    if (GetParent() != nullptr)
    {
        GetParent()->Layout();
    }
    Refresh();
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
        if (tabs_[idx].file_path == file_path && !tabs_[idx].is_closing)
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

void markamp::ui::TabBar::OnSetFocus(wxFocusEvent& event)
{
    if (focused_tab_index_ < 0 && !tabs_.empty())
    {
        focused_tab_index_ = FindTabIndex(GetActiveTabPath());
        if (focused_tab_index_ < 0)
        {
            focused_tab_index_ = 0;
        }
    }
    FocusManager::get().set_focus(FocusZoneId::kEditorArea, focused_tab_index_);
    Refresh();

    // Announce initially focused tab
    if (focused_tab_index_ >= 0 && focused_tab_index_ < static_cast<int>(tabs_.size()))
    {
        const auto& tab = tabs_[static_cast<size_t>(focused_tab_index_)];
        accessibility::AccessibilityController::get().announce_focus(
            tab.display_name, "Tab", tab.is_active ? "Selected" : "");
    }

    event.Skip();
}

void markamp::ui::TabBar::OnKillFocus(wxFocusEvent& event)
{
    if (FocusManager::get().current_zone() == FocusZoneId::kEditorArea)
    {
        FocusManager::get().set_item(-1);
    }
    Refresh();
    event.Skip();
}

void markamp::ui::TabBar::OnKeyDown(wxKeyEvent& event)
{
    if (tabs_.empty())
    {
        event.Skip();
        return;
    }

    const int key_code = event.GetKeyCode();
    bool focus_moved = false;

    if (key_code == WXK_LEFT)
    {
        if (focused_tab_index_ > 0)
        {
            focused_tab_index_--;
            focus_moved = true;
        }
    }
    else if (key_code == WXK_RIGHT)
    {
        if (focused_tab_index_ < static_cast<int>(tabs_.size()) - 1)
        {
            focused_tab_index_++;
            focus_moved = true;
        }
    }
    else if (key_code == WXK_SPACE || key_code == WXK_RETURN)
    {
        if (focused_tab_index_ >= 0 && focused_tab_index_ < static_cast<int>(tabs_.size()))
        {
            std::string selected_path = tabs_[static_cast<size_t>(focused_tab_index_)].file_path;
            SetActiveTab(selected_path);
            const core::events::TabSwitchedEvent evt(selected_path);
            event_bus_.publish(evt);

            // Announce selection
            const auto& tab = tabs_[static_cast<size_t>(focused_tab_index_)];
            accessibility::AccessibilityController::get().announce_focus(
                tab.display_name, "Tab", "Selected");
        }
    }
    else if (key_code == WXK_DELETE || key_code == WXK_BACK)
    {
        if (focused_tab_index_ >= 0 && focused_tab_index_ < static_cast<int>(tabs_.size()))
        {
            std::string selected_path = tabs_[static_cast<size_t>(focused_tab_index_)].file_path;

            // Phase 11 Task 19: Close focused tab via keyboard
            core::events::TabCloseRequestEvent evt(selected_path);
            event_bus_.publish(evt);

            accessibility::AccessibilityController::get().announce_focus(
                "Closing " + tabs_[static_cast<size_t>(focused_tab_index_)].display_name,
                "Tab",
                "");
        }
    }
    else
    {
        event.Skip();
    }

    if (focus_moved)
    {
        EnsureTabVisible(focused_tab_index_);
        FocusManager::get().set_item(focused_tab_index_);
        Refresh();

        const auto& tab = tabs_[static_cast<size_t>(focused_tab_index_)];
        accessibility::AccessibilityController::get().announce_focus(
            tab.display_name, "Tab", tab.is_active ? "Selected" : "");
    }
}

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

// Phase 11 Task 7: Assign groups based on directory and assign a static color
void markamp::ui::TabBar::UpdateTabGroups()
{
    std::unordered_map<std::string, int> dir_counts;
    for (const auto& tab : tabs_)
    {
        try
        {
            auto parent = std::filesystem::path(tab.file_path).parent_path().string();
            if (!parent.empty())
            {
                dir_counts[parent]++;
            }
        }
        catch (...)
        {
        }
    }

    const auto& teng = theme_engine();
    const std::array<wxColour, kGroupColorCount> kGroupColors = {{
        teng.color(core::ThemeColorToken::AccentPrimary),
        teng.color(core::ThemeColorToken::SuccessColor),
        teng.color(core::ThemeColorToken::SyntaxKeyword),
        teng.color(core::ThemeColorToken::AccentSecondary),
        teng.color(core::ThemeColorToken::SyntaxType),
        teng.color(core::ThemeColorToken::SyntaxString),
    }};

    for (auto& tab : tabs_)
    {
        try
        {
            auto parent = std::filesystem::path(tab.file_path).parent_path().string();
            if (!parent.empty() && dir_counts[parent] > 1)
            {
                tab.group_id = parent;
                const std::size_t hash_val = std::hash<std::string>{}(parent);
                tab.group_color = kGroupColors.at(hash_val % kGroupColorCount);
            }
            else
            {
                tab.group_id.clear();
                tab.group_color = wxColour(0, 0, 0, 0); // Transparent
            }
        }
        catch (...)
        {
            tab.group_id.clear();
            tab.group_color = wxColour(0, 0, 0, 0);
        }
    }
}
