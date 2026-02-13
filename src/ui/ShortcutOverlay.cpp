#include "ShortcutOverlay.h"

#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/dcbuffer.h>

#include <algorithm>
#include <set>

namespace markamp::ui
{

ShortcutOverlay::ShortcutOverlay(wxWindow* parent,
                                 core::ThemeEngine& theme_engine,
                                 const core::ShortcutManager& shortcut_manager)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER)
    , shortcut_manager_(shortcut_manager)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // Phase 7B: create search filter input
    filter_input_ = new wxTextCtrl(this,
                                   wxID_ANY,
                                   wxEmptyString,
                                   wxDefaultPosition,
                                   wxSize(-1, 28),
                                   wxTE_PROCESS_ENTER | wxNO_BORDER);
    filter_input_->SetHint("Search shortcuts\u2026");
    filter_input_->SetBackgroundColour(
        theme_engine.color(core::ThemeColorToken::BgPanel).ChangeLightness(110));
    filter_input_->SetForegroundColour(theme_engine.color(core::ThemeColorToken::TextMain));
    filter_input_->SetFont(theme_engine.font(core::ThemeFontToken::MonoRegular));
    filter_input_->Hide();

    Bind(wxEVT_PAINT, &ShortcutOverlay::OnPaint, this);
    Bind(wxEVT_CHAR_HOOK, &ShortcutOverlay::OnKeyDown, this);
    filter_input_->Bind(wxEVT_TEXT, &ShortcutOverlay::OnFilterChanged, this);

    // R18 Fix 35: Click on a shortcut entry to copy it to clipboard
    Bind(wxEVT_LEFT_DOWN,
         [this](wxMouseEvent& mouse_evt)
         {
             const wxPoint click_pos = mouse_evt.GetPosition();
             // Hit-test against rendered shortcut positions
             for (const auto& [rect, shortcut_str] : shortcut_hit_rects_)
             {
                 if (rect.Contains(click_pos))
                 {
                     if (wxTheClipboard->Open())
                     {
                         wxTheClipboard->SetData(new wxTextDataObject(shortcut_str));
                         wxTheClipboard->Close();
                     }
                     copied_flash_text_ = shortcut_str;
                     copied_flash_timer_.Start(1500, wxTIMER_ONE_SHOT);
                     Refresh();
                     return;
                 }
             }
         });

    copied_flash_timer_.Bind(wxEVT_TIMER,
                             [this](wxTimerEvent&)
                             {
                                 copied_flash_text_.clear();
                                 Refresh();
                             });

    // Start hidden
    wxWindow::Hide();
}

void ShortcutOverlay::ShowOverlay()
{
    BuildCategories();
    filter_text_.clear();
    filter_input_->SetValue("");
    FilterCategories();
    is_visible_ = true;

    // Center on parent
    auto* parent_window = GetParent();
    if (parent_window != nullptr)
    {
        auto parent_size = parent_window->GetClientSize();
        const int overlay_width = std::min(parent_size.GetWidth() - 80, 700);
        const int overlay_height = std::min(parent_size.GetHeight() - 80, 500);
        const int pos_x = (parent_size.GetWidth() - overlay_width) / 2;
        const int pos_y = (parent_size.GetHeight() - overlay_height) / 2;
        SetSize(pos_x, pos_y, overlay_width, overlay_height);
    }

    // Position filter input at top of overlay
    constexpr int kFilterPad = 24;
    constexpr int kFilterTopY = 80;
    filter_input_->SetPosition(wxPoint(kFilterPad, kFilterTopY));
    filter_input_->SetSize(GetClientSize().GetWidth() - kFilterPad * 2, 28);
    filter_input_->Show();

    wxWindow::Show();
    Raise();
    filter_input_->SetFocus();
    Refresh();
}

void ShortcutOverlay::HideOverlay()
{
    is_visible_ = false;
    filter_input_->Hide();
    wxWindow::Hide();
}

void ShortcutOverlay::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);

    if (filter_input_ != nullptr)
    {
        filter_input_->SetBackgroundColour(
            theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(110));
        filter_input_->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMain));
    }

    if (is_visible_)
    {
        Refresh();
    }
}

