// test_formula_evaluator.cpp — 10 tests for FormulaEvaluator
#include "core/FormulaEvaluator.h"

#include <catch2/catch_test_macros.hpp>

#include <cmath>

using namespace markamp::core;

TEST_CASE("FormulaEvaluator evaluates simple number", "[formula]")
{
    FormulaEvaluator eval;
    auto result = eval.evaluate("42");
    CHECK_FALSE(result.is_error);
    CHECK(result.value == 42.0);
}

TEST_CASE("FormulaEvaluator evaluates SUM function", "[formula]")
{
    FormulaEvaluator eval;
    auto result = eval.evaluate("SUM(1,2,3)");
    CHECK_FALSE(result.is_error);
    CHECK(result.value == 6.0);
}

TEST_CASE("FormulaEvaluator evaluates AVG function", "[formula]")
{
    FormulaEvaluator eval;
    auto result = eval.evaluate("AVG(10,20,30)");
    CHECK_FALSE(result.is_error);
    CHECK(result.value == 20.0);
}

TEST_CASE("FormulaEvaluator register and use variable", "[formula]")
{
    FormulaEvaluator eval;
    eval.register_variable("x", 10.0);
    CHECK(eval.has_variable("x"));
    CHECK(eval.get_variable("x") == 10.0);
    CHECK(eval.variable_count() == 1);
}

TEST_CASE("FormulaEvaluator validates expressions", "[formula]")
{
    FormulaEvaluator eval;
    CHECK(eval.validate("42"));
    CHECK(eval.validate("SUM(1,2)"));
}

TEST_CASE("FormulaEvaluator invalid expression returns error", "[formula]")
{
    FormulaEvaluator eval;
    auto result = eval.evaluate("INVALID_FUNC()");
    CHECK(result.is_error);
    CHECK_FALSE(result.error_msg.empty());
}

TEST_CASE("FormulaEvaluator clear_variables removes all", "[formula]")
{
    FormulaEvaluator eval;
    eval.register_variable("a", 1.0);
    eval.register_variable("b", 2.0);
    CHECK(eval.variable_count() == 2);
    eval.clear_variables();
    CHECK(eval.variable_count() == 0);
}

TEST_CASE("FormulaEvaluator custom function", "[formula]")
{
    FormulaEvaluator eval;
    eval.register_function("DOUBLE",
                           [](const std::vector<double>& args) -> double
                           { return args.empty() ? 0.0 : args[0] * 2.0; });
    auto result = eval.evaluate("DOUBLE(21)");
    CHECK_FALSE(result.is_error);
    CHECK(result.value == 42.0);
}

TEST_CASE("FormulaEvaluator MIN and MAX functions", "[formula]")
{
    FormulaEvaluator eval;
    auto min_result = eval.evaluate("MIN(5,3,8,1)");
    CHECK_FALSE(min_result.is_error);
    CHECK(min_result.value == 1.0);

    auto max_result = eval.evaluate("MAX(5,3,8,1)");
    CHECK_FALSE(max_result.is_error);
    CHECK(max_result.value == 8.0);
}

TEST_CASE("FormulaEvaluator COUNT function", "[formula]")
{
    FormulaEvaluator eval;
    auto result = eval.evaluate("COUNT(10,20,30,40)");
    CHECK_FALSE(result.is_error);
    CHECK(result.value == 4.0);
}
