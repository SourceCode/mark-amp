#pragma once

#include "core/ThemeEngine.h"

#include <wx/popupwin.h>
#include <wx/wx.h>

#include <functional>
#include <string>

namespace markamp::ui
{

/// Floating formatting toolbar that appears above text selections.
/// Provides one-click markdown formatting: Bold, Italic, Code, Link, Quote, Heading, Table.
class FloatingFormatBar : public wxPopupTransientWindow
{
public:
    /// Actions the format bar can trigger.
    enum class Action
    {
        Bold,
        Italic,
        InlineCode,
        Link,
        Blockquote,
        Heading,
        Table
    };

    using ActionCallback = std::function<void(Action)>;

    FloatingFormatBar(wxWindow* parent, core::ThemeEngine& theme_engine, ActionCallback callback);

    /// Update styling from theme.
    void ApplyTheme();

private:
    core::ThemeEngine& theme_engine_;
    ActionCallback callback_;

    void CreateButtons();
    void OnButtonClicked(Action action);
};

} // namespace markamp::ui
