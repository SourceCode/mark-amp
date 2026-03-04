/// @file LinkResolver.h
/// @brief V13 Phase 32 Task 6 — Resolves wiki-links, markdown links, and block refs.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// Result of resolving a link.
struct ResolvedLink
{
    std::string document_path; ///< Absolute path to the target file
    int line{0};               ///< Target line (0 if top of file)
    bool success{false};       ///< True if resolution succeeded
    std::string error;         ///< Error message if resolution failed
    std::string anchor;        ///< Heading anchor (if present, e.g., "Section 2")
};

/// Type of link being resolved.
enum class LinkType : uint8_t
{
    kWikiLink = 0,     ///< [[target]] or [[target#anchor]]
    kMarkdownLink = 1, ///< [text](url)
    kBlockRef = 2,     ///< ((block-id))
    kUnknown = 3
};

/// Resolves various link types to file paths and line numbers.
///
/// Supports:
///   - `[[wiki-links]]` — resolve to workspace files
///   - `[text](url)` — resolve relative URLs
///   - `((block-refs))` — resolve block IDs
///   - `[[Doc#anchor]]` — resolve heading anchors
///
/// Usage:
/// ```cpp
/// LinkResolver resolver("/workspace/root");
/// auto result = resolver.resolve_wiki_link("MyDocument");
/// if (result.success) { /* navigate to result.document_path */ }
/// ```
class LinkResolver
{
public:
    /// Construct with workspace root path.
    explicit LinkResolver(std::string workspace_root);

    /// Resolve a wiki-link (without brackets).
    /// E.g., "MyDocument" or "MyDocument#Section 2"
    [[nodiscard]] auto resolve_wiki_link(const std::string& link) const -> ResolvedLink;

    /// Resolve a markdown link URL (relative or absolute).
    /// E.g., "./other.md" or "../docs/readme.md"
    [[nodiscard]] auto resolve_markdown_link(const std::string& url,
                                             const std::string& source_file) const -> ResolvedLink;

    /// Resolve a block reference ID.
    /// E.g., "abc123" from ((abc123))
    [[nodiscard]] auto resolve_block_ref(const std::string& block_id) const -> ResolvedLink;

    /// Parse a raw link token from editor text and determine its type.
    /// Returns the parsed link text and its type.
    [[nodiscard]] static auto parse_link_token(const std::string& token)
        -> std::pair<std::string, LinkType>;

    /// Get the workspace root.
    [[nodiscard]] auto workspace_root() const -> const std::string&;

    /// Register known files in the workspace for resolution.
    void set_workspace_files(std::vector<std::string> files);

    /// Get workspace file count.
    [[nodiscard]] auto workspace_file_count() const -> size_t;

private:
    std::string workspace_root_;
    std::vector<std::string> workspace_files_;

    /// Find a file by name (case-insensitive, with/without .md).
    [[nodiscard]] auto find_file(const std::string& name) const -> std::string;
};

} // namespace markamp::core
