#include "ShortcutOverlay.h"

#include <wx/dcbuffer.h>

#include <algorithm>

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

    Bind(wxEVT_PAINT, &ShortcutOverlay::OnPaint, this);
    Bind(wxEVT_CHAR_HOOK, &ShortcutOverlay::OnKeyDown, this);

    // Start hidden
    wxWindow::Hide();
}

void ShortcutOverlay::ShowOverlay()
{
    BuildCategories();
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

    wxWindow::Show();
    Raise();
    SetFocus();
    Refresh();
}

void ShortcutOverlay::HideOverlay()
{
    is_visible_ = false;
    wxWindow::Hide();
}

void ShortcutOverlay::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    if (is_visible_)
    {
        Refresh();
    }
}

void ShortcutOverlay::BuildCategories()
{
    categories_.clear();

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

void ShortcutOverlay::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    auto client_size = GetClientSize();
    const int width = client_size.GetWidth();
    const int height = client_size.GetHeight();

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

    // Separator
    paint_dc.SetPen(theme_engine().pen(core::ThemeColorToken::BorderLight, 1));
    paint_dc.DrawLine(kPadding, current_y, width - kPadding, current_y);
    current_y += 16;

    // Categories in two columns
    paint_dc.SetFont(theme_engine().font(core::ThemeFontToken::UISmall));

    const int col_width = (width - kPadding * 3) / 2;
    const int col1_x = kPadding;
    const int col2_x = kPadding + col_width + kPadding;
    int col1_y = current_y;
    int col2_y = current_y;
    bool use_col2 = false;

    for (const auto& category : categories_)
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

            // Shortcut key
            paint_dc.SetTextForeground(
                theme_engine().color(core::ThemeColorToken::AccentSecondary));
            paint_dc.DrawText(shortcut_text, target_x, target_y);

            // Description
            paint_dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
            paint_dc.DrawText(description, target_x + 120, target_y);

            target_y += 18;
        }

        target_y += 12; // Gap between categories
        use_col2 = !use_col2;
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
