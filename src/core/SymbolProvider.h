/// @file SymbolProvider.h
/// @brief V9 Phase 36 Tasks 8, 9 — Document symbol extraction for Go to Symbol.
///
/// Extracts symbols (headings, code blocks, links, frontmatter keys) from
/// a Markdown document. Used by CommandPalette in kGoToSymbol mode ("@" prefix).
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// Kind of a document symbol.
enum class SymbolKind
{
    kHeading,       // Markdown heading (H1-H6)
    kCodeBlock,     // Fenced code block
    kLink,          // Wiki-link or URL
    kFrontmatterKey // YAML frontmatter key
};

/// A single symbol extracted from a document.
struct DocumentSymbol
{
    std::string name;   // Symbol text (heading text, code language, link target, key name)
    SymbolKind kind;    // Type of symbol
    int line{0};        // 1-based line number in the document
    int level{0};       // For headings: 1-6; for others: 0
    std::string detail; // Additional detail (code language, link URL, frontmatter value)
};

/// Returns a human-readable name for a SymbolKind.
[[nodiscard]] inline auto symbol_kind_name(SymbolKind kind) -> std::string
{
    switch (kind)
    {
        case SymbolKind::kHeading:
            return "Heading";
        case SymbolKind::kCodeBlock:
            return "Code Block";
        case SymbolKind::kLink:
            return "Link";
        case SymbolKind::kFrontmatterKey:
            return "Frontmatter";
    }
    return "Unknown";
}

// ============================================================================
// SymbolProvider — extracts document symbols from Markdown content
// ============================================================================

/// Extracts and provides document symbols for the Go to Symbol palette mode.
///
/// Usage:
/// ```cpp
/// SymbolProvider provider;
/// auto symbols = provider.get_symbols(document_content);
/// auto headings = provider.get_headings_only(document_content);
/// ```
class SymbolProvider
{
public:
    SymbolProvider() = default;

    /// Extract all symbols from document content.
    [[nodiscard]] auto get_symbols(const std::string& content) const -> std::vector<DocumentSymbol>;

    /// Extract only headings (for "#" prefix mode).
    [[nodiscard]] auto get_headings_only(const std::string& content) const
        -> std::vector<DocumentSymbol>;

    /// Fuzzy search symbols by name.
    [[nodiscard]] auto search_symbols(const std::string& query,
                                      const std::vector<DocumentSymbol>& symbols) const
        -> std::vector<DocumentSymbol>;

    /// Get the line number for a symbol name (first match).
    [[nodiscard]] auto find_symbol_line(const std::string& name,
                                        const std::vector<DocumentSymbol>& symbols) const -> int;

private:
    /// Parse headings from content.
    [[nodiscard]] static auto parse_headings(const std::string& content)
        -> std::vector<DocumentSymbol>;

    /// Parse fenced code blocks from content.
    [[nodiscard]] static auto parse_code_blocks(const std::string& content)
        -> std::vector<DocumentSymbol>;

    /// Parse links (wiki-links and URLs) from content.
    [[nodiscard]] static auto parse_links(const std::string& content)
        -> std::vector<DocumentSymbol>;

    /// Parse YAML frontmatter keys from content.
    [[nodiscard]] static auto parse_frontmatter(const std::string& content)
        -> std::vector<DocumentSymbol>;
};

} // namespace markamp::core
