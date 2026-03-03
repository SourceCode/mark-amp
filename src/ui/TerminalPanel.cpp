#include "TerminalPanel.h"

#include "core/Events.h"
#include "core/Logger.h"

#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>
#include <wx/menu.h>

namespace markamp::ui
{

auto BuildTerminalColorScheme(const core::Theme& theme) -> TerminalColorScheme
{
    TerminalColorScheme scheme;
    scheme.background = theme.colors.editor_bg.to_wx_colour();
    scheme.foreground = theme.colors.editor_fg.to_wx_colour();
    scheme.cursor = theme.colors.accent_primary.to_wx_colour();
    const auto accent = theme.colors.accent_primary;
    scheme.selection_bg = wxColour(accent.r, accent.g, accent.b, 80);

    // Standard ANSI colors (dark palette)
    scheme.ansi_palette[0] = wxColour(0, 0, 0);       // Black
    scheme.ansi_palette[1] = wxColour(205, 49, 49);   // Red
    scheme.ansi_palette[2] = wxColour(13, 188, 121);  // Green
    scheme.ansi_palette[3] = wxColour(229, 229, 16);  // Yellow
    scheme.ansi_palette[4] = wxColour(36, 114, 200);  // Blue
    scheme.ansi_palette[5] = wxColour(188, 63, 188);  // Magenta
    scheme.ansi_palette[6] = wxColour(17, 168, 205);  // Cyan
    scheme.ansi_palette[7] = wxColour(229, 229, 229); // White

    // Bright ANSI colors
    scheme.ansi_palette[8] = wxColour(102, 102, 102);  // Bright Black
    scheme.ansi_palette[9] = wxColour(241, 76, 76);    // Bright Red
    scheme.ansi_palette[10] = wxColour(35, 209, 139);  // Bright Green
    scheme.ansi_palette[11] = wxColour(245, 245, 67);  // Bright Yellow
    scheme.ansi_palette[12] = wxColour(59, 142, 234);  // Bright Blue
    scheme.ansi_palette[13] = wxColour(214, 112, 214); // Bright Magenta
    scheme.ansi_palette[14] = wxColour(41, 184, 219);  // Bright Cyan
    scheme.ansi_palette[15] = wxColour(255, 255, 255); // Bright White

    return scheme;
}

TerminalPanel::TerminalPanel(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             core::EventBus& event_bus,
                             core::TerminalService& terminal_service)
    : ThemeAwareWindow(parent, theme_engine, wxID_ANY)
    , event_bus_(event_bus)
    , terminal_service_(terminal_service)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // Set up monospace font (Menlo on macOS)
    terminal_font_ = wxFont(kDefaultFontSize,
                            wxFONTFAMILY_TELETYPE,
                            wxFONTSTYLE_NORMAL,
                            wxFONTWEIGHT_NORMAL,
                            false,
                            "Menlo");
    CalculateCellDimensions();
    BuildColorScheme();

    // Bind events
    Bind(wxEVT_PAINT, &TerminalPanel::OnPaint, this);
    Bind(wxEVT_KEY_DOWN, &TerminalPanel::OnKeyDown, this);
    Bind(wxEVT_CHAR, &TerminalPanel::OnChar, this);
    Bind(wxEVT_LEFT_DOWN, &TerminalPanel::OnMouseDown, this);
    Bind(wxEVT_MOTION, &TerminalPanel::OnMouseMove, this);
    Bind(wxEVT_LEFT_UP, &TerminalPanel::OnMouseUp, this);
    Bind(wxEVT_MOUSEWHEEL, &TerminalPanel::OnMouseWheel, this);
    Bind(wxEVT_SIZE, &TerminalPanel::OnSize, this);
    Bind(wxEVT_RIGHT_DOWN, [this](wxMouseEvent& /*evt*/) { ShowContextMenu(); });

    // Cursor blink timer
    cursor_blink_timer_.SetOwner(this);
    Bind(wxEVT_TIMER, &TerminalPanel::OnCursorBlink, this, cursor_blink_timer_.GetId());
    cursor_blink_timer_.Start(kCursorBlinkMs);

