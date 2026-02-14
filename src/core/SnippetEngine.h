#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// A parsed tab stop within a snippet body.
/// Supports simple (`$1`), placeholder (`${1:text}`), and choice (`${1|a,b,c|}`) styles.
struct SnippetTabStop
{
    int index{0};                     ///< Tab stop index (0 = final cursor position)
    std::string placeholder;          ///< Default placeholder text
    std::vector<std::string> choices; ///< Choice options (for `${1|a,b,c|}` syntax)
};

/// A single snippet definition, matching VS Code's snippet format.
struct Snippet
{
    std::string name;        ///< Display name (e.g. "Callout")
    std::string prefix;      ///< Trigger text (e.g. "!callout")
    std::string body;        ///< Raw body with tab stops (e.g. "> [!NOTE]\n> $0")
    std::string description; ///< Optional description shown in autocomplete
    std::string scope;       ///< Language scope (e.g. "markdown"), empty = all
};

// Forward declaration for contribution import
struct SnippetContribution;

/// Engine for managing, parsing, and expanding VS Code-style snippets.
///
/// Mirrors VS Code's snippet system: extensions contribute snippets via
/// `contributes.snippets` in their manifest, and the engine handles prefix
/// lookup, body expansion, and tab stop resolution.
///
/// Usage:
/// ```cpp
/// SnippetEngine engine;
/// engine.register_snippet({.name = "Callout", .prefix = "!callout",
///                          .body = "> [!${1|NOTE,TIP,WARNING|}]\n> $0"});
/// auto expanded = engine.expand("!callout");
/// // → "> [!NOTE]\n> "
/// ```
class SnippetEngine
{
public:
    SnippetEngine() = default;

    /// Register a single snippet.
    void register_snippet(Snippet snippet);

    /// Parse and register snippets from VS Code JSON format.
    /// Format: `{ "Name": { "prefix": "...", "body": ["line1", "line2"], "description": "..." } }`
    /// Returns the number of snippets successfully parsed.
    auto register_from_json(const std::string& json_string) -> std::size_t;

    /// Import snippets from PluginManifest contribution points.
    void register_from_contributions(const std::vector<SnippetContribution>& contributions);

    /// Expand a snippet by its exact prefix.
    /// Returns the expanded body with tab stops resolved to their placeholders
    /// (or empty strings if no placeholder). Returns nullopt if no snippet matches.
    [[nodiscard]] auto expand(const std::string& prefix) const -> std::optional<std::string>;

    /// Get all registered snippets.
    [[nodiscard]] auto get_snippets() const -> const std::vector<Snippet>&;

    /// Find snippets whose prefix starts with the given text (for autocomplete).
    [[nodiscard]] auto get_for_prefix(const std::string& prefix_start) const
        -> std::vector<Snippet>;

    /// Get a snippet by exact name.
    [[nodiscard]] auto get_by_name(const std::string& name) const -> const Snippet*;

    /// Total number of registered snippets.
    [[nodiscard]] auto count() const -> std::size_t;

    /// Remove all registered snippets.
    void clear();

    /// Expand a raw snippet body string, resolving tab stops to their placeholder values.
    /// This is a static utility for expanding body text without needing a registered snippet.
    [[nodiscard]] static auto expand_body(const std::string& body) -> std::string;

private:
    std::vector<Snippet> snippets_;
    std::unordered_map<std::string, std::size_t> prefix_index_; ///< prefix → index in snippets_
};

} // namespace markamp::core
