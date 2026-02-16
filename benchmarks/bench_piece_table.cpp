/// bench_piece_table.cpp — Benchmarks for PieceTable insert/delete/getline
///
/// Per Phase 06 doc: measure insert, delete, and line retrieval operations
/// parameterized by document size (1KB, 100KB, 1MB).

#include "core/PieceTable.h"

#include <benchmark/benchmark.h>
#include <cstddef>
#include <string>

using markamp::core::PieceTable;

namespace
{

/// Generate a document of approximately the given size in bytes
auto make_document(std::size_t target_bytes) -> std::string
{
    const std::string line = "The quick brown fox jumps over the lazy dog.\n";
    std::string document;
    document.reserve(target_bytes);
    while (document.size() < target_bytes)
    {
        document += line;
    }
    return document;
}

/// BM_PieceTable_Insert: insert operations at random positions
void BM_PieceTable_Insert(benchmark::State& state)
{
    const auto doc_size = static_cast<std::size_t>(state.range(0));
    const std::string content = make_document(doc_size);
    const std::string insertion = "INSERTED TEXT ";

    for (auto _ : state)
    {
        PieceTable table(content);
        // Insert at beginning, middle, and end to exercise split logic
        table.insert(0, insertion);
        table.insert(table.size() / 2, insertion);
        table.insert(table.size(), insertion);
        benchmark::DoNotOptimize(table.size());
    }
}
BENCHMARK(BM_PieceTable_Insert)->Arg(1024)->Arg(100'000)->Arg(1'000'000);

/// BM_PieceTable_Delete: delete operations
void BM_PieceTable_Delete(benchmark::State& state)
{
    const auto doc_size = static_cast<std::size_t>(state.range(0));
    const std::string content = make_document(doc_size);

    for (auto _ : state)
    {
        PieceTable table(content);
        // Delete from beginning, middle, and end
        const auto table_size = table.size();
        if (table_size > 30)
        {
            table.erase(0, 10);
            table.erase(table.size() / 2, 10);
            if (table.size() > 10)
            {
                table.erase(table.size() - 10, 10);
            }
        }
        benchmark::DoNotOptimize(table.size());
    }
}
BENCHMARK(BM_PieceTable_Delete)->Arg(1024)->Arg(100'000)->Arg(1'000'000);

/// BM_PieceTable_GetText: full text retrieval after edits
void BM_PieceTable_GetText(benchmark::State& state)
{
    const auto doc_size = static_cast<std::size_t>(state.range(0));
    const std::string content = make_document(doc_size);
    PieceTable table(content);

    // Apply some edits to fragment the piece sequence
    for (int idx = 0; idx < 20; ++idx)
    {
        const auto position = static_cast<std::size_t>(idx) * (table.size() / 21);
        table.insert(position, "EDIT");
    }

    for (auto _ : state)
    {
        auto result = table.text();
        benchmark::DoNotOptimize(result.data());
    }
    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(table.size()));
}
BENCHMARK(BM_PieceTable_GetText)->Arg(1024)->Arg(100'000)->Arg(1'000'000);

} // namespace
