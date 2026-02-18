/// test_template_functions.cpp — Unit tests

#include "core/TemplateFunctions.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TemplateFunctionRegistry: default construction", "[template_functions]")
{
    TemplateFunctionRegistry reg;
    (void)reg;
}
