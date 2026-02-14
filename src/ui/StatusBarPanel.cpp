#include "StatusBarPanel.h"

#include "core/Events.h"
#include "core/Logger.h"

#include <wx/dcbuffer.h>

#include <array>
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

    // R17 Fix 8: Flash "SAVED" on save event
    save_sub_ = event_bus_.subscribe<core::events::TabSaveRequestEvent>(
        [this](const core::events::TabSaveRequestEvent& /*evt*/)
        {
            save_flash_active_ = true;
            ready_state_ = "SAVED \xE2\x9C\x93";
            RebuildItems();
            Refresh();
            save_flash_timer_.StartOnce(800);
        });
    save_flash_timer_.Bind(wxEVT_TIMER,
                           [this](wxTimerEvent& /*evt*/)
                           {
                               save_flash_active_ = false;
                               ready_state_ = "READY";
                               RebuildItems();
                               Refresh();
                           });

    // R18 Fix 12: Progress spinner timer
    progress_spinner_timer_.SetOwner(this);
    progress_spinner_timer_.Bind(wxEVT_TIMER,
                                 [this](wxTimerEvent& /*evt*/)
                                 {
                                     spinner_frame_ = (spinner_frame_ + 1) % 8;
                                     RebuildItems();
                                     Refresh();
                                 });
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

// R2 Fix 13: Active filename setter
void StatusBarPanel::set_filename(const std::string& filename)
{
    filename_ = filename;
    RebuildItems();
    Refresh();
}

// R2 Fix 14: Language setter
void StatusBarPanel::set_language(const std::string& language)
{
    language_ = language;
    RebuildItems();
    Refresh();
}

// R2 Fix 19: File size setter
void StatusBarPanel::set_file_size(std::size_t size_bytes)
{
    file_size_bytes_ = size_bytes;
    RebuildItems();
    Refresh();
}

// R4 Fix 9: EOL mode display (LF / CRLF)
void StatusBarPanel::set_eol_mode(const std::string& eol_mode)
{
    eol_mode_ = eol_mode;
    RebuildItems();
    Refresh();
}

// R6 Fix 14: Indent mode display
void StatusBarPanel::set_indent_mode(const std::string& indent_mode)
{
    indent_mode_ = indent_mode;
    RebuildItems();
    Refresh();
}

// R13: Zoom indicator
void StatusBarPanel::set_zoom_level(int zoom_level)
{
    zoom_level_ = zoom_level;
    RebuildItems();
    Refresh();
}

// R18 Fix 12: Progress spinner
void StatusBarPanel::set_progress(bool active, const std::string& label)
{
    progress_active_ = active;
    progress_label_ = label;
    if (active && !progress_spinner_timer_.IsRunning())
    {
        spinner_frame_ = 0;
        progress_spinner_timer_.Start(80);
    }
    else if (!active && progress_spinner_timer_.IsRunning())
    {
        progress_spinner_timer_.Stop();
    }
    RebuildItems();
    Refresh();
}

