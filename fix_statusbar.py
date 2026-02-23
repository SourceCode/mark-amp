import re

with open('src/ui/StatusBarPanel.cpp', 'r') as f:
    content = f.read()

# Chunk 1: Includes
old_includes = """#include "StatusBarPanel.h"

#include "ComponentSizeResolver.h"
#include "LayoutMetrics.h"
#include "SpacingGrid.h"
#include "TypographyScale.h"
#include "core/Events.h"
#include "core/Logger.h"
"""
new_includes = """#include "StatusBarPanel.h"

#include "ComponentSizeResolver.h"
#include "FileTypeIconResolver.h"
#include "IconManager.h"
#include "LayoutMetrics.h"
#include "SpacingGrid.h"
#include "TypographyScale.h"
#include "core/Events.h"
#include "core/Logger.h"
"""
if old_includes in content:
    content = content.replace(old_includes, new_includes)

# Chunk 2: FileTypeIconResolver
old_file_icon = """        // The default filetype icon
        file_item.icon_name = "filetype-default";"""
new_file_icon = """        file_item.icon_name = FileTypeIconResolver::GetFileIcon(filename_);"""
if old_file_icon in content:
    content = content.replace(old_file_icon, new_file_icon)

# Chunk 3: OnPaint Left Items
old_left_items = """        // R16 Fix 14: bold for accent items
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
            dc.SetTextForeground(item.is_accent
                                     ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                                     : theme_engine().color(core::ThemeColorToken::TextMuted));
        }

        int text_width = dc.GetTextExtent(item.text).GetWidth();

        // Font reset
        dc.SetFont(small_font);

        // Draw string with warning dot coloring
        if (item.has_warning_dot && item.text.starts_with("\\xE2\\x97\\x8F "))
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::EditorGutterWarn));
            dc.DrawText("\\xE2\\x97\\x8F", left_x, text_y);
            int dot_w = dc.GetTextExtent("\\xE2\\x97\\x8F ").GetWidth();

            if (item.is_success)
            {
                dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::SuccessColor));
            }
            else
            {
                dc.SetTextForeground(
                    item.is_accent ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                                   : theme_engine().color(core::ThemeColorToken::TextMuted));
            }
            dc.DrawText(item.text.substr(4), left_x + dot_w, text_y);
        }
        else if (item.has_warning_dot && item.text.ends_with(" \\xE2\\x97\\x8F"))
        {
            std::string main_text = item.text.substr(0, item.text.length() - 4);
            int main_w = dc.GetTextExtent(main_text).GetWidth();
            dc.DrawText(main_text, left_x, text_y);

            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::EditorGutterWarn));
            dc.DrawText(" \\xE2\\x97\\x8F", left_x + main_w, text_y);
        }
        else
        {
            dc.DrawText(item.text, left_x, text_y);
        }
        // 23. Label hover highlight (only if clickable)
        if (item.is_clickable && item.bounds.Contains(ScreenToClient(wxGetMousePosition())))
        {
            auto hover_bg =
                theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(115);
            dc.SetBrush(wxBrush(hover_bg));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRoundedRectangle(left_x - 4, 3, text_width + 8, height - 6, 3);
        }

        item.bounds = wxRect(left_x, 0, text_width, height);
        dc.DrawText(item.text, left_x, text_y);

        // Reset font if we changed it
        if (item.is_accent)
        {
            dc.SetFont(small_font);
        }

        left_x += text_width + kSeparatorGap;"""

new_left_items = """        // R16 Fix 14: bold for accent items
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
            dc.SetTextForeground(item.is_accent
                                     ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                                     : theme_engine().color(core::ThemeColorToken::TextMuted));
        }

        int text_width = dc.GetTextExtent(item.text).GetWidth();
        int icon_size = 14;
        int icon_spacing = (!item.text.empty() && !item.icon_name.empty()) ? 4 : 0;
        if (item.text.empty() && !item.icon_name.empty()) {
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
            IconManager::get().draw_icon(dc, item.icon_name, current_x, icon_y, wxSize(icon_size, icon_size), current_fg);
            current_x += icon_size + icon_spacing;
        }

        // Font reset
        dc.SetFont(small_font);

        // Draw string with warning dot coloring
        if (item.has_warning_dot && item.text.starts_with("\\xE2\\x97\\x8F "))
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::EditorGutterWarn));
            dc.DrawText("\\xE2\\x97\\x8F", current_x, text_y);
            int dot_w = dc.GetTextExtent("\\xE2\\x97\\x8F ").GetWidth();

            dc.SetTextForeground(current_fg);
            dc.DrawText(item.text.substr(4), current_x + dot_w, text_y);
        }
        else if (item.has_warning_dot && item.text.ends_with(" \\xE2\\x97\\x8F"))
        {
            std::string main_text = item.text.substr(0, item.text.length() - 4);
            int main_w = dc.GetTextExtent(main_text).GetWidth();
            dc.DrawText(main_text, current_x, text_y);

            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::EditorGutterWarn));
            dc.DrawText(" \\xE2\\x97\\x8F", current_x + main_w, text_y);
            
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

        left_x += item_width + kSeparatorGap;"""
