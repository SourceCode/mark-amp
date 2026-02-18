#pragma once

/// @file GoToDefinitionProvider.h
/// @brief V9 Phase 19 — Go-to-definition for markdown elements.
///
/// Resolves wiki-links, heading refs, footnote refs, and file links
/// to their target locations.

#include "CodeIntelligenceTypes.h"

#include <optional>
#include <string>

namespace markamp::core
{

/// Provider for go-to-definition navigation in markdown documents.
///
/// Given a cursor position, identifies the reference under cursor and
/// resolves it to a target location (file + line + range).
///
/// Usage:
/// ```cpp
/// GoToDefinitionProvider provider;
/// auto link = provider.provide_definition("[[page]]", 0, 3);
/// if (link) {
///     // link->target_uri, link->target_start_line, etc.
/// }
/// ```
class GoToDefinitionProvider
{
public:
    GoToDefinitionProvider() = default;

    /// Provide go-to-definition at the given position.
    /// @param content Full document content
    /// @param line Cursor line (0-based)
    /// @param character Cursor character offset (0-based)
    /// @param document_uri URI of the current document (for relative path resolution)
    /// @return LocationLink if a definition was found, nullopt otherwise
    [[nodiscard]] auto provide_definition(const std::string& content,
                                          int line,
                                          int character,
                                          const std::string& document_uri = "") const
        -> std::optional<LocationLink>;

    /// Set the workspace root for resolving relative paths.
    void set_workspace_root(const std::string& root);

private:
    std::string workspace_root_;

    /// Resolve a wiki-link [[target]] to a LocationLink.
    [[nodiscard]] auto resolve_wiki_link(const std::string& line_text,
                                         int character,
                                         int line) const -> std::optional<LocationLink>;

    /// Resolve a heading reference #heading to a LocationLink.
    [[nodiscard]] static auto resolve_heading_ref(const std::string& line_text,
                                                  int character,
                                                  int line,
                                                  const std::string& content)
        -> std::optional<LocationLink>;

    /// Resolve a footnote reference [^id] to its definition.
    [[nodiscard]] static auto resolve_footnote_ref(const std::string& line_text,
                                                   int character,
                                                   int line,
                                                   const std::string& content)
        -> std::optional<LocationLink>;

    /// Resolve a markdown link [text](url) to a LocationLink.
    [[nodiscard]] auto resolve_link(const std::string& line_text,
                                    int character,
                                    int line,
                                    const std::string& document_uri) const
        -> std::optional<LocationLink>;

    /// Get line text at given line number.
    [[nodiscard]] static auto get_line_text(const std::string& content, int line) -> std::string;

    /// Find the line number of a heading by slug match.
    [[nodiscard]] static auto find_heading_line(const std::string& content,
                                                const std::string& heading_slug) -> int;

    /// Find the line number of a footnote definition.
    [[nodiscard]] static auto find_footnote_line(const std::string& content,
                                                 const std::string& footnote_id) -> int;

    /// Convert heading text to a URL slug for matching.
    [[nodiscard]] static auto slugify(const std::string& heading) -> std::string;
};

} // namespace markamp::core
