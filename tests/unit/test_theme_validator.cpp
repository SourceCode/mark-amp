/// test_theme_validator.cpp — Unit tests

#include "core/ThemeValidator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ValidationResult: default is valid", "[theme_validator]")
{
    ValidationResult result;
    REQUIRE(result.errors.empty());
}

TEST_CASE("ThemeValidator: default construction", "[theme_validator]")
{
    ThemeValidator validator;
    (void)validator;
}
