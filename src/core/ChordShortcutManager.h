/// @file ChordShortcutManager.h
/// @brief V9 Phase 36 Task 11 — Chord (multi-key) shortcuts: Cmd+K followed by a second key.
///
/// Manages two-key chord shortcuts. The first key enters a "waiting" state;
/// the second key completes the chord. Escape cancels, 2-second timeout.
#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// ChordBinding — a two-key chord shortcut definition
// ============================================================================

struct ChordBinding
{
    int first_key{0};        // First key code (e.g. 'K')
    int first_modifiers{0};  // First key modifiers (e.g. wxMOD_CMD)
    int second_key{0};       // Second key code
    int second_modifiers{0}; // Second key modifiers
    std::string command_id;  // Command to execute when chord completes
    std::string description; // Human-readable description
};

// ============================================================================
// ChordShortcutManager — state machine for chord shortcuts
// ============================================================================

/// State of the chord shortcut manager.
enum class ChordState
{
    kIdle,            // No chord in progress
    kWaitingForSecond // First key pressed, waiting for second key
};

/// Manages chord (two-key) shortcuts like Cmd+K Cmd+S.
///
/// Usage:
/// ```cpp
/// ChordShortcutManager manager;
/// manager.register_chord({.first_key = 'K', .first_modifiers = wxMOD_CMD,
///                          .second_key = 'S', .second_modifiers = wxMOD_CMD,
///                          .command_id = "workbench.action.files.saveAll"});
///
/// auto result = manager.handle_key('K', wxMOD_CMD);
/// // result == ChordResult::kWaiting
/// result = manager.handle_key('S', wxMOD_CMD);
/// // result == ChordResult::kMatched, chord executed
/// ```
class ChordShortcutManager
{
public:
    /// Result of handling a key event.
    enum class ChordResult
    {
        kNotConsumed, // Key not part of any chord
        kWaiting,     // First key matched, waiting for second
        kMatched,     // Chord completed, command identified
        kCanceled     // Chord was canceled (Escape or timeout)
    };

    ChordShortcutManager();

    /// Register a chord shortcut.
    void register_chord(ChordBinding binding);

    /// Handle a key press. Returns the result of chord processing.
    auto handle_key(int key_code, int modifiers) -> ChordResult;

    /// Cancel any pending chord (e.g. when Escape is pressed).
    void cancel();

    /// Check if the manager is currently waiting for a second key.
    [[nodiscard]] auto is_waiting() const -> bool;

    /// Get a description of the current waiting state (for status bar display).
    [[nodiscard]] auto waiting_description() const -> std::string;

    /// Get the command ID from the last matched chord.
    [[nodiscard]] auto last_matched_command() const -> std::string;

    /// Check if a chord has timed out (call periodically, e.g. on timer).
    auto check_timeout() -> bool;

    /// Get all registered chord bindings.
    [[nodiscard]] auto all_chords() const -> const std::vector<ChordBinding>&;

    /// Number of registered chords.
    [[nodiscard]] auto chord_count() const -> std::size_t;

    /// Set the timeout duration (default: 2 seconds).
    void set_timeout(std::chrono::milliseconds timeout);

    /// Register the 10+ built-in chord shortcuts.
    void register_built_in_chords();

private:
    std::vector<ChordBinding> chords_;

    ChordState state_{ChordState::kIdle};
    int pending_first_key_{0};
    int pending_first_modifiers_{0};
    std::chrono::steady_clock::time_point pending_start_;
    std::string last_matched_command_;

    std::chrono::milliseconds timeout_{2000};

    /// Find chords matching the first key.
    [[nodiscard]] auto find_first_key_matches(int key_code, int modifiers) const
        -> std::vector<const ChordBinding*>;

    /// Find a chord matching both keys.
    [[nodiscard]] auto
    find_chord_match(int first_key, int first_mods, int second_key, int second_mods) const
        -> const ChordBinding*;
};

} // namespace markamp::core
