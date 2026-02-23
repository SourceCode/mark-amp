#include "StatusBarPanel.h"

#include "ComponentSizeResolver.h"
#include "FileTypeIconResolver.h"
#include "IconManager.h"
#include "LayoutMetrics.h"
#include "SpacingGrid.h"
#include "TooltipWindow.h"
#include "TypographyScale.h"
#include "core/Events.h"
#include "core/Logger.h"

#include <wx/app.h>
#include <wx/dcbuffer.h>

#include <array>
#include <cctype>
#include <fmt/format.h>
#include <sstream>

namespace markamp::ui
{

StatusBarPanel::StatusBarPanel(wxWindow* parent,
                               DesignSystemContext& context,
                               core::EventBus& event_bus)
    : ThemeAwareWindow(parent,
                       context.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxSize(-1, context.metrics.status_bar_height()),
                       wxNO_BORDER)
    , ds_(context)
    , event_bus_(event_bus)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    UpdateLayoutMetrics();

    density_sub_ = event_bus_.subscribe<core::events::DensityProfileChangedEvent>(
        [this](const core::events::DensityProfileChangedEvent& /*evt*/) { UpdateLayoutMetrics(); });

    theme_name_sub_ = ds_.theme.subscribe_theme_change(
        [this](const std::string& /*theme_id*/)
        {
            theme_name_ = ds_.theme.current_theme().name;
            RebuildItems();
            Refresh();
        });

    cursor_sub_ = event_bus_.subscribe<core::events::CursorPositionChangedEvent>(
        [this](const core::events::CursorPositionChangedEvent& evt)
        { set_cursor_position(evt.line, evt.column); });

    content_sub_ = event_bus_.subscribe<core::events::EditorStatsChangedEvent>(
        [this](const core::events::EditorStatsChangedEvent& evt)
        { set_stats(evt.word_count, evt.char_count, evt.line_count, evt.selection_length); });

    view_mode_sub_ = event_bus_.subscribe<core::events::ViewModeChangedEvent>(
        [this](const core::events::ViewModeChangedEvent& evt) { set_view_mode(evt.mode); });

    encoding_sub_ = event_bus_.subscribe<core::events::FileEncodingDetectedEvent>(
        [this](const core::events::FileEncodingDetectedEvent& evt)
        {
            encoding_ = evt.encoding_name;
            RebuildItems();
            Refresh();
        });

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
    Bind(wxEVT_LEAVE_WINDOW, &StatusBarPanel::OnMouseLeave, this);

