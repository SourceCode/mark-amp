/// test_html_importer.cpp — Unit tests
#include "core/HtmlImporter.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("HtmlImporter: compiles", "[html_importer]")
{
    static_assert(sizeof(HtmlImporter) > 0);
}
