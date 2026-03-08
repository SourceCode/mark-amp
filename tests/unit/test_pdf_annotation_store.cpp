/// test_pdf_annotation_store.cpp
#include "core/PDFAnnotationStore.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("PDFAnnotationStore: type compiles", "[pdf_annotation_store]")
{
    static_assert(sizeof(PDFAnnotationStore) > 0);
}
