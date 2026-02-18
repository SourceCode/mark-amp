/// @file PlatformKeyboard.cpp
/// @brief V9 Phase 32 – Platform-specific keyboard implementation.

#include "PlatformKeyboard.h"

namespace markamp::core
{

auto PlatformKeyboard::detect_platform() -> PlatformType
{
#if defined(__APPLE__)
    return PlatformType::kMacOS;
#elif defined(_WIN32) || defined(_WIN64)
    return PlatformType::kWindows;
#else
    return PlatformType::kLinux;
#endif
}

auto PlatformKeyboard::get_primary_modifier() -> KeyModifier
{
    if (detect_platform() == PlatformType::kMacOS)
    {
        return KeyModifier::kMeta; // Cmd
    }
    return KeyModifier::kCtrl;
}

auto PlatformKeyboard::get_shortcut(KeyAction action) -> PlatformShortcut
{
    const auto mod = get_primary_modifier();
    const auto platform = detect_platform();

    PlatformShortcut shortcut;
    shortcut.action = action;

    switch (action)
    {
        case KeyAction::kCopy:
            shortcut.modifier = mod;
            shortcut.key = 'C';
            break;
        case KeyAction::kCut:
            shortcut.modifier = mod;
            shortcut.key = 'X';
            break;
        case KeyAction::kPaste:
            shortcut.modifier = mod;
            shortcut.key = 'V';
            break;
        case KeyAction::kUndo:
            shortcut.modifier = mod;
            shortcut.key = 'Z';
            break;
        case KeyAction::kRedo:
            shortcut.modifier = mod | KeyModifier::kShift;
            shortcut.key = 'Z';
            break;
        case KeyAction::kSave:
            shortcut.modifier = mod;
            shortcut.key = 'S';
            break;
        case KeyAction::kSaveAs:
            shortcut.modifier = mod | KeyModifier::kShift;
            shortcut.key = 'S';
            break;
        case KeyAction::kOpen:
            shortcut.modifier = mod;
            shortcut.key = 'O';
            break;
        case KeyAction::kNew:
            shortcut.modifier = mod;
            shortcut.key = 'N';
            break;
        case KeyAction::kClose:
            shortcut.modifier = mod;
            shortcut.key = 'W';
            break;
        case KeyAction::kQuit:
            if (platform == PlatformType::kMacOS)
            {
                shortcut.modifier = KeyModifier::kMeta;
                shortcut.key = 'Q';
            }
            else
            {
                shortcut.modifier = KeyModifier::kAlt;
                shortcut.key = 'F'; // Alt+F4 (represented as F for F4)
            }
            break;
        case KeyAction::kFind:
            shortcut.modifier = mod;
            shortcut.key = 'F';
            break;
        case KeyAction::kReplace:
            shortcut.modifier = mod;
            shortcut.key = 'H';
            break;
        case KeyAction::kSelectAll:
            shortcut.modifier = mod;
            shortcut.key = 'A';
            break;
        case KeyAction::kPreferences:
            if (platform == PlatformType::kMacOS)
            {
                shortcut.modifier = KeyModifier::kMeta;
                shortcut.key = ',';
            }
            else
            {
                shortcut.modifier = KeyModifier::kCtrl;
                shortcut.key = ',';
            }
            break;
        case KeyAction::kToggleSidebar:
            shortcut.modifier = mod;
            shortcut.key = 'B';
            break;
        case KeyAction::kZoomIn:
            shortcut.modifier = mod;
            shortcut.key = '+';
            break;
        case KeyAction::kZoomOut:
            shortcut.modifier = mod;
            shortcut.key = '-';
            break;
        case KeyAction::kZoomReset:
            shortcut.modifier = mod;
            shortcut.key = '0';
            break;
        case KeyAction::kBold:
            shortcut.modifier = mod;
            shortcut.key = 'B';
            break;
        case KeyAction::kItalic:
            shortcut.modifier = mod;
            shortcut.key = 'I';
            break;
    }

    shortcut.display_string = format_shortcut(shortcut);
    return shortcut;
}

auto PlatformKeyboard::get_all_shortcuts() -> std::vector<PlatformShortcut>
{
    static constexpr KeyAction kAllActions[] = {
        KeyAction::kCopy,          KeyAction::kCut,       KeyAction::kPaste,
        KeyAction::kUndo,          KeyAction::kRedo,      KeyAction::kSave,
        KeyAction::kSaveAs,        KeyAction::kOpen,      KeyAction::kNew,
        KeyAction::kClose,         KeyAction::kQuit,      KeyAction::kFind,
        KeyAction::kReplace,       KeyAction::kSelectAll, KeyAction::kPreferences,
        KeyAction::kToggleSidebar, KeyAction::kZoomIn,    KeyAction::kZoomOut,
        KeyAction::kZoomReset,     KeyAction::kBold,      KeyAction::kItalic,
    };

    std::vector<PlatformShortcut> result;
    result.reserve(std::size(kAllActions));
    for (auto action : kAllActions)
    {
        result.push_back(get_shortcut(action));
    }
    return result;
}

auto PlatformKeyboard::format_shortcut(const PlatformShortcut& shortcut) -> std::string
{
    const auto platform = detect_platform();
    std::string result;

    if (platform == PlatformType::kMacOS)
    {
        // macOS uses symbol glyphs
        if (static_cast<uint8_t>(shortcut.modifier & KeyModifier::kCtrl) != 0)
        {
            result += "⌃";
        }
        if (static_cast<uint8_t>(shortcut.modifier & KeyModifier::kAlt) != 0)
        {
            result += "⌥";
        }
        if (static_cast<uint8_t>(shortcut.modifier & KeyModifier::kShift) != 0)
        {
            result += "⇧";
        }
        if (static_cast<uint8_t>(shortcut.modifier & KeyModifier::kMeta) != 0)
        {
            result += "⌘";
        }
        result += shortcut.key;
    }
    else
    {
        // Windows / Linux use text labels
        if (static_cast<uint8_t>(shortcut.modifier & KeyModifier::kCtrl) != 0)
        {
            result += "Ctrl+";
        }
        if (static_cast<uint8_t>(shortcut.modifier & KeyModifier::kAlt) != 0)
        {
            result += "Alt+";
        }
        if (static_cast<uint8_t>(shortcut.modifier & KeyModifier::kShift) != 0)
        {
            result += "Shift+";
        }
        if (static_cast<uint8_t>(shortcut.modifier & KeyModifier::kMeta) != 0)
        {
            result += "Win+";
        }
        result += shortcut.key;
    }

    return result;
}

auto PlatformKeyboard::is_quit_shortcut(char key, KeyModifier mods) -> bool
{
    if (detect_platform() == PlatformType::kMacOS)
    {
        return key == 'Q' && static_cast<uint8_t>(mods & KeyModifier::kMeta) != 0;
    }
    // Windows/Linux: Alt+F4 — we represent as Alt + 'F'
    return key == 'F' && static_cast<uint8_t>(mods & KeyModifier::kAlt) != 0;
}

auto PlatformKeyboard::supports_ime() -> bool
{
    // IME is supported on all platforms
    return true;
}

auto PlatformKeyboard::get_menu_structure() -> std::vector<MenuGroup>
{
    if (detect_platform() != PlatformType::kMacOS)
    {
        return {}; // Only macOS has HIG-specific menus
    }

    std::vector<MenuGroup> menus;

    // Use Unicode symbols directly (UTF-8 source encoding)
    const std::string cmd = "\xE2\x8C\x98"; // ⌘
    const std::string shift_cmd = "\xE2\x87\xA7"
                                  "\xE2\x8C\x98"; // ⇧⌘

    // Application menu
    MenuGroup app_menu;
    app_menu.title = "MarkAmp";
    app_menu.items = {
        {"About MarkAmp", "", KeyAction::kNew, false},
        {"", "", KeyAction::kNew, true}, // separator
        {"Preferences...", cmd + ",", KeyAction::kPreferences, false},
        {"", "", KeyAction::kNew, true},
        {"Hide MarkAmp", cmd + "H", KeyAction::kNew, false},
        {"Quit MarkAmp", cmd + "Q", KeyAction::kQuit, false},
    };
    menus.push_back(std::move(app_menu));

    // File menu
    MenuGroup file_menu;
    file_menu.title = "File";
    file_menu.items = {
        {"New", cmd + "N", KeyAction::kNew, false},
        {"Open...", cmd + "O", KeyAction::kOpen, false},
        {"", "", KeyAction::kNew, true},
        {"Save", cmd + "S", KeyAction::kSave, false},
        {"Save As...", shift_cmd + "S", KeyAction::kSaveAs, false},
        {"", "", KeyAction::kNew, true},
        {"Close", cmd + "W", KeyAction::kClose, false},
    };
    menus.push_back(std::move(file_menu));

    // Edit menu
    MenuGroup edit_menu;
    edit_menu.title = "Edit";
    edit_menu.items = {
        {"Undo", cmd + "Z", KeyAction::kUndo, false},
        {"Redo", shift_cmd + "Z", KeyAction::kRedo, false},
        {"", "", KeyAction::kNew, true},
        {"Cut", cmd + "X", KeyAction::kCut, false},
        {"Copy", cmd + "C", KeyAction::kCopy, false},
        {"Paste", cmd + "V", KeyAction::kPaste, false},
        {"Select All", cmd + "A", KeyAction::kSelectAll, false},
    };
    menus.push_back(std::move(edit_menu));

    // Window menu
    MenuGroup window_menu;
    window_menu.title = "Window";
    window_menu.items = {
        {"Minimize", cmd + "M", KeyAction::kNew, false},
        {"Zoom", "", KeyAction::kNew, false},
        {"", "", KeyAction::kNew, true},
        {"Bring All to Front", "", KeyAction::kNew, false},
    };
    menus.push_back(std::move(window_menu));

    // Help menu
    MenuGroup help_menu;
    help_menu.title = "Help";
    help_menu.items = {
        {"Search", "", KeyAction::kFind, false},
    };
    menus.push_back(std::move(help_menu));

    return menus;
}

} // namespace markamp::core
