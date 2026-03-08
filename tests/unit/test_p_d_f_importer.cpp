/// test_p_d_f_importer.cpp
#include "canvas/PDFImporter.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("PDFImporter: type compiles", "[pdf_importer]")
{
    static_assert(sizeof(PDFImporter) > 0);
}
