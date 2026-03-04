#include "FloatingFormatBar.h"

#include "core/Events.h"

namespace markamp::ui
{

FloatingFormatBar::FloatingFormatBar(wxWindow* parent,
                                     core::ThemeEngine& theme_engine,
                                     core::EventBus& event_bus,
                                     ActionCallback callback)
    : FloatingToolbar(parent, theme_engine, event_bus, "format_bar")
    , callback_(std::move(callback))
{
    SetDraggable(false);
    SetSnapToEdges(false);
    SetAutoHideMs(0); // Hidden when selection clears
    BuildButtons();
}

void FloatingFormatBar::ShowAboveSelection(const wxPoint& selection_start,
                                           const wxPoint& selection_end)
{
    // Center horizontally between selection endpoints, position above
    const int center_x = (selection_start.x + selection_end.x) / 2;
    const auto toolbar_size = FloatingToolbar::CalculateSize();
    const int toolbar_x = center_x - toolbar_size.x / 2;
    const int toolbar_y = selection_start.y - toolbar_size.y - 8;

    // If above would be off-screen, show below instead
    if (toolbar_y < 0)
    {
        ShowAt(wxPoint(toolbar_x, selection_end.y + 8));
    }
    else
    {
        ShowAt(wxPoint(toolbar_x, toolbar_y));
    }
}

void FloatingFormatBar::BuildButtons()
{
    std::vector<FloatingToolbarButton> buttons;

    auto make_action = [this](const std::string& btn_id,
                              const std::string& label,
                              const std::string& tooltip_text,
                              Action action) -> FloatingToolbarButton
    {
        FloatingToolbarButton btn;
        btn.id = btn_id;
        btn.label = label;
        btn.tooltip = tooltip_text;
        btn.callback = [this, action]()
        {
            if (callback_)
            {
                callback_(action);
            }
            HideToolbar();
        };
        return btn;
    };

    buttons.push_back(make_action("bold", "B", "Bold (\u2318B)", Action::Bold));
    buttons.push_back(make_action("italic", "I", "Italic (\u2318I)", Action::Italic));
    buttons.push_back(
        make_action("strikethrough", "S\u0336", "Strikethrough", Action::Strikethrough));
    buttons.push_back(make_action("code", "</>", "Inline Code", Action::InlineCode));
    buttons.push_back(make_action("link", "\U0001F517", "Insert Link (\u2318K)", Action::Link));

    // Separator
    FloatingToolbarButton sep1;
    sep1.is_separator = true;
    buttons.push_back(std::move(sep1));

    buttons.push_back(make_action("blockquote", ">", "Blockquote", Action::Blockquote));
    buttons.push_back(make_action("heading", "H", "Cycle Heading", Action::Heading));
    buttons.push_back(make_action("table", "\u229E", "Insert Table", Action::Table));

    // Separator
    FloatingToolbarButton sep2;
    sep2.is_separator = true;
    buttons.push_back(std::move(sep2));

    buttons.push_back(make_action("highlight", "\u2588", "Highlight", Action::Highlight));
    buttons.push_back(make_action("footnote", "\u00B9", "Insert Footnote", Action::Footnote));

    SetButtons(std::move(buttons));
}

} // namespace markamp::ui
