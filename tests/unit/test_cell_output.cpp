/// @file test_cell_output.cpp
/// @brief V4 Phase 31 – CellOutputRenderer tests.

#include "core/EventBus.h"
#include "ui/CellOutputRenderer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

// ============================================================================
// Test fixture helper
// ============================================================================

struct OutputFixture
{
    EventBus event_bus;
    CellOutputRenderer renderer{event_bus};
};

// ============================================================================
// MIME bundle rendering
// ============================================================================

TEST_CASE("CellOutputRenderer renders text/plain from MimeBundle", "[phase31][cell_output]")
{
    OutputFixture ctx;
    MimeBundle bundle;
    bundle.data["text/plain"] = "Hello, World!";

    auto output = ctx.renderer.render_output(bundle);
    REQUIRE(output.type == OutputType::kPlainText);
    REQUIRE(output.content == "Hello, World!");
    REQUIRE(output.mime_type == "text/plain");
}

TEST_CASE("CellOutputRenderer selects text/html over text/plain", "[phase31][cell_output]")
{
    OutputFixture ctx;
    MimeBundle bundle;
    bundle.data["text/plain"] = "plain";
    bundle.data["text/html"] = "<b>bold</b>";

    auto output = ctx.renderer.render_output(bundle);
    REQUIRE(output.type == OutputType::kHtml);
    REQUIRE(output.content == "<b>bold</b>");
    REQUIRE(output.mime_type == "text/html");
}

TEST_CASE("CellOutputRenderer selects image/png over text/plain", "[phase31][cell_output]")
{
    OutputFixture ctx;
    MimeBundle bundle;
    bundle.data["text/plain"] = "fallback";
    bundle.data["image/png"] = "iVBORw0KGgo=";

    auto output = ctx.renderer.render_output(bundle);
    REQUIRE(output.type == OutputType::kImage);
    REQUIRE(output.mime_type == "image/png");
}

TEST_CASE("CellOutputRenderer selects application/json", "[phase31][cell_output]")
{
    OutputFixture ctx;
    MimeBundle bundle;
    bundle.data["text/plain"] = "fallback";
    bundle.data["application/json"] = R"({"key":"value"})";

    auto output = ctx.renderer.render_output(bundle);
    REQUIRE(output.type == OutputType::kJson);
    REQUIRE(output.mime_type == "application/json");
}

// ============================================================================
// Error rendering
// ============================================================================

TEST_CASE("CellOutputRenderer renders error with traceback", "[phase31][cell_output]")
{
    OutputFixture ctx;
    std::vector<std::string> traceback = {
        "File \"test.py\", line 1", "  x = 1/0", "ZeroDivisionError"};

    auto output = ctx.renderer.render_error("ZeroDivisionError", "division by zero", traceback);
    REQUIRE(output.type == OutputType::kError);
    REQUIRE(output.is_error);
    REQUIRE(output.content.find("ZeroDivisionError: division by zero") != std::string::npos);
    REQUIRE(output.traceback.size() == 3);
}

TEST_CASE("CellOutputRenderer renders error without traceback", "[phase31][cell_output]")
{
    OutputFixture ctx;
    auto output = ctx.renderer.render_error("ValueError", "invalid", {});
    REQUIRE(output.content == "ValueError: invalid");
    REQUIRE(output.traceback.empty());
}

// ============================================================================
// Stream rendering
// ============================================================================

TEST_CASE("CellOutputRenderer renders stdout stream", "[phase31][cell_output]")
{
    OutputFixture ctx;
    auto output = ctx.renderer.render_stream("stdout", "Hello from print!");
    REQUIRE(output.type == OutputType::kStream);
    REQUIRE(output.stream_name == "stdout");
    REQUIRE(output.content == "Hello from print!");
}

// ============================================================================
// ANSI stripping
// ============================================================================

TEST_CASE("CellOutputRenderer strips ANSI escape codes", "[phase31][cell_output]")
{
    // Bold red text: \033[1;31mERROR\033[0m
    std::string ansi = "\033[1;31mERROR\033[0m normal";
    auto result = CellOutputRenderer::ansi_to_styled(ansi);
    REQUIRE(result == "ERROR normal");
}

TEST_CASE("CellOutputRenderer passes through plain text unchanged", "[phase31][cell_output]")
{
    std::string plain = "no escape codes here";
    auto result = CellOutputRenderer::ansi_to_styled(plain);
    REQUIRE(result == plain);
}

// ============================================================================
// Output state management
// ============================================================================

TEST_CASE("CellOutputRenderer manages cell output state", "[phase31][cell_output]")
{
    OutputFixture ctx;

    // Initially no state.
    REQUIRE(ctx.renderer.get_state("cell-1") == nullptr);

    // Add outputs.
    CellOutput output;
    output.type = OutputType::kPlainText;
    output.content = "result";
    ctx.renderer.add_output("cell-1", output);

    auto* state = ctx.renderer.get_state("cell-1");
    REQUIRE(state != nullptr);
    REQUIRE(state->outputs.size() == 1);
    REQUIRE(state->outputs[0].content == "result");
}

TEST_CASE("CellOutputRenderer clears outputs for a cell", "[phase31][cell_output]")
{
    OutputFixture ctx;

    CellOutput output;
    output.content = "data";
    ctx.renderer.add_output("cell-1", output);
    ctx.renderer.clear_outputs("cell-1");

    auto* state = ctx.renderer.get_state("cell-1");
    REQUIRE(state != nullptr);
    REQUIRE(state->outputs.empty());
    REQUIRE(state->cleared);
}

TEST_CASE("CellOutputRenderer toggles collapsed state", "[phase31][cell_output]")
{
    OutputFixture ctx;

    ctx.renderer.toggle_collapsed("cell-1");
    auto* state = ctx.renderer.get_state("cell-1");
    REQUIRE(state != nullptr);
    REQUIRE(state->collapsed == true);

    ctx.renderer.toggle_collapsed("cell-1");
    state = ctx.renderer.get_state("cell-1");
    REQUIRE(state->collapsed == false);
}