    tooltip_delay_timer_.SetOwner(this);
    tooltip_delay_timer_.Bind(
        wxEVT_TIMER,
        [this](wxTimerEvent& /*evt*/)
        {
            tooltip_delay_timer_.Stop();
            if (pending_tooltip_index_ >= 0)
            {
                auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_);
                if (pending_tooltip_is_left_ &&
                    pending_tooltip_index_ < static_cast<int>(left_items_.size()))
                {
                    const auto& item =
                        left_items_[static_cast<std::size_t>(pending_tooltip_index_)];
                    wxPoint pos =
                        ClientToScreen(wxPoint(item.bounds.GetX(), item.bounds.GetY() - 28));
                    tooltip->ShowTooltip(item.tooltip, pos);
                }
                else if (!pending_tooltip_is_left_ &&
                         pending_tooltip_index_ < static_cast<int>(right_items_.size()))
                {
                    const auto& item =
                        right_items_[static_cast<std::size_t>(pending_tooltip_index_)];
                    wxPoint pos =
                        ClientToScreen(wxPoint(item.bounds.GetX(), item.bounds.GetY() - 28));
                    tooltip->ShowTooltip(item.tooltip, pos);
                }
            }
        });

    // Phase 04: Animation configs
    animation::AnimationConfig flash_cfg;
    flash_cfg.duration = std::chrono::milliseconds(800);
    flash_cfg.easing_type = animation::EasingType::Linear;
    transition_manager_.register_transition("save_flash", flash_cfg);

    animation::AnimationConfig spinner_cfg;
    spinner_cfg.duration = std::chrono::milliseconds(640);
    spinner_cfg.easing_type = animation::EasingType::Linear;
    transition_manager_.register_transition("progress_spinner", spinner_cfg);

    // R17 Fix 8: Flash "SAVED" on save event
    save_sub_ = event_bus_.subscribe<core::events::TabSaveRequestEvent>(
        [this](const core::events::TabSaveRequestEvent& /*evt*/)
        {
            save_flash_active_ = true;
            ready_state_ = "SAVED \xE2\x9C\x93";
            RebuildItems();
            Refresh();

            transition_manager_.start<int>(
                "save_flash",
                0,
                1,
                [](int) {},
                [this]()
                {
                    save_flash_active_ = false;
                    ready_state_ = "READY";
                    RebuildItems();
                    Refresh();
                });
        });

    // Phase 06 Task 15: Subscribe to sidebar mode changes
    sidebar_mode_sub_ = event_bus_.subscribe<core::events::SidebarModeChangedEvent>(
        [this](const core::events::SidebarModeChangedEvent& evt)
        {
            sidebar_mode_name_ = sidebar_mode_label(evt.new_mode);
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
void StatusBarPanel::StartSpinnerCycle()
{
    if (!progress_active_)
        return;

    transition_manager_.start<int>(
        "progress_spinner",
        0,
        8,
        [this](int frame)
        {
            if (!progress_active_)
                return;
            if (frame >= 8)
                frame = 0;
            if (spinner_frame_ != frame)
            {
                spinner_frame_ = frame;
                RebuildItems();
                Refresh();
            }
        },
        [this]()
        {
            if (progress_active_)
            {
                StartSpinnerCycle();
            }
        });
}

void StatusBarPanel::set_progress(bool active, const std::string& label)
{
    bool was_active = progress_active_;
    progress_active_ = active;
    progress_label_ = label;

    if (active && !was_active)
    {
        spinner_frame_ = 0;
        StartSpinnerCycle();
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
void StatusBarPanel::RebuildItems()
{
    left_items_.clear();
    right_items_.clear();

    // Left zone: READY [●] • LN X, COL Y • UTF-8 • SRC/SPLIT/VIEW
    std::string ready_text = ready_state_;
    bool has_warning_dot = false;
    if (file_modified_)
    {
        ready_text += " \xE2\x97\x8F"; // UTF-8 for ● (black circle / modified indicator)
        has_warning_dot = true;
    }
    StatusItem state_item{ready_text, {}, file_modified_, false, nullptr, "Editor status"};
    state_item.is_success = save_flash_active_;
    state_item.has_warning_dot = has_warning_dot;
    left_items_.push_back(state_item);

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
                           "Click to go to line",
                           false,
                           false,
                           ""});

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
         "Click to change encoding",
         false,
         false,
         ""});

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
             "Click to change line ending",
             false,
             false,
             ""});
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
         "Click to change indentation",
         false,
         false,
         ""});

    // R13: Zoom indicator
    {
        auto zoom_text = fmt::format("Zoom: {}%", 100 + (zoom_level_ * 10));
        left_items_.push_back(
            {zoom_text, {}, false, false, nullptr, "Current zoom level", false, false, ""});
    }

    left_items_.push_back({view_mode_label(view_mode_),
                           {},
                           false,
                           false,
                           nullptr,
                           "Current view mode",
                           false,
                           false,
                           ""});

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
        left_items_.push_back(
            {spin_text, {}, true, false, nullptr, "Background operation", false, false, ""});
    }

    // R18 Fix 13: Git branch
    if (!git_branch_.empty())
    {
        auto branch_text = git_branch_;
        // Phase 02 Task 18: git branch icon
        left_items_.push_back({branch_text,
                               {},
                               false,
                               false,
                               nullptr,
                               "Current git branch",
                               false,
                               false,
                               "status-git"});
    }

    // Phase 06 Task 15: Sidebar mode indicator
    left_items_.push_back({sidebar_mode_name_,
                           {},
                           false,
                           true,
                           [this]()
                           {
                               core::events::SidebarToggleEvent toggle_evt;
                               event_bus_.publish(toggle_evt);
                           },
                           "Active sidebar panel — click to toggle",
                           false,
                           false,
                           ""});

    // Right zone: {N} WORDS • {M} CHARS • SEL: {LEN} • MERMAID: {STATUS} • Theme Name
    if (word_count_ > 0)
    {
        auto words_text = fmt::format("{} WORDS", word_count_);
        right_items_.push_back(
            {words_text, {}, false, false, nullptr, "Total word count", false, false, ""});

        // R20 Fix 13: Reading time estimate (~N min read at 200 WPM)
        int reading_minutes = std::max(1, word_count_ / 200);
        auto read_time_text = fmt::format("~{} min read", reading_minutes);
        right_items_.push_back({read_time_text,
                                {},
                                false,
                                false,
                                nullptr,
                                "Estimated reading time",
                                false,
                                false,
                                ""});
    }

    if (char_count_ > 0)
    {
        auto chars_text = fmt::format("{} CHARS", char_count_);
        right_items_.push_back(
            {chars_text, {}, false, false, nullptr, "Total character count", false, false, ""});
    }

    if (selection_len_ > 0)
    {
        // R18 Fix 14 + R20 Fix 11: Selection count badge with accent highlight
        auto sel_text = fmt::format("Sel: {} chars", selection_len_);
        right_items_.push_back(
            {sel_text, {}, true, false, nullptr, "Selected text length", false, false, ""});
    }

    auto mermaid_text = fmt::format("MERMAID: {}", mermaid_status_);
    bool mermaid_is_accent = mermaid_active_;
    right_items_.push_back({mermaid_text,
                            {},
                            mermaid_is_accent,
                            false,
                            nullptr,
                            "Mermaid diagram status",
                            false,
                            false,
                            ""});

    right_items_.push_back(
        {theme_name_, {}, false, false, nullptr, "Active theme", false, false, ""});

    // R2 Fix 13 + R20 Fix 15: Filename with modified dot indicator
    if (!filename_.empty())
    {
        std::string display_name = filename_;
        bool file_has_warning_dot = false;
        if (file_modified_)
        {
            display_name = "\xE2\x97\x8F " + display_name; // ● prefix when modified
            file_has_warning_dot = true;
        }
        StatusItem file_item{display_name, {}, file_modified_, false, nullptr, "Active file"};
        file_item.has_warning_dot = file_has_warning_dot;
        file_item.icon_name = FileTypeIconResolver::GetFileIcon(filename_);
        left_items_.push_back(file_item);
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
                                "Click to change language",
                                false,
                                false,
                                ""});
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
#pragma GCC diagnostic pop

