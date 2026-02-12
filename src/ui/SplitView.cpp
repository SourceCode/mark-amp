#include "SplitView.h"

#include "BevelPanel.h"
#include "EditorPanel.h"
#include "PreviewPanel.h"
#include "core/Config.h"
#include "core/Events.h"
#include "core/Logger.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <regex>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════
// Easing
// ═══════════════════════════════════════════════════════

auto SplitView::EaseOutCubic(double progress) -> double
{
    double inv = 1.0 - progress;
    return 1.0 - (inv * inv * inv);
}

// ═══════════════════════════════════════════════════════
// Constructor
// ═══════════════════════════════════════════════════════

SplitView::SplitView(wxWindow* parent,
                     core::ThemeEngine& theme_engine,
                     core::EventBus& event_bus,
                     core::Config* config)
    : ThemeAwareWindow(parent, theme_engine)
    , event_bus_(event_bus)
    , config_(config)
{
    // --- Create child panels ---
    editor_panel_ = new EditorPanel(this, theme_engine, event_bus);
    preview_panel_ = new PreviewPanel(this, theme_engine, event_bus);

    // --- Divider (custom painted) ---
    divider_panel_ = new wxPanel(this, wxID_ANY);
    divider_panel_->SetBackgroundStyle(wxBG_STYLE_PAINT);
    divider_panel_->SetCursor(wxCursor(wxCURSOR_SIZEWE));

    divider_panel_->Bind(wxEVT_PAINT, &SplitView::OnDividerPaint, this);
    divider_panel_->Bind(wxEVT_LEFT_DOWN, &SplitView::OnDividerMouseDown, this);
    divider_panel_->Bind(wxEVT_MOTION, &SplitView::OnDividerMouseMove, this);
    divider_panel_->Bind(wxEVT_LEFT_UP, &SplitView::OnDividerMouseUp, this);
    divider_panel_->Bind(wxEVT_ENTER_WINDOW, &SplitView::OnDividerMouseEnter, this);
    divider_panel_->Bind(wxEVT_LEAVE_WINDOW, &SplitView::OnDividerMouseLeave, this);
    divider_panel_->Bind(wxEVT_LEFT_DCLICK, &SplitView::OnDividerDoubleClick, this);

    // --- Editor bevel overlay ---
    // NOTE: BevelPanel overlays do not work on macOS — they paint opaque and
    // block the underlying panel even with wxTRANSPARENT_WINDOW.
    editor_bevel_ = new BevelPanel(this, wxID_ANY, BevelPanel::Style::Sunken);
    editor_bevel_->set_shadow_colour(wxColour(0, 0, 0, 51));
    editor_bevel_->set_highlight_colour(wxColour(255, 255, 255, 13));
    editor_bevel_->Enable(false);
    editor_bevel_->Hide();

    // --- Size event ---
    Bind(wxEVT_SIZE, &SplitView::OnSize, this);

    // --- Transition timer ---
    transition_timer_.Bind(wxEVT_TIMER, &SplitView::OnTransitionTimer, this);

    // --- Subscribe to external view mode changes ---
    view_mode_sub_ = event_bus_.subscribe<core::events::ViewModeChangedEvent>(
        [this](const core::events::ViewModeChangedEvent& evt) { SetViewMode(evt.mode); });

    // --- Subscribe to content changes for heading index ---
    content_sub_ = event_bus_.subscribe<core::events::EditorContentChangedEvent>(
        [this](const core::events::EditorContentChangedEvent& evt)
        { RebuildHeadingIndex(evt.content); });

    // --- Subscribe to focus mode toggle ---
    focus_mode_sub_ = event_bus_.subscribe<core::events::FocusModeChangedEvent>(
        [this](const core::events::FocusModeChangedEvent& evt)
        {
            if (evt.active != focus_mode_)
            {
                ToggleFocusMode();
            }
        });

    // --- Keyboard shortcuts: Cmd+1=Source, Cmd+2=Split, Cmd+3=Preview, Cmd+Shift+F=Focus ---
    enum
    {
        ID_MODE_EDITOR = wxID_HIGHEST + 100,
        ID_MODE_SPLIT,
        ID_MODE_PREVIEW,
        ID_FOCUS_MODE
    };

    wxAcceleratorEntry accel_entries[4];
    accel_entries[0].Set(wxACCEL_CMD, '1', ID_MODE_EDITOR);
    accel_entries[1].Set(wxACCEL_CMD, '2', ID_MODE_SPLIT);
    accel_entries[2].Set(wxACCEL_CMD, '3', ID_MODE_PREVIEW);
    accel_entries[3].Set(wxACCEL_CMD | wxACCEL_SHIFT, 'F', ID_FOCUS_MODE);
    wxAcceleratorTable accel_table(4, accel_entries);
    SetAcceleratorTable(accel_table);

    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& /*evt*/)
        { event_bus_.publish(core::events::ViewModeChangedEvent{core::events::ViewMode::Editor}); },
        ID_MODE_EDITOR);
    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& /*evt*/)
        { event_bus_.publish(core::events::ViewModeChangedEvent{core::events::ViewMode::Split}); },
        ID_MODE_SPLIT);
    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& /*evt*/) {
            event_bus_.publish(core::events::ViewModeChangedEvent{core::events::ViewMode::Preview});
        },
        ID_MODE_PREVIEW);
    Bind(
        wxEVT_MENU, [this](wxCommandEvent& /*evt*/) { ToggleFocusMode(); }, ID_FOCUS_MODE);

    // --- Restore persisted split ratio ---
    RestoreSplitRatio();

    // --- Initial layout ---
    UpdateLayout();
}

