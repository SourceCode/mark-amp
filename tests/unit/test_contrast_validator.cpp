#include "core/ContrastRatioValidator.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::core;

TEST_CASE("ContrastRatioValidator calculates WCAG luminance and contrast correctly",
          "[theme][v2][a11y]")
{
    ContrastRatioValidator validator;

    // Black and white
    wxColour black(0, 0, 0);
    wxColour white(255, 255, 255);

    SECTION("Relative Luminance")
    {
        REQUIRE_THAT(validator.calculate_luminance(black), Catch::Matchers::WithinAbs(0.0, 0.01));
        REQUIRE_THAT(validator.calculate_luminance(white), Catch::Matchers::WithinAbs(1.0, 0.01));
    }

    SECTION("Contrast Ratio")
    {
        // Black vs White is 21:1
        REQUIRE_THAT(validator.calculate_contrast_ratio(black, white),
                     Catch::Matchers::WithinAbs(21.0, 0.1));
        REQUIRE_THAT(validator.calculate_contrast_ratio(white, black),
                     Catch::Matchers::WithinAbs(21.0, 0.1));

        // Same color is 1:1
        REQUIRE_THAT(validator.calculate_contrast_ratio(white, white),
                     Catch::Matchers::WithinAbs(1.0, 0.01));
    }

    SECTION("Compliance Levels")
    {
        // 21:1 passes everything
        REQUIRE(validator.passes_wcag_aa(white, black));
        REQUIRE(validator.passes_wcag_aa_large_text(white, black));
        REQUIRE(validator.passes_wcag_aaa(white, black));

        // Low contrast gray on white
        wxColour light_gray(200, 200, 200); // approx 2.43 contrast with white
        REQUIRE_FALSE(validator.passes_wcag_aa(light_gray, white));
        REQUIRE_FALSE(validator.passes_wcag_aa_large_text(light_gray, white));

        // Medium contrast gray on white
        wxColour med_gray(118, 118, 118); // approx 4.5 contrast with white
        REQUIRE(validator.passes_wcag_aa(med_gray, white));
        REQUIRE(validator.passes_wcag_aa_large_text(med_gray, white));
        REQUIRE_FALSE(validator.passes_wcag_aaa(med_gray, white)); // needs 7:1
    }
}
