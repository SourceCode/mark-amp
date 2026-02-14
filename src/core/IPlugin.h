#pragma once

#include "PluginContext.h"

#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ── Contribution Point Types ──

/// A command contributed by a plugin, shown in the command palette.
struct CommandContribution
{
    std::string id;          // e.g. "markdown-extras.insertToc"
    std::string title;       // e.g. "Insert Table of Contents"
    std::string category;    // e.g. "Markdown"
    std::string description; // Tooltip / longer description
};

/// A keybinding contributed by a plugin.
struct KeybindingContribution
{
    std::string command_id; // Must reference a contributed command ID
    int key_code{0};        // wxWidgets key code (WXK_*)
    int modifiers{0};       // wxMOD_CONTROL, wxMOD_ALT, wxMOD_SHIFT, wxMOD_META
    std::string context;    // "global", "editor", "sidebar"
};

/// A snippet contributed by a plugin.
struct SnippetContribution
{
    std::string name;    // e.g. "Callout"
    std::string trigger; // e.g. "!callout"
    std::string body;    // e.g. "> [!NOTE]\n> $0"
};

/// A menu item contributed by a plugin.
struct MenuContribution
{
    std::string command_id; // References a contributed command
    std::string group;      // Menu group: "file", "edit", "view", "tools"
    int order{0};           // Sort order within the group
};

/// A setting contributed by a plugin.
enum class SettingType
{
    Boolean,
    Integer,
    Double,
    String,
    Choice
};

struct SettingContribution
{
    std::string id;          // e.g. "markdown-extras.autoToc"
    std::string label;       // e.g. "Auto-generate TOC"
    std::string description; // Longer description
    std::string category;    // Settings category: "Editor", "Appearance", etc.
    SettingType type{SettingType::Boolean};
    std::string default_value;        // Serialized default
    std::vector<std::string> choices; // Only for SettingType::Choice
};

/// A theme contributed by a plugin.
struct ThemeContribution
{
    std::string id;    // e.g. "solarized-dark"
    std::string label; // e.g. "Solarized Dark"
    std::string path;  // Relative path to theme file within plugin resources
};

// ── Plugin Manifest ──

/// Describes everything a plugin contributes. Modeled after VS Code's
/// `package.json` contribution points (commands, keybindings, snippets,
/// menus, configuration, themes).
struct PluginManifest
{
    std::string id;          // Unique plugin identifier
    std::string name;        // Human-readable name
    std::string version;     // SemVer string
    std::string description; // One-line description
    std::string author;      // Author name

    struct ContributionPoints
    {
        std::vector<CommandContribution> commands;
        std::vector<KeybindingContribution> keybindings;
        std::vector<SnippetContribution> snippets;
        std::vector<MenuContribution> menus;
        std::vector<SettingContribution> settings;
        std::vector<ThemeContribution> themes;
    } contributes;
};

// ── Plugin Interface ──

/// Abstract interface that all plugins must implement.
///
/// Lifecycle:
///   1. Plugin is constructed and registered with PluginManager
///   2. `manifest()` is called to read contribution points
///   3. `activate(ctx)` is called during app startup → plugin wires its handlers
///   4. `deactivate()` is called during app shutdown → plugin cleans up
///
/// Example:
/// ```cpp
/// class MarkdownExtrasPlugin : public IPlugin {
///     const PluginManifest& manifest() const override { return manifest_; }
///     void activate(PluginContext& ctx) override { /* register handlers */ }
///     void deactivate() override { /* cleanup */ }
/// };
/// ```
class IPlugin
{
public:
    virtual ~IPlugin() = default;

    /// Return the plugin's manifest describing all contribution points.
    [[nodiscard]] virtual auto manifest() const -> const PluginManifest& = 0;

    /// Called when the plugin is activated. Use the context to register
    /// command handlers and subscribe to events.
    virtual void activate(PluginContext& ctx) = 0;

    /// Called when the plugin is deactivated (app shutdown or user request).
    /// Clean up any resources.
    virtual void deactivate() = 0;

    /// Whether the plugin is currently active.
    [[nodiscard]] virtual auto is_active() const -> bool
    {
        return active_;
    }

protected:
    bool active_{false};
};

} // namespace markamp::core
