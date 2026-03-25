/// @file test_v20_artifact_creation_service.cpp
/// @brief V20 Phase 01 – ArtifactCreationService unit tests.

#include "core/ArtifactCreationService.h"
#include "core/ArtifactRegistry.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("ArtifactCreationService: construction", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;

    ArtifactCreationService service(bus, registry, config);

    REQUIRE(service.creation_count() == 0);
    REQUIRE(service.creation_count_by_kind(ArtifactKind::kTextFile) == 0);
    REQUIRE(service.creation_count_by_kind(ArtifactKind::kTextFile) == 0);
    REQUIRE(service.creation_count_by_kind(ArtifactKind::kTextFile) == 0);
}

// ============================================================================
// Text file creation
// ============================================================================

TEST_CASE("ArtifactCreationService: create text file", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    auto result = service.create_text_file("test.md", "markdown", "menu");

    REQUIRE(result.ok());
    REQUIRE_FALSE(result.id.empty());
    REQUIRE(registry.count() == 1);

    const auto* record = registry.find(result.id);
    REQUIRE(record != nullptr);
    REQUIRE(record->display_name == "test.md");
    REQUIRE(record->kind == ArtifactKind::kTextFile);
    REQUIRE(record->language_id == "markdown");
    REQUIRE(record->is_unsaved());
}

TEST_CASE("ArtifactCreationService: create text file with default name", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    auto result = service.create_text_file();

    REQUIRE(result.ok());

    const auto* record = registry.find(result.id);
    REQUIRE(record != nullptr);
    REQUIRE_FALSE(record->display_name.empty());
    // Should contain "Untitled"
    REQUIRE(record->display_name.find("Untitled") != std::string::npos);
}

// ============================================================================
// Notebook creation
// ============================================================================

TEST_CASE("ArtifactCreationService: create notebook", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    auto result = service.create_text_file("analysis-nb", "markdown", "palette");

    REQUIRE(result.ok());
    REQUIRE_FALSE(result.id.empty());

    const auto* record = registry.find(result.id);
    REQUIRE(record != nullptr);
    REQUIRE(record->kind == ArtifactKind::kTextFile);
    REQUIRE(record->display_name == "analysis-nb");
    REQUIRE(record->language_id == "markdown");
}

TEST_CASE("ArtifactCreationService: create notebook with default name", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    auto result = service.create_text_file();

    REQUIRE(result.ok());

    const auto* record = registry.find(result.id);
    REQUIRE(record != nullptr);
    REQUIRE_FALSE(record->display_name.empty());
}

// ============================================================================
// Additional text file creation
// ============================================================================

TEST_CASE("ArtifactCreationService: create text file with source", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    auto result = service.create_text_file("whiteboard", "markdown", "shortcut");

    REQUIRE(result.ok());

    const auto* record = registry.find(result.id);
    REQUIRE(record != nullptr);
    REQUIRE(record->kind == ArtifactKind::kTextFile);
    REQUIRE(record->display_name == "whiteboard");
    REQUIRE(record->language_id == "markdown");
}

TEST_CASE("ArtifactCreationService: create text file default has name", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    auto result = service.create_text_file();

    REQUIRE(result.ok());

    const auto* record = registry.find(result.id);
    REQUIRE(record != nullptr);
    REQUIRE_FALSE(record->display_name.empty());
}

// ============================================================================
// Generic create
// ============================================================================

TEST_CASE("ArtifactCreationService: create with request", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kTextFile;
    req.display_name = "custom.py";
    req.language_id = "python";
    req.source = "context-menu";

    auto result = service.create(req);

    REQUIRE(result.ok());

    const auto* record = registry.find(result.id);
    REQUIRE(record != nullptr);
    REQUIRE(record->display_name == "custom.py");
    REQUIRE(record->language_id == "python");
    REQUIRE(record->source == "context-menu");
}

TEST_CASE("ArtifactCreationService: create uses default language", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kTextFile;
    req.display_name = "no-lang.md";
    // language_id left empty

    auto result = service.create(req);
    REQUIRE(result.ok());

    const auto* record = registry.find(result.id);
    REQUIRE(record != nullptr);
    REQUIRE(record->language_id == "markdown"); // Default for text files
}

// ============================================================================
// Counting
// ============================================================================

TEST_CASE("ArtifactCreationService: creation counts", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    service.create_text_file("a.md");
    service.create_text_file("b.md");
    service.create_text_file("nb.markamp-nb");
    service.create_text_file("board.markamp-canvas");

    REQUIRE(service.creation_count() == 4);
    REQUIRE(service.creation_count_by_kind(ArtifactKind::kTextFile) == 4);
}

// ============================================================================
// Event publishing
// ============================================================================

TEST_CASE("ArtifactCreationService: publishes ArtifactCreatedEvent", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    std::string created_id;
    int created_kind = -1;
    std::string created_source;

    auto sub = bus.subscribe<events::ArtifactCreatedEvent>(
        [&](const events::ArtifactCreatedEvent& evt)
        {
            created_id = evt.artifact_id;
            created_kind = evt.artifact_kind;
            created_source = evt.source;
        });

    auto result = service.create_text_file("event-test.md", "markdown", "test-harness");

    REQUIRE_FALSE(created_id.empty());
    REQUIRE(created_kind == static_cast<int>(ArtifactKind::kTextFile));
    REQUIRE(created_source == "test-harness");
}

TEST_CASE("ArtifactCreationService: multiple creates produce unique IDs",
          "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    auto r1 = service.create_text_file("a.md");
    auto r2 = service.create_text_file("b.md");
    auto r3 = service.create_text_file("nb.markamp-nb");

    REQUIRE(r1.id != r2.id);
    REQUIRE(r2.id != r3.id);
    REQUIRE(r1.id != r3.id);
}

// ============================================================================
// All artifacts are initially unsaved
// ============================================================================

TEST_CASE("ArtifactCreationService: all new artifacts are unsaved", "[v20][artifact-creation]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService service(bus, registry, config);

    auto r1 = service.create_text_file("f.md");
    auto r2 = service.create_text_file();
    auto r3 = service.create_text_file();

    REQUIRE(registry.find(r1.id)->is_unsaved());
    REQUIRE(registry.find(r2.id)->is_unsaved());
    REQUIRE(registry.find(r3.id)->is_unsaved());
    REQUIRE_FALSE(registry.find(r1.id)->has_path());
}
