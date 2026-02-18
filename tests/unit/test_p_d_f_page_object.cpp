/// test_p_d_f_page_object.cpp — Unit tests
#include "canvas/PDFPageObject.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("PDFPageObject: compiles", "[p_d_f_page_object]")
{
    static_assert(sizeof(PDFPageObject) > 0);
}
