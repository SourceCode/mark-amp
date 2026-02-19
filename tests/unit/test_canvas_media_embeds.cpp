// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/MediaEmbedModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Media metadata", "[media][metadata]")
{
    MediaEmbedModel model;
    model.set_source("https://example.com/video.mp4");
    model.set_poster("thumb.jpg");
    model.set_duration_seconds(120.5);
    REQUIRE(model.source() == "https://example.com/video.mp4");
    REQUIRE(model.poster() == "thumb.jpg");
    REQUIRE(model.duration_seconds() == 120.5);
}

TEST_CASE("Playback state machine", "[media][playback]")
{
    MediaEmbedModel model;
    REQUIRE(model.playback_state() == PlaybackState::kStopped);
    model.play();
    REQUIRE(model.playback_state() == PlaybackState::kPlaying);
    model.pause();
    REQUIRE(model.playback_state() == PlaybackState::kPaused);
    model.stop();
    REQUIRE(model.playback_state() == PlaybackState::kStopped);
}

TEST_CASE("Mute toggle", "[media][playback]")
{
    MediaEmbedModel model;
    REQUIRE_FALSE(model.is_muted());
    model.set_muted(true);
    REQUIRE(model.is_muted());
}

TEST_CASE("Aspect lock default on", "[media][aspect]")
{
    MediaEmbedModel model;
    REQUIRE(model.aspect_locked());
    model.set_aspect_locked(false);
    REQUIRE_FALSE(model.aspect_locked());
}

TEST_CASE("Load state and error", "[media][error]")
{
    MediaEmbedModel model;
    REQUIRE(model.load_state() == MediaLoadState::kPlaceholder);
    model.set_load_state(MediaLoadState::kError);
    model.set_error_message("File not found");
    REQUIRE(model.load_state() == MediaLoadState::kError);
    REQUIRE(model.error_message() == "File not found");
}

TEST_CASE("Duration clamped to positive", "[media][metadata]")
{
    MediaEmbedModel model;
    model.set_duration_seconds(-10.0);
    REQUIRE(model.duration_seconds() == 0.0);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
