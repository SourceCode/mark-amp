#include "core/ColorBlindnessSimulator.h"

#include <catch2/catch_test_macros.hpp>
#include <wx/colour.h>

using namespace markamp::core;

TEST_CASE("ColorBlindnessSimulator applies color matrices", "[theme][v2][a11y]")
{
    ColorBlindnessSimulator simulator;

    wxColour red(255, 0, 0);
    wxColour green(0, 255, 0);
    wxColour blue(0, 0, 255);

    SECTION("Normal Vision is identity")
    {
        REQUIRE(ColorBlindnessSimulator::simulate(
                    red, ColorBlindnessSimulator::VisionType::Normal) == red);
        REQUIRE(ColorBlindnessSimulator::simulate(
                    green, ColorBlindnessSimulator::VisionType::Normal) == green);
        REQUIRE(ColorBlindnessSimulator::simulate(
                    blue, ColorBlindnessSimulator::VisionType::Normal) == blue);
    }

    SECTION("Achromatopsia is grayscale")
    {
        auto simulated_red = ColorBlindnessSimulator::simulate(
            red, ColorBlindnessSimulator::VisionType::Achromatopsia);
        // Red, green, blue channels should be equal for grayscale
        REQUIRE(simulated_red.Red() == simulated_red.Green());
        REQUIRE(simulated_red.Green() == simulated_red.Blue());
    }

    SECTION("Protanopia affects red perception")
    {
        auto simulated_red =
            ColorBlindnessSimulator::simulate(red, ColorBlindnessSimulator::VisionType::Protanopia);
        // Red channel is significantly reduced or mixed with green
        REQUIRE(simulated_red.Red() < 255);
        REQUIRE(simulated_red != red);
    }
}
