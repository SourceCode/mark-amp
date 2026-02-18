/// @file PlatformKeyboard.h
/// @brief V9 Phase 32 – Platform-specific keyboard shortcuts and menu conventions.

#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// Target platform identifier.
enum class PlatformType : uint8_t
{
    kMacOS,
    kWindows,
    kLinux
};

/// Keyboard modifier flags.
enum class KeyModifier : uint8_t
{
    kNone = 0,
    kCtrl = 1 << 0,
    kShift = 1 << 1,
    kAlt = 1 << 2,
    kMeta = 1 << 3 ///< Cmd on macOS, Win key on Windows/Linux
};

/// Bitwise OR for KeyModifier.
inline auto operator|(KeyModifier lhs, KeyModifier rhs) -> KeyModifier
{
    return static_cast<KeyModifier>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

/// Bitwise AND for KeyModifier.
inline auto operator&(KeyModifier lhs, KeyModifier rhs) -> KeyModifier
{
    return static_cast<KeyModifier>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

/// Named keyboard action.
enum class KeyAction : uint8_t
{
    kCopy,
    kCut,
    kPaste,
    kUndo,
    kRedo,
    kSave,
    kSaveAs,
    kOpen,
    kNew,
    kClose,
    kQuit,
    kFind,
    kReplace,
    kSelectAll,
    kPreferences,
    kToggleSidebar,
    kZoomIn,
    kZoomOut,
    kZoomReset,
    kBold,
    kItalic
};

/// A platform-specific shortcut binding.
struct PlatformShortcut
{
    KeyAction action{KeyAction::kCopy};
    KeyModifier modifier{KeyModifier::kNone};
    char key{'\0'};
    std::string display_string; ///< Human-readable (e.g. "⌘C" or "Ctrl+C")
};

/// A menu bar item (for macOS HIG menu structure).
struct MenuItem
{
    std::string label;
    std::string shortcut_display;
    KeyAction action{KeyAction::kCopy};
    bool separator{false};
};

/// A menu group (e.g. File, Edit, Window).
struct MenuGroup
{
    std::string title;
    std::vector<MenuItem> items;
};

/// Cross-platform keyboard and menu service.
class PlatformKeyboard
{
public:
    /// Detect the current platform at compile time.
    [[nodiscard]] static auto detect_platform() -> PlatformType;

    /// Get the primary modifier for the current platform (Cmd on macOS, Ctrl otherwise).
    [[nodiscard]] static auto get_primary_modifier() -> KeyModifier;

    /// Get the platform-appropriate shortcut for an action.
    [[nodiscard]] static auto get_shortcut(KeyAction action) -> PlatformShortcut;

    /// Get all platform shortcuts.
    [[nodiscard]] static auto get_all_shortcuts() -> std::vector<PlatformShortcut>;

    /// Format a shortcut for display.
    [[nodiscard]] static auto format_shortcut(const PlatformShortcut& shortcut) -> std::string;

    /// Check if a key+modifier combination is the quit shortcut.
    [[nodiscard]] static auto is_quit_shortcut(char key, KeyModifier mods) -> bool;

    /// Check if IME (Input Method Editor) is supported on this platform.
    [[nodiscard]] static auto supports_ime() -> bool;

    /// Get the macOS HIG menu structure (returns empty on other platforms).
    [[nodiscard]] static auto get_menu_structure() -> std::vector<MenuGroup>;
};

} // namespace markamp::core
