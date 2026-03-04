#pragma once

/// @file SelectionActionBar.h
/// @brief V13 Phase 30 — Action bar for bulk operations on selected text.

#include "FloatingToolbar.h"

namespace markamp::ui
{

/// Floating toolbar for multi-line selection operations:
/// Copy, Cut, Comment, Indent, Outdent.
class SelectionActionBar : public FloatingToolbar
{
public:
    SelectionActionBar(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       core::EventBus& event_bus);

    /// Show below the end of a text selection.
    void ShowBelowSelection(const wxPoint& selection_end);

private:
    void BuildButtons();
};

} // namespace markamp::ui
