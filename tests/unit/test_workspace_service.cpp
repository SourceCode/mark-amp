/// test_workspace_service.cpp — Unit tests

#include "core/WorkspaceService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TextDocumentIdentifier: uri", "[workspace_service]")
{
    TextDocumentIdentifier id;
    id.uri = "file:///test.md";
    REQUIRE(id.uri == "file:///test.md");
}

TEST_CASE("ContentChange: default", "[workspace_service]")
{
    ContentChange change;
    REQUIRE(change.text.empty());
}

TEST_CASE("TextDocumentItem: fields", "[workspace_service]")
{
    TextDocumentItem item;
    item.uri = "file:///a.md";
    item.language_id = "markdown";
    REQUIRE(item.language_id == "markdown");
}
