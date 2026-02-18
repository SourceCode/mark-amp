/// test_extension_events.cpp — Unit tests

#include "core/ExtensionEvents.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TextDocumentSaveReason: enum values", "[extension_events]")
{
    REQUIRE(static_cast<uint8_t>(TextDocumentSaveReason::kManual) != static_cast<uint8_t>(TextDocumentSaveReason::kAfterDelay));
}

TEST_CASE("ExtensionEventBus: compiles", "[extension_events]")
{
    static_assert(sizeof(ExtensionEventBus) > 0);
}
