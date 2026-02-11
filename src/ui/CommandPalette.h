#pragma once

#include "core/ThemeEngine.h"

#include <wx/dialog.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Entry representing a command in the palette
struct PaletteCommand
{
    std::string label;            // Display name (e.g. "Toggle Word Wrap")
    std::string category;         // Category (e.g. "View", "Editor", "File")
    std::string shortcut;         // Keyboard shortcut hint (e.g. "Cmd+Shift+W")
    std::function<void()> action; // Callback when selected
};

/// A command palette overlay inspired by VSCode's Cmd+Shift+P.
/// Shows a filterable list of commands. Fuzzy-matches on both category and label.
class CommandPalette : public wxDialog
{
public:
    CommandPalette(wxWindow* parent, core::ThemeEngine& theme_engine);

    /// Register a command in the palette
    void RegisterCommand(PaletteCommand command);

    /// Register multiple commands at once
    void RegisterCommands(std::vector<PaletteCommand> commands);

    /// Clear all registered commands
    void ClearCommands();

    /// Show the palette and reset the filter
    void ShowPalette();

private:
    void OnFilterChanged(wxCommandEvent& event);
    void OnCommandSelected(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void ApplyFilter();
    void ExecuteSelected();
    void ApplyTheme();

    /// Score a candidate string against the filter (higher = better, 0 = no match)
    static auto FuzzyScore(const std::string& filter, const std::string& candidate) -> int;

    core::ThemeEngine& theme_engine_;
    wxTextCtrl* input_{nullptr};
    wxListBox* list_{nullptr};

    std::vector<PaletteCommand> all_commands_;
    std::vector<size_t> filtered_indices_; // indices into all_commands_
};

} // namespace markamp::ui
