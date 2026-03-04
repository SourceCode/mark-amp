#pragma once

/// @file OutputPanel.h
/// @brief Output Panel V2 — multi-channel output display with ANSI rendering.
///
/// Features:
///   - Multi-channel output with themed channel selector
///   - Log level filtering (Trace/Info/Warning/Error)
///   - ANSI color rendering via OutputPanelRenderer
///   - Timestamps, word wrap, lock scroll, auto-reveal
///   - Configurable fonts, search/filter, clipboard support
///   - Virtual list mode for large outputs
///   - "Open in Editor" context menu action
///   - EventBus integration and filtered view caching
///
/// @see OutputChannelService, OutputPanelRenderer, LogLevel

#include "core/LogLevel.h"
#include "core/OutputChannelService.h"
#include "ui/OutputPanelRenderer.h"

#include <wx/choice.h>
#include <wx/clipbrd.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/srchctrl.h>
#include <wx/textctrl.h>
#include <wx/tglbtn.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

class PanelContainer;

/// Output Panel V2 — tabbed output channel viewer in the bottom panel.
/// Features: channel selector with unread badges, log level filter toggles,
/// ANSI color rendering, search/filter bar, word wrap toggle, lock scroll,
/// timestamps, and "Copy" / "Open in Editor" toolbar actions.
class OutputPanel : public wxPanel
{
public:
    /// Data-only constructor (for tests — no wxPanel parent).
    OutputPanel();

    /// UI constructor with rendering support.
    OutputPanel(wxWindow* parent, core::OutputChannelService* service);

    /// Set the output channel service to read from.
    void set_service(core::OutputChannelService* service);

    /// Get the currently active channel name.
    [[nodiscard]] auto active_channel() const -> const std::string&;

    /// Set the active channel.
    void set_active_channel(const std::string& channel_name);

    /// Get the list of available channel names.
    [[nodiscard]] auto channel_names() const -> std::vector<std::string>;

    /// Get the content for the active channel.
    [[nodiscard]] auto active_content() const -> std::string;

    /// Whether auto-scroll to bottom is enabled.
    [[nodiscard]] auto auto_scroll() const -> bool;
    void set_auto_scroll(bool enabled);

    /// Clear the active channel's content.
    void clear_active_channel();

    /// Lock / unlock scrolling.
    void lock_scroll();
    void unlock_scroll();

    // ── V2 API ──

    /// Current log level filter.
    [[nodiscard]] auto log_level_filter() const -> core::LogLevel;
    void set_log_level_filter(core::LogLevel level);

    /// Whether word wrap is enabled.
    [[nodiscard]] auto word_wrap() const -> bool;
    void set_word_wrap(bool enabled);

    /// Whether timestamps are shown.
    [[nodiscard]] auto show_timestamps() const -> bool;
    void set_show_timestamps(bool enabled);

    /// Current search filter text.
    [[nodiscard]] auto search_text() const -> const std::string&;
    void set_search_text(const std::string& text);

    /// Refresh displayed content from the service.
    void RefreshContent();

    /// Apply theme colors to all controls.
    void ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour);

    /// Configurable font settings.
    void set_font_size(int size);
    void set_font_family(const std::string& family);

    /// Open the file referenced in the currently selected output line.
    void OpenSelectedInEditor();

    /// Enable virtual list rendering mode for large output (custom-painted).
    void set_virtual_list_mode(bool enabled);
    [[nodiscard]] auto virtual_list_mode() const -> bool;

private:
    void CreateLayout(wxWindow* parent);
    void OnChannelChanged(wxCommandEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void CopySelectedToClipboard();
    void CopyAllToClipboard();
    void RenderLines();

    core::OutputChannelService* service_{nullptr};
    OutputPanelRenderer renderer_;

    std::string active_channel_;
    bool auto_scroll_{true};
    core::LogLevel log_level_filter_{core::LogLevel::kTrace};
    bool word_wrap_{true};
    bool show_timestamps_{false};
    std::string search_text_;
    int font_size_{11};
    std::string font_family_{"Menlo"};
    std::vector<std::string> filtered_lines_cache_; ///< Cached filtered output lines
    bool filtered_cache_dirty_{true};               ///< Whether cache needs rebuild
    bool virtual_list_mode_{false};                 ///< Custom-painted mode for large output

    // UI controls (null in data-only / test mode)
    wxWindow* toolbar_{nullptr};
    wxChoice* channel_selector_{nullptr};
    wxTextCtrl* text_area_{nullptr};
    wxSearchCtrl* search_ctrl_{nullptr};
    wxToggleButton* wrap_btn_{nullptr};
    wxToggleButton* timestamp_btn_{nullptr};
};

} // namespace markamp::ui
