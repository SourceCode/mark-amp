#include "core/SyntaxPresentationCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SyntaxPresentationCoordinator keyword token weight", "[v22][syntax]")
{
    SyntaxPresentationCoordinator coord;
    auto weight = coord.token_weight(SyntaxPresentationCoordinator::TokenPriority::kKeyword);

    REQUIRE(weight.is_bold);
    REQUIRE(weight.opacity == 1.0F);
    REQUIRE(weight.priority == SyntaxPresentationCoordinator::TokenPriority::kKeyword);
}

TEST_CASE("SyntaxPresentationCoordinator comment token weight", "[v22][syntax]")
{
    SyntaxPresentationCoordinator coord;
    auto weight = coord.token_weight(SyntaxPresentationCoordinator::TokenPriority::kComment);

    REQUIRE(weight.is_italic);
    REQUIRE(weight.opacity < 1.0F);
}

TEST_CASE("SyntaxPresentationCoordinator all priorities resolve", "[v22][syntax]")
{
    SyntaxPresentationCoordinator coord;

    for (auto p : {SyntaxPresentationCoordinator::TokenPriority::kKeyword,
                   SyntaxPresentationCoordinator::TokenPriority::kType,
                   SyntaxPresentationCoordinator::TokenPriority::kFunction,
                   SyntaxPresentationCoordinator::TokenPriority::kVariable,
                   SyntaxPresentationCoordinator::TokenPriority::kOperator,
                   SyntaxPresentationCoordinator::TokenPriority::kLiteral,
                   SyntaxPresentationCoordinator::TokenPriority::kString,
                   SyntaxPresentationCoordinator::TokenPriority::kComment})
    {
        auto weight = coord.token_weight(p);
        REQUIRE(weight.opacity > 0.0F);
        REQUIRE(weight.opacity <= 1.0F);
    }
}

TEST_CASE("SyntaxPresentationCoordinator operator opacity is lower than keyword", "[v22][syntax]")
{
    SyntaxPresentationCoordinator coord;
    auto kw = coord.token_weight(SyntaxPresentationCoordinator::TokenPriority::kKeyword);
    auto op = coord.token_weight(SyntaxPresentationCoordinator::TokenPriority::kOperator);

    REQUIRE(kw.opacity >= op.opacity);
}

TEST_CASE("SyntaxPresentationCoordinator comment style", "[v22][syntax]")
{
    SyntaxPresentationCoordinator coord;
    auto style = coord.comment_style();

    REQUIRE(style.is_italic);
    REQUIRE(style.opacity > 0.0F);
    REQUIRE(style.opacity < 1.0F);
    REQUIRE(style.dim_line_comments);
}

TEST_CASE("SyntaxPresentationCoordinator code fence style", "[v22][syntax]")
{
    SyntaxPresentationCoordinator coord;
    auto style = coord.code_fence_style();

    REQUIRE(style.corner == CornerRadiusToken::kMd);
    REQUIRE(style.border == BorderWeightToken::kThin);
    REQUIRE(style.padding_h > 0);
    REQUIRE(style.padding_v > 0);
    REQUIRE(style.bg_opacity > 0.0F);
}

TEST_CASE("SyntaxPresentationCoordinator degradation config", "[v22][syntax]")
{
    SyntaxPresentationCoordinator coord;
    auto config = coord.degradation_config();

    REQUIRE(config.large_file_threshold_lines > 0);
    REQUIRE(config.very_large_threshold_lines > config.large_file_threshold_lines);
}

TEST_CASE("SyntaxPresentationCoordinator should_degrade threshold", "[v22][syntax]")
{
    SyntaxPresentationCoordinator coord;

    REQUIRE_FALSE(coord.should_degrade(100));
    REQUIRE_FALSE(coord.should_degrade(9999));
    REQUIRE(coord.should_degrade(10000));
    REQUIRE(coord.should_degrade(50000));
}

TEST_CASE("SyntaxPresentationCoordinator is_very_large threshold", "[v22][syntax]")
{
    SyntaxPresentationCoordinator coord;

    REQUIRE_FALSE(coord.is_very_large(50000));
    REQUIRE(coord.is_very_large(100000));
    REQUIRE(coord.is_very_large(200000));
}