// R18 Fix 13: Git branch display
void StatusBarPanel::set_git_branch(const std::string& branch)
{
    git_branch_ = branch;
    RebuildItems();
    Refresh();
}

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
    left_items_.push_back({ready_text, {}, file_modified_, false, nullptr, "Editor status"});

    auto cursor_text = fmt::format("LN {}, COL {}", cursor_line_, cursor_col_);
    // R4 Fix 16: Cursor position is clickable — triggers Go-To-Line
    left_items_.push_back({cursor_text,
                           {},
                           false,
                           true,
                           [this]()
                           {
                               const core::events::GoToLineRequestEvent go_evt;
                               event_bus_.publish(go_evt);
                           },
                           "Click to go to line"});

    // R5 Fix 15: Encoding is clickable — cycles through encodings
    left_items_.push_back(
        {encoding_,
         {},
         false,
         true,
         [this]()
         {
             static const std::array<std::string, 3> kEncodings = {"UTF-8", "ASCII", "ISO-8859-1"};
             for (size_t idx = 0; idx < kEncodings.size(); ++idx)
             {
                 if (encoding_ == kEncodings[idx])
                 {
                     encoding_ = kEncodings[(idx + 1) % kEncodings.size()];
                     break;
                 }
             }
             RebuildItems();
             Refresh();
         },
         "Click to change encoding"});

    // R4 Fix 9: Line ending mode — R7: clickable, cycles LF/CRLF/CR
    if (!eol_mode_.empty())
    {
        left_items_.push_back(
            {eol_mode_,
             {},
             false,
             true,
             [this]()
             {
                 static const std::array<std::string, 3> kEols = {"LF", "CRLF", "CR"};
                 for (size_t idx = 0; idx < kEols.size(); ++idx)
                 {
                     if (eol_mode_ == kEols[idx])
                     {
                         eol_mode_ = kEols[(idx + 1) % kEols.size()];
                         break;
                     }
                 }
                 RebuildItems();
                 Refresh();
             },
             "Click to change line ending"});
    }

    // R6 Fix 14: Indent mode indicator — R7: clickable, cycles modes
    left_items_.push_back(
        {indent_mode_,
         {},
         false,
         true,
         [this]()
         {
             static const std::array<std::string, 3> kIndents = {"Spaces: 4", "Spaces: 2", "Tabs"};
             for (size_t idx = 0; idx < kIndents.size(); ++idx)
             {
                 if (indent_mode_ == kIndents[idx])
                 {
                     indent_mode_ = kIndents[(idx + 1) % kIndents.size()];
                     break;
                 }
             }
             RebuildItems();
             Refresh();
         },
         "Click to change indentation"});

    // R13: Zoom indicator
    {
        auto zoom_text = fmt::format("Zoom: {}%", 100 + (zoom_level_ * 10));
        left_items_.push_back({zoom_text, {}, false, false, nullptr, "Current zoom level"});
    }

    left_items_.push_back(
        {view_mode_label(view_mode_), {}, false, false, nullptr, "Current view mode"});

    // R18 Fix 12: Progress spinner
    if (progress_active_)
    {
        static const std::array<std::string, 8> kSpinnerFrames = {"\xE2\xA3\xBE",
                                                                  "\xE2\xA3\xBD",
                                                                  "\xE2\xA3\xBB",
                                                                  "\xE2\xA2\xBF",
                                                                  "\xE2\xA1\xBF",
                                                                  "\xE2\xA3\x9F",
                                                                  "\xE2\xA3\xAF",
                                                                  "\xE2\xA3\xB7"};
        auto spin_text =
            kSpinnerFrames[static_cast<size_t>(spinner_frame_)] + " " + progress_label_;
        left_items_.push_back({spin_text, {}, true, false, nullptr, "Background operation"});
    }

    // R18 Fix 13: Git branch
    if (!git_branch_.empty())
    {
        auto branch_text = "\xE2\x8E\x87 " + git_branch_;
        left_items_.push_back({branch_text, {}, false, false, nullptr, "Current git branch"});
    }

    // Right zone: {N} WORDS • {M} CHARS • SEL: {LEN} • MERMAID: {STATUS} • Theme Name
    if (word_count_ > 0)
    {
        auto words_text = fmt::format("{} WORDS", word_count_);
        right_items_.push_back({words_text, {}, false, false, nullptr, "Total word count"});

        // R20 Fix 13: Reading time estimate (~N min read at 200 WPM)
        int reading_minutes = std::max(1, word_count_ / 200);
        auto read_time_text = fmt::format("~{} min read", reading_minutes);
        right_items_.push_back(
            {read_time_text, {}, false, false, nullptr, "Estimated reading time"});
    }

    if (char_count_ > 0)
    {
        auto chars_text = fmt::format("{} CHARS", char_count_);
        right_items_.push_back({chars_text, {}, false, false, nullptr, "Total character count"});
    }

    if (selection_len_ > 0)
    {
        // R18 Fix 14 + R20 Fix 11: Selection count badge with accent highlight
        auto sel_text = fmt::format("Sel: {} chars", selection_len_);
        right_items_.push_back({sel_text, {}, true, false, nullptr, "Selected text length"});
    }

    auto mermaid_text = fmt::format("MERMAID: {}", mermaid_status_);
    bool mermaid_is_accent = mermaid_active_;
    right_items_.push_back(
        {mermaid_text, {}, mermaid_is_accent, false, nullptr, "Mermaid diagram status"});

    right_items_.push_back({theme_name_, {}, false, false, nullptr, "Active theme"});

    // R2 Fix 13 + R20 Fix 15: Filename with modified dot indicator
    if (!filename_.empty())
    {
        std::string display_name = filename_;
        if (file_modified_)
        {
            display_name = "\xE2\x97\x8F " + display_name; // ● prefix when modified
        }
        left_items_.push_back({display_name, {}, file_modified_, false, nullptr, "Active file"});
    }

    // R2 Fix 14: Language in right items — R7: clickable, cycles languages
    if (!language_.empty())
    {
        right_items_.push_back({language_,
                                {},
                                false,
                                true,
                                [this]()
                                {
                                    static const std::array<std::string, 3> kLangs = {
                                        "Markdown", "Plain Text", "HTML"};
                                    for (size_t idx = 0; idx < kLangs.size(); ++idx)
                                    {
                                        if (language_ == kLangs[idx])
                                        {
                                            language_ = kLangs[(idx + 1) % kLangs.size()];
                                            break;
                                        }
                                    }
                                    RebuildItems();
                                    Refresh();
                                },
                                "Click to change language"});
    }

    // R2 Fix 18: Line count
    if (line_count_ > 0)
    {
        auto lines_text = fmt::format("{} LINES", line_count_);
        right_items_.push_back({lines_text, {}, false, false, nullptr, "Total line count"});
    }

    // R2 Fix 19: File size
    if (file_size_bytes_ > 0)
    {
        std::string size_text;
        if (file_size_bytes_ >= 1024 * 1024)
        {
            size_text =
                fmt::format("{:.1f} MB", static_cast<double>(file_size_bytes_) / (1024.0 * 1024.0));
        }
        else if (file_size_bytes_ >= 1024)
        {
            size_text = fmt::format("{:.1f} KB", static_cast<double>(file_size_bytes_) / 1024.0);
        }
        else
        {
            size_text = fmt::format("{} B", file_size_bytes_);
        }
        right_items_.push_back({size_text, {}, false, false, nullptr, "File size on disk"});
    }
}

