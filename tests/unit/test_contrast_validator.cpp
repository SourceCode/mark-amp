#include "core/ContrastRatioValidator.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::core;

TEST_CASE("ContrastRatioValidator calculates WCAG luminance and contrast correctly",
          "[theme][v2][a11y]")
{
    // Black and white
    wxColour black(0, 0, 0);
    wxColour white(255, 255, 255);

    SECTION("Contrast Ratio")
    {
        // Black vs White is 21:1
        REQUIRE_THAT(ContrastRatioValidator::calculate_ratio(black, white),
                     Catch::Matchers::WithinAbs(21.0, 0.1));
        REQUIRE_THAT(ContrastRatioValidator::calculate_ratio(white, black),
                     Catch::Matchers::WithinAbs(21.0, 0.1));

        // Same color is 1:1
        REQUIRE_THAT(ContrastRatioValidator::calculate_ratio(white, white),
                     Catch::Matchers::WithinAbs(1.0, 0.01));
    }

    SECTION("Compliance Levels")
    {
        // 21:1 passes everything
        auto bw_res = ContrastRatioValidator::validate(white, black);
        REQUIRE(bw_res.is_aa_compliant());
        REQUIRE(bw_res.large_text >= ContrastRatioValidator::ComplianceLevel::AA);
        REQUIRE(bw_res.is_aaa_compliant());

        // Low contrast gray on white
        wxColour light_gray(200, 200, 200); // approx 2.43 contrast with white
        auto lg_res = ContrastRatioValidator::validate(light_gray, white);
        REQUIRE_FALSE(lg_res.is_aa_compliant());
        REQUIRE_FALSE(lg_res.large_text >= ContrastRatioValidator::ComplianceLevel::AA);

        // Medium contrast gray on white
        wxColour med_gray(118, 118, 118); // approx 4.5 contrast with white
        auto mg_res = ContrastRatioValidator::validate(med_gray, white);
        REQUIRE(mg_res.is_aa_compliant());
        REQUIRE(mg_res.large_text >= ContrastRatioValidator::ComplianceLevel::AA);
        REQUIRE_FALSE(mg_res.is_aaa_compliant()); // needs 7:1
    }
}
