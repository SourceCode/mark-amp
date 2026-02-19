#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// Key modifiers and shortcut binding
// ---------------------------------------------------------------------------

enum class KeyModifier : uint8_t
{
    kNone = 0,
    kCtrl = 1 << 0,
    kShift = 1 << 1,
    kAlt = 1 << 2,
    kMeta = 1 << 3 ///< Cmd on macOS
};

inline auto operator|(KeyModifier lhs, KeyModifier rhs) -> KeyModifier
{
    return static_cast<KeyModifier>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

inline auto operator&(KeyModifier lhs, KeyModifier rhs) -> KeyModifier
{
    return static_cast<KeyModifier>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

inline auto has_modifier(KeyModifier flags, KeyModifier test) -> bool
{
    return (static_cast<uint8_t>(flags) & static_cast<uint8_t>(test)) != 0;
}

/// A keyboard shortcut binding.
struct ShortcutBinding
{
    std::string command_id; ///< The command this shortcut triggers
    int key_code{0};        ///< Key code (platform-agnostic)
    KeyModifier modifiers{KeyModifier::kNone};
    std::string display_text; ///< Human-readable representation (e.g. "Ctrl+C")
    std::string description;  ///< Accessibility description
};

// ---------------------------------------------------------------------------
// NodeShortcuts — keyboard shortcut registry for node editor commands
// ---------------------------------------------------------------------------

class NodeShortcuts
{
public:
    NodeShortcuts();

    // --- Registration -----------------------------------------------------

    /// Register a keyboard shortcut.
    void register_shortcut(const std::string& command_id,
                           int key_code,
                           KeyModifier modifiers,
                           const std::string& display_text = {},
                           const std::string& description = {});

    /// Unregister a shortcut by command ID.
    void unregister(const std::string& command_id);

    /// Remove all custom shortcuts (keeps defaults).
    void reset_to_defaults();

    // --- Lookup -----------------------------------------------------------

    /// Find the command bound to a key + modifiers.
    [[nodiscard]] auto find_command(int key_code, KeyModifier modifiers) const
        -> std::optional<std::string>;

    /// Get all shortcuts for a command.
    [[nodiscard]] auto shortcuts_for(const std::string& command_id) const
        -> std::vector<ShortcutBinding>;

    /// Get all bindings.
    [[nodiscard]] auto all_bindings() const -> const std::vector<ShortcutBinding>&;

    /// Total number of bindings.
    [[nodiscard]] auto binding_count() const -> std::size_t;

    // --- Accessibility ----------------------------------------------------

    /// Get human-readable description for a command.
    [[nodiscard]] auto describe_action(const std::string& command_id) const -> std::string;

    // --- Conflict detection -----------------------------------------------

    /// Check if a key + modifiers combination is already bound.
    [[nodiscard]] auto is_bound(int key_code, KeyModifier modifiers) const -> bool;

private:
    std::vector<ShortcutBinding> bindings_;

    void register_defaults();
};

} // namespace markamp::node_editor