    // Resize debounce timer
    resize_debounce_timer_.SetOwner(this);
    Bind(wxEVT_TIMER, &TerminalPanel::OnResizeDebounce, this, resize_debounce_timer_.GetId());

    // Subscribe to terminal data output for repaint
    data_sub_ = event_bus_.subscribe<core::events::TerminalDataOutputEvent>(
        [this](const core::events::TerminalDataOutputEvent& evt)
        {
            if (evt.terminal_id == active_terminal_id_)
            {
                Refresh();
            }
        });

    destroyed_sub_ = event_bus_.subscribe<core::events::TerminalDestroyedEvent>(
        [this](const core::events::TerminalDestroyedEvent& evt)
        {
            if (evt.terminal_id == active_terminal_id_)
            {
                Refresh();
            }
        });

    SetFocus();
}

void TerminalPanel::SetActiveTerminal(int terminal_id)
{
    active_terminal_id_ = terminal_id;
    scroll_offset_ = 0;
    Refresh();
}

auto TerminalPanel::active_terminal_id() const -> int
{
    return active_terminal_id_;
}

void TerminalPanel::ScrollToBottom()
{
    scroll_offset_ = 0;
    Refresh();
}

void TerminalPanel::ScrollUp(int lines)
{
    auto* buf = terminal_service_.get_buffer(active_terminal_id_);
    if (buf == nullptr)
    {
        return;
    }
    scroll_offset_ = std::min(scroll_offset_ + lines, buf->scrollback_lines());
    Refresh();
}

void TerminalPanel::ScrollDown(int lines)
{
    scroll_offset_ = std::max(0, scroll_offset_ - lines);
    Refresh();
}

void TerminalPanel::SelectAll()
{
    auto* buf = terminal_service_.get_buffer(active_terminal_id_);
    if (buf == nullptr)
    {
        return;
    }
    sel_start_row_ = 0;
    sel_start_col_ = 0;
    sel_end_row_ = buf->rows() - 1;
    sel_end_col_ = buf->cols() - 1;
    Refresh();
}

void TerminalPanel::CopySelection()
{
    auto* buf = terminal_service_.get_buffer(active_terminal_id_);
    if (buf == nullptr)
    {
        return;
    }

    const std::string text =
        buf->get_selection_text(sel_start_row_, sel_start_col_, sel_end_row_, sel_end_col_);

    if (!text.empty() && wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(text));
        wxTheClipboard->Close();
    }
}

void TerminalPanel::PasteClipboard()
{
    if (!wxTheClipboard->Open())
    {
        return;
    }

    wxTextDataObject clipboard_data;
    if (wxTheClipboard->GetData(clipboard_data))
    {
        const std::string text = clipboard_data.GetText().ToStdString();
        terminal_service_.send_text(active_terminal_id_, text);
    }
    wxTheClipboard->Close();
}

void TerminalPanel::ClearTerminal()
{
    auto* buf = terminal_service_.get_buffer(active_terminal_id_);
    if (buf != nullptr)
    {
        buf->clear_screen();
        buf->clear_scrollback();
        scroll_offset_ = 0;
        Refresh();
    }
}

void TerminalPanel::SoftClear()
{
    terminal_service_.send_text(active_terminal_id_, "\x1b[2J\x1b[H");
}

void TerminalPanel::ClearScrollback()
{
    auto* buf = terminal_service_.get_buffer(active_terminal_id_);
    if (buf != nullptr)
    {
        buf->clear_scrollback();
        scroll_offset_ = 0;
        Refresh();
    }
}

void TerminalPanel::FindInTerminal(const std::string& query)
{
    find_query_ = query;
    find_matches_.clear();
    find_current_index_ = -1;

    auto* buf = terminal_service_.get_buffer(active_terminal_id_);
    if (buf == nullptr || query.empty())
    {
        Refresh();
        return;
    }

    // Search all visible lines
    for (int row = 0; row < buf->rows(); ++row)
    {
        const std::string line_text = buf->get_line_text(row);
        std::size_t pos = 0;
        while ((pos = line_text.find(query, pos)) != std::string::npos)
        {
            find_matches_.emplace_back(row, static_cast<int>(pos));
            pos += query.size();
        }
    }

    if (!find_matches_.empty())
    {
        find_current_index_ = 0;
    }
    Refresh();
}