// --- Drawing ---

void StatusBarPanel::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    auto client_size = GetClientSize();
    int width = client_size.GetWidth();
    int height = client_size.GetHeight();

    // Background with 8D gradient: slightly darker at bottom
    {
        auto base_col = theme_engine().color(core::ThemeColorToken::BgPanel);
        auto darker = base_col.ChangeLightness(97);
        for (int row = 0; row < height; ++row)
        {
            const double frac =
                static_cast<double>(row) / static_cast<double>(std::max(height - 1, 1));
            auto lerp = [](int from, int to, double ratio) -> unsigned char
            {
                return static_cast<unsigned char>(
                    std::clamp(static_cast<int>(from + ratio * (to - from)), 0, 255));
            };
            dc.SetPen(wxPen(wxColour(lerp(base_col.Red(), darker.Red(), frac),
                                     lerp(base_col.Green(), darker.Green(), frac),
                                     lerp(base_col.Blue(), darker.Blue(), frac)),
                            1));
            dc.DrawLine(0, row, width, row);
        }
    }

    // 8B: Soft top border — BorderLight at 60% alpha
    {
        auto border_col = theme_engine().color(core::ThemeColorToken::BorderLight);
        dc.SetPen(wxPen(wxColour(border_col.Red(), border_col.Green(), border_col.Blue(), 153), 1));
        dc.DrawLine(0, 0, width, 0);
    }

    // Font: 10px monospace, uppercase
    dc.SetFont(theme_engine().font(core::ThemeFontToken::UISmall));

    const int padding = 16; // 8E: was 12
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

        // R16 Fix 14: bold for accent items
        if (item.is_accent)
        {
            wxFont bold_font = theme_engine().font(core::ThemeFontToken::UISmall);
            bold_font.SetWeight(wxFONTWEIGHT_SEMIBOLD);
            dc.SetFont(bold_font);
        }

        dc.SetTextForeground(item.is_accent
                                 ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                                 : theme_engine().color(core::ThemeColorToken::TextMuted));

        int text_width = dc.GetTextExtent(item.text).GetWidth();

        // R20 Fix 12: Hover highlight for ALL items (extended from R16 Fix 11)
        // R16 Fix 11: Subtle hover highlight for clickable items
        if (item.is_clickable || item.bounds.Contains(ScreenToClient(wxGetMousePosition())))
        {
            auto hover_bg =
                theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(115);
            dc.SetBrush(wxBrush(hover_bg));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRoundedRectangle(left_x - 4, 2, text_width + 8, height - 4, 3);
        }

        item.bounds = wxRect(left_x, 0, text_width, height);
        dc.DrawText(item.text, left_x, text_y);

        // Reset font if we changed it
        if (item.is_accent)
        {
            dc.SetFont(theme_engine().font(core::ThemeFontToken::UISmall));
        }

        left_x += text_width + separator_gap;
    }

    // --- Right section ---
    int right_x = width - padding;

    for (auto it = right_items_.rbegin(); it != right_items_.rend(); ++it)
    {
        auto& item = *it;
        int text_width = dc.GetTextExtent(item.text).GetWidth();
        right_x -= text_width;

        // R16 Fix 14: bold for accent items (right section)
        if (item.is_accent)
        {
            wxFont bold_font = theme_engine().font(core::ThemeFontToken::UISmall);
            bold_font.SetWeight(wxFONTWEIGHT_SEMIBOLD);
            dc.SetFont(bold_font);
        }

        dc.SetTextForeground(item.is_accent
                                 ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                                 : theme_engine().color(core::ThemeColorToken::TextMuted));

        // R20 Fix 12: Hover highlight for ALL items (right side)
        if (item.is_clickable || item.bounds.Contains(ScreenToClient(wxGetMousePosition())))
        {
            auto hover_bg =
                theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(115);
            dc.SetBrush(wxBrush(hover_bg));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRoundedRectangle(right_x - 4, 2, text_width + 8, height - 4, 3);
        }

        item.bounds = wxRect(right_x, 0, text_width, height);
        dc.DrawText(item.text, right_x, text_y);

        // Reset font if we changed it
        if (item.is_accent)
        {
            dc.SetFont(theme_engine().font(core::ThemeFontToken::UISmall));
        }

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
    std::string hovered_tooltip;

    for (const auto& item : left_items_)
    {
        if (item.bounds.Contains(pos))
        {
            hovered_tooltip = item.tooltip;
            if (item.is_clickable)
            {
                over_clickable = true;
            }
            break;
        }
    }

    if (hovered_tooltip.empty())
    {
        for (const auto& item : right_items_)
        {
            if (item.bounds.Contains(pos))
            {
                hovered_tooltip = item.tooltip;
                if (item.is_clickable)
                {
                    over_clickable = true;
                }
                break;
            }
        }
    }

    // R6 Fix 9: Show tooltip on hover
    if (!hovered_tooltip.empty())
    {
        SetToolTip(hovered_tooltip);
    }
    else
    {
        UnsetToolTip();
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
