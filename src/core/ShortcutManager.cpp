#include "ShortcutManager.h"

#include "Logger.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace markamp::core
{

ShortcutManager::ShortcutManager(EventBus& event_bus)
    : event_bus_(event_bus)
{
    (void)event_bus_; // Will be used for shortcut change notifications
}

// ═══════════════════════════════════════════════════════
//  Registration
// ═══════════════════════════════════════════════════════

void ShortcutManager::register_shortcut(Shortcut shortcut)
{
    // Overwrite existing shortcut with same ID
    unregister_shortcut(shortcut.id);
    shortcuts_.push_back(std::move(shortcut));
}

void ShortcutManager::unregister_shortcut(const std::string& shortcut_id)
{
    std::erase_if(shortcuts_,
                  [&shortcut_id](const Shortcut& shortcut) { return shortcut.id == shortcut_id; });
}

// ═══════════════════════════════════════════════════════
//  Key processing
// ═══════════════════════════════════════════════════════

auto ShortcutManager::process_key_event(int key_code, int modifiers, const std::string& context)
    -> bool
{
    // Priority 1: Context-specific shortcuts matching the current focus context
    for (const auto& shortcut : shortcuts_)
    {
        if (shortcut.context != "global" && shortcut.context == context &&
            shortcut.key_code == key_code && shortcut.modifiers == modifiers)
        {
            if (shortcut.action)
            {
                shortcut.action();
            }
            return true;
        }
    }

    // Priority 2: Global shortcuts
    for (const auto& shortcut : shortcuts_)
    {
        if (shortcut.context == "global" && shortcut.key_code == key_code &&
            shortcut.modifiers == modifiers)
        {
            if (shortcut.action)
            {
                shortcut.action();
            }
            return true;
        }
    }

    return false;
}

// ═══════════════════════════════════════════════════════
//  Queries
// ═══════════════════════════════════════════════════════

auto ShortcutManager::get_all_shortcuts() const -> const std::vector<Shortcut>&
{
    return shortcuts_;
}

auto ShortcutManager::get_shortcuts_for_context(const std::string& context) const
    -> std::vector<Shortcut>
{
    std::vector<Shortcut> result;
    for (const auto& shortcut : shortcuts_)
    {
        if (shortcut.context == context)
        {
            result.push_back(shortcut);
        }
    }
    return result;
}

auto ShortcutManager::get_shortcuts_for_category(const std::string& category) const
    -> std::vector<Shortcut>
{
    std::vector<Shortcut> result;
    for (const auto& shortcut : shortcuts_)
    {
        if (shortcut.category == category)
        {
            result.push_back(shortcut);
        }
    }
    return result;
}

auto ShortcutManager::get_shortcut_text(const std::string& shortcut_id) const -> std::string
{
    const auto* shortcut = find_shortcut(shortcut_id);
    if (shortcut == nullptr)
    {
        return "";
    }
    return format_shortcut(shortcut->key_code, shortcut->modifiers);
}

auto ShortcutManager::find_shortcut(const std::string& shortcut_id) const -> const Shortcut*
{
    auto iter = std::find_if(shortcuts_.begin(),
                             shortcuts_.end(),
                             [&shortcut_id](const Shortcut& shortcut)
                             { return shortcut.id == shortcut_id; });
    if (iter != shortcuts_.end())
    {
        return &(*iter);
    }
    return nullptr;
}

auto ShortcutManager::has_conflict(int key_code, int modifiers, const std::string& context) const
    -> bool
{
    int count = 0;
    for (const auto& shortcut : shortcuts_)
    {
        if (shortcut.key_code == key_code && shortcut.modifiers == modifiers &&
            shortcut.context == context)
        {
            ++count;
            if (count > 1)
            {
                return true;
            }
        }
    }
    return false;
}

// ═══════════════════════════════════════════════════════
//  Customization
// ═══════════════════════════════════════════════════════

void ShortcutManager::remap_shortcut(const std::string& shortcut_id, int key_code, int modifiers)
{
    auto iter = std::find_if(shortcuts_.begin(),
                             shortcuts_.end(),
                             [&shortcut_id](const Shortcut& shortcut)
                             { return shortcut.id == shortcut_id; });
    if (iter != shortcuts_.end())
    {
        // Save defaults if not already saved
        if (default_shortcuts_.empty())
        {
            default_shortcuts_ = shortcuts_;
        }
        iter->key_code = key_code;
        iter->modifiers = modifiers;
    }
}

void ShortcutManager::reset_to_defaults()
{
    if (!default_shortcuts_.empty())
    {
        // Preserve action callbacks from current shortcuts
        for (auto& default_shortcut : default_shortcuts_)
        {
            auto current = std::find_if(shortcuts_.begin(),
                                        shortcuts_.end(),
                                        [&default_shortcut](const Shortcut& shortcut)
                                        { return shortcut.id == default_shortcut.id; });
            if (current != shortcuts_.end())
            {
                default_shortcut.action = current->action;
            }
        }
        shortcuts_ = default_shortcuts_;
        default_shortcuts_.clear();
    }
}

// ═══════════════════════════════════════════════════════
//  Formatting
// ═══════════════════════════════════════════════════════

auto ShortcutManager::platform_modifier_name() -> std::string
{
#ifdef __APPLE__
    return "\xE2\x8C\x98"; // ⌘ (Command symbol)
#else
    return "Ctrl";
#endif
}

auto ShortcutManager::format_key_name(int key_code) -> std::string
{
    // Special keys
    switch (key_code)
    {
        case 0x7F: // WXK_DELETE = 127
            return "Delete";
        case 0x08: // WXK_BACK = 8
            return "Backspace";
        case 0x09: // WXK_TAB = 9
            return "Tab";
        case 0x0D: // WXK_RETURN = 13
            return "Enter";
        case 0x1B: // WXK_ESCAPE = 27
            return "Escape";
        case 0x20: // WXK_SPACE = 32
            return "Space";
        default:
            break;
    }

    // Function keys: WXK_F1 = 340 through WXK_F12 = 351
    if (key_code >= 340 && key_code <= 351)
    {
        return "F" + std::to_string(key_code - 339);
    }

    // Navigation keys
    switch (key_code)
    {
        case 312: // WXK_HOME
            return "Home";
        case 313: // WXK_END
            return "End";
        case 314: // WXK_LEFT
            return "Left";
        case 315: // WXK_UP
            return "Up";
        case 316: // WXK_RIGHT
            return "Right";
        case 317: // WXK_DOWN
            return "Down";
        case 366: // WXK_PAGEUP
            return "PageUp";
        case 367: // WXK_PAGEDOWN
            return "PageDown";
        default:
            break;
    }

    // Printable ASCII characters
    if (key_code >= 33 && key_code <= 126)
    {
        // Show uppercase letter for readability
        auto character = static_cast<char>(key_code);
        if (character >= 'a' && character <= 'z')
        {
            character = static_cast<char>(character - 32); // to uppercase
        }
        return std::string(1, character);
    }

    return "?";
}

auto ShortcutManager::format_shortcut(int key_code, int modifiers) -> std::string
{
    std::string result;

    // Modifier order: Ctrl/Cmd → Shift → Alt
    // Modifier flag values (wxWidgets):
    // wxMOD_ALT     = 0x0001
    // wxMOD_CONTROL = 0x0002
    // wxMOD_SHIFT   = 0x0004
    // wxMOD_META    = 0x0008 (Cmd on macOS)
    // wxMOD_CMD     = wxMOD_META on macOS, wxMOD_CONTROL elsewhere

    constexpr int kModControl = 0x0002;
    constexpr int kModShift = 0x0004;
    constexpr int kModAlt = 0x0001;
    constexpr int kModMeta = 0x0008;

    // On macOS, Cmd (Meta) is the primary modifier
#ifdef __APPLE__
    if ((modifiers & kModMeta) != 0)
    {
        result += "\xE2\x8C\x98"; // ⌘
        result += "+";
    }
    if ((modifiers & kModControl) != 0)
    {
        result += "\xE2\x8C\x83"; // ⌃
        result += "+";
    }
#else
    if ((modifiers & kModControl) != 0 || (modifiers & kModMeta) != 0)
    {
        result += "Ctrl+";
    }
#endif

    if ((modifiers & kModShift) != 0)
    {
        result += "Shift+";
    }

    if ((modifiers & kModAlt) != 0)
    {
#ifdef __APPLE__
        result += "\xE2\x8C\xA5"; // ⌥ (Option)
        result += "+";
#else
        result += "Alt+";
#endif
    }

    result += format_key_name(key_code);
    return result;
}

// ═══════════════════════════════════════════════════════
//  Persistence (keybindings.md)
// ═══════════════════════════════════════════════════════

void ShortcutManager::save_keybindings(const std::filesystem::path& config_dir) const
{
    const auto file_path = config_dir / "keybindings.md";

    // Only save if there are custom remaps (default_shortcuts_ non-empty)
    if (default_shortcuts_.empty())
    {
        // No remaps have been made — remove file if it exists
        std::error_code error_code;
        std::filesystem::remove(file_path, error_code);
        return;
    }

    std::ofstream out_file(file_path);
    if (!out_file.is_open())
    {
        MARKAMP_LOG_WARN("Failed to write keybindings.md: {}", file_path.string());
        return;
    }

    out_file << "---\n";
    out_file << "title: Custom Keybindings\n";
    out_file << "description: User-defined keyboard shortcut remappings\n";
    out_file << "---\n\n";
    out_file << "# Keybindings\n\n";
    out_file << "Custom keyboard shortcut mappings. Each line is `id: key_code,modifiers`.\n\n";
    out_file << "```keybindings\n";

    // Write shortcuts that differ from their defaults
    for (const auto& shortcut : shortcuts_)
    {
        // Find corresponding default
        auto default_iter = std::find_if(default_shortcuts_.begin(),
                                         default_shortcuts_.end(),
                                         [&shortcut](const Shortcut& def_shortcut)
                                         { return def_shortcut.id == shortcut.id; });

        if (default_iter != default_shortcuts_.end())
        {
            // Only write if key or modifiers differ from default
            if (shortcut.key_code != default_iter->key_code ||
                shortcut.modifiers != default_iter->modifiers)
            {
                out_file << shortcut.id << ": " << shortcut.key_code << "," << shortcut.modifiers
                         << "\n";
            }
        }
    }

    out_file << "```\n";
    out_file.close();

    MARKAMP_LOG_INFO("Saved keybindings to {}", file_path.string());
}

void ShortcutManager::load_keybindings(const std::filesystem::path& config_dir)
{
    const auto file_path = config_dir / "keybindings.md";

    if (!std::filesystem::exists(file_path))
    {
        return; // No custom keybindings file
    }

    std::ifstream in_file(file_path);
    if (!in_file.is_open())
    {
        MARKAMP_LOG_WARN("Failed to read keybindings.md: {}", file_path.string());
        return;
    }

    bool in_block = false;
    std::string line;
    int remap_count = 0;

    while (std::getline(in_file, line))
    {
        // Detect the start/end of the keybindings code block
        if (line.starts_with("```keybindings"))
        {
            in_block = true;
            continue;
        }
        if (in_block && line.starts_with("```"))
        {
            break; // End of block
        }

        if (!in_block || line.empty())
        {
            continue;
        }

        // Parse "id: key_code,modifiers"
        const auto colon_pos = line.find(':');
        if (colon_pos == std::string::npos)
        {
            continue;
        }

        const std::string binding_id = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);

        // Trim leading whitespace
        const auto first_non_space = value.find_first_not_of(' ');
        if (first_non_space == std::string::npos)
        {
            continue;
        }
        value = value.substr(first_non_space);

        const auto comma_pos = value.find(',');
        if (comma_pos == std::string::npos)
        {
            continue;
        }

        try
        {
            const int key_code = std::stoi(value.substr(0, comma_pos));
            const int modifiers = std::stoi(value.substr(comma_pos + 1));
            remap_shortcut(binding_id, key_code, modifiers);
            ++remap_count;
        }
        catch (const std::exception& parse_error)
        {
            MARKAMP_LOG_WARN("Invalid keybinding entry '{}': {}", binding_id, parse_error.what());
        }
    }

    in_file.close();
    MARKAMP_LOG_INFO("Loaded {} keybinding remaps from {}", remap_count, file_path.string());
}

} // namespace markamp::core
