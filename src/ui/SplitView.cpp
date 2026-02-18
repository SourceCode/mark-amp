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
                     core::Config* config,
                     core::IMermaidRenderer* mermaid_renderer,
                     core::IMathRenderer* math_renderer)
    : ThemeAwareWindow(parent, theme_engine)
    , event_bus_(event_bus)
    , config_(config)
{
    // --- Create child panels ---
    editor_panel_ = new EditorPanel(this, theme_engine, event_bus);
    preview_panel_ =
        new PreviewPanel(this, theme_engine, event_bus, mermaid_renderer, nullptr, math_renderer);

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

    // Phase 09 Task 18: Accessibility labels
    divider_panel_->SetName("Split view divider");
    divider_panel_->SetHelpText(
        "Drag to resize editor and preview panes. Double-click to cycle presets.");
    editor_panel_->SetName("Markdown editor");
    editor_panel_->SetHelpText("Edit markdown source text");
    preview_panel_->SetName("Rendered preview");
    preview_panel_->SetHelpText("Live-rendered HTML preview of editor content");

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

    // --- Subscribe to content changes for heading index + adaptive throttle ---
    content_sub_ = event_bus_.subscribe<core::events::EditorContentChangedEvent>(
        [this](const core::events::EditorContentChangedEvent& evt)
        {
            RebuildHeadingIndex(evt.content);
            // Phase 09 Task 5: Adaptive throttle
            UpdateRenderThrottle(evt.content.size());
            // Phase 09 Task 3: Track edit time
            last_edit_time_ = std::chrono::steady_clock::now();
            render_pending_ = true;
            // Update line count for cursor-anchored sync
            int lines = 1;
            for (const char character : evt.content)
            {
                if (character == '\n')
                {
                    ++lines;
                }
            }
            total_line_count_ = std::max(lines, 1);
        });

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

    // --- Phase 09 Task 1: Subscribe to cursor position for cursor-anchored sync ---
    cursor_sync_sub_ = event_bus_.subscribe<core::events::CursorPositionChangedEvent>(
        [this](const core::events::CursorPositionChangedEvent& evt)
        {
            OnCursorPositionChanged(evt.line, total_line_count_);
            // Phase 09 Task 4: Update breadcrumb on cursor move
            UpdateBreadcrumb(evt.line);
        });

    // --- Phase 09 Task 2: Subscribe to selection changes for mirroring ---
    selection_mirror_sub_ = event_bus_.subscribe<core::events::CursorPositionChangedEvent>(
        [this](const core::events::CursorPositionChangedEvent& evt)
        {
            if (evt.selection_length > 0 && editor_panel_ != nullptr)
            {
                // Selection active — publish highlight event
                // The actual selected text is available from editor
                auto* stc = editor_panel_->GetStyledTextCtrl();
                if (stc != nullptr)
                {
                    const std::string sel_text = stc->GetSelectedText().ToStdString();
                    OnSelectionChanged(sel_text);
                }
            }
            else if (evt.selection_length == 0 && !last_selection_text_.empty())
            {
                // Selection cleared
                OnSelectionChanged("");
            }
        });

    // --- Phase 09 Task 13: Subscribe to export HTML requests ---
    export_html_sub_ = event_bus_.subscribe<core::events::ExportHtmlRequestEvent>(
        [this](const core::events::ExportHtmlRequestEvent& /*evt*/) { ExportHtml(); });

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
        case core::events::ViewMode::LivePreview:
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
        case core::events::ViewMode::LivePreview:
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
        case SnapPreset::Balanced:
            current_snap_ = SnapPreset::EditorWide;
            split_ratio_ = 0.7;
            break;
        case SnapPreset::EditorWide:
            current_snap_ = SnapPreset::EditorFocus;
            split_ratio_ = 0.85;
            break;
        case SnapPreset::EditorFocus:
            current_snap_ = SnapPreset::PreviewWide;
            split_ratio_ = 0.3;
            break;
        case SnapPreset::PreviewWide:
            current_snap_ = SnapPreset::Review;
            split_ratio_ = 0.4;
            break;
        case SnapPreset::Review:
            current_snap_ = SnapPreset::PreviewFocus;
            split_ratio_ = 0.15;
            break;
        case SnapPreset::PreviewFocus:
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
    heading_texts_.clear();
    heading_levels_.clear();

    // Find lines starting with # (markdown headings)
    std::istringstream stream(content);
    std::string line;
    int line_num = 0;
    while (std::getline(stream, line))
    {
        if (!line.empty() && line[0] == '#')
        {
            heading_positions_.push_back(line_num);

            // Phase 09 Task 4: Extract heading level and text
            int level = 0;
            std::size_t pos = 0;
            while (pos < line.size() && line[pos] == '#')
            {
                ++level;
                ++pos;
            }
            // Skip whitespace after #
            while (pos < line.size() && line[pos] == ' ')
            {
                ++pos;
            }
            heading_levels_.push_back(std::min(level, 6));
            heading_texts_.push_back(line.substr(pos));
        }
        ++line_num;
    }

    // Phase 09 Task 3: Render completed after heading rebuild
    last_render_time_ = std::chrono::steady_clock::now();
    render_pending_ = false;
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

void SplitView::set_feature_registry(core::FeatureRegistry* registry)
{
    if (editor_panel_ != nullptr)
    {
        editor_panel_->set_feature_registry(registry);
    }
}

void SplitView::set_mermaid_enabled(bool enabled)
{
    if (preview_panel_ != nullptr)
    {
        preview_panel_->set_mermaid_enabled(enabled);
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

    // R20 Fix 32: Draw floating percentage label during drag
    if (is_dragging_)
    {
        int left_pct = static_cast<int>(std::round(split_ratio_ * 100.0));
        int right_pct = 100 - left_pct;
        wxString ratio_text = wxString::Format("%d / %d", left_pct, right_pct);

        wxFont label_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        paint_dc.SetFont(label_font);

        auto label_extent = paint_dc.GetTextExtent(ratio_text);
        const int label_w = label_extent.GetWidth() + 12;
        const int label_h = label_extent.GetHeight() + 6;
        const int label_x = center_x - label_w / 2;
        const int label_y = center_y - dot_gap * 2 - label_h - 4;

        // Pill background
        wxColour pill_bg(current_theme.colors.bg_header.to_rgba_string());
        paint_dc.SetBrush(wxBrush(pill_bg));
        paint_dc.SetPen(wxPen(line_col, 1));
        paint_dc.DrawRoundedRectangle(label_x, label_y, label_w, label_h, 3);

        // Text
        wxColour label_fg(current_theme.colors.accent_primary.to_rgba_string());
        paint_dc.SetTextForeground(label_fg);
        paint_dc.DrawText(ratio_text,
                          label_x + (label_w - label_extent.GetWidth()) / 2,
                          label_y + (label_h - label_extent.GetHeight()) / 2);
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

    // Phase 09 Task 6: Determine drag axis based on split direction
    const bool is_vertical = (split_direction_ == core::events::SplitDirection::Vertical);
    const int delta = is_vertical ? (event.GetY() - drag_start_x_) : (event.GetX() - drag_start_x_);
    const int total_extent = is_vertical ? GetClientSize().GetHeight() : GetClientSize().GetWidth();
    if (total_extent <= 0)
    {
        return;
    }

    const double delta_ratio = static_cast<double>(delta) / static_cast<double>(total_extent);
    double new_ratio = std::clamp(drag_start_ratio_ + delta_ratio, kMinSplitRatio, kMaxSplitRatio);

    // Phase 09 Task 7: Magnetic snap points
    const double snapped = FindNearestSnapPoint(new_ratio);
    if (std::abs(new_ratio - snapped) < kSnapThreshold)
    {
        new_ratio = snapped;
    }

    split_ratio_ = new_ratio;
    UpdateLayout();

    // R18 Fix 22: Show split ratio tooltip during drag
    const int left_pct = static_cast<int>(std::round(split_ratio_ * 100.0));
    const int right_pct = 100 - left_pct;
    divider_panel_->SetToolTip(wxString::Format("%d / %d", left_pct, right_pct));

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
    // R17 Fix 38: Double-click divider resets to 50/50 split
    SetSplitRatio(0.5);
    UpdateLayout();
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
        case core::events::ViewMode::LivePreview:
        {
            // Phase 09 Task 6: Support vertical split direction
            if (split_direction_ == core::events::SplitDirection::Vertical)
            {
                // Vertical: editor on top, preview on bottom
                const int split_pos =
                    std::clamp(static_cast<int>(static_cast<double>(height) * split_ratio_),
                               kDividerWidth * 2,
                               height - kDividerWidth * 2);
                const int editor_height = split_pos;
                const int preview_y = split_pos + kDividerWidth;
                const int preview_height = height - preview_y;

                editor_panel_->SetSize(0, 0, width, editor_height);
                divider_panel_->SetSize(0, split_pos, width, kDividerWidth);
                preview_panel_->SetSize(0, preview_y, width, preview_height);
            }
            else
            {
                // Horizontal: editor on left, preview on right
                const int split_pos =
                    std::clamp(static_cast<int>(static_cast<double>(width) * split_ratio_),
                               kDividerWidth * 2,
                               width - kDividerWidth * 2);
                const int editor_width = split_pos;
                const int preview_x = split_pos + kDividerWidth;
                const int preview_width = width - preview_x;

                editor_panel_->SetSize(0, 0, editor_width, height);
                divider_panel_->SetSize(split_pos, 0, kDividerWidth, height);
                preview_panel_->SetSize(preview_x, 0, preview_width, height);
            }
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

// ═══════════════════════════════════════════════════════
// Phase 09 Task 1: Cursor-Anchored Scroll Sync
// ═══════════════════════════════════════════════════════

void SplitView::OnCursorPositionChanged(int line, int total_lines)
{
    last_cursor_line_ = line;
    total_line_count_ = std::max(total_lines, 1);

    // Only sync in cursor-anchored mode while in split view
    if (scroll_sync_mode_ != core::events::ScrollSyncMode::CursorAnchored)
    {
        return;
    }
    if (current_mode_ != core::events::ViewMode::Split)
    {
        return;
    }
    if (preview_panel_ == nullptr)
    {
        return;
    }

    // Compute fractional position based on cursor line in total lines
    const double fraction =
        static_cast<double>(line) / static_cast<double>(std::max(total_lines - 1, 1));
    preview_panel_->SetScrollFraction(std::clamp(fraction, 0.0, 1.0));
}

// ═══════════════════════════════════════════════════════
// Phase 09 Task 2: Selection Mirroring
// ═══════════════════════════════════════════════════════

void SplitView::OnSelectionChanged(const std::string& selected_text)
{
    if (selected_text == last_selection_text_)
    {
        return; // No change
    }
    last_selection_text_ = selected_text;

    // Publish highlight event for preview panel (or any subscriber)
    core::events::SelectionHighlightEvent highlight_evt;
    if (selected_text.empty())
    {
        highlight_evt.clear = true;
    }
    else
    {
        highlight_evt.selected_text = selected_text;
        highlight_evt.clear = false;
    }
    event_bus_.publish(highlight_evt);
}

// ═══════════════════════════════════════════════════════
// Phase 09 Task 3: Sync Accuracy Indicator
// ═══════════════════════════════════════════════════════

auto SplitView::GetSyncHealth() const -> SyncHealth
{
    if (!render_pending_)
    {
        return SyncHealth::kSynced;
    }

    // If we have a pending render but render happened after edit, still synced
    if (last_render_time_ >= last_edit_time_)
    {
        return SyncHealth::kSynced;
    }

    // Check elapsed time since last edit
    const auto elapsed = std::chrono::steady_clock::now() - last_edit_time_;
    const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    // If within debounce window, rendering is in progress
    if (elapsed_ms < 500)
    {
        return SyncHealth::kRendering;
    }

    // More than 500ms since edit with no render completion => out of sync
    return SyncHealth::kOutOfSync;
}

// ═══════════════════════════════════════════════════════
// Phase 09 Task 4: Breadcrumbs
// ═══════════════════════════════════════════════════════

void SplitView::UpdateBreadcrumb(int cursor_line)
{
    if (heading_positions_.empty())
    {
        if (!current_breadcrumb_.empty())
        {
            current_breadcrumb_.clear();
            core::events::BreadcrumbsChangedEvent evt;
            evt.breadcrumb_path = "";
            evt.heading_line = 0;
            event_bus_.publish(evt);
        }
        return;
    }

    // Build breadcrumb hierarchy: find all headings at or before cursor line
    // that form a nested hierarchy (each deeper level than its predecessor)
    std::string path;
    int last_level = 0;
    int innermost_line = 0;

    for (std::size_t idx = 0; idx < heading_positions_.size(); ++idx)
    {
        if (heading_positions_[idx] > cursor_line)
        {
            break; // Past cursor position
        }

        const int level = heading_levels_[idx];
        // Only add if this heading is deeper than current context (or resets it)
        if (level > last_level || path.empty())
        {
            if (!path.empty())
            {
                path += " > ";
            }
            // Prefix with heading markers for visual clarity
            for (int hash_idx = 0; hash_idx < level; ++hash_idx)
            {
                path += '#';
            }
            path += ' ';
            path += heading_texts_[idx];
            last_level = level;
            innermost_line = heading_positions_[idx];
        }
        else if (level <= last_level)
        {
            // Reset: this heading is at same or higher level
            path.clear();
            for (int hash_idx = 0; hash_idx < level; ++hash_idx)
            {
                path += '#';
            }
            path += ' ';
            path += heading_texts_[idx];
            last_level = level;
            innermost_line = heading_positions_[idx];
        }
    }

    if (path != current_breadcrumb_)
    {
        current_breadcrumb_ = path;
        core::events::BreadcrumbsChangedEvent evt;
        evt.breadcrumb_path = current_breadcrumb_;
        evt.heading_line = innermost_line;
        event_bus_.publish(evt);
    }
}

auto SplitView::GetCurrentBreadcrumb() const -> std::string
{
    return current_breadcrumb_;
}

// ═══════════════════════════════════════════════════════
// Phase 09 Task 5: Adaptive Render Throttling
// ═══════════════════════════════════════════════════════

void SplitView::UpdateRenderThrottle(std::size_t content_size)
{
    if (preview_panel_ == nullptr)
    {
        return;
    }

    int debounce_ms = kMediumDocDebounceMs; // default
    if (content_size < kSmallDocThreshold)
    {
        debounce_ms = kSmallDocDebounceMs;
    }
    else if (content_size > kLargeDocThreshold)
    {
        debounce_ms = kLargeDocDebounceMs;
    }

    spdlog::trace("SplitView: adaptive throttle -> {}ms for {} bytes", debounce_ms, content_size);
}

// ═══════════════════════════════════════════════════════
// Phase 09 Task 11: Typewriter Scroll Mode
// ═══════════════════════════════════════════════════════

void SplitView::SetTypewriterMode(bool enabled)
{
    if (enabled == typewriter_mode_)
    {
        return;
    }
    typewriter_mode_ = enabled;

    if (editor_panel_ == nullptr)
    {
        return;
    }

    auto* stc = editor_panel_->GetStyledTextCtrl();
    if (stc == nullptr)
    {
        return;
    }

    if (enabled)
    {
        // Center the cursor vertically using Scintilla caret policy
        stc->SetYCaretPolicy(wxSTC_CARET_STRICT | wxSTC_CARET_EVEN, 0);
        spdlog::debug("SplitView: typewriter mode ENABLED");
    }
    else
    {
        // Restore default caret policy — SLOP with small margin
        stc->SetYCaretPolicy(wxSTC_CARET_SLOP, 3);
        spdlog::debug("SplitView: typewriter mode DISABLED");
    }
}

auto SplitView::IsTypewriterMode() const -> bool
{
    return typewriter_mode_;
}

void SplitView::ToggleTypewriterMode()
{
    SetTypewriterMode(!typewriter_mode_);
}

// ═══════════════════════════════════════════════════════
// Phase 09 Task 13: Export Rendered HTML
// ═══════════════════════════════════════════════════════

void SplitView::ExportHtml()
{
    if (preview_panel_ == nullptr)
    {
        return;
    }

    // Show file dialog to choose export path
    wxFileDialog save_dialog(this,
                             "Export HTML",
                             "",
                             "export.html",
                             "HTML files (*.html)|*.html|All files (*.*)|*.*",
                             wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (save_dialog.ShowModal() == wxID_CANCEL)
    {
        return; // User cancelled
    }

    const std::string path = save_dialog.GetPath().ToStdString();
    const bool success = preview_panel_->ExportHtml(std::filesystem::path(path));

    if (success)
    {
        spdlog::info("SplitView: HTML exported to {}", path);
        event_bus_.publish(core::events::NotificationEvent{
            "HTML exported to " + path, core::events::NotificationLevel::Success});
    }
    else
    {
        MARKAMP_LOG_ERROR("SplitView: HTML export failed for {}", path);
        event_bus_.publish(core::events::NotificationEvent{"HTML export failed",
                                                           core::events::NotificationLevel::Error});
    }
}

// ═══════════════════════════════════════════════════════
// Phase 09 Task 14: Reveal Commands
// ═══════════════════════════════════════════════════════

void SplitView::RevealInEditor(int heading_index)
{
    if (editor_panel_ == nullptr)
    {
        return;
    }

    const auto idx = static_cast<std::size_t>(heading_index);
    if (idx >= heading_positions_.size())
    {
        return;
    }

    const int target_line = heading_positions_[idx];
    auto* stc = editor_panel_->GetStyledTextCtrl();
    if (stc != nullptr)
    {
        stc->GotoLine(target_line);
        stc->EnsureVisibleEnforcePolicy(target_line);
        stc->SetFocus();
    }
}

void SplitView::RevealInPreview(int editor_line)
{
    if (preview_panel_ == nullptr)
    {
        return;
    }

    // Find the nearest heading at or before editor_line to compute scroll fraction
    const int heading_idx = FindNearestHeading(editor_line);
    if (heading_idx < 0)
    {
        return;
    }

    // Compute fraction based on heading position relative to total headings
    if (heading_positions_.empty())
    {
        return;
    }

    const double fraction =
        static_cast<double>(heading_idx) /
        static_cast<double>(std::max(heading_positions_.size() - 1, std::size_t{1}));
    preview_panel_->SetScrollFraction(std::clamp(fraction, 0.0, 1.0));
}
// ═══════════════════════════════════════════════════════
// Phase 09 Task 6: Split Direction
// ═══════════════════════════════════════════════════════

void SplitView::SetSplitDirection(core::events::SplitDirection direction)
{
    if (direction == split_direction_)
    {
        return;
    }
    split_direction_ = direction;

    // Update divider cursor based on direction
    if (split_direction_ == core::events::SplitDirection::Vertical)
    {
        divider_panel_->SetCursor(wxCursor(wxCURSOR_SIZENS));
    }
    else
    {
        divider_panel_->SetCursor(wxCursor(wxCURSOR_SIZEWE));
    }

    UpdateLayout();

    // Persist
    if (config_ != nullptr)
    {
        const std::string dir_val =
            direction == core::events::SplitDirection::Vertical ? "vertical" : "horizontal";
        config_->set("split_direction", dir_val);
    }

    core::events::SplitDirectionChangedEvent dir_evt;
    dir_evt.direction = direction;
    event_bus_.publish(dir_evt);
    spdlog::debug("SplitView: direction -> {}",
                  direction == core::events::SplitDirection::Vertical ? "Vertical" : "Horizontal");
}

auto SplitView::GetSplitDirection() const -> core::events::SplitDirection
{
    return split_direction_;
}

void SplitView::ToggleSplitDirection()
{
    SetSplitDirection(split_direction_ == core::events::SplitDirection::Horizontal
                          ? core::events::SplitDirection::Vertical
                          : core::events::SplitDirection::Horizontal);
}

// ═══════════════════════════════════════════════════════
// Phase 09 Task 7: Magnetic Snap Points
// ═══════════════════════════════════════════════════════

auto SplitView::FindNearestSnapPoint(double ratio) -> double
{
    double nearest = ratio;
    double min_dist = kSnapThreshold + 1.0; // outside threshold by default

    for (const double snap : kSnapPoints)
    {
        const double dist = std::abs(ratio - snap);
        if (dist < min_dist)
        {
            min_dist = dist;
            nearest = snap;
        }
    }

    return nearest;
}

// ═══════════════════════════════════════════════════════
// Phase 09 Task 8: Pin Preview
// ═══════════════════════════════════════════════════════

void SplitView::SetPinPreview(bool pinned)
{
    if (pinned == pin_preview_)
    {
        return;
    }
    pin_preview_ = pinned;

    if (pinned && preview_panel_ != nullptr)
    {
        // Freeze current preview content
        // (PreviewPanel retains its current HTML until updated)
        spdlog::debug("SplitView: preview PINNED");
    }
    else
    {
        // Unpin: re-sync to current editor content
        if (editor_panel_ != nullptr && preview_panel_ != nullptr)
        {
            preview_panel_->SetMarkdownContent(editor_panel_->GetContent());
        }
        spdlog::debug("SplitView: preview UNPINNED");
    }

    // Repaint divider to show/hide pin indicator
    if (divider_panel_ != nullptr)
    {
        divider_panel_->Refresh();
    }
}

auto SplitView::IsPinPreview() const -> bool
{
    return pin_preview_;
}

void SplitView::TogglePinPreview()
{
    SetPinPreview(!pin_preview_);
}

// ═══════════════════════════════════════════════════════
// Phase 09 Task 9: Open in Side
// ═══════════════════════════════════════════════════════

void SplitView::OpenInSide(const std::string& file_path)
{
    if (file_path.empty())
    {
        return;
    }

    // Switch to split mode if not already
    if (current_mode_ != core::events::ViewMode::Split)
    {
        SetViewMode(core::events::ViewMode::Split);
    }

    // Read file and render in preview
    std::ifstream in_file(file_path);
    if (in_file.is_open())
    {
        std::string content((std::istreambuf_iterator<char>(in_file)),
                            std::istreambuf_iterator<char>());
        in_file.close();

        if (preview_panel_ != nullptr)
        {
            preview_panel_->SetMarkdownContent(content);
            // Pin the preview so it doesn't get overwritten
            SetPinPreview(true);
        }
    }
    else
    {
        MARKAMP_LOG_ERROR("OpenInSide: failed to read file: {}", file_path);
    }

    // Publish event for other listeners
    core::events::OpenInSideEvent evt;
    evt.file_path = file_path;
    event_bus_.publish(evt);
}

// ═══════════════════════════════════════════════════════
// Phase 09 Task 10: Per-File State Persistence
// ═══════════════════════════════════════════════════════

void SplitView::SavePerFileState(const std::string& file_path)
{
    if (file_path.empty())
    {
        return;
    }

    PerFileState state;
    state.split_ratio = split_ratio_;
    state.view_mode = current_mode_;
    state.direction = split_direction_;
    per_file_states_[file_path] = state;
    current_file_path_ = file_path;

    // Also persist to config for cross-session restoration
    if (config_ != nullptr)
    {
        const std::string key_prefix =
            "per_file." + std::to_string(std::hash<std::string>{}(file_path));
        config_->set(key_prefix + ".ratio", state.split_ratio);
        config_->set(key_prefix + ".mode", static_cast<int>(state.view_mode));
        const std::string dir_val =
            state.direction == core::events::SplitDirection::Vertical ? "vertical" : "horizontal";
        config_->set(key_prefix + ".direction", dir_val);
    }
}

void SplitView::RestorePerFileState(const std::string& file_path)
{
    if (file_path.empty())
    {
        return;
    }

    current_file_path_ = file_path;

    // Check in-memory cache first
    auto found = per_file_states_.find(file_path);
    if (found != per_file_states_.end())
    {
        const auto& state = found->second;
        split_ratio_ = std::clamp(state.split_ratio, kMinSplitRatio, kMaxSplitRatio);
        split_direction_ = state.direction;
        SetViewMode(state.view_mode);
        UpdateLayout();
        return;
    }

    // Try config persistence
    if (config_ != nullptr)
    {
        const std::string key_prefix =
            "per_file." + std::to_string(std::hash<std::string>{}(file_path));
        const double ratio = config_->get_double(key_prefix + ".ratio", kDefaultSplitRatio);
        split_ratio_ = std::clamp(ratio, kMinSplitRatio, kMaxSplitRatio);

        const int mode_int =
            config_->get_int(key_prefix + ".mode", static_cast<int>(core::events::ViewMode::Split));
        const auto mode = static_cast<core::events::ViewMode>(mode_int);

        const std::string dir_str = config_->get_string(key_prefix + ".direction", "horizontal");
        split_direction_ = (dir_str == "vertical") ? core::events::SplitDirection::Vertical
                                                   : core::events::SplitDirection::Horizontal;

        SetViewMode(mode);
        UpdateLayout();
    }
}

// ═══════════════════════════════════════════════════════
// V8 Phase 12: Paired Traverse Mode
// ═══════════════════════════════════════════════════════

void SplitView::SetPairMode(PairMode mode)
{
    pair_mode_ = mode;
}

auto SplitView::GetPairMode() const -> PairMode
{
    return pair_mode_;
}

auto SplitView::IsPaired() const -> bool
{
    return pair_mode_ != PairMode::kNone;
}

} // namespace markamp::ui
