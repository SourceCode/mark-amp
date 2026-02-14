#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ── Activation Events ──

/// Represents a VS Code-style activation event.
/// Examples: "onLanguage:markdown", "onCommand:ext.foo", "*", "onStartupFinished"
enum class ActivationEventKind
{
    kOnLanguage,        // "onLanguage:<langId>"
    kOnCommand,         // "onCommand:<commandId>"
    kOnView,            // "onView:<viewId>"
    kOnUri,             // "onUri"
    kOnFileSystem,      // "onFileSystem:<scheme>"
    kOnCustomEditor,    // "onCustomEditor:<viewType>"
    kOnStartupFinished, // "onStartupFinished"
    kStar,              // "*"  (activate on startup)
    kUnknown            // Unrecognized activation event
};

struct ActivationEvent
{
    ActivationEventKind kind{ActivationEventKind::kUnknown};
    std::string argument; // e.g. "markdown" for "onLanguage:markdown"
    std::string raw;      // Original string, e.g. "onLanguage:markdown"

    /// Parse a raw activation event string into an ActivationEvent.
    static auto parse(const std::string& raw_event) -> ActivationEvent;
};

// ── Extension Identifier ──

/// Case-insensitive identifier in "publisher.name" format, matching VS Code's
/// `ExtensionIdentifier` class.
class ExtensionIdentifier
{
public:
    ExtensionIdentifier() = default;
    ExtensionIdentifier(const std::string& publisher, const std::string& name);

    /// Parse from a "publisher.name" string.
    static auto from_string(const std::string& identifier_string) -> ExtensionIdentifier;

    /// Compare two identifiers (case-insensitive).
    [[nodiscard]] auto equals(const ExtensionIdentifier& other) const -> bool;

    /// Return the canonical "publisher.name" key (lowercased).
    [[nodiscard]] auto to_key() const -> std::string;

    /// Return "publisher.name" preserving original case.
    [[nodiscard]] auto to_string() const -> std::string;

    [[nodiscard]] auto publisher() const -> const std::string&
    {
        return publisher_;
    }
    [[nodiscard]] auto name() const -> const std::string&
    {
        return name_;
    }

    auto operator==(const ExtensionIdentifier& other) const -> bool
    {
        return equals(other);
    }
    auto operator!=(const ExtensionIdentifier& other) const -> bool
    {
        return !equals(other);
    }

private:
    std::string publisher_;
    std::string name_;

    /// Lowercase helper (ASCII only).
    static auto to_lower(const std::string& str) -> std::string;
};

// ── Extension Contributions ──

/// Describes a single command contributed by an extension via package.json.
struct ExtensionCommand
{
    std::string command;  // e.g. "myExt.sayHello"
    std::string title;    // e.g. "Say Hello"
    std::string category; // e.g. "My Extension"
    std::string icon;     // Optional icon path
};

/// A keybinding contributed by an extension.
struct ExtensionKeybinding
{
    std::string command; // References a contributed command
    std::string key;     // e.g. "ctrl+shift+p"
    std::string mac;     // macOS override, e.g. "cmd+shift+p"
    std::string when;    // Context expression, e.g. "editorTextFocus"
};

/// A language contribution.
struct ExtensionLanguage
{
    std::string language_id;             // e.g. "markdown"
    std::vector<std::string> extensions; // e.g. [".md", ".mdx"]
    std::vector<std::string> aliases;    // e.g. ["Markdown", "md"]
    std::string configuration;           // Path to language configuration file
};

/// A grammar (TextMate) contribution.
struct ExtensionGrammar
{
    std::string language;   // Language ID this grammar is for
    std::string scope_name; // e.g. "source.markdown"
    std::string path;       // Path to the grammar file (.tmLanguage.json)
};

/// A theme contribution.
struct ExtensionTheme
{
    std::string theme_id; // e.g. "monokai"
    std::string label;    // e.g. "Monokai"
    std::string ui_theme; // "vs-dark", "vs", "hc-black", "hc-light"
    std::string path;     // Path to the theme file
};

/// A snippet contribution.
struct ExtensionSnippet
{
    std::string language; // Language ID
    std::string path;     // Path to the snippets file
};

/// A configuration contribution (settings declared by an extension).
struct ExtensionConfiguration
{
    std::string title; // Category title in settings UI
    struct Property
    {
        std::string key;  // e.g. "myExt.setting1"
        std::string type; // "boolean", "string", "number", "array", "object"
        std::string description;
        std::string default_value;            // Serialized default
        std::vector<std::string> enum_values; // For enum types
    };
    std::vector<Property> properties;
};

