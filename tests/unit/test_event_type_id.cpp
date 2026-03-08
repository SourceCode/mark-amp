// test_event_type_id.cpp — 10 tests for EventTypeId and event_type_name
#include "core/EventTypeId.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("EventTypeId names are non-empty for known types", "[event_type_id]")
{
    CHECK(event_type_name(EventTypeId::EditorContentChanged) == "EditorContentChanged");
    CHECK(event_type_name(EventTypeId::ThemeChanged) == "ThemeChanged");
    CHECK(event_type_name(EventTypeId::FileSaved) == "FileSaved");
}

TEST_CASE("EventTypeId Unknown for out-of-range", "[event_type_id]")
{
    CHECK(event_type_name(EventTypeId::_ReservedStart) == "Unknown");
}

TEST_CASE("EventTypeId kMaxEventTypes is 512", "[event_type_id]")
{
    CHECK(kMaxEventTypes == 512);
}

TEST_CASE("EventTypeId CursorPositionChanged is 1", "[event_type_id]")
{
    CHECK(static_cast<uint16_t>(EventTypeId::CursorPositionChanged) == 1);
}

TEST_CASE("EventTypeId file events have correct names", "[event_type_id]")
{
    CHECK(event_type_name(EventTypeId::FileOpened) == "FileOpened");
    CHECK(event_type_name(EventTypeId::ActiveFileChanged) == "ActiveFileChanged");
    CHECK(event_type_name(EventTypeId::FileReloadRequest) == "FileReloadRequest");
}

TEST_CASE("EventTypeId tab events have correct names", "[event_type_id]")
{
    CHECK(event_type_name(EventTypeId::TabSwitched) == "TabSwitched");
    CHECK(event_type_name(EventTypeId::TabCloseRequest) == "TabCloseRequest");
}

TEST_CASE("EventTypeId plugin events have correct names", "[event_type_id]")
{
    CHECK(event_type_name(EventTypeId::PluginActivated) == "PluginActivated");
    CHECK(event_type_name(EventTypeId::ExtensionInstalled) == "ExtensionInstalled");
}

TEST_CASE("EventTypeId canvas events have correct names", "[event_type_id]")
{
    CHECK(event_type_name(EventTypeId::CanvasViewportChanged) == "CanvasViewportChanged");
    CHECK(event_type_name(EventTypeId::CanvasToolChanged) == "CanvasToolChanged");
}

TEST_CASE("EventTypeId git events have correct names", "[event_type_id]")
{
    CHECK(event_type_name(EventTypeId::GitRepoOpened) == "GitRepoOpened");
    CHECK(event_type_name(EventTypeId::GitStatusChanged) == "GitStatusChanged");
    CHECK(event_type_name(EventTypeId::GitCommitCreated) == "GitCommitCreated");
}

TEST_CASE("EventTypeId all named types return non-Unknown", "[event_type_id]")
{
    // Check that the first 52 types all have proper names
    for (uint16_t i = 0; i < 52; ++i)
    {
        auto name = event_type_name(static_cast<EventTypeId>(i));
        CHECK(name != "Unknown");
    }
}
