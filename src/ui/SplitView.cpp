#include "SplitView.h"

#include "BevelPanel.h"
#include "EditorPanel.h"
#include "PreviewPanel.h"
#include "core/Config.h"
#include "core/Events.h"
#include "core/Logger.h"

#include <algorithm>

namespace markamp::ui
{

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

    // --- Divider ---
    divider_panel_ = new wxPanel(this, wxID_ANY);
    divider_panel_->SetBackgroundColour(wxColour(theme().colors.border_dark.to_rgba_string()));
    divider_panel_->SetCursor(wxCursor(wxCURSOR_SIZEWE));

    divider_panel_->Bind(wxEVT_LEFT_DOWN, &SplitView::OnDividerMouseDown, this);
    divider_panel_->Bind(wxEVT_MOTION, &SplitView::OnDividerMouseMove, this);
    divider_panel_->Bind(wxEVT_LEFT_UP, &SplitView::OnDividerMouseUp, this);
    divider_panel_->Bind(wxEVT_ENTER_WINDOW, &SplitView::OnDividerMouseEnter, this);
    divider_panel_->Bind(wxEVT_LEAVE_WINDOW, &SplitView::OnDividerMouseLeave, this);

    // --- Editor bevel overlay ---
    // NOTE: BevelPanel overlays do not work on macOS — they paint opaque and
    // block the underlying panel even with wxTRANSPARENT_WINDOW.  Keep the
    // object alive (other code references it) but never show it.
    editor_bevel_ = new BevelPanel(this, wxID_ANY, BevelPanel::Style::Sunken);
    editor_bevel_->set_shadow_colour(wxColour(0, 0, 0, 51));          // rgba(0,0,0,0.2)
    editor_bevel_->set_highlight_colour(wxColour(255, 255, 255, 13)); // rgba(255,255,255,0.05)
    editor_bevel_->Enable(false);                                     // Non-interactive
    editor_bevel_->Hide(); // Disabled: blocks editor on macOS

    // --- Size event ---
    Bind(wxEVT_SIZE, &SplitView::OnSize, this);

    // --- Subscribe to external view mode changes ---
    view_mode_sub_ = event_bus_.subscribe<core::events::ViewModeChangedEvent>(
        [this](const core::events::ViewModeChangedEvent& evt) { SetViewMode(evt.mode); });

    // --- Keyboard shortcuts: Cmd+1=Source, Cmd+2=Split, Cmd+3=Preview ---
    enum
    {
        ID_MODE_EDITOR = wxID_HIGHEST + 100,
        ID_MODE_SPLIT,
        ID_MODE_PREVIEW
    };

    wxAcceleratorEntry accel_entries[3];
    accel_entries[0].Set(wxACCEL_CMD, '1', ID_MODE_EDITOR);
    accel_entries[1].Set(wxACCEL_CMD, '2', ID_MODE_SPLIT);
    accel_entries[2].Set(wxACCEL_CMD, '3', ID_MODE_PREVIEW);
    wxAcceleratorTable accel_table(3, accel_entries);
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

    // --- Restore persisted split ratio ---
    RestoreSplitRatio();

    // --- Initial layout ---
    UpdateLayout();
}

// ═══════════════════════════════════════════════════════
// View Mode
// ═══════════════════════════════════════════════════════

void SplitView::SetViewMode(core::events::ViewMode mode)
{
    if (mode == current_mode_)
    {
        return;
    }

    // Save editor state before switching
    auto editor_state = SaveEditorState();

    current_mode_ = mode;

    // Update visibility
    const bool show_editor =
        (mode == core::events::ViewMode::Editor || mode == core::events::ViewMode::Split);
    const bool show_preview =
        (mode == core::events::ViewMode::Preview || mode == core::events::ViewMode::Split);
    const bool show_divider = (mode == core::events::ViewMode::Split);

    editor_panel_->Show(show_editor);
    preview_panel_->Show(show_preview);
    divider_panel_->Show(show_divider);
    // Bevel disabled — it blocks the editor on macOS
    editor_bevel_->Hide();

    // Re-layout
    UpdateLayout();

    // Restore editor state
    RestoreEditorState(editor_state);

    spdlog::debug("SplitView: mode changed to {}",
                  mode == core::events::ViewMode::Editor    ? "Editor"
                  : mode == core::events::ViewMode::Preview ? "Preview"
                                                            : "Split");
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
// Divider Drag
// ═══════════════════════════════════════════════════════

void SplitView::OnDividerMouseDown(wxMouseEvent& event)
{
    is_dragging_ = true;
    drag_start_x_ = event.GetX();
    drag_start_ratio_ = split_ratio_;
    divider_panel_->CaptureMouse();
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
        SaveSplitRatio();
    }
    event.Skip();
}

void SplitView::OnDividerMouseEnter(wxMouseEvent& /*event*/)
{
    divider_panel_->SetCursor(wxCursor(wxCURSOR_SIZEWE));
}

void SplitView::OnDividerMouseLeave(wxMouseEvent& /*event*/)
{
    if (!is_dragging_)
    {
        divider_panel_->SetCursor(wxNullCursor);
    }
}

// ═══════════════════════════════════════════════════════
// Layout
// ═══════════════════════════════════════════════════════

void SplitView::UpdateLayout()
{
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
            // Ensure minimum panel widths
            split_pos = std::clamp(split_pos, kDividerWidth * 2, width - kDividerWidth * 2);

            int editor_width = split_pos;
            int divider_x = split_pos;
            int preview_x = split_pos + kDividerWidth;
            int preview_width = width - preview_x;

            editor_panel_->SetSize(0, 0, editor_width, height);
            divider_panel_->SetSize(divider_x, 0, kDividerWidth, height);
            preview_panel_->SetSize(preview_x, 0, preview_width, height);

            // Bevel overlay disabled — it blocks the editor on macOS
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
        // Content should already be there — just restore cursor position
        editor_panel_->SetCursorPosition(state.cursor_line, state.cursor_col);
    }
}

// ═══════════════════════════════════════════════════════
// Theme
// ═══════════════════════════════════════════════════════

void SplitView::OnThemeChanged(const core::Theme& new_theme)
{
    if (divider_panel_ != nullptr)
    {
        divider_panel_->SetBackgroundColour(
            wxColour(new_theme.colors.border_dark.to_rgba_string()));
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