/// A views container contribution.
struct ExtensionViewsContainer
{
    std::string container_id; // e.g. "myExtExplorer"
    std::string title;        // e.g. "My Extension Explorer"
    std::string icon;         // Icon path
};

/// A view contribution.
struct ExtensionView
{
    std::string view_id; // e.g. "myExtView1"
    std::string name;    // Display name
    std::string when;    // Visibility context expression
};

/// A color contribution.
struct ExtensionColor
{
    std::string color_id; // e.g. "myExt.decorationColor"
    std::string description;
    struct Defaults
    {
        std::string dark;
        std::string light;
        std::string high_contrast;
    } defaults;
};

/// A menu item contribution.
struct ExtensionMenuItem
{
    std::string command; // References a contributed command
    std::string when;    // Visibility context expression
    std::string group;   // Menu group like "navigation"
};

/// A submenu contribution (VS Code's contributes.submenus).
struct ExtensionSubmenu
{
    std::string submenu_id; // e.g. "myExt.submenu1"
    std::string label;      // Display label
    std::string icon;       // Optional icon path
};

/// A single step in a getting-started walkthrough.
struct ExtensionWalkthroughStep
{
    std::string step_id;                        // e.g. "myExt.step1"
    std::string title;                          // Step title
    std::string description;                    // Markdown description
    std::string media_path;                     // Optional image/SVG path
    std::string media_type;                     // "image", "svg", "markdown"
    std::string when;                           // Visibility when-clause
    std::vector<std::string> completion_events; // e.g. ["onCommand:myExt.run"]
};

/// A getting-started walkthrough contribution (VS Code's contributes.walkthroughs).
struct ExtensionWalkthrough
{
    std::string walkthrough_id; // e.g. "myExt.gettingStarted"
    std::string title;          // Walkthrough title
    std::string description;    // Overview description
    std::string icon;           // Icon path
    std::string when;           // Visibility when-clause
    std::vector<ExtensionWalkthroughStep> steps;
};

/// A custom editor contribution (VS Code's contributes.customEditors).
struct ExtensionCustomEditor
{
    std::string view_type;    // e.g. "myExt.catEdit"
    std::string display_name; // e.g. "Cat Editor"
    struct Selector
    {
        std::string file_name_pattern; // e.g. "*.cat"
    };
    std::vector<Selector> selectors;
    std::string priority; // "default", "option"
};

/// A task definition contribution (VS Code's contributes.taskDefinitions).
struct ExtensionTaskDefinition
{
    std::string type;                                        // e.g. "myCustomTask"
    std::vector<std::string> required;                       // Required properties
    std::unordered_map<std::string, std::string> properties; // Property name → type
};

/// A problem pattern used by problem matchers.
struct ExtensionProblemPattern
{
    std::string name;   // Pattern name (when named)
    std::string regexp; // Regular expression
    int file{1};        // Match group for file
    int line{2};        // Match group for line
    int column{0};      // Match group for column (0 = unused)
    int severity{0};    // Match group for severity (0 = unused)
    int message{3};     // Match group for message
};

/// A problem matcher contribution (VS Code's contributes.problemMatchers).
struct ExtensionProblemMatcher
{
    std::string name;          // e.g. "$myMatcher"
    std::string owner;         // e.g. "myExt"
    std::string file_location; // "absolute", "relative", "autoDetect"
    std::string source;        // Source label
    std::vector<ExtensionProblemPattern> patterns;
};

/// A terminal profile contribution (VS Code's contributes.terminal.profiles).
struct ExtensionTerminalProfile
{
    std::string profile_id; // e.g. "myExt.customShell"
    std::string title;      // Display title
    std::string icon;       // Optional icon ID (codicon name)
};

/// A status bar item contribution (VS Code's contributes.statusBarItems).
struct ExtensionStatusBarItem
{
    std::string item_id;    // e.g. "myExt.statusItem"
    std::string name;       // Display name (for settings)
    std::string text;       // Initial text
    std::string tooltip;    // Tooltip text
    std::string command;    // Command to run on click
    std::string alignment;  // "left" or "right"
    int priority{0};        // Sort order
    std::string access_key; // Accessibility key
};

/// A JSON validation contribution (VS Code's contributes.jsonValidation).
struct ExtensionJsonValidation
{
    std::string file_match; // Glob pattern, e.g. "*.myconfig.json"
    std::string url;        // URL or path to JSON schema
};