void TerminalPanel::FindNext()
{
    if (find_matches_.empty())
    {
        return;
    }
    find_current_index_ = (find_current_index_ + 1) % static_cast<int>(find_matches_.size());
    Refresh();
}

void TerminalPanel::FindPrevious()
{
    if (find_matches_.empty())
    {
        return;
    }
    find_current_index_ = (find_current_index_ - 1 + static_cast<int>(find_matches_.size())) %
                          static_cast<int>(find_matches_.size());
    Refresh();
}

void TerminalPanel::ShowContextMenu()
{
    wxMenu menu;

    constexpr int kIdCopy = 10001;
    constexpr int kIdPaste = 10002;
    constexpr int kIdSelectAll = 10003;
    constexpr int kIdClear = 10004;

    menu.Append(kIdCopy, "Copy\tCmd+C");
    menu.Append(kIdPaste, "Paste\tCmd+V");
    menu.AppendSeparator();
    menu.Append(kIdSelectAll, "Select All\tCmd+A");
    menu.AppendSeparator();
    menu.Append(kIdClear, "Clear Terminal\tCmd+K");

    const bool has_selection = (sel_start_row_ != sel_end_row_ || sel_start_col_ != sel_end_col_);
    menu.Enable(kIdCopy, has_selection);

    menu.Bind(wxEVT_MENU,
              [this](wxCommandEvent& evt)
              {
                  switch (evt.GetId())
                  {
                      case 10001:
                          CopySelection();
                          break;
                      case 10002:
                          PasteClipboard();
                          break;
                      case 10003:
                          SelectAll();
                          break;
                      case 10004:
                          ClearTerminal();
                          break;
                      default:
                          break;
                  }
              });

    PopupMenu(&menu);
}

void TerminalPanel::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    BuildColorScheme();
    Refresh();
}

void TerminalPanel::OnPaint(wxPaintEvent& /*event*/)
{
    wxBufferedPaintDC device_context(this);
    device_context.SetFont(terminal_font_);

    // Fill background
    device_context.SetBackground(wxBrush(color_scheme_.background));
    device_context.Clear();

    auto* buf = terminal_service_.get_buffer(active_terminal_id_);
    if (buf == nullptr)
    {
        device_context.SetTextForeground(color_scheme_.foreground);
        device_context.DrawText("No active terminal", 10, 10);
        return;
    }

    // Render buffer cells
    for (int row = 0; row < buf->rows(); ++row)
    {
        for (int col = 0; col < buf->cols(); ++col)
        {
            const auto& cell = buf->cell_at(row, col);
            const int pixel_x = col * char_width_;
            const int pixel_y = row * char_height_;

            // Background
            const wxColour bg_color = MapAnsiColorToWx(cell.attributes.background, false);
            if (bg_color != color_scheme_.background)
            {
                device_context.SetBrush(wxBrush(bg_color));
                device_context.SetPen(*wxTRANSPARENT_PEN);
                device_context.DrawRectangle(pixel_x, pixel_y, char_width_, char_height_);
            }

            // Text
            if (cell.character != U' ')
            {
                const wxColour fg_color = MapAnsiColorToWx(cell.attributes.foreground, true);
                device_context.SetTextForeground(fg_color);

                wxFont cell_font = terminal_font_;
                if (cell.attributes.bold)
                {
                    cell_font.SetWeight(wxFONTWEIGHT_BOLD);
                }
                if (cell.attributes.italic)
                {
                    cell_font.SetStyle(wxFONTSTYLE_ITALIC);
                }
                device_context.SetFont(cell_font);

                const wxString char_str(static_cast<wchar_t>(cell.character));
                device_context.DrawText(char_str, pixel_x, pixel_y);

                if (cell.attributes.underline)
                {
                    device_context.SetPen(wxPen(fg_color));
                    device_context.DrawLine(pixel_x,
                                            pixel_y + char_height_ - 1,
                                            pixel_x + char_width_,
                                            pixel_y + char_height_ - 1);
                }

                // Restore base font
                device_context.SetFont(terminal_font_);
            }
        }
    }

    // Draw cursor
    if (cursor_visible_)
    {
        const int cursor_x = buf->cursor_col() * char_width_;
        const int cursor_y = buf->cursor_row() * char_height_;
        device_context.SetBrush(*wxTRANSPARENT_BRUSH);
        device_context.SetPen(wxPen(color_scheme_.cursor, 2));
        device_context.DrawRectangle(cursor_x, cursor_y, char_width_, char_height_);
    }
}

