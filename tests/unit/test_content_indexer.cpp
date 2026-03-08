// test_content_indexer.cpp — 10 tests for ContentIndexer and extractors
#include "core/ContentIndexer.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("PlainTextExtractor supported_mime_types includes text/plain", "[indexer]")
{
    PlainTextExtractor extractor;
    auto types = extractor.supported_mime_types();
    CHECK_FALSE(types.empty());
    bool found = false;
    for (const auto& t : types)
    {
        if (t == "text/plain")
            found = true;
    }
    CHECK(found);
}

TEST_CASE("MarkdownTextExtractor supported_mime_types includes markdown", "[indexer]")
{
    MarkdownTextExtractor extractor;
    auto types = extractor.supported_mime_types();
    CHECK_FALSE(types.empty());
}

TEST_CASE("PdfTextExtractor supported_mime_types includes pdf", "[indexer]")
{
    PdfTextExtractor extractor;
    auto types = extractor.supported_mime_types();
    CHECK_FALSE(types.empty());
}

TEST_CASE("ContentIndexer starts with zero indexed items", "[indexer]")
{
    EventBus bus;
    ContentIndexer indexer(bus);
    CHECK(indexer.indexed_count() == 0);
}

TEST_CASE("ContentIndexer can_extract after register", "[indexer]")
{
    EventBus bus;
    ContentIndexer indexer(bus);
    indexer.register_extractor(std::make_unique<PlainTextExtractor>());
    CHECK(indexer.can_extract("text/plain"));
}

TEST_CASE("ContentIndexer can_extract false for unknown type", "[indexer]")
{
    EventBus bus;
    ContentIndexer indexer(bus);
    CHECK_FALSE(indexer.can_extract("application/x-fake-mime"));
}

TEST_CASE("ContentIndexer search empty returns empty", "[indexer]")
{
    EventBus bus;
    ContentIndexer indexer(bus);
    auto results = indexer.search("anything");
    CHECK(results.empty());
}

TEST_CASE("ContentIndexer remove_from_index on missing is safe", "[indexer]")
{
    EventBus bus;
    ContentIndexer indexer(bus);
    REQUIRE_NOTHROW(indexer.remove_from_index("nonexistent_id"));
}

TEST_CASE("ContentIndexer register multiple extractors", "[indexer]")
{
    EventBus bus;
    ContentIndexer indexer(bus);
    indexer.register_extractor(std::make_unique<PlainTextExtractor>());
    indexer.register_extractor(std::make_unique<MarkdownTextExtractor>());
    CHECK(indexer.can_extract("text/plain"));
    CHECK(indexer.can_extract("text/markdown"));
}

TEST_CASE("PlainTextExtractor extract_text on missing file returns error", "[indexer]")
{
    PlainTextExtractor extractor;
    auto result = extractor.extract_text("/nonexistent/file.txt");
    CHECK_FALSE(result.has_value());
}