/// An icon theme contribution (VS Code's contributes.iconThemes).
struct ExtensionIconTheme
{
    std::string theme_id; // e.g. "my-icon-theme"
    std::string label;    // Human-readable label
    std::string path;     // Path to the icon theme definition file
};

/// A product icon theme contribution (VS Code's contributes.productIconThemes).
struct ExtensionProductIconTheme
{
    std::string theme_id; // e.g. "my-product-icons"
    std::string label;    // Human-readable label
    std::string path;     // Path to the product icon theme definition file
};

/// A resource label formatter contribution (VS Code's contributes.resourceLabelFormatters).
struct ExtensionResourceLabelFormatter
{
    std::string scheme;    // URI scheme, e.g. "ftp"
    std::string authority; // Optional authority pattern
    struct Formatting
    {
        std::string label;     // Label template with ${path}, ${authority}
        std::string separator; // Path separator
        std::string strip_path_starting_separator{"false"}; // "true" or "false"
    } formatting;
};

/// All contribution points from an extension's package.json "contributes" field.
/// Mirrors VS Code's IExtensionContributions.
struct ExtensionContributions
{
    // ── Existing contribution vectors ──
    std::vector<ExtensionCommand> commands;
    std::vector<ExtensionConfiguration> configuration;
    std::vector<ExtensionKeybinding> keybindings;
    std::vector<ExtensionLanguage> languages;
    std::vector<ExtensionGrammar> grammars;
    std::vector<ExtensionTheme> themes;
    std::vector<ExtensionSnippet> snippets;
    std::vector<ExtensionViewsContainer> views_containers;
    std::vector<ExtensionView> views;
    std::vector<ExtensionColor> colors;
    std::vector<ExtensionMenuItem> menus;

    // ── New contribution vectors (VS Code parity) ──
    std::vector<ExtensionSubmenu> submenus;
    std::vector<ExtensionWalkthrough> walkthroughs;
    std::vector<ExtensionCustomEditor> custom_editors;
    std::vector<ExtensionTaskDefinition> task_definitions;
    std::vector<ExtensionProblemPattern> problem_patterns;
    std::vector<ExtensionProblemMatcher> problem_matchers;
    std::vector<ExtensionTerminalProfile> terminal_profiles;
    std::vector<ExtensionStatusBarItem> status_bar_items;
    std::vector<ExtensionJsonValidation> json_validations;
    std::vector<ExtensionIconTheme> icon_themes;
    std::vector<ExtensionProductIconTheme> product_icon_themes;
    std::vector<ExtensionResourceLabelFormatter> resource_label_formatters;
};

// ── Extension Manifest ──

/// Repository metadata.
struct RepositoryInfo
{
    std::string type; // "git"
    std::string url;  // e.g. "https://github.com/foo/bar"
};

/// VS Code-compatible extension manifest parsed from package.json.
/// Mirrors IExtensionManifest.
struct ExtensionManifest
{
    // -- Required fields --
    std::string name;      // Extension name (package.json "name")
    std::string version;   // SemVer version
    std::string publisher; // Publisher identifier

    // -- Optional display fields --
    std::string display_name; // Human-readable name
    std::string description;  // One-line description
    std::string icon;         // Path to icon file
    std::string license;      // License identifier

    // -- VS Code engine constraint --
    std::string engines_vscode; // e.g. "^1.60.0"

    // -- Entry point --
    std::string main; // Relative path to extension entry point

    // -- Activation --
    std::vector<ActivationEvent> activation_events;

    // -- Categories and tags --
    std::vector<std::string> categories;
    std::vector<std::string> keywords;

    // -- Dependencies --
    std::vector<std::string> extension_dependencies; // Other extension IDs
    std::vector<std::string> extension_pack;         // Extension pack members

    // -- Contribution points --
    ExtensionContributions contributes;

    // -- Repository info --
    std::optional<RepositoryInfo> repository;
    std::string bugs_url;

    /// Derive an ExtensionIdentifier from publisher + name.
    [[nodiscard]] auto identifier() const -> ExtensionIdentifier;
};

// ── Manifest Parser ──

/// Parse VS Code-compatible package.json into ExtensionManifest.
class ManifestParser
{
public:
    /// Parse a JSON string into an ExtensionManifest.
    /// Throws std::runtime_error on invalid JSON or missing required fields.
    static auto parse(const std::string& json_string) -> ExtensionManifest;

    /// Read and parse a package.json file from disk.
    /// Throws std::runtime_error on file read failure or invalid content.
    static auto parse_file(const std::string& path) -> ExtensionManifest;
};

} // namespace markamp::core