void TerminalPanel::OnKeyDown(wxKeyEvent& event)
{
    const bool cmd_down = event.CmdDown();
    const int key_code = event.GetKeyCode();

    if (cmd_down)
    {
        const bool has_selection =
            (sel_start_row_ != sel_end_row_ || sel_start_col_ != sel_end_col_);
        if (key_code == 'C')
        {
            if (has_selection)
            {
                CopySelection();
            }
            else
            {
                // Send Ctrl+C interrupt
                terminal_service_.send_text(active_terminal_id_, "\x03");
            }
            return;
        }
        if (key_code == 'V')
        {
            PasteClipboard();
            return;
        }
        if (key_code == 'A')
        {
            SelectAll();
            return;
        }
        if (key_code == 'K')
        {
            ClearTerminal();
            return;
        }
    }

    event.Skip();
}

void TerminalPanel::OnChar(wxKeyEvent& event)
{
    const auto unicode_key = event.GetUnicodeKey();
    if (unicode_key != WXK_NONE && unicode_key >= 32)
    {
        const wxString key_str(static_cast<wchar_t>(unicode_key));
        terminal_service_.send_text(active_terminal_id_, key_str.ToStdString());
        return;
    }

    // Handle special keys
    const int key_code = event.GetKeyCode();
    switch (key_code)
    {
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            terminal_service_.send_text(active_terminal_id_, "\r");
            break;
        case WXK_BACK:
            terminal_service_.send_text(active_terminal_id_, "\x7f");
            break;
        case WXK_TAB:
            terminal_service_.send_text(active_terminal_id_, "\t");
            break;
        case WXK_ESCAPE:
            terminal_service_.send_text(active_terminal_id_, "\x1b");
            break;
        case WXK_UP:
            terminal_service_.send_text(active_terminal_id_, "\x1b[A");
            break;
        case WXK_DOWN:
            terminal_service_.send_text(active_terminal_id_, "\x1b[B");
            break;
        case WXK_RIGHT:
            terminal_service_.send_text(active_terminal_id_, "\x1b[C");
            break;
        case WXK_LEFT:
            terminal_service_.send_text(active_terminal_id_, "\x1b[D");
            break;
        case WXK_HOME:
            terminal_service_.send_text(active_terminal_id_, "\x1b[H");
            break;
        case WXK_END:
            terminal_service_.send_text(active_terminal_id_, "\x1b[F");
            break;
        case WXK_DELETE:
            terminal_service_.send_text(active_terminal_id_, "\x1b[3~");
            break;
        default:
            if (key_code >= 1 && key_code <= 26)
            {
                // Ctrl+letter
                const std::string ctrl_char(1, static_cast<char>(key_code));
                terminal_service_.send_text(active_terminal_id_, ctrl_char);
            }
            break;
    }
}

void TerminalPanel::OnMouseDown(wxMouseEvent& event)
{
    SetFocus();
    auto [row, col] = ScreenToCell(event.GetPosition());
    sel_start_row_ = row;
    sel_start_col_ = col;
    sel_end_row_ = row;
    sel_end_col_ = col;
    selecting_ = true;
    CaptureMouse();
}

void TerminalPanel::OnMouseMove(wxMouseEvent& event)
{
    if (!selecting_)
    {
        return;
    }
    auto [row, col] = ScreenToCell(event.GetPosition());
    sel_end_row_ = row;
    sel_end_col_ = col;
    Refresh();
}

