/// @file WorkspaceSymbolIndex.h
/// @brief V13 Phase 32 Task 4 — Cached workspace-wide symbol index.
#pragma once

#include "core/ISymbolProvider.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// An indexed symbol with its source document.
struct IndexedSymbol
{
    SymbolInfo info;         ///< The symbol info
    std::string document_id; ///< Document containing this symbol
};

/// Cached symbol index across all workspace documents.
///
/// Supports incremental updates when documents change and search
/// across all indexed symbols.
class WorkspaceSymbolIndex
{
public:
    /// Rebuild the entire index from a set of documents and their symbols.
    void rebuild(const std::vector<std::pair<std::string, std::vector<SymbolInfo>>>& documents);

    /// Update a single document's symbols (incremental).
    void update_document(const std::string& document_id, std::vector<SymbolInfo> symbols);

    /// Remove a document from the index.
    void remove_document(const std::string& document_id);

    /// Search for symbols matching a query across all documents.
    /// Returns results limited to the given count.
    [[nodiscard]] auto search(const std::string& query, int limit = 100) const
        -> std::vector<IndexedSymbol>;

    /// Get all symbols for a specific document.
    [[nodiscard]] auto symbols_for_document(const std::string& document_id) const
        -> std::vector<SymbolInfo>;

    /// Get the total number of indexed symbols.
    [[nodiscard]] auto symbol_count() const -> size_t;

    /// Get the number of indexed documents.
    [[nodiscard]] auto document_count() const -> size_t;

    /// Clear the entire index.
    void clear();

private:
    /// Per-document symbol storage.
    std::unordered_map<std::string, std::vector<SymbolInfo>> index_;
};

} // namespace markamp::core
