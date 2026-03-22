/// @file test_v25_p16_rendering.cpp
/// @brief V25 Phase 16: Renderer capability matrix tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P16: Renderer classification", "[v25][p16]")
{
    RendererCapabilityMatrix matrix;
    RendererPathEntry e;
    e.renderer_id = "html";
    e.scope = RendererScope::kSupported;
    e.has_real_output = true;
    matrix.add_entry(e);
    REQUIRE(matrix.supported_renderers().size() == 1);
}

TEST_CASE("V25 P16: Placeholder renderer blocking", "[v25][p16]")
{
    RendererCapabilityMatrix matrix;
    RendererPathEntry e;
    e.renderer_id = "pdf";
    e.scope = RendererScope::kSupported;
    e.is_placeholder = true;
    matrix.add_entry(e);
    REQUIRE(matrix.has_placeholder_on_release_path());
    REQUIRE(matrix.placeholder_renderers().size() == 1);
}

TEST_CASE("V25 P16: Gated placeholder does not block", "[v25][p16]")
{
    RendererCapabilityMatrix matrix;
    RendererPathEntry e;
    e.renderer_id = "video";
    e.scope = RendererScope::kMustGate;
    e.is_placeholder = true;
    matrix.add_entry(e);
    REQUIRE_FALSE(matrix.has_placeholder_on_release_path());
}
