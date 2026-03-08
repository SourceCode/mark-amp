/// test_notebook_service.cpp
#include "core/NotebookService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("NotebookService: type compiles", "[notebook_service]")
{
    static_assert(sizeof(NotebookService) > 0);
}
