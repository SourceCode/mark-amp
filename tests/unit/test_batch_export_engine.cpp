// test_batch_export_engine.cpp — 10 tests for BatchExportEngine
#include "core/BatchExportEngine.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("BatchExportEngine is not cancelled initially", "[batch_export]")
{
    EventBus bus;
    BatchExportEngine engine(bus);
    CHECK_FALSE(engine.is_cancelled());
}

TEST_CASE("BatchExportEngine cancel sets cancelled flag", "[batch_export]")
{
    EventBus bus;
    BatchExportEngine engine(bus);
    engine.cancel();
    CHECK(engine.is_cancelled());
}

TEST_CASE("BatchExportEngine slugify lowercases and hyphenates", "[batch_export]")
{
    auto slug = BatchExportEngine::slugify("Hello World!");
    CHECK(slug == "hello-world");
}

TEST_CASE("BatchExportEngine slugify handles special chars", "[batch_export]")
{
    auto slug = BatchExportEngine::slugify("My Doc (v2.0) — Draft");
    CHECK_FALSE(slug.empty());
    // Should not contain parens, em-dash, or spaces
    CHECK(slug.find(' ') == std::string::npos);
    CHECK(slug.find('(') == std::string::npos);
}

TEST_CASE("BatchExportEngine resolve_filename with title token", "[batch_export]")
{
    auto name =
        BatchExportEngine::resolve_filename("{title}", "My Document", 1, "html", "2025-01-01");
    CHECK(name.find("my-document") != std::string::npos);
}

TEST_CASE("BatchExportEngine resolve_filename with index token", "[batch_export]")
{
    auto name =
        BatchExportEngine::resolve_filename("{index}_{title}", "Doc", 5, "pdf", "2025-01-01");
    CHECK(name.find("5") != std::string::npos);
}

TEST_CASE("BatchExportEngine generate_index produces HTML", "[batch_export]")
{
    std::vector<BatchItemResult> items = {
        {.document_id = "1", .title = "Doc A", .output_filename = "doc-a.html", .success = true},
        {.document_id = "2", .title = "Doc B", .output_filename = "doc-b.html", .success = true},
    };
    auto html = BatchExportEngine::generate_index("Test Batch", items);
    CHECK_FALSE(html.empty());
    CHECK(html.find("Doc A") != std::string::npos);
    CHECK(html.find("Doc B") != std::string::npos);
}

TEST_CASE("BatchExportResult default values", "[batch_export]")
{
    BatchExportResult result;
    CHECK_FALSE(result.completed);
    CHECK_FALSE(result.cancelled);
    CHECK(result.total == 0);
    CHECK(result.succeeded == 0);
    CHECK(result.failed == 0);
}

TEST_CASE("BatchExportJob default naming pattern", "[batch_export]")
{
    BatchExportJob job;
    CHECK(job.naming_pattern == "{title}");
    CHECK(job.generate_index_page);
    CHECK_FALSE(job.stop_on_error);
}

TEST_CASE("BatchDocumentEntry fields", "[batch_export]")
{
    BatchDocumentEntry entry{
        .document_id = "doc1",
        .title = "Test Document",
        .markdown_source = "# Title\nContent",
    };
    CHECK(entry.document_id == "doc1");
    CHECK(entry.title == "Test Document");
    CHECK_FALSE(entry.markdown_source.empty());
}