void ShortcutOverlay::BuildCategories()
{
    categories_.clear();
    conflicting_shortcuts_.clear();

    // Collect unique category names preserving order
    std::vector<std::string> category_names;
    for (const auto& shortcut : shortcut_manager_.get_all_shortcuts())
    {
        if (std::find(category_names.begin(), category_names.end(), shortcut.category) ==
            category_names.end())
        {
            category_names.push_back(shortcut.category);
        }
    }

    // R18 Fix 36: Detect conflicting shortcuts (same key binding across all categories)
    std::map<std::string, int> binding_count;
    for (const auto& shortcut : shortcut_manager_.get_all_shortcuts())
    {
        auto text = core::ShortcutManager::format_shortcut(shortcut.key_code, shortcut.modifiers);
        binding_count[text]++;
    }
    for (const auto& [binding, count] : binding_count)
    {
        if (count > 1)
        {
            conflicting_shortcuts_.insert(binding);
        }
    }

    // Build each category
    for (const auto& category_name : category_names)
    {
        ShortcutCategory cat;
        cat.name = category_name;

        auto shortcuts = shortcut_manager_.get_shortcuts_for_category(category_name);
        for (const auto& shortcut : shortcuts)
        {
            auto text =
                core::ShortcutManager::format_shortcut(shortcut.key_code, shortcut.modifiers);
            cat.entries.emplace_back(text, shortcut.description);
        }

        if (!cat.entries.empty())
        {
            categories_.push_back(std::move(cat));
        }
    }
}

void ShortcutOverlay::FilterCategories()
{
    filtered_categories_.clear();

    if (filter_text_.empty())
    {
        filtered_categories_ = categories_;
        return;
    }

    // Case-insensitive filter text
    std::string lower_filter = filter_text_;
    std::transform(lower_filter.begin(),
                   lower_filter.end(),
                   lower_filter.begin(),
                   [](unsigned char char_val)
                   { return static_cast<char>(std::tolower(char_val)); });

    for (const auto& category : categories_)
    {
        ShortcutCategory filtered_cat;
        filtered_cat.name = category.name;

        for (const auto& [shortcut_text, description] : category.entries)
        {
            // Match against description or shortcut text (case-insensitive)
            std::string lower_desc = description;
            std::transform(lower_desc.begin(),
                           lower_desc.end(),
                           lower_desc.begin(),
                           [](unsigned char char_val)
                           { return static_cast<char>(std::tolower(char_val)); });

            std::string lower_shortcut = shortcut_text;
            std::transform(lower_shortcut.begin(),
                           lower_shortcut.end(),
                           lower_shortcut.begin(),
                           [](unsigned char char_val)
                           { return static_cast<char>(std::tolower(char_val)); });

            if (lower_desc.find(lower_filter) != std::string::npos ||
                lower_shortcut.find(lower_filter) != std::string::npos)
            {
                filtered_cat.entries.emplace_back(shortcut_text, description);
            }
        }

        if (!filtered_cat.entries.empty())
        {
            filtered_categories_.push_back(std::move(filtered_cat));
        }
    }
}

void ShortcutOverlay::OnFilterChanged(wxCommandEvent& /*event*/)
{
    filter_text_ = filter_input_->GetValue().ToStdString();
    FilterCategories();
    Refresh();
}

