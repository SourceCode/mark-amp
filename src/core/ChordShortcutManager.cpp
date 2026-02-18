/// @file ChordShortcutManager.cpp
/// @brief V9 Phase 36 Task 11 — ChordShortcutManager implementation.

#include "ChordShortcutManager.h"

namespace markamp::core
{

ChordShortcutManager::ChordShortcutManager() = default;

void ChordShortcutManager::register_chord(ChordBinding binding)
{
    chords_.push_back(std::move(binding));
}

auto ChordShortcutManager::handle_key(int key_code, int modifiers) -> ChordResult
{
    if (state_ == ChordState::kIdle)
    {
        // Check if this key starts any chord
        auto matches = find_first_key_matches(key_code, modifiers);
        if (matches.empty())
        {
            return ChordResult::kNotConsumed;
        }

        // Enter waiting state
        state_ = ChordState::kWaitingForSecond;
        pending_first_key_ = key_code;
        pending_first_modifiers_ = modifiers;
        pending_start_ = std::chrono::steady_clock::now();
        return ChordResult::kWaiting;
    }

    // We're in kWaitingForSecond
    if (key_code == 27) // Escape
    {
        cancel();
        return ChordResult::kCanceled;
    }

    // Try to match second key
    const auto* matched =
        find_chord_match(pending_first_key_, pending_first_modifiers_, key_code, modifiers);

    if (matched != nullptr)
    {
        last_matched_command_ = matched->command_id;
        state_ = ChordState::kIdle;
        return ChordResult::kMatched;
    }

    // Second key didn't match — cancel chord
    cancel();
    return ChordResult::kCanceled;
}

void ChordShortcutManager::cancel()
{
    state_ = ChordState::kIdle;
    pending_first_key_ = 0;
    pending_first_modifiers_ = 0;
}

auto ChordShortcutManager::is_waiting() const -> bool
{
    return state_ == ChordState::kWaitingForSecond;
}

auto ChordShortcutManager::waiting_description() const -> std::string
{
    if (!is_waiting())
    {
        return "";
    }
    return "Chord started, awaiting second key...";
}

auto ChordShortcutManager::last_matched_command() const -> std::string
{
    return last_matched_command_;
}

auto ChordShortcutManager::check_timeout() -> bool
{
    if (!is_waiting())
    {
        return false;
    }

    auto elapsed = std::chrono::steady_clock::now() - pending_start_;
    if (elapsed >= timeout_)
    {
        cancel();
        return true; // Timed out
    }
    return false;
}

auto ChordShortcutManager::all_chords() const -> const std::vector<ChordBinding>&
{
    return chords_;
}

auto ChordShortcutManager::chord_count() const -> std::size_t
{
    return chords_.size();
}

void ChordShortcutManager::set_timeout(std::chrono::milliseconds timeout)
{
    timeout_ = timeout;
}

void ChordShortcutManager::register_built_in_chords()
{
    // 10 built-in chord shortcuts following VS Code conventions
    // All use Cmd+K as the first key

    constexpr int kKeyK = 'K';
    constexpr int kModCmd = 0x1000; // wxMOD_CMD equivalent

    register_chord(
        {kKeyK, kModCmd, 'S', kModCmd, "workbench.action.files.saveAll", "Save All Files"});
    register_chord(
        {kKeyK, kModCmd, 'C', kModCmd, "editor.action.addCommentLine", "Add Line Comment"});
    register_chord(
        {kKeyK, kModCmd, 'U', kModCmd, "editor.action.removeCommentLine", "Remove Line Comment"});
    register_chord(
        {kKeyK, kModCmd, 'F', kModCmd, "editor.action.formatDocument", "Format Document"});
    register_chord(
        {kKeyK, kModCmd, 'T', kModCmd, "workbench.action.selectTheme", "Select Color Theme"});
    register_chord(
        {kKeyK, kModCmd, 'Z', kModCmd, "workbench.action.toggleZenMode", "Toggle Zen Mode"});
    register_chord(
        {kKeyK, kModCmd, 'W', kModCmd, "workbench.action.closeAllEditors", "Close All Editors"});
    register_chord(
        {kKeyK, kModCmd, 'P', kModCmd, "workbench.action.copyFilePath", "Copy File Path"});
    register_chord(
        {kKeyK, kModCmd, 'R', kModCmd, "workbench.action.reloadWindow", "Reload Window"});
    register_chord({kKeyK, kModCmd, 'M', kModCmd, "editor.action.toggleMinimap", "Toggle Minimap"});
    register_chord(
        {kKeyK, kModCmd, 'I', kModCmd, "workbench.action.showHoverInfo", "Show Hover Information"});
}

auto ChordShortcutManager::find_first_key_matches(int key_code, int modifiers) const
    -> std::vector<const ChordBinding*>
{
    std::vector<const ChordBinding*> matches;
    for (const auto& chord : chords_)
    {
        if (chord.first_key == key_code && chord.first_modifiers == modifiers)
        {
            matches.push_back(&chord);
        }
    }
    return matches;
}

auto ChordShortcutManager::find_chord_match(int first_key,
                                            int first_mods,
                                            int second_key,
                                            int second_mods) const -> const ChordBinding*
{
    for (const auto& chord : chords_)
    {
        if (chord.first_key == first_key && chord.first_modifiers == first_mods &&
            chord.second_key == second_key && chord.second_modifiers == second_mods)
        {
            return &chord;
        }
    }
    return nullptr;
}

} // namespace markamp::core
