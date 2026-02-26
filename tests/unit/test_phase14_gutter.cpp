#include "ui/CodeLensProvider.h"
#include "ui/GitGutterProvider.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("Phase14_GitGutterProvider Instantiation", "[gutter]")
{
    GitGutterProvider provider;
    REQUIRE(provider.GetProviderId() == "provider.git_gutter");
    REQUIRE(provider.GetDecorations().empty());
}

TEST_CASE("Phase14_CodeLensProvider Instantiation", "[gutter]")
{
    CodeLensProvider provider;
    REQUIRE(provider.GetProviderId() == "provider.codelens");
    REQUIRE(provider.GetDecorations().empty());
}
