/// test_video_embed_object.cpp — Unit tests
#include "canvas/VideoEmbedObject.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("VideoEmbedObject: compiles", "[video_embed_object]")
{
    static_assert(sizeof(VideoEmbedObject) > 0);
}
