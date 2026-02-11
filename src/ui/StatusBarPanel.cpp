#include "StatusBarPanel.h"

#include "core/Events.h"
#include "core/Logger.h"

#include <wx/dcbuffer.h>

#include <cctype>
#include <fmt/format.h>
#include <sstream>

namespace markamp::ui
{

StatusBarPanel::StatusBarPanel(wxWindow* parent,
                               core::ThemeEngine& theme_engine,
                               core::EventBus& event_bus)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxSize(-1, kHeight), wxNO_BORDER)
    , event_bus_(event_bus)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, kHeight));
    SetMaxSize(wxSize(-1, kHeight));

    // Cache current theme name
    theme_name_ = theme_engine.current_theme().name;

    // --- Event subscriptions ---

    // Theme changes → update displayed theme name
    theme_name_sub_ = theme_engine.subscribe_theme_change(
        [this](const std::string& /*theme_id*/)
        {
            theme_name_ = this->theme_engine().current_theme().name;
            RebuildItems();
            Refresh();
        });

    // Cursor position changes
    cursor_sub_ = event_bus_.subscribe<core::events::CursorPositionChangedEvent>(
        [this](const core::events::CursorPositionChangedEvent& evt)
        {
            cursor_line_ = evt.line;
            cursor_col_ = evt.column;
            RebuildItems();
            Refresh();
        });

    // Editor stats changes
    content_sub_ = event_bus_.subscribe<core::events::EditorStatsChangedEvent>(
        [this](const core::events::EditorStatsChangedEvent& evt)
        { set_stats(evt.word_count, evt.char_count, evt.line_count, evt.selection_length); });

    // View mode changes
    view_mode_sub_ = event_bus_.subscribe<core::events::ViewModeChangedEvent>(
        [this](const core::events::ViewModeChangedEvent& evt)
        {
            view_mode_ = evt.mode;
            RebuildItems();
            Refresh();
        });

    // File encoding detected
    encoding_sub_ = event_bus_.subscribe<core::events::FileEncodingDetectedEvent>(
        [this](const core::events::FileEncodingDetectedEvent& evt)
        {
            encoding_ = evt.encoding_name;
            RebuildItems();
            Refresh();
        });

    // Mermaid rendering status
    mermaid_sub_ = event_bus_.subscribe<core::events::MermaidRenderStatusEvent>(
        [this](const core::events::MermaidRenderStatusEvent& evt)
        {
            mermaid_status_ = evt.status;
            mermaid_active_ = evt.active;
            RebuildItems();
            Refresh();
        });

    // Build initial layout items
    RebuildItems();

    Bind(wxEVT_PAINT, &StatusBarPanel::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &StatusBarPanel::OnMouseDown, this);
    Bind(wxEVT_MOTION, &StatusBarPanel::OnMouseMove, this);
}

// --- State setters ---

void StatusBarPanel::set_cursor_position(int line, int column)
{
    cursor_line_ = line;
    cursor_col_ = column;
    // Don't rebuild/refresh here if we assume stats event comes frequently?
    // But cursor position updates on arrow keys, stats updates on debounce.
    // We want fast cursor updates.
    RebuildItems();
    Refresh();
}

void StatusBarPanel::set_encoding(const std::string& encoding)
{
    encoding_ = encoding;
    RebuildItems();
    Refresh();
}

void StatusBarPanel::set_ready_state(const std::string& state)
{
    ready_state_ = state;
    RebuildItems();
    Refresh();
}

void StatusBarPanel::set_mermaid_status(const std::string& status, bool active)
{
    mermaid_status_ = status;
    mermaid_active_ = active;
    RebuildItems();
    Refresh();
}

void StatusBarPanel::set_stats(int word_count, int char_count, int line_count, int selection_len)
{
    word_count_ = word_count;
    char_count_ = char_count;
    line_count_ = line_count;
    selection_len_ = selection_len;
    RebuildItems();
    Refresh();
}

void StatusBarPanel::set_word_count(int count)
{
    word_count_ = count;
    RebuildItems();
    Refresh();
}

void StatusBarPanel::set_file_modified(bool modified)
{
    file_modified_ = modified;
    RebuildItems();
    Refresh();
}

void StatusBarPanel::set_view_mode(core::events::ViewMode mode)
{
    view_mode_ = mode;
    RebuildItems();
    Refresh();
}

// --- Theme ---

void StatusBarPanel::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    theme_name_ = new_theme.name;
    RebuildItems();
    Refresh();
}

// --- Layout ---

void StatusBarPanel::RebuildItems()
{
    left_items_.clear();
    right_items_.clear();

    // Left zone: READY [●] • LN X, COL Y • UTF-8 • SRC/SPLIT/VIEW
    std::string ready_text = ready_state_;
    if (file_modified_)
    {
        ready_text += " \xE2\x97\x8F"; // UTF-8 for ● (black circle / modified indicator)
    }
    left_items_.push_back({ready_text, {}, file_modified_, false, nullptr});

    auto cursor_text = fmt::format("LN {}, COL {}", cursor_line_, cursor_col_);
    left_items_.push_back({cursor_text, {}, false, false, nullptr});

    left_items_.push_back({encoding_, {}, false, false, nullptr});

    left_items_.push_back({view_mode_label(view_mode_), {}, false, false, nullptr});

    // Right zone: {N} WORDS • {M} CHARS • SEL: {LEN} • MERMAID: {STATUS} • Theme Name
    if (word_count_ > 0)
    {
        auto words_text = fmt::format("{} WORDS", word_count_);
        right_items_.push_back({words_text, {}, false, false, nullptr});
    }

    if (char_count_ > 0)
    {
        auto chars_text = fmt::format("{} CHARS", char_count_);
        right_items_.push_back({chars_text, {}, false, false, nullptr});
    }

    if (selection_len_ > 0)
    {
        auto sel_text = fmt::format("SEL: {}", selection_len_);
        right_items_.push_back({sel_text, {}, false, false, nullptr});
    }

    auto mermaid_text = fmt::format("MERMAID: {}", mermaid_status_);
    bool mermaid_is_accent = mermaid_active_;
    right_items_.push_back({mermaid_text, {}, mermaid_is_accent, false, nullptr});

    right_items_.push_back({theme_name_, {}, false, false, nullptr});
}

