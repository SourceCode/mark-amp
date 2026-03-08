/// test_language_provider_registry.cpp — Unit tests
#include "core/LanguageProviderRegistry.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("LanguageProviderRegistry: type compiles", "[language_provider_registry]")
{
    static_assert(sizeof(LanguageProviderRegistry) > 0);
}
