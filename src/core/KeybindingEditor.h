/// @file KeybindingEditor.h
/// @brief V9 Phase 36 Task 15 — Keybinding editor: view, search, record, conflict detect, reset.
///
/// Manages all keybindings in the application. Supports viewing, searching,
/// recording new bindings, detecting conflicts, and persisting custom bindings
/// to `.markamp/keybindings.json`.
#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Source of a keybinding (used to distinguish default vs. user-modified).
enum class KeybindingSource
{
    kDefault, // Built-in default binding
    kUser     // User-customized binding
};

/// A single keybinding entry.
struct KeybindingEntry
{
    std::string command_id; // Command this binding triggers
    int key_code{0};        // Primary key code
    int modifiers{0};       // Modifier flags
    std::string context;    // When-clause context for this binding
    KeybindingSource source{KeybindingSource::kDefault};
    std::string display_string; // Human-readable representation (e.g. "Cmd+S")
};

/// A conflict between two keybindings.
struct KeybindingConflict
{
    KeybindingEntry existing; // The already-registered binding
    KeybindingEntry proposed; // The new binding that conflicts
};

// ============================================================================
// KeybindingEditor — manages keybinding viewing, editing, and persistence
// ============================================================================

/// Editor for viewing and modifying keyboard shortcuts.
///
/// Usage:
/// ```cpp
/// KeybindingEditor editor;
/// auto all = editor.all_keybindings();
/// auto results = editor.search_keybindings("save");
/// editor.set_keybinding("file.save", WXK_RETURN, wxMOD_CMD);
/// auto conflicts = editor.detect_conflicts(WXK_RETURN, wxMOD_CMD, "");
/// editor.save_to_json(".markamp/keybindings.json");
/// ```
class KeybindingEditor
{
public:
    KeybindingEditor() = default;

    // ── View and Search ──

    /// Get all registered keybindings.
    [[nodiscard]] auto all_keybindings() const -> const std::vector<KeybindingEntry>&;

    /// Search keybindings by command name or key combination string.
    [[nodiscard]] auto search_keybindings(const std::string& query) const
        -> std::vector<KeybindingEntry>;

    /// Get the keybinding for a specific command.
    [[nodiscard]] auto get_binding(const std::string& command_id) const
        -> std::optional<KeybindingEntry>;

    // ── Record and Edit ──

    /// Set or update a keybinding for a command. Returns any conflicts detected.
    auto set_keybinding(const std::string& command_id, int key_code, int modifiers)
        -> std::vector<KeybindingConflict>;

    /// Reset a command's keybinding to its default.
    void reset_keybinding(const std::string& command_id);

    /// Remove a keybinding entirely.
    void remove_keybinding(const std::string& command_id);

    // ── Conflict Detection ──

    /// Detect conflicts for a proposed key combination.
    [[nodiscard]] auto
    detect_conflicts(int key_code, int modifiers, const std::string& context) const
        -> std::vector<KeybindingConflict>;

    // ── Registration ──

    /// Register a default keybinding.
    void register_default(KeybindingEntry entry);

    /// Register multiple defaults.
    void register_defaults(std::vector<KeybindingEntry> entries);

    // ── Persistence ──

    /// Save custom keybindings to JSON file.
    [[nodiscard]] auto save_to_json(const std::string& path) const -> bool;

    /// Load custom keybindings from JSON file.
    auto load_from_json(const std::string& path) -> bool;

    /// Total number of registered keybindings.
    [[nodiscard]] auto binding_count() const -> std::size_t;

    /// Convert a key code + modifiers to a display string (e.g. "Cmd+Shift+S").
    [[nodiscard]] static auto key_to_string(int key_code, int modifiers) -> std::string;

private:
    std::vector<KeybindingEntry> bindings_;
    std::vector<KeybindingEntry> default_bindings_;
};

} // namespace markamp::core
