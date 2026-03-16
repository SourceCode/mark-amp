#pragma once

#include "EventBus.h"

#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Represents a single keyboard shortcut binding.
struct Shortcut
{
    std::string id;               // Unique action identifier, e.g. "file.open"
    std::string description;      // Human-readable, e.g. "Open file"
    int key_code{0};              // wxWidgets key code (WXK_*)
    int modifiers{0};             // wxMOD_CONTROL, wxMOD_ALT, wxMOD_SHIFT, wxMOD_META
    std::string context;          // "global", "editor", "sidebar", "gallery"
    std::string category;         // "File", "Edit", "View", "Navigation", "Markdown"
    std::function<void()> action; // Callback when shortcut fires
};

/// Centralized keyboard shortcut manager with context-aware filtering.
///
/// Shortcuts are processed in priority order:
///   1. Context-specific shortcuts matching the current focus context
///   2. Global shortcuts (context = "global")
///
/// This resolves conflicts like Cmd+B meaning "bold" in editor
/// but "toggle sidebar" globally.
class ShortcutManager
{
public:
    explicit ShortcutManager(EventBus& event_bus);

    // --- Registration ---

    /// Register a shortcut. Overwrites any existing shortcut with the same ID.
    void register_shortcut(Shortcut shortcut);

    /// Remove a shortcut by its unique ID.
    void unregister_shortcut(const std::string& shortcut_id);

    // --- Key processing ---

    /// Process a key event. Returns true if a matching shortcut was found and fired.
    /// @param key_code  wxWidgets key code
    /// @param modifiers Modifier flags (wxMOD_CMD | wxMOD_SHIFT etc.)
    /// @param context   Current focus context ("editor", "sidebar", "gallery", "global")
    auto process_key_event(int key_code, int modifiers, const std::string& context) -> bool;

    // --- Queries ---

    [[nodiscard]] auto get_all_shortcuts() const -> const std::vector<Shortcut>&;

    [[nodiscard]] auto get_shortcuts_for_context(const std::string& context) const
        -> std::vector<Shortcut>;

    [[nodiscard]] auto get_shortcuts_for_category(const std::string& category) const
        -> std::vector<Shortcut>;

    /// Get the human-readable shortcut text for a given ID (e.g. "⌘+S").
    [[nodiscard]] auto get_shortcut_text(const std::string& shortcut_id) const -> std::string;

    /// Find a shortcut by ID. Returns nullptr if not found.
    [[nodiscard]] auto find_shortcut(const std::string& shortcut_id) const -> const Shortcut*;

    /// Check if a key+modifiers+context binding already exists.
    [[nodiscard]] auto has_conflict(int key_code, int modifiers, const std::string& context) const
        -> bool;

    // --- Customization ---

    /// Remap a shortcut to a new key binding.
    void remap_shortcut(const std::string& shortcut_id, int key_code, int modifiers);

    /// Reset all shortcuts to their default bindings.
    void reset_to_defaults();

    // --- Persistence (keybindings.md) ---

    /// Save all remapped keybindings to a keybindings.md file.
    void save_keybindings(const std::filesystem::path& config_dir) const;

    /// Load keybindings from a keybindings.md file and apply remaps.
    void load_keybindings(const std::filesystem::path& config_dir);

    // --- Formatting ---

    /// Format a key+modifiers combination as a human-readable string.
    /// Uses platform-appropriate symbols (⌘ on macOS, Ctrl on Linux/Windows).
    [[nodiscard]] static auto format_shortcut(int key_code, int modifiers) -> std::string;

    /// Get the platform modifier name ("⌘" on macOS, "Ctrl" elsewhere).
    [[nodiscard]] static auto platform_modifier_name() -> std::string;

    /// Format a key code as a human-readable string (e.g. WXK_F1 → "F1").
    [[nodiscard]] static auto format_key_name(int key_code) -> std::string;

    // --- Phase 35: Shortcut Customization ---

    /// Export all shortcuts as a formatted Markdown cheat-sheet.
    [[nodiscard]] auto export_as_markdown() const -> std::string;

    /// Export all shortcuts as a JSON string for external tooling.
    [[nodiscard]] auto export_as_json() const -> std::string;

    /// Import shortcut remaps from a JSON string.
    void import_from_json(const std::string& json_str);

    /// Get a deduplicated list of all categories.
    [[nodiscard]] auto get_all_categories() const -> std::vector<std::string>;

    /// Detect conflicting bindings and return pairs of shortcut IDs.
    [[nodiscard]] auto get_conflicts() const -> std::vector<std::pair<std::string, std::string>>;

    /// Total number of registered shortcuts.
    [[nodiscard]] auto shortcut_count() const -> std::size_t;

    // ── Batch 19-22 (#130-132) ──

    /// (#130) Return the number of global (context-independent) shortcuts.
    [[nodiscard]] auto global_shortcut_count() const -> std::size_t;

    /// (#131) Return the number of shortcuts that have been remapped from defaults.
    [[nodiscard]] auto remapped_count() const -> std::size_t;

    /// (#132) Check if a shortcut with a given ID exists.
    [[nodiscard]] auto has_shortcut(const std::string& shortcut_id) const -> bool;

private:
    std::vector<Shortcut> shortcuts_;
    std::vector<Shortcut> default_shortcuts_; // Saved for reset_to_defaults()
    EventBus& event_bus_;
};

} // namespace markamp::core
