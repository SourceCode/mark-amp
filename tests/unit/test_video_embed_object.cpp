/// test_video_embed_object.cpp
#include "canvas/VideoEmbedObject.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("VideoEmbedObject: type compiles", "[video_embed_object]")
{
    static_assert(sizeof(VideoEmbedObject) > 0);
}
