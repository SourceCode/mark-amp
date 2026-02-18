#pragma once

#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <wx/dialog.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{
class CommandRegistry;
} // namespace markamp::core

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

/// Mode the command palette can operate in.
/// Corresponds to different prefix triggers: ">", "#", "@", ":".
enum class PaletteMode
{
    kCommands,   // ">" prefix — show all commands (default)
    kQuickOpen,  // No prefix — file picker (Cmd+P)
    kGoToSymbol, // "@" prefix — symbols in current file
    kGoToLine    // ":" prefix — go to line number
};

/// A command palette overlay inspired by VSCode's Cmd+Shift+P.
/// Shows a filterable list of commands. Fuzzy-matches on both category and label.
class CommandPalette : public wxDialog
{
public:
    CommandPalette(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    /// Register a command in the palette
    void RegisterCommand(PaletteCommand command);

    /// Register multiple commands at once
    void RegisterCommands(std::vector<PaletteCommand> commands);

    /// Clear all registered commands
    void ClearCommands();

    /// Show the palette and reset the filter
    void ShowPalette();

    /// Set the current palette mode (Phase 36)
    void SetMode(PaletteMode mode);

    /// Get the current palette mode
    [[nodiscard]] auto GetMode() const -> PaletteMode;

    /// Set an optional CommandRegistry reference for context-aware commands
    void SetRegistry(core::CommandRegistry* registry);

private:
    void OnFilterChanged(wxCommandEvent& event);
    void OnCommandSelected(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void ApplyFilter();
    void ExecuteSelected();
    void ApplyTheme();
    void DetectModeFromPrefix();

    /// Score a candidate string against the filter (higher = better, 0 = no match)
    static auto FuzzyScore(const std::string& filter, const std::string& candidate) -> int;

    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::Subscription theme_sub_;

    wxTextCtrl* input_{nullptr};
    wxListBox* list_{nullptr};

    std::vector<PaletteCommand> all_commands_;
    std::vector<size_t> filtered_indices_; // indices into all_commands_
    std::vector<std::string> mru_history_; // R18 Fix 17: recently used command labels

    PaletteMode current_mode_{PaletteMode::kCommands};
    core::CommandRegistry* registry_{nullptr};
};

} // namespace markamp::ui
