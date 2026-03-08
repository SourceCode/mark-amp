/// test_export_service.cpp — Unit tests for ExportService types
#include "core/ExportService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("ExportService: type compiles", "[export_service]")
{
    static_assert(sizeof(ExportService) > 0);
}

TEST_CASE("ExportFormatDescriptor: default values", "[export_service]")
{
    ExportFormatDescriptor desc;
    REQUIRE(desc.name.empty());
    REQUIRE(desc.description.empty());
}

TEST_CASE("ExportFormatDescriptor: field assignment", "[export_service]")
{
    ExportFormatDescriptor desc;
    desc.name = "PDF";
    desc.description = "Export as PDF document";
    REQUIRE(desc.name == "PDF");
}