// --- Drawing ---

void StatusBarPanel::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    auto client_size = GetClientSize();
    int width = client_size.GetWidth();
    int height = client_size.GetHeight();

    // Background
    dc.SetBrush(theme_engine().brush(core::ThemeColorToken::BgPanel));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(client_size);

    // Top border: 1px border_light
    dc.SetPen(theme_engine().pen(core::ThemeColorToken::BorderLight, 1));
    dc.DrawLine(0, 0, width, 0);

    // Font: 10px monospace, uppercase
    dc.SetFont(theme_engine().font(core::ThemeFontToken::UISmall));

    const int padding = 12;
    const int text_y = (height - dc.GetCharHeight()) / 2;
    const int separator_gap = 16;

    // Separator character
    const wxString separator = wxString::FromUTF8("\xE2\x80\xA2"); // • (bullet)
    const int separator_width = dc.GetTextExtent(separator).GetWidth();

    // --- Left section ---
    int left_x = padding;

    for (size_t idx = 0; idx < left_items_.size(); ++idx)
    {
        auto& item = left_items_[idx];

        // Draw separator before item (except the first)
        if (idx > 0)
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
            dc.DrawText(separator, left_x, text_y);
            left_x += separator_width + separator_gap;
        }

        dc.SetTextForeground(item.is_accent
                                 ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                                 : theme_engine().color(core::ThemeColorToken::TextMuted));

        int text_width = dc.GetTextExtent(item.text).GetWidth();
        item.bounds = wxRect(left_x, 0, text_width, height);
        dc.DrawText(item.text, left_x, text_y);

        left_x += text_width + separator_gap;
    }

    // --- Right section ---
    int right_x = width - padding;

    for (auto it = right_items_.rbegin(); it != right_items_.rend(); ++it)
    {
        auto& item = *it;
        int text_width = dc.GetTextExtent(item.text).GetWidth();
        right_x -= text_width;

        dc.SetTextForeground(item.is_accent
                                 ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                                 : theme_engine().color(core::ThemeColorToken::TextMuted));

        item.bounds = wxRect(right_x, 0, text_width, height);
        dc.DrawText(item.text, right_x, text_y);

        right_x -= separator_gap;

        // Draw separator after each right item (except the last one, which is first in reverse)
        if (std::next(it) != right_items_.rend())
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
            right_x -= separator_width;
            dc.DrawText(separator, right_x, text_y);
            right_x -= separator_gap;
        }
    }
}

// --- Mouse interaction ---

void StatusBarPanel::OnMouseDown(wxMouseEvent& event)
{
    auto pos = event.GetPosition();

    // Check left items
    for (const auto& item : left_items_)
    {
        if (item.is_clickable && item.bounds.Contains(pos) && item.on_click)
        {
            item.on_click();
            return;
        }
    }

    // Check right items
    for (const auto& item : right_items_)
    {
        if (item.is_clickable && item.bounds.Contains(pos) && item.on_click)
        {
            item.on_click();
            return;
        }
    }

    event.Skip();
}

void StatusBarPanel::OnMouseMove(wxMouseEvent& event)
{
    auto pos = event.GetPosition();
    bool over_clickable = false;

    for (const auto& item : left_items_)
    {
        if (item.is_clickable && item.bounds.Contains(pos))
        {
            over_clickable = true;
            break;
        }
    }

    if (!over_clickable)
    {
        for (const auto& item : right_items_)
        {
            if (item.is_clickable && item.bounds.Contains(pos))
            {
                over_clickable = true;
                break;
            }
        }
    }

    SetCursor(over_clickable ? wxCursor(wxCURSOR_HAND) : wxCursor(wxCURSOR_DEFAULT));
    event.Skip();
}

// --- Helpers ---

auto StatusBarPanel::count_words(const std::string& content) -> int
{
    if (content.empty())
    {
        return 0;
    }

    int count = 0;
    bool in_word = false;

    for (char character : content)
    {
        if (std::isspace(static_cast<unsigned char>(character)) != 0)
        {
            in_word = false;
        }
        else if (!in_word)
        {
            in_word = true;
            ++count;
        }
    }

    return count;
}

auto StatusBarPanel::view_mode_label(core::events::ViewMode mode) -> std::string
{
    switch (mode)
    {
        case core::events::ViewMode::Editor:
            return "SRC";
        case core::events::ViewMode::Preview:
            return "VIEW";
        case core::events::ViewMode::Split:
            return "SPLIT";
    }
    return "SPLIT"; // fallback
}

} // namespace markamp::ui