// ═══════════════════════════════════════════════════════
// View Mode (with animated transitions)
// ═══════════════════════════════════════════════════════

void SplitView::SetViewMode(core::events::ViewMode mode)
{
    if (mode == current_mode_)
    {
        return;
    }

    // Exit focus mode if entering a different mode
    if (focus_mode_)
    {
        focus_mode_ = false;
        event_bus_.publish(core::events::FocusModeChangedEvent{false});
    }

    // Start animated transition
    StartTransition(mode);

    spdlog::debug("SplitView: transitioning to {}",
                  mode == core::events::ViewMode::Editor    ? "Editor"
                  : mode == core::events::ViewMode::Preview ? "Preview"
                                                            : "Split");
}

void SplitView::StartTransition(core::events::ViewMode target_mode)
{
    // Save current state
    auto editor_state = SaveEditorState();

    transition_target_mode_ = target_mode;
    transition_progress_ = 0.0;

    // Determine start/target ratios for animation
    switch (current_mode_)
    {
        case core::events::ViewMode::Editor:
            transition_start_ratio_ = 1.0; // editor takes all
            break;
        case core::events::ViewMode::Preview:
            transition_start_ratio_ = 0.0; // preview takes all
            break;
        case core::events::ViewMode::Split:
            transition_start_ratio_ = split_ratio_;
            break;
    }

    switch (target_mode)
    {
        case core::events::ViewMode::Editor:
            transition_target_ratio_ = 1.0;
            break;
        case core::events::ViewMode::Preview:
            transition_target_ratio_ = 0.0;
            break;
        case core::events::ViewMode::Split:
            transition_target_ratio_ = split_ratio_;
            break;
    }

    // Show both panels during transition
    editor_panel_->Show(true);
    preview_panel_->Show(true);
    divider_panel_->Show(target_mode == core::events::ViewMode::Split);
    editor_bevel_->Hide();

    transition_show_editor_ = (target_mode == core::events::ViewMode::Editor ||
                               target_mode == core::events::ViewMode::Split);
    transition_show_preview_ = (target_mode == core::events::ViewMode::Preview ||
                                target_mode == core::events::ViewMode::Split);

    current_mode_ = target_mode;

    // Start animation timer
    transition_timer_.Start(kAnimFrameMs);

    // Restore state
    RestoreEditorState(editor_state);
}

void SplitView::OnTransitionTimer(wxTimerEvent& /*event*/)
{
    transition_progress_ += static_cast<double>(kAnimFrameMs) / kTransitionDurationMs;

    if (transition_progress_ >= 1.0)
    {
        transition_progress_ = 1.0;
        transition_timer_.Stop();

        // Final visibility
        editor_panel_->Show(transition_show_editor_);
        preview_panel_->Show(transition_show_preview_);

        // R2 Fix 20: Editor focus on view mode change
        if (transition_show_editor_ && editor_panel_ != nullptr)
        {
            editor_panel_->SetFocus();
        }
    }

    // Interpolate ratio with easing
    double eased = EaseOutCubic(transition_progress_);
    double current =
        transition_start_ratio_ + (transition_target_ratio_ - transition_start_ratio_) * eased;

    // Layout based on interpolated ratio
    wxSize client = GetClientSize();
    int width = client.GetWidth();
    int height = client.GetHeight();

    if (width <= 0 || height <= 0)
    {
        return;
    }

    int split_pos = static_cast<int>(static_cast<double>(width) * current);
    split_pos = std::clamp(split_pos, 0, width);

    int editor_width = split_pos;
    int divider_x = split_pos;
    int preview_x = split_pos + (divider_panel_->IsShown() ? kDividerWidth : 0);
    int preview_width = width - preview_x;

    if (editor_width > 0)
    {
        editor_panel_->SetSize(0, 0, editor_width, height);
    }
    if (divider_panel_->IsShown())
    {
        divider_panel_->SetSize(divider_x, 0, kDividerWidth, height);
    }
    if (preview_width > 0)
    {
        preview_panel_->SetSize(preview_x, 0, preview_width, height);
    }
}

