#pragma once

#include "animation/TransitionManager.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/vlbox.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{
class CommandRegistry;
} // namespace markamp::core

namespace markamp::ui
{

class PaletteListBox;

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

    /// Hide the palette with animation
    void HidePalette();

    /// Set the current palette mode (Phase 36)
    void SetMode(PaletteMode mode);

    /// Get the current palette mode
    [[nodiscard]] auto GetMode() const -> PaletteMode;

    /// Set an optional CommandRegistry reference for context-aware commands
    void SetRegistry(core::CommandRegistry* registry);

    // Rendering for wxVListBox
    void DrawListItem(wxDC& dc, const wxRect& rect, size_t n) const;
    wxCoord MeasureListItem(size_t n) const;
    void DrawListItemBackground(wxDC& dc, const wxRect& rect, size_t n) const;

    // ── 100 Editor UX/UI Improvements: Batch 9 — Command Palette Accessors (#81–#88) ──

    /// #81 Total number of registered commands.
    [[nodiscard]] inline auto command_count() const noexcept -> std::size_t
    {
        return all_commands_.size();
    }

    /// #82 True when at least one command is registered.
    [[nodiscard]] inline auto has_commands() const noexcept -> bool
    {
        return !all_commands_.empty();
    }

    /// #83 Number of display items (after filtering).
    [[nodiscard]] inline auto display_item_count() const noexcept -> std::size_t
    {
        return display_items_.size();
    }

    /// #84 Current filter string.
    [[nodiscard]] inline auto current_filter() const noexcept -> const std::string&
    {
        return current_filter_;
    }

    /// #85 True when a filter is active.
    [[nodiscard]] inline auto has_filter() const noexcept -> bool
    {
        return !current_filter_.empty();
    }

    /// #86 True when the palette is in closing animation.
    [[nodiscard]] inline auto is_closing() const noexcept -> bool
    {
        return is_closing_;
    }

    /// #87 True when a CommandRegistry is attached.
    [[nodiscard]] inline auto has_registry() const noexcept -> bool
    {
        return registry_ != nullptr;
    }

    /// #88 Number of most-recently-used command entries.
    [[nodiscard]] inline auto mru_count() const noexcept -> std::size_t
    {
        return mru_history_.size();
    }

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
    PaletteListBox* list_{nullptr};

    enum class ItemType
    {
        Header,
        Command,
        Empty
    };
    struct DisplayItem
    {
        ItemType type;
        size_t cmd_index;  // Valid if Command
        std::string label; // Valid if Header or Empty
    };

    std::vector<PaletteCommand> all_commands_;
    std::vector<DisplayItem> display_items_;
    std::vector<std::string> mru_history_; // R18 Fix 17: recently used command labels
    std::string current_filter_;

    PaletteMode current_mode_{PaletteMode::kCommands};
    core::CommandRegistry* registry_{nullptr};

    // Animation components
    animation::TransitionManager transition_manager_{this};
    bool is_closing_{false};
};

} // namespace markamp::ui
