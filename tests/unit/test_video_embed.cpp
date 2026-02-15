#include "canvas/VideoEmbedObject.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("VideoEmbedObject: construction defaults", "[video_embed]")
{
    VideoEmbedObject video;
    REQUIRE(video.type() == CanvasObjectType::VideoEmbed);
    REQUIRE(video.display_width() == 480.0);
    REQUIRE(video.display_height() == 270.0);
    REQUIRE(video.url().empty());
    REQUIRE_FALSE(video.is_local_file());
}

TEST_CASE("VideoEmbedObject: set URL", "[video_embed]")
{
    VideoEmbedObject video;
    video.set_url("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
    REQUIRE(video.url() == "https://www.youtube.com/watch?v=dQw4w9WgXcQ");
}

TEST_CASE("VideoEmbedObject: oEmbed data", "[video_embed]")
{
    VideoEmbedObject video;
    OEmbedData data;
    data.title = "Test Video";
    data.provider_name = "YouTube";
    data.thumbnail_url = "https://img.youtube.com/vi/abc/0.jpg";
    video.set_oembed(data);

    REQUIRE(video.oembed().title == "Test Video");
    REQUIRE(video.oembed().provider_name == "YouTube");
    REQUIRE(video.oembed().thumbnail_url == "https://img.youtube.com/vi/abc/0.jpg");
}

TEST_CASE("VideoEmbedObject: local file", "[video_embed]")
{
    VideoEmbedObject video;
    REQUIRE_FALSE(video.is_local_file());

    video.set_local_file("/videos/demo.mp4");
    REQUIRE(video.is_local_file());
    REQUIRE(video.local_file() == std::filesystem::path("/videos/demo.mp4"));
}

TEST_CASE("VideoEmbedObject: JSON round-trip", "[video_embed]")
{
    VideoEmbedObject video;
    video.set_url("https://vimeo.com/12345");
    OEmbedData data;
    data.title = "Demo";
    data.provider_name = "Vimeo";
    video.set_oembed(data);

    const auto json = video.to_json();
    REQUIRE(json.find("vimeo.com/12345") != std::string::npos);
    REQUIRE(json.find("Demo") != std::string::npos);
    REQUIRE(json.find("Vimeo") != std::string::npos);
}

TEST_CASE("VideoEmbedObject: display dimensions", "[video_embed]")
{
    VideoEmbedObject video;
    video.set_display_dimensions(1920.0, 1080.0);
    REQUIRE(video.display_width() == 1920.0);
    REQUIRE(video.display_height() == 1080.0);

    const auto bounds = video.local_bounds();
    REQUIRE(bounds.max_x == 1920.0);
    REQUIRE(bounds.max_y == 1080.0);
}