if old_left_items in content:
    content = content.replace(old_left_items, new_left_items)
else:
    print("WARNING: Left items chunk not found!")

# Chunk 4: OnPaint Right Items
old_right_items = """        // R16 Fix 14: bold for accent items (right section)
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
            dc.SetTextForeground(item.is_accent
                                     ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                                     : theme_engine().color(core::ThemeColorToken::TextMuted));
        }

        // 23. Label hover highlight for right side (only if clickable)
        if (item.is_clickable && item.bounds.Contains(ScreenToClient(wxGetMousePosition())))
        {
            auto hover_bg =
                theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(115);
            dc.SetBrush(wxBrush(hover_bg));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRoundedRectangle(right_x - 4, 3, text_width + 8, height - 6, 3);
        }

        item.bounds = wxRect(right_x, 0, text_width, height);
        dc.DrawText(item.text, right_x, text_y);

        // Reset font if we changed it
        if (item.is_accent)
        {
            dc.SetFont(small_font);
        }

        right_x -= kSeparatorGap;

        // Draw separator after each right item (except the last one, which is first in reverse)
        if (std::next(it) != right_items_.rend())
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
            right_x -= separator_width;
            dc.DrawText(separator, right_x, text_y);
            right_x -= kSeparatorGap;
        }"""
        
new_right_items = """        // R16 Fix 14: bold for accent items (right section)
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
            dc.SetTextForeground(item.is_accent
                                     ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                                     : theme_engine().color(core::ThemeColorToken::TextMuted));
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
            IconManager::get().draw_icon(dc, item.icon_name, current_x, icon_y, wxSize(icon_size, icon_size), current_fg);
            current_x += icon_size + icon_spacing;
        }

        // Font reset
        dc.SetFont(small_font);

        if (item.has_warning_dot && item.text.starts_with("\\xE2\\x97\\x8F "))
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::EditorGutterWarn));
            dc.DrawText("\\xE2\\x97\\x8F", current_x, text_y);
            int dot_w = dc.GetTextExtent("\\xE2\\x97\\x8F ").GetWidth();

            dc.SetTextForeground(current_fg);
            dc.DrawText(item.text.substr(4), current_x + dot_w, text_y);
        }
        else if (item.has_warning_dot && item.text.ends_with(" \\xE2\\x97\\x8F"))
        {
            std::string main_text = item.text.substr(0, item.text.length() - 4);
            int main_w = dc.GetTextExtent(main_text).GetWidth();
            dc.DrawText(main_text, current_x, text_y);

            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::EditorGutterWarn));
            dc.DrawText(" \\xE2\\x97\\x8F", current_x + main_w, text_y);
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
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
            right_x -= separator_width;
            dc.DrawText(separator, right_x, text_y);
            right_x -= kSeparatorGap;
        }"""
        
# wait, my original `old_right_items` is trying to replace code that depends on my memory, 
# let me look at `view_file` output from lines 732 to 780 to ensure the exact matching text.
# The original code looks like this:
exact_old_right_items = """        // R16 Fix 14: bold for accent items (right section)
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
            dc.SetTextForeground(item.is_accent
                                     ? theme_engine().color(core::ThemeColorToken::AccentPrimary)
                                     : theme_engine().color(core::ThemeColorToken::TextMuted));
        }

        // 23. Label hover highlight for right side (only if clickable)
        if (item.is_clickable && item.bounds.Contains(ScreenToClient(wxGetMousePosition())))
        {
            auto hover_bg =
                theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(115);
            dc.SetBrush(wxBrush(hover_bg));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRoundedRectangle(right_x - 4, 3, text_width + 8, height - 6, 3);
        }

        item.bounds = wxRect(right_x, 0, text_width, height);
        dc.DrawText(item.text, right_x, text_y);

        // Reset font if we changed it
        if (item.is_accent)
        {
            dc.SetFont(small_font);
        }

        right_x -= kSeparatorGap;

        // Draw separator after each right item (except the last one, which is first in reverse)
        if (std::next(it) != right_items_.rend())
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
            right_x -= separator_width;
            dc.DrawText(separator, right_x, text_y);
            right_x -= kSeparatorGap;
        }"""

if exact_old_right_items in content:
    content = content.replace(exact_old_right_items, new_right_items)
else:
    print("WARNING: Right items chunk not found!")

with open('src/ui/StatusBarPanel.cpp', 'w') as f:
    f.write(content)
