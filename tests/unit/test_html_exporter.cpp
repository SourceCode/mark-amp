/// test_html_exporter.cpp — Unit tests
#include "core/HtmlExporter.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("HtmlExporter: compiles", "[html_exporter]")
{
    static_assert(sizeof(HtmlExporter) > 0);
}