auto SplitView::GetViewMode() const -> core::events::ViewMode
{
    return current_mode_;
}

// ═══════════════════════════════════════════════════════
// Child Access
// ═══════════════════════════════════════════════════════

auto SplitView::GetEditorPanel() -> EditorPanel*
{
    return editor_panel_;
}

auto SplitView::GetPreviewPanel() -> PreviewPanel*
{
    return preview_panel_;
}

// ═══════════════════════════════════════════════════════
// Split Ratio
// ═══════════════════════════════════════════════════════

void SplitView::SetSplitRatio(double ratio)
{
    split_ratio_ = std::clamp(ratio, kMinSplitRatio, kMaxSplitRatio);
    UpdateLayout();
}

auto SplitView::GetSplitRatio() const -> double
{
    return split_ratio_;
}

// ═══════════════════════════════════════════════════════
// Snap Presets
// ═══════════════════════════════════════════════════════

void SplitView::CycleSnapPreset()
{
    switch (current_snap_)
    {
        case SnapPreset::Even:
            current_snap_ = SnapPreset::EditorWide;
            split_ratio_ = 0.7;
            break;
        case SnapPreset::EditorWide:
            current_snap_ = SnapPreset::PreviewWide;
            split_ratio_ = 0.3;
            break;
        case SnapPreset::PreviewWide:
            current_snap_ = SnapPreset::Even;
            split_ratio_ = 0.5;
            break;
    }

    UpdateLayout();
    SaveSplitRatio();

    spdlog::debug("SplitView: snap preset changed to ratio={:.1f}", split_ratio_);
}

auto SplitView::GetCurrentSnap() const -> SnapPreset
{
    return current_snap_;
}

// ═══════════════════════════════════════════════════════
// Focus Mode
// ═══════════════════════════════════════════════════════

void SplitView::ToggleFocusMode()
{
    focus_mode_ = !focus_mode_;

    if (focus_mode_)
    {
        // Enter focus mode: hide preview/divider, center editor
        preview_panel_->Hide();
        divider_panel_->Hide();
        editor_bevel_->Hide();
        editor_panel_->Show(true);
        UpdateFocusLayout();
    }
    else
    {
        // Exit focus mode: restore previous view mode
        SetViewMode(core::events::ViewMode::Split);
    }

    event_bus_.publish(core::events::FocusModeChangedEvent{focus_mode_});

    spdlog::debug("SplitView: focus mode {}", focus_mode_ ? "ON" : "OFF");
}

auto SplitView::IsFocusMode() const -> bool
{
    return focus_mode_;
}