void TerminalPanel::OnMouseUp(wxMouseEvent& /*event*/)
{
    if (selecting_)
    {
        selecting_ = false;
        if (HasCapture())
        {
            ReleaseMouse();
        }
    }
}

void TerminalPanel::OnMouseWheel(wxMouseEvent& event)
{
    const int delta = event.GetWheelRotation();
    if (delta > 0)
    {
        ScrollUp(3);
    }
    else
    {
        ScrollDown(3);
    }
}

void TerminalPanel::OnSize(wxSizeEvent& event)
{
    CalculateCellDimensions();
    if (char_width_ > 0 && char_height_ > 0)
    {
        const wxSize client_size = GetClientSize();
        pending_cols_ = std::max(client_size.GetWidth() / char_width_, kMinCols);
        pending_rows_ = std::max(client_size.GetHeight() / char_height_, kMinRows);
        resize_debounce_timer_.StartOnce(kResizeDebounceMs);
    }
    event.Skip();
}

void TerminalPanel::OnCursorBlink(wxTimerEvent& /*event*/)
{
    cursor_visible_ = !cursor_visible_;
    Refresh();
}

void TerminalPanel::OnResizeDebounce(wxTimerEvent& /*event*/)
{
    terminal_service_.resize_terminal(active_terminal_id_, pending_cols_, pending_rows_);
    Refresh();
}

void TerminalPanel::CalculateCellDimensions()
{
    wxClientDC device_context(this);
    device_context.SetFont(terminal_font_);
    const wxSize char_size = device_context.GetTextExtent("M");
    char_width_ = char_size.GetWidth();
    char_height_ = static_cast<int>(static_cast<double>(char_size.GetHeight()) * 1.2);
}

void TerminalPanel::CalculateTerminalSize()
{
    const wxSize client_size = GetClientSize();
    if (char_width_ > 0 && char_height_ > 0)
    {
        pending_cols_ = std::max(client_size.GetWidth() / char_width_, kMinCols);
        pending_rows_ = std::max(client_size.GetHeight() / char_height_, kMinRows);
    }
}

auto TerminalPanel::ScreenToCell(const wxPoint& point) -> std::pair<int, int>
{
    if (char_width_ <= 0 || char_height_ <= 0)
    {
        return {0, 0};
    }
    const int col = std::max(0, point.x / char_width_);
    const int row = std::max(0, point.y / char_height_);
    return {row, col};
}

auto TerminalPanel::MapAnsiColorToWx(const core::AnsiColor& color, bool is_foreground) -> wxColour
{
    using Type = core::AnsiColor::Type;

    switch (color.type)
    {
        case Type::kDefault:
            return is_foreground ? color_scheme_.foreground : color_scheme_.background;

        case Type::kStandard:
            if (color.index < 8)
            {
                return color_scheme_.ansi_palette[color.index];
            }
            return is_foreground ? color_scheme_.foreground : color_scheme_.background;

        case Type::kBright:
            if (color.index < 8)
            {
                return color_scheme_.ansi_palette[color.index + 8];
            }
            return is_foreground ? color_scheme_.foreground : color_scheme_.background;

        case Type::k256:
        {
            if (color.index < 16)
            {
                return color_scheme_.ansi_palette[color.index];
            }
            if (color.index < 232)
            {
                // 216-color cube
                const int idx = color.index - 16;
                const int red_val = (idx / 36) * 51;
                const int green_val = ((idx % 36) / 6) * 51;
                const int blue_val = (idx % 6) * 51;
                return {static_cast<unsigned char>(red_val),
                        static_cast<unsigned char>(green_val),
                        static_cast<unsigned char>(blue_val)};
            }
            // Grayscale ramp
            const int gray = (color.index - 232) * 10 + 8;
            return {static_cast<unsigned char>(gray),
                    static_cast<unsigned char>(gray),
                    static_cast<unsigned char>(gray)};
        }

        case Type::kRgb:
            return {color.red_channel, color.green_channel, color.blue_channel};
    }

    return is_foreground ? color_scheme_.foreground : color_scheme_.background;
}

void TerminalPanel::BuildColorScheme()
{
    color_scheme_ = BuildTerminalColorScheme(theme());
}

} // namespace markamp::ui
