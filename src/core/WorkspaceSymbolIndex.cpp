/// @file WorkspaceSymbolIndex.cpp
/// @brief V13 Phase 32 Task 4 — Workspace symbol index implementation.

#include "core/WorkspaceSymbolIndex.h"

#include "core/FuzzyScorer.h"

#include <algorithm>

namespace markamp::core
{

void WorkspaceSymbolIndex::rebuild(
    const std::vector<std::pair<std::string, std::vector<SymbolInfo>>>& documents)
{
    index_.clear();
    for (const auto& [doc_id, symbols] : documents)
    {
        index_[doc_id] = symbols;
    }
}

void WorkspaceSymbolIndex::update_document(const std::string& document_id,
                                           std::vector<SymbolInfo> symbols)
{
    index_[document_id] = std::move(symbols);
}

void WorkspaceSymbolIndex::remove_document(const std::string& document_id)
{
    index_.erase(document_id);
}

auto WorkspaceSymbolIndex::search(const std::string& query, int limit) const
    -> std::vector<IndexedSymbol>
{
    struct ScoredSymbol
    {
        IndexedSymbol symbol;
        int match_score;
    };

    std::vector<ScoredSymbol> scored;

    for (const auto& [doc_id, symbols] : index_)
    {
        for (const auto& sym : symbols)
        {
            auto result = FuzzyScorer::score(query, sym.name);
            if (result.score > 0)
            {
                IndexedSymbol indexed;
                indexed.info = sym;
                indexed.document_id = doc_id;
                scored.push_back({std::move(indexed), result.score});
            }
        }
    }

    std::sort(scored.begin(),
              scored.end(),
              [](const ScoredSymbol& lhs, const ScoredSymbol& rhs)
              { return lhs.match_score > rhs.match_score; });

    std::vector<IndexedSymbol> results;
    const int result_count = std::min(limit, static_cast<int>(scored.size()));
    results.reserve(static_cast<size_t>(result_count));

    for (int idx = 0; idx < result_count; ++idx)
    {
        results.push_back(std::move(scored[static_cast<size_t>(idx)].symbol));
    }

    return results;
}

auto WorkspaceSymbolIndex::symbols_for_document(const std::string& document_id) const
    -> std::vector<SymbolInfo>
{
    auto iter = index_.find(document_id);
    if (iter != index_.end())
    {
        return iter->second;
    }
    return {};
}

auto WorkspaceSymbolIndex::symbol_count() const -> size_t
{
    size_t total = 0;
    for (const auto& [doc_id, symbols] : index_)
    {
        total += symbols.size();
    }
    return total;
}

auto WorkspaceSymbolIndex::document_count() const -> size_t
{
    return index_.size();
}

void WorkspaceSymbolIndex::clear()
{
    index_.clear();
}

} // namespace markamp::core