void SplitView::UpdateFocusLayout()
{
    wxSize client = GetClientSize();
    int width = client.GetWidth();
    int height = client.GetHeight();

    if (width <= 0 || height <= 0)
    {
        return;
    }

    // Calculate 80ch width based on font metrics
    int char_width = 8; // Default fallback
    if (editor_panel_ != nullptr)
    {
        wxClientDC temp_dc(editor_panel_);
        wxFont mono_font(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        temp_dc.SetFont(mono_font);
        char_width = temp_dc.GetCharWidth();
    }

    int max_width = kFocusMaxChars * char_width + kFocusPaddingH * 2;
    int editor_width = std::min(width, max_width);
    int editor_x = (width - editor_width) / 2;

    editor_panel_->SetSize(editor_x, 0, editor_width, height);
}

// ═══════════════════════════════════════════════════════
// Scroll Sync
// ═══════════════════════════════════════════════════════

void SplitView::SetScrollSyncMode(core::events::ScrollSyncMode mode)
{
    scroll_sync_mode_ = mode;
}

auto SplitView::GetScrollSyncMode() const -> core::events::ScrollSyncMode
{
    return scroll_sync_mode_;
}

void SplitView::RebuildHeadingIndex(const std::string& content)
{
    heading_positions_.clear();

    // Find lines starting with # (markdown headings)
    std::istringstream stream(content);
    std::string line;
    int line_num = 0;
    while (std::getline(stream, line))
    {
        if (!line.empty() && line[0] == '#')
        {
            heading_positions_.push_back(line_num);
        }
        ++line_num;
    }
}

auto SplitView::FindNearestHeading(int editor_line) const -> int
{
    if (heading_positions_.empty())
    {
        return -1;
    }

    int nearest = heading_positions_[0];
    int min_dist = std::abs(editor_line - nearest);

    for (size_t idx = 1; idx < heading_positions_.size(); ++idx)
    {
        int dist = std::abs(editor_line - heading_positions_[idx]);
        if (dist < min_dist)
        {
            min_dist = dist;
            nearest = heading_positions_[idx];
        }
        // Headings are sorted; once distance starts increasing, stop
        if (heading_positions_[idx] > editor_line)
        {
            break;
        }
    }

    return nearest;
}

// ═══════════════════════════════════════════════════════
// File Operations
// ═══════════════════════════════════════════════════════

void SplitView::SaveFile(const std::string& path)
{
    if (path.empty() || !editor_panel_)
    {
        return;
    }

    // Item 16: Trim Trailing Whitespace
    bool trim = false;
    if (config_ != nullptr)
    {
        trim = config_->get_bool("editor.trim_trailing_whitespace", false);
    }

    if (trim)
    {
        editor_panel_->TrimTrailingWhitespace();
    }

    std::string content = editor_panel_->GetContent();

    std::ofstream out(path);
    if (out.is_open())
    {
        out << content;
        out.close();
        MARKAMP_LOG_INFO("Saved file: {}", path);
    }
    else
    {
        MARKAMP_LOG_ERROR("Failed to save file: {}", path);
        wxMessageBox("Failed to save file: " + path, "Error", wxICON_ERROR);
    }
}

// ═══════════════════════════════════════════════════════
// Divider Drawing
// ═══════════════════════════════════════════════════════

void SplitView::OnDividerPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(divider_panel_);
    wxSize size = divider_panel_->GetSize();

    const auto& current_theme = theme();

    // Background: subtle surface color
    wxColour bg_col(current_theme.colors.bg_panel.to_rgba_string());
    paint_dc.SetBackground(wxBrush(bg_col));
    paint_dc.Clear();

    // Center line: accent on hover, border_light normally
    wxColour line_col;
    if (divider_hovered_ || is_dragging_)
    {
        line_col = wxColour(current_theme.colors.accent_primary.to_rgba_string());
    }
    else
    {
        line_col = wxColour(current_theme.colors.border_light.to_rgba_string());
    }

    int center_x = size.GetWidth() / 2;
    paint_dc.SetPen(wxPen(line_col, 1));
    paint_dc.DrawLine(center_x, 0, center_x, size.GetHeight());

    // Grip dots (3 dots centered vertically)
    int center_y = size.GetHeight() / 2;
    int dot_size = 3;
    int dot_gap = 6;
    wxColour dot_col;
    if (divider_hovered_ || is_dragging_)
    {
        dot_col = wxColour(current_theme.colors.accent_primary.to_rgba_string());
    }
    else
    {
        dot_col = wxColour(current_theme.colors.text_muted.to_rgba_string());
    }

    paint_dc.SetBrush(wxBrush(dot_col));
    paint_dc.SetPen(*wxTRANSPARENT_PEN);

    for (int idx = -1; idx <= 1; ++idx)
    {
        int dot_y = center_y + idx * dot_gap;
        paint_dc.DrawCircle(center_x, dot_y, dot_size / 2);
    }
}

// ═══════════════════════════════════════════════════════
// Divider Drag
// ═══════════════════════════════════════════════════════

void SplitView::OnDividerMouseDown(wxMouseEvent& event)
{
    is_dragging_ = true;
    drag_start_x_ = event.GetX();
    drag_start_ratio_ = split_ratio_;
    divider_panel_->CaptureMouse();
    divider_panel_->Refresh();
    event.Skip();
}

