/// @file MarkdownSymbolProvider.h
/// @brief V13 Phase 31 Task 4 — Markdown-specific symbol provider.
///
/// Extracts headings from Markdown documents and presents them as SymbolInfo.
/// Wraps OutlineService for heading extraction.
#pragma once

#include "core/ISymbolProvider.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Extracts symbols from Markdown documents.
///
/// Maps Markdown headings to SymbolInfo entries:
///   - `kind` = SymbolKind::kHeading
///   - `level` = heading level (1-6)
///   - `name` = heading text (without # prefix)
///   - `container_name` = parent heading (if nested)
///
/// Standalone implementation that parses raw markdown text directly,
/// without requiring OutlineService (avoids circular dependency).
class MarkdownSymbolProvider : public ISymbolProvider
{
public:
    /// Get all heading symbols from a Markdown document.
    /// @param document_id  The file path or document identifier.
    /// @return Symbols sorted by document line number.
    [[nodiscard]] auto get_symbols(const std::string& document_id) const
        -> std::vector<SymbolInfo> override;

    /// Check if the document is a Markdown file (.md, .markdown, .mdx).
    [[nodiscard]] auto supports(const std::string& document_id) const -> bool override;

    /// Extract symbols from raw Markdown text (for testing without file I/O).
    [[nodiscard]] static auto extract_from_text(const std::string& text) -> std::vector<SymbolInfo>;
};

} // namespace markamp::core