void ShortcutOverlay::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    auto client_size = GetClientSize();
    const int width = client_size.GetWidth();
    const int height = client_size.GetHeight();

    // 8A: Drop shadow behind overlay (8px graduated)
    {
        constexpr int kShadowSize = 8;
        for (int band = kShadowSize; band > 0; --band)
        {
            auto alpha = static_cast<unsigned char>((kShadowSize - band + 1) * 8);
            paint_dc.SetPen(wxPen(wxColour(0, 0, 0, alpha), 1));
            paint_dc.SetBrush(*wxTRANSPARENT_BRUSH);
            paint_dc.DrawRoundedRectangle(
                -band, -band, width + band * 2, height + band * 2, 8 + band);
        }
    }

    // Background with rounded rectangle
    paint_dc.SetBrush(theme_engine().brush(core::ThemeColorToken::BgPanel));
    paint_dc.SetPen(theme_engine().pen(core::ThemeColorToken::BorderLight, 2));
    paint_dc.DrawRoundedRectangle(0, 0, width, height, 8);

    // Title
    paint_dc.SetFont(theme_engine().font(core::ThemeFontToken::UIHeading));
    paint_dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMain));

    constexpr int kPadding = 24;
    int current_y = kPadding;

    paint_dc.DrawText("KEYBOARD SHORTCUTS", kPadding, current_y);
    current_y += 32;

    paint_dc.DrawText("Press F1 or Escape to close", kPadding, current_y);
    current_y += 24;

    // Filter input sits here (positioned as child widget at y=80)
    current_y += 36; // Skip past the filter input area

    // Separator
    paint_dc.SetPen(theme_engine().pen(core::ThemeColorToken::BorderLight, 1));
    paint_dc.DrawLine(kPadding, current_y, width - kPadding, current_y);
    current_y += 16;

    // Categories in two columns (use filtered_categories_ for search support)
    paint_dc.SetFont(theme_engine().font(core::ThemeFontToken::UISmall));

    const int col_width = (width - kPadding * 3) / 2;
    const int col1_x = kPadding;
    const int col2_x = kPadding + col_width + kPadding;
    int col1_y = current_y;
    int col2_y = current_y;
    bool use_col2 = false;
    shortcut_hit_rects_.clear(); // R18 Fix 35: rebuild hit rects

    for (const auto& category : filtered_categories_)
    {
        int& target_y = use_col2 ? col2_y : col1_y;
        const int target_x = use_col2 ? col2_x : col1_x;

        // Category header
        paint_dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::AccentPrimary));
        paint_dc.SetFont(theme_engine().font(core::ThemeFontToken::UIHeading));
        paint_dc.DrawText(category.name, target_x, target_y);
        target_y += 22;

        paint_dc.SetFont(theme_engine().font(core::ThemeFontToken::UISmall));

        for (const auto& [shortcut_text, description] : category.entries)
        {
            if (target_y + 18 > height - kPadding)
            {
                break; // Don't overflow
            }

            // R18 Fix 36: Highlight conflicting shortcuts with warning background
            if (conflicting_shortcuts_.count(shortcut_text) > 0)
            {
                paint_dc.SetBrush(wxBrush(wxColour(200, 150, 0, 40)));
                paint_dc.SetPen(*wxTRANSPARENT_PEN);
                paint_dc.DrawRectangle(target_x - 2, target_y - 1, col_width, 16);
            }

            // Shortcut key
            paint_dc.SetTextForeground(
                theme_engine().color(core::ThemeColorToken::AccentSecondary));
            paint_dc.DrawText(shortcut_text, target_x, target_y);

            // Description
            paint_dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
            paint_dc.DrawText(description, target_x + 120, target_y);

            // R18 Fix 35: Store hit rect for click-to-copy
            shortcut_hit_rects_.emplace_back(wxRect(target_x, target_y, col_width, 16),
                                             shortcut_text);

            // R18 Fix 35: Show "Copied!" flash next to the shortcut
            if (!copied_flash_text_.empty() && copied_flash_text_ == shortcut_text)
            {
                paint_dc.SetTextForeground(
                    theme_engine().color(core::ThemeColorToken::AccentPrimary));
                paint_dc.DrawText("Copied!", target_x + col_width - 60, target_y);
            }

            target_y += 16; // 8E: was 18
        }

        target_y += 16; // 8E: was 12, gap between categories
        use_col2 = !use_col2;
    }

    // Show "No matches" if filtering returned nothing
    if (filtered_categories_.empty() && !filter_text_.empty())
    {
        paint_dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
        paint_dc.SetFont(theme_engine().font(core::ThemeFontToken::UISmall));
        paint_dc.DrawText("No shortcuts match your search.", kPadding, current_y);
    }
}

void ShortcutOverlay::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_ESCAPE || event.GetKeyCode() == WXK_F1)
    {
        HideOverlay();
        return;
    }
    event.Skip();
}

} // namespace markamp::ui
