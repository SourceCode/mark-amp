/// @file KeybindingEditor.cpp
/// @brief Implementation of KeybindingEditor.

#include "KeybindingEditor.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace markamp::core
{

auto KeybindingEditor::all_keybindings() const -> const std::vector<KeybindingEntry>&
{
    return bindings_;
}

auto KeybindingEditor::search_keybindings(const std::string& query) const
    -> std::vector<KeybindingEntry>
{
    std::vector<KeybindingEntry> result;
    for (const auto& binding : bindings_)
    {
        if (binding.command_id.find(query) != std::string::npos ||
            binding.display_string.find(query) != std::string::npos)
        {
            result.push_back(binding);
        }
    }
    return result;
}

auto KeybindingEditor::get_binding(const std::string& command_id) const
    -> std::optional<KeybindingEntry>
{
    for (const auto& binding : bindings_)
    {
        if (binding.command_id == command_id)
        {
            return binding;
        }
    }
    return std::nullopt;
}

auto KeybindingEditor::set_keybinding(const std::string& command_id, int key_code, int modifiers)
    -> std::vector<KeybindingConflict>
{
    auto conflicts = detect_conflicts(key_code, modifiers, "");

    // Update existing or add new
    for (auto& binding : bindings_)
    {
        if (binding.command_id == command_id)
        {
            binding.key_code = key_code;
            binding.modifiers = modifiers;
            binding.source = KeybindingSource::kUser;
            binding.display_string = key_to_string(key_code, modifiers);
            return conflicts;
        }
    }

    KeybindingEntry new_entry;
    new_entry.command_id = command_id;
    new_entry.key_code = key_code;
    new_entry.modifiers = modifiers;
    new_entry.source = KeybindingSource::kUser;
    new_entry.display_string = key_to_string(key_code, modifiers);
    bindings_.push_back(std::move(new_entry));

    return conflicts;
}

void KeybindingEditor::reset_keybinding(const std::string& command_id)
{
    for (const auto& def : default_bindings_)
    {
        if (def.command_id == command_id)
        {
            for (auto& binding : bindings_)
            {
                if (binding.command_id == command_id)
                {
                    binding = def;
                    return;
                }
            }
        }
    }
}

void KeybindingEditor::remove_keybinding(const std::string& command_id)
{
    bindings_.erase(std::remove_if(bindings_.begin(),
                                   bindings_.end(),
                                   [&](const KeybindingEntry& binding)
                                   { return binding.command_id == command_id; }),
                    bindings_.end());
}

auto KeybindingEditor::detect_conflicts(int key_code,
                                        int modifiers,
                                        const std::string& /*context*/) const
    -> std::vector<KeybindingConflict>
{
    std::vector<KeybindingConflict> conflicts;
    for (const auto& binding : bindings_)
    {
        if (binding.key_code == key_code && binding.modifiers == modifiers)
        {
            KeybindingConflict conflict;
            conflict.existing = binding;
            conflicts.push_back(std::move(conflict));
        }
    }
    return conflicts;
}

void KeybindingEditor::register_default(KeybindingEntry entry)
{
    default_bindings_.push_back(entry);
    bindings_.push_back(std::move(entry));
}

void KeybindingEditor::register_defaults(std::vector<KeybindingEntry> entries)
{
    for (auto& entry : entries)
    {
        register_default(std::move(entry));
    }
}

auto KeybindingEditor::save_to_json(const std::string& path) const -> bool
{
    std::ofstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    file << "[\n";
    bool first = true;
    for (const auto& binding : bindings_)
    {
        if (binding.source != KeybindingSource::kUser)
        {
            continue;
        }
        if (!first)
        {
            file << ",\n";
        }
        first = false;
        file << "  {\"command\":\"" << binding.command_id << "\",\"key\":\""
             << binding.display_string << "\"}";
    }
    file << "\n]\n";
    return true;
}

auto KeybindingEditor::load_from_json(const std::string& /*path*/) -> bool
{
    // Stub — full JSON parsing requires a JSON library
    return false;
}

auto KeybindingEditor::binding_count() const -> std::size_t
{
    return bindings_.size();
}

auto KeybindingEditor::key_to_string(int key_code, int modifiers) -> std::string
{
    std::string result;
    if (modifiers & 0x1000)
    {
        result += "Cmd+";
    }
    if (modifiers & 0x0100)
    {
        result += "Shift+";
    }
    if (modifiers & 0x0200)
    {
        result += "Alt+";
    }
    if (modifiers & 0x0400)
    {
        result += "Ctrl+";
    }

    if (key_code >= 'A' && key_code <= 'Z')
    {
        result += static_cast<char>(key_code);
    }
    else
    {
        result += std::to_string(key_code);
    }
    return result;
}

} // namespace markamp::core