void SplitView::OnDividerMouseMove(wxMouseEvent& event)
{
    if (!is_dragging_)
    {
        return;
    }

    int delta_x = event.GetX() - drag_start_x_;
    int total_width = GetClientSize().GetWidth();
    if (total_width <= 0)
    {
        return;
    }

    double delta_ratio = static_cast<double>(delta_x) / static_cast<double>(total_width);
    double new_ratio = std::clamp(drag_start_ratio_ + delta_ratio, kMinSplitRatio, kMaxSplitRatio);

    split_ratio_ = new_ratio;
    UpdateLayout();

    event.Skip();
}

void SplitView::OnDividerMouseUp(wxMouseEvent& event)
{
    if (is_dragging_)
    {
        is_dragging_ = false;
        if (divider_panel_->HasCapture())
        {
            divider_panel_->ReleaseMouse();
        }
        divider_panel_->Refresh();
        SaveSplitRatio();
    }
    event.Skip();
}

void SplitView::OnDividerMouseEnter(wxMouseEvent& /*event*/)
{
    divider_hovered_ = true;
    divider_panel_->SetCursor(wxCursor(wxCURSOR_SIZEWE));
    divider_panel_->Refresh();
}

void SplitView::OnDividerMouseLeave(wxMouseEvent& /*event*/)
{
    if (!is_dragging_)
    {
        divider_hovered_ = false;
        divider_panel_->SetCursor(wxNullCursor);
        divider_panel_->Refresh();
    }
}

void SplitView::OnDividerDoubleClick(wxMouseEvent& /*event*/)
{
    CycleSnapPreset();
}

// ═══════════════════════════════════════════════════════
// Layout
// ═══════════════════════════════════════════════════════

void SplitView::UpdateLayout()
{
    // Don't override animated layout
    if (transition_timer_.IsRunning())
    {
        return;
    }

    // Handle focus mode separately
    if (focus_mode_)
    {
        UpdateFocusLayout();
        return;
    }

    wxSize client = GetClientSize();
    int width = client.GetWidth();
    int height = client.GetHeight();

    if (width <= 0 || height <= 0)
    {
        return;
    }

    switch (current_mode_)
    {
        case core::events::ViewMode::Editor:
            editor_panel_->SetSize(0, 0, width, height);
            break;

        case core::events::ViewMode::Preview:
            preview_panel_->SetSize(0, 0, width, height);
            break;

        case core::events::ViewMode::Split:
        {
            int split_pos = static_cast<int>(static_cast<double>(width) * split_ratio_);
            split_pos = std::clamp(split_pos, kDividerWidth * 2, width - kDividerWidth * 2);

            int editor_width = split_pos;
            int divider_x = split_pos;
            int preview_x = split_pos + kDividerWidth;
            int preview_width = width - preview_x;

            editor_panel_->SetSize(0, 0, editor_width, height);
            divider_panel_->SetSize(divider_x, 0, kDividerWidth, height);
            preview_panel_->SetSize(preview_x, 0, preview_width, height);
            break;
        }
    }
}

void SplitView::OnSize(wxSizeEvent& event)
{
    UpdateLayout();
    event.Skip();
}

// ═══════════════════════════════════════════════════════
// State Preservation
// ═══════════════════════════════════════════════════════

auto SplitView::SaveEditorState() -> EditorState
{
    EditorState state;
    if (editor_panel_ != nullptr)
    {
        state.cursor_line = editor_panel_->GetCursorLine();
        state.cursor_col = editor_panel_->GetCursorColumn();
        state.content = editor_panel_->GetContent();
    }
    return state;
}

void SplitView::RestoreEditorState(const EditorState& state)
{
    if (editor_panel_ != nullptr && !state.content.empty())
    {
        editor_panel_->SetCursorPosition(state.cursor_line, state.cursor_col);
    }
}

// ═══════════════════════════════════════════════════════
// Theme
// ═══════════════════════════════════════════════════════

void SplitView::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);

    // Divider repaints itself using theme() in OnDividerPaint
    if (divider_panel_ != nullptr)
    {
        divider_panel_->Refresh();
    }
}

// ═══════════════════════════════════════════════════════
// Persistence
// ═══════════════════════════════════════════════════════

void SplitView::SaveSplitRatio()
{
    if (config_ != nullptr)
    {
        config_->set("split_ratio", split_ratio_);
    }
}

void SplitView::RestoreSplitRatio()
{
    if (config_ != nullptr)
    {
        double ratio = config_->get_double("split_ratio", kDefaultSplitRatio);
        split_ratio_ = std::clamp(ratio, kMinSplitRatio, kMaxSplitRatio);
    }
}

} // namespace markamp::ui
