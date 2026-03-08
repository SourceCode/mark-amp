// test_presentation_manager.cpp — 10 tests for PresentationManager
#include "core/PresentationManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("PresentationManager starts empty", "[presentation][manager]")
{
    PresentationManager manager;
    CHECK(manager.presentation_count() == 0);
}

TEST_CASE("PresentationManager create_presentation", "[presentation][manager]")
{
    PresentationManager manager;
    auto pid = manager.create_presentation("My Talk", 10);
    CHECK_FALSE(pid.empty());
    CHECK(manager.presentation_count() == 1);
}

TEST_CASE("PresentationManager find_presentation", "[presentation][manager]")
{
    PresentationManager manager;
    auto pid = manager.create_presentation("Talk", 5);
    auto found = manager.find_presentation(pid);
    REQUIRE(found != nullptr);
    CHECK(found->title == "Talk");
    CHECK(found->slide_count == 5);
}

TEST_CASE("PresentationManager close_presentation", "[presentation][manager]")
{
    PresentationManager manager;
    auto pid = manager.create_presentation("Close Me", 3);
    CHECK(manager.close_presentation(pid));
    CHECK(manager.presentation_count() == 0);
}

TEST_CASE("PresentationManager start/pause/stop", "[presentation][manager]")
{
    PresentationManager manager;
    auto pid = manager.create_presentation("Playback", 10);
    CHECK(manager.start_presentation(pid));
    auto found = manager.find_presentation(pid);
    REQUIRE(found != nullptr);
    CHECK(found->state == PresentationState::kPlaying);
    CHECK(manager.pause_presentation(pid));
    CHECK(manager.stop_presentation(pid));
}

TEST_CASE("PresentationManager next_slide increments", "[presentation][manager]")
{
    PresentationManager manager;
    auto pid = manager.create_presentation("Nav", 5);
    manager.start_presentation(pid);
    CHECK(manager.next_slide(pid));
    auto found = manager.find_presentation(pid);
    REQUIRE(found != nullptr);
    CHECK(found->current_slide == 2);
}

TEST_CASE("PresentationManager go_to_slide", "[presentation][manager]")
{
    PresentationManager manager;
    auto pid = manager.create_presentation("GoTo", 10);
    manager.start_presentation(pid);
    CHECK(manager.go_to_slide(pid, 7));
}

TEST_CASE("PresentationManager toggle_presenter_mode", "[presentation][manager]")
{
    PresentationManager manager;
    auto pid = manager.create_presentation("Presenter", 5);
    CHECK(manager.toggle_presenter_mode(pid));
}

TEST_CASE("PresentationManager set_loop and set_auto_advance", "[presentation][manager]")
{
    PresentationManager manager;
    auto pid = manager.create_presentation("Loop", 5);
    CHECK(manager.set_loop(pid, true));
    CHECK(manager.set_auto_advance(pid, 5));
}

TEST_CASE("PresentationManager close_all", "[presentation][manager]")
{
    PresentationManager manager;
    manager.create_presentation("A", 3);
    manager.create_presentation("B", 5);
    manager.close_all();
    CHECK(manager.presentation_count() == 0);
}
