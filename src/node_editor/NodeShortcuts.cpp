#include "NodeShortcuts.h"

#include <algorithm>

namespace markamp::node_editor
{

// Key codes for common keys (platform-agnostic).
namespace keys
{
constexpr int kKeyDelete = 127;
constexpr int kKeyTab = 9;
constexpr int kKeyHome = 278;
constexpr int kKeyA = 'A';
constexpr int kKeyB = 'B';
constexpr int kKeyC = 'C';
constexpr int kKeyD = 'D';
constexpr int kKeyF = 'F';
constexpr int kKeyG = 'G';
constexpr int kKeyV = 'V';
constexpr int kKeyX = 'X';
constexpr int kKeyZ = 'Z';
constexpr int kKeyEscape = 27;
} // namespace keys

NodeShortcuts::NodeShortcuts()
{
    register_defaults();
}

// ---------------------------------------------------------------------------
// Default bindings (Blender-inspired + standard editor shortcuts)
// ---------------------------------------------------------------------------

void NodeShortcuts::register_defaults()
{
    // Standard shortcuts
    register_shortcut(
        "node.copy", keys::kKeyC, KeyModifier::kCtrl, "Ctrl+C", "Copy selected nodes");
    register_shortcut(
        "node.paste", keys::kKeyV, KeyModifier::kCtrl, "Ctrl+V", "Paste nodes from clipboard");
    register_shortcut("node.cut", keys::kKeyX, KeyModifier::kCtrl, "Ctrl+X", "Cut selected nodes");
    register_shortcut(
        "node.duplicate", keys::kKeyD, KeyModifier::kCtrl, "Ctrl+D", "Duplicate selected nodes");
    register_shortcut("node.undo", keys::kKeyZ, KeyModifier::kCtrl, "Ctrl+Z", "Undo last action");
    register_shortcut("node.redo",
                      keys::kKeyZ,
                      KeyModifier::kCtrl | KeyModifier::kShift,
                      "Ctrl+Shift+Z",
                      "Redo last undone action");
    register_shortcut(
        "node.select_all", keys::kKeyA, KeyModifier::kCtrl, "Ctrl+A", "Select all nodes");

    // Blender-style shortcuts
    register_shortcut(
        "node.delete", keys::kKeyDelete, KeyModifier::kNone, "Delete", "Delete selected nodes");
    register_shortcut("node.add", keys::kKeyA, KeyModifier::kNone, "A", "Open add node menu");
    register_shortcut(
        "node.grab", keys::kKeyG, KeyModifier::kNone, "G", "Grab and move selected nodes");
    register_shortcut(
        "node.box_select", keys::kKeyB, KeyModifier::kNone, "B", "Box selection mode");
    register_shortcut(
        "node.zoom_fit", keys::kKeyF, KeyModifier::kNone, "F", "Zoom to fit all nodes");
    register_shortcut(
        "node.reset_view", keys::kKeyHome, KeyModifier::kNone, "Home", "Reset viewport to default");
    register_shortcut(
        "node.focus_next", keys::kKeyTab, KeyModifier::kNone, "Tab", "Focus next node");
    register_shortcut(
        "node.focus_prev", keys::kKeyTab, KeyModifier::kShift, "Shift+Tab", "Focus previous node");
    register_shortcut(
        "node.cancel", keys::kKeyEscape, KeyModifier::kNone, "Escape", "Cancel current operation");
}

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

void NodeShortcuts::register_shortcut(const std::string& command_id,
                                      int key_code,
                                      KeyModifier modifiers,
                                      const std::string& display_text,
                                      const std::string& description)
{
    ShortcutBinding binding;
    binding.command_id = command_id;
    binding.key_code = key_code;
    binding.modifiers = modifiers;
    binding.display_text = display_text;
    binding.description = description;
    bindings_.push_back(binding);
}

void NodeShortcuts::unregister(const std::string& command_id)
{
    bindings_.erase(std::remove_if(bindings_.begin(),
                                   bindings_.end(),
                                   [&](const ShortcutBinding& binding)
                                   { return binding.command_id == command_id; }),
                    bindings_.end());
}

void NodeShortcuts::reset_to_defaults()
{
    bindings_.clear();
    register_defaults();
}

// ---------------------------------------------------------------------------
// Lookup
// ---------------------------------------------------------------------------

auto NodeShortcuts::find_command(int key_code, KeyModifier modifiers) const
    -> std::optional<std::string>
{
    for (const auto& binding : bindings_)
    {
        if (binding.key_code == key_code && binding.modifiers == modifiers)
        {
            return binding.command_id;
        }
    }
    return std::nullopt;
}

auto NodeShortcuts::shortcuts_for(const std::string& command_id) const
    -> std::vector<ShortcutBinding>
{
    std::vector<ShortcutBinding> result;
    for (const auto& binding : bindings_)
    {
        if (binding.command_id == command_id)
        {
            result.push_back(binding);
        }
    }
    return result;
}

auto NodeShortcuts::all_bindings() const -> const std::vector<ShortcutBinding>&
{
    return bindings_;
}

auto NodeShortcuts::binding_count() const -> std::size_t
{
    return bindings_.size();
}

// ---------------------------------------------------------------------------
// Accessibility
// ---------------------------------------------------------------------------

auto NodeShortcuts::describe_action(const std::string& command_id) const -> std::string
{
    for (const auto& binding : bindings_)
    {
        if (binding.command_id == command_id && !binding.description.empty())
        {
            return binding.description;
        }
    }
    return "Unknown action: " + command_id;
}

// ---------------------------------------------------------------------------
// Conflict detection
// ---------------------------------------------------------------------------

auto NodeShortcuts::is_bound(int key_code, KeyModifier modifiers) const -> bool
{
    return std::any_of(bindings_.begin(),
                       bindings_.end(),
                       [&](const ShortcutBinding& binding)
                       { return binding.key_code == key_code && binding.modifiers == modifiers; });
}

} // namespace markamp::node_editor
