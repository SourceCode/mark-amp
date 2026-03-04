#pragma once

/// @file FloatingFormatBar.h
/// @brief V13 Phase 30 — Floating format bar inheriting from FloatingToolbar base.
///
/// Provides one-click markdown formatting: Bold, Italic, Strikethrough, Code,
/// Link, Blockquote, Heading, Table, Highlight, Footnote.
/// Appears above text selections in the editor.

#include "FloatingToolbar.h"

#include <functional>
#include <string>

namespace markamp::ui
{

class FloatingFormatBar : public FloatingToolbar
{
public:
    /// Actions the format bar can trigger.
    enum class Action
    {
        Bold,
        Italic,
        Strikethrough,
        InlineCode,
        Link,
        Blockquote,
        Heading,
        Table,
        Highlight,
        Footnote
    };

    using ActionCallback = std::function<void(Action)>;

    FloatingFormatBar(wxWindow* parent,
                      core::ThemeEngine& theme_engine,
                      core::EventBus& event_bus,
                      ActionCallback callback);

    /// Show above a text selection, centered horizontally.
    void ShowAboveSelection(const wxPoint& selection_start, const wxPoint& selection_end);

private:
    ActionCallback callback_;

    void BuildButtons();
};

} // namespace markamp::ui
