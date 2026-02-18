/// test_notebook_diff_engine.cpp — Unit tests

#include "core/NotebookDiffEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("NotebookDiffEngine: compiles", "[notebook_diff_engine]")
{
    static_assert(sizeof(NotebookDiffEngine) > 0);
}