void StatusBarPanel::UpdateLayoutMetrics()
{
    const int kHeight = ds_.metrics.status_bar_height();
    SetMinSize(wxSize(-1, kHeight));
    SetMaxSize(wxSize(-1, kHeight));
    if (GetParent() != nullptr)
    {
        GetParent()->Layout();
    }
    Refresh();
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
        auto base_col = theme_engine()
                            .resolve_token("statusBar.bg")
                            .value_or(theme_engine().color(core::ThemeColorToken::BgPanel));
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
        auto border_col = theme_engine()
                              .resolve_token("statusBar.border")
                              .value_or(theme_engine().color(core::ThemeColorToken::BorderLight));
        dc.SetPen(wxPen(wxColour(border_col.Red(), border_col.Green(), border_col.Blue(), 153), 1));
        dc.DrawLine(0, 0, width, 0);
    }

    auto small_font = ds_.typography.font(TypeSlot::kCaption);
    dc.SetFont(small_font);

    const int kPadding = ds_.spacing.scaled(SpacingToken::kLg); // ~16px
    const int text_y = (height - dc.GetCharHeight()) / 2 + 1;   // 26. Vertical Centering (+1px)
    const int kSeparatorGap = ds_.spacing.scaled(SpacingToken::kXxl); // ~24px

    // Separator character
    const wxString separator = wxString::FromUTF8("\xE2\x80\xA2"); // • (bullet)
    const int separator_width = dc.GetTextExtent(separator).GetWidth();

    // --- Left section ---
    int left_x = kPadding;

    for (size_t idx = 0; idx < left_items_.size(); ++idx)
    {
        auto& item = left_items_[idx];

        // Draw separator before item (except the first)
        if (idx > 0)
        {
            dc.SetTextForeground(
                theme_engine()
                    .resolve_token("statusBar.fg")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMuted)));
            dc.DrawText(separator, left_x, text_y);
            left_x += separator_width + kSeparatorGap;
        }

        // R16 Fix 14: bold for accent items
        if (item.is_accent)
        {
            wxFont bold_font = ds_.typography.font(TypeSlot::kCaption);
            bold_font.SetWeight(wxFONTWEIGHT_SEMIBOLD);
            dc.SetFont(bold_font);
        }

        if (item.is_success)
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::SuccessColor));
        }
        else
        {
            dc.SetTextForeground(
                item.is_accent
                    ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                    : theme_engine()
                          .resolve_token("statusBar.fg")
                          .value_or(theme_engine().color(core::ThemeColorToken::TextMuted)));
        }

        int text_width = dc.GetTextExtent(item.text).GetWidth();
        int icon_size = 14;
        int icon_spacing = (!item.text.empty() && !item.icon_name.empty()) ? 4 : 0;
        if (item.text.empty() && !item.icon_name.empty())
        {
            icon_spacing = 0;
        }
        int item_width = text_width;
        if (!item.icon_name.empty())
        {
            item_width += icon_size + icon_spacing;
        }

        // 23. Label hover highlight (only if clickable)
        if (item.is_clickable && item.bounds.Contains(ScreenToClient(wxGetMousePosition())))
        {
            auto hover_bg =
                theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(115);
            dc.SetBrush(wxBrush(hover_bg));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRoundedRectangle(left_x - 4, 3, item_width + 8, height - 6, 3);
        }

        int current_x = left_x;
        wxColour current_fg = dc.GetTextForeground();

        // Draw icon
        if (!item.icon_name.empty())
        {
            int icon_y = (height - icon_size) / 2;
            IconManager::get().draw_icon(
                dc, item.icon_name, current_x, icon_y, wxSize(icon_size, icon_size), current_fg);
            current_x += icon_size + icon_spacing;
        }

        // Font reset
        dc.SetFont(small_font);

        // Draw string with warning dot coloring
        if (item.has_warning_dot && item.text.starts_with("\xE2\x97\x8F "))
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::EditorGutterWarn));
            dc.DrawText("\xE2\x97\x8F", current_x, text_y);
            int dot_w = dc.GetTextExtent("\xE2\x97\x8F ").GetWidth();

            dc.SetTextForeground(current_fg);
            dc.DrawText(item.text.substr(4), current_x + dot_w, text_y);
        }
        else if (item.has_warning_dot && item.text.ends_with(" \xE2\x97\x8F"))
        {
            std::string main_text = item.text.substr(0, item.text.length() - 4);
            int main_w = dc.GetTextExtent(main_text).GetWidth();
            dc.DrawText(main_text, current_x, text_y);

            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::EditorGutterWarn));
            dc.DrawText(" \xE2\x97\x8F", current_x + main_w, text_y);

            // Restore foreground
            dc.SetTextForeground(current_fg);
        }
        else if (!item.text.empty())
        {
            dc.DrawText(item.text, current_x, text_y);
        }

        item.bounds = wxRect(left_x, 0, item_width, height);

        // Reset font if we changed it
        if (item.is_accent)
        {
            dc.SetFont(small_font);
        }

        left_x += item_width + kSeparatorGap;
    }

    // --- Right section ---
    int right_x = width - kPadding;

    for (auto it = right_items_.rbegin(); it != right_items_.rend(); ++it)
    {
        auto& item = *it;
        int text_width = dc.GetTextExtent(item.text).GetWidth();
        right_x -= text_width;

        // R16 Fix 14: bold for accent items (right section)
        if (item.is_accent)
        {
            wxFont bold_font = ds_.typography.font(TypeSlot::kCaption);
            bold_font.SetWeight(wxFONTWEIGHT_SEMIBOLD);
            dc.SetFont(bold_font);
        }

        if (item.is_success)
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::SuccessColor));
        }
        else
        {
            dc.SetTextForeground(
                item.is_accent
                    ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                    : theme_engine()
                          .resolve_token("statusBar.fg")
                          .value_or(theme_engine().color(core::ThemeColorToken::TextMuted)));
        }

        int icon_size = 14;
        int icon_spacing = (!item.text.empty() && !item.icon_name.empty()) ? 4 : 0;
        int item_width = text_width;
        if (!item.icon_name.empty())
        {
            item_width += icon_size + icon_spacing;
        }

        right_x -= item_width;

        // 23. Label hover highlight for right side (only if clickable)
        if (item.is_clickable && item.bounds.Contains(ScreenToClient(wxGetMousePosition())))
        {
            auto hover_bg =
                theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(115);
            dc.SetBrush(wxBrush(hover_bg));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRoundedRectangle(right_x - 4, 3, item_width + 8, height - 6, 3);
        }

        int current_x = right_x;
        wxColour current_fg = dc.GetTextForeground();

        if (!item.icon_name.empty())
        {
            int icon_y = (height - icon_size) / 2;
            IconManager::get().draw_icon(
                dc, item.icon_name, current_x, icon_y, wxSize(icon_size, icon_size), current_fg);
            current_x += icon_size + icon_spacing;
        }

        // Font reset
        dc.SetFont(small_font);

        if (item.has_warning_dot && item.text.starts_with("\xE2\x97\x8F "))
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::EditorGutterWarn));
            dc.DrawText("\xE2\x97\x8F", current_x, text_y);
            int dot_w = dc.GetTextExtent("\xE2\x97\x8F ").GetWidth();

            dc.SetTextForeground(current_fg);
            dc.DrawText(item.text.substr(4), current_x + dot_w, text_y);
        }
        else if (item.has_warning_dot && item.text.ends_with(" \xE2\x97\x8F"))
        {
            std::string main_text = item.text.substr(0, item.text.length() - 4);
            int main_w = dc.GetTextExtent(main_text).GetWidth();
            dc.DrawText(main_text, current_x, text_y);

            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::EditorGutterWarn));
            dc.DrawText(" \xE2\x97\x8F", current_x + main_w, text_y);
            dc.SetTextForeground(current_fg);
        }
        else if (!item.text.empty())
        {
            dc.DrawText(item.text, current_x, text_y);
        }

        item.bounds = wxRect(right_x, 0, item_width, height);

        // Reset font if we changed it
        if (item.is_accent)
        {
            dc.SetFont(small_font);
        }

        right_x -= kSeparatorGap;

        // Draw separator after each right item (except the last one, which is first in reverse)
        if (std::next(it) != right_items_.rend())
        {
            dc.SetTextForeground(
                theme_engine()
                    .resolve_token("statusBar.fg")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMuted)));
            right_x -= separator_width;
            dc.DrawText(separator, right_x, text_y);
            right_x -= kSeparatorGap;
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
    bool any_hovered = false;

    int new_hovered_index = -1;
    bool new_hovered_is_left = true;

    // Check left items
    for (size_t idx = 0; idx < left_items_.size(); ++idx)
    {
        const auto& item = left_items_[idx];
        if (item.bounds.Contains(pos))
        {
            any_hovered = true;
            new_hovered_index = static_cast<int>(idx);
            new_hovered_is_left = true;
            if (item.is_clickable)
            {
                over_clickable = true;
            }
            break;
        }
    }

    if (!any_hovered)
    {
        // Check right items
        for (size_t idx = 0; idx < right_items_.size(); ++idx)
        {
            const auto& item = right_items_[idx];
            if (item.bounds.Contains(pos))
            {
                any_hovered = true;
                new_hovered_index = static_cast<int>(idx);
                new_hovered_is_left = false;
                if (item.is_clickable)
                {
                    over_clickable = true;
                }
                break;
            }
        }
    }

    if (!any_hovered)
    {
        if (auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_))
        {
            tooltip->HideTooltip();
        }
        tooltip_delay_timer_.Stop();
        pending_tooltip_index_ = -1;
    }
    else if (pending_tooltip_index_ != new_hovered_index ||
             pending_tooltip_is_left_ != new_hovered_is_left)
    {
        if (auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_))
        {
            tooltip->HideTooltip();
        }
        pending_tooltip_index_ = new_hovered_index;
        pending_tooltip_is_left_ = new_hovered_is_left;
        tooltip_delay_timer_.StartOnce(400);
    }

    SetCursor(over_clickable ? wxCursor(wxCURSOR_HAND) : wxCursor(wxCURSOR_DEFAULT));
    event.Skip();
}

void StatusBarPanel::OnMouseLeave(wxMouseEvent& /*event*/)
{
    if (auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_))
    {
        tooltip->HideTooltip();
    }
    tooltip_delay_timer_.Stop();
    pending_tooltip_index_ = -1;
    SetCursor(wxCursor(wxCURSOR_DEFAULT));
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
        case core::events::ViewMode::LivePreview:
            return "LIVE";
    }
    return "SPLIT"; // fallback
}

auto StatusBarPanel::sidebar_mode_label(int mode) -> std::string
{
    switch (mode)
    {
        case 0:
            return "EXPLORER";
        case 1:
            return "SEARCH";
        case 2:
            return "SETTINGS";
        case 3:
            return "THEMES";
        case 4:
            return "EXTENSIONS";
        case 5:
            return "NOTEBOOKS";
        case 6:
            return "CANVAS";
        case 7:
            return "GRAPH";
        default:
            return "EXPLORER";
    }
}

} // namespace markamp::ui
