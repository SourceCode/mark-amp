#include "SelectionActionBar.h"

#include "core/Events.h"

namespace markamp::ui
{

SelectionActionBar::SelectionActionBar(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       core::EventBus& event_bus)
    : FloatingToolbar(parent, theme_engine, event_bus, "selection_bar")
{
    SetDraggable(false);
    SetSnapToEdges(false);
    SetAutoHideMs(0); // Hidden when selection clears
    BuildButtons();
}

void SelectionActionBar::ShowBelowSelection(const wxPoint& selection_end)
{
    // Position slightly below the selection endpoint
    const wxPoint offset_pos(selection_end.x - CalculateSize().x / 2, selection_end.y + 8);
    ShowAt(offset_pos);
}

void SelectionActionBar::BuildButtons()
{
    std::vector<FloatingToolbarButton> buttons;

    // Copy
    FloatingToolbarButton copy_btn;
    copy_btn.id = "copy";
    copy_btn.label = "\u2398"; // ⎘ (copy)
    copy_btn.tooltip = "Copy (\u2318C)";
    copy_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "editor.copy";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(copy_btn));

    // Cut
    FloatingToolbarButton cut_btn;
    cut_btn.id = "cut";
    cut_btn.label = "\u2702"; // ✂ (scissors)
    cut_btn.tooltip = "Cut (\u2318X)";
    cut_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "editor.cut";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(cut_btn));

    // Separator
    FloatingToolbarButton sep1;
    sep1.is_separator = true;
    buttons.push_back(std::move(sep1));

    // Comment
    FloatingToolbarButton comment_btn;
    comment_btn.id = "comment";
    comment_btn.label = "//";
    comment_btn.tooltip = "Toggle Comment (\u2318/)";
    comment_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "editor.toggle_comment";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(comment_btn));

    // Indent
    FloatingToolbarButton indent_btn;
    indent_btn.id = "indent";
    indent_btn.label = "\u21E5"; // ⇥
    indent_btn.tooltip = "Indent (Tab)";
    indent_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "editor.indent";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(indent_btn));

    // Outdent
    FloatingToolbarButton outdent_btn;
    outdent_btn.id = "outdent";
    outdent_btn.label = "\u21E4"; // ⇤
    outdent_btn.tooltip = "Outdent (Shift+Tab)";
    outdent_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "editor.outdent";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(outdent_btn));

    SetButtons(std::move(buttons));
}

} // namespace markamp::ui
