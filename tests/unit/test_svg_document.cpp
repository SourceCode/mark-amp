#include "ui/SvgDocument.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("SvgDocument: parse simple path string", "[svg_document]")
{
    std::string d = "M 10 20 L 30 40 Z";
    auto commands = ParseSvgPath(d);

    REQUIRE(commands.size() == 3);

    CHECK(commands[0].type == 'M');
    REQUIRE(commands[0].args.size() == 2);
    CHECK(commands[0].args[0] == 10.0f);
    CHECK(commands[0].args[1] == 20.0f);

    CHECK(commands[1].type == 'L');
    REQUIRE(commands[1].args.size() == 2);
    CHECK(commands[1].args[0] == 30.0f);
    CHECK(commands[1].args[1] == 40.0f);

    CHECK(commands[2].type == 'Z');
    REQUIRE(commands[2].args.empty());
}

TEST_CASE("SvgDocument: parse tight path string with commas and negatives", "[svg_document]")
{
    std::string d = "m-10.5,20.25l30,-40.5z";
    auto commands = ParseSvgPath(d);

    REQUIRE(commands.size() == 3);

    CHECK(commands[0].type == 'm');
    REQUIRE(commands[0].args.size() == 2);
    CHECK(commands[0].args[0] == -10.5f);
    CHECK(commands[0].args[1] == 20.25f);

    CHECK(commands[1].type == 'l');
    REQUIRE(commands[1].args.size() == 2);
    CHECK(commands[1].args[0] == 30.0f);
    CHECK(commands[1].args[1] == -40.5f);

    CHECK(commands[2].type == 'z');
    REQUIRE(commands[2].args.empty());
}

TEST_CASE("SvgDocument: parse repeated arguments", "[svg_document]")
{
    std::string d = "M 10 20 30 40 50 60";
    auto commands = ParseSvgPath(d);

    // M consumes two arguments, but our current basic parser groups all following numbers into the
    // same command. The spec says subsequent coordinates imply line-to commands, but since our
    // parser just collects args, we expect 6 args on the 'M' command. The IconRenderer will handle
    // the "implicit L" logic.
    REQUIRE(commands.size() == 1);
    CHECK(commands[0].type == 'M');
    REQUIRE(commands[0].args.size() == 6);
    CHECK(commands[0].args[0] == 10.0f);
    CHECK(commands[0].args[2] == 30.0f);
    CHECK(commands[0].args[4] == 50.0f);
}

TEST_CASE("SvgDocument: parse complex path with multiple commands", "[svg_document]")
{
    std::string d = "M10,20 C10,20 30,40 50,60 S70,80 90,100";
    auto commands = ParseSvgPath(d);

    REQUIRE(commands.size() == 3);

    CHECK(commands[0].type == 'M');
    REQUIRE(commands[0].args.size() == 2);

    CHECK(commands[1].type == 'C');
    REQUIRE(commands[1].args.size() == 6);
    CHECK(commands[1].args[0] == 10.0f);
    CHECK(commands[1].args[5] == 60.0f);

    CHECK(commands[2].type == 'S');
    REQUIRE(commands[2].args.size() == 4);
    CHECK(commands[2].args[0] == 70.0f);
    CHECK(commands[2].args[3] == 100.0f);
}

TEST_CASE("SvgDocument: parse XML document", "[svg_document]")
{
    std::string xml = R"(
        <svg viewBox="0 0 24 24" fill="none">
            <path d="M 0 0 L 24 24" fill="red" stroke="blue" stroke-width="2.5" />
            <circle cx="12" cy="12" r="10" />
            <rect x="2" y="2" width="20" height="20" rx="4" />
        </svg>
    )";

    SvgDocument doc;
    bool success = doc.parse(xml);

    REQUIRE(success);

    CHECK(doc.viewBox().x == 0.0f);
    CHECK(doc.viewBox().y == 0.0f);
    CHECK(doc.viewBox().width == 24.0f);
    CHECK(doc.viewBox().height == 24.0f);

    REQUIRE(doc.shapes().size() == 3);

    auto* path = std::get_if<SvgPath>(&doc.shapes()[0]);
    REQUIRE(path != nullptr);
    CHECK(path->commands.size() == 2);
    CHECK(path->fill == "red");
    CHECK(path->stroke == "blue");
    CHECK(path->stroke_width == 2.5f);

    auto* circle = std::get_if<SvgCircle>(&doc.shapes()[1]);
    REQUIRE(circle != nullptr);
    CHECK(circle->cx == 12.0f);
    CHECK(circle->cy == 12.0f);
    CHECK(circle->r == 10.0f);

    auto* rect = std::get_if<SvgRect>(&doc.shapes()[2]);
    REQUIRE(rect != nullptr);
    CHECK(rect->x == 2.0f);
    CHECK(rect->y == 2.0f);
    CHECK(rect->width == 20.0f);
    CHECK(rect->height == 20.0f);
    CHECK(rect->rx == 4.0f);
}
