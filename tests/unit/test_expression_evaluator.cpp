// test_expression_evaluator.cpp — 10 tests for ExpressionEvaluator
#include "core/ExpressionEvaluator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ExpressionEvaluator .help returns non-error", "[expression]")
{
    ExpressionEvaluator eval;
    auto result = eval.evaluate(".help");
    CHECK_FALSE(result.is_error);
    CHECK_FALSE(result.output.empty());
}

TEST_CASE("ExpressionEvaluator .version returns output", "[expression]")
{
    ExpressionEvaluator eval;
    auto result = eval.evaluate(".version");
    CHECK_FALSE(result.is_error);
    CHECK_FALSE(result.output.empty());
}

TEST_CASE("ExpressionEvaluator is_command recognizes dot commands", "[expression]")
{
    ExpressionEvaluator eval;
    CHECK(eval.is_command(".help"));
    CHECK(eval.is_command(".version"));
    CHECK_FALSE(eval.is_command("not a command"));
}

TEST_CASE("ExpressionEvaluator registered_commands includes built-ins", "[expression]")
{
    ExpressionEvaluator eval;
    auto commands = eval.registered_commands();
    CHECK_FALSE(commands.empty());
}

TEST_CASE("ExpressionEvaluator custom handler can be registered", "[expression]")
{
    ExpressionEvaluator eval;
    eval.register_handler(".greet",
                          [](const std::string& args) -> EvalResult
                          { return {.output = "Hello " + args, .is_error = false}; });
    CHECK(eval.is_command(".greet"));
    auto result = eval.evaluate(".greet World");
    CHECK_FALSE(result.is_error);
    CHECK(result.output == "Hello World");
}

TEST_CASE("ExpressionEvaluator remove_handler removes command", "[expression]")
{
    ExpressionEvaluator eval;
    eval.register_handler(".test_cmd",
                          [](const std::string&) -> EvalResult { return {.output = "test"}; });
    CHECK(eval.is_command(".test_cmd"));
    eval.remove_handler(".test_cmd");
    CHECK_FALSE(eval.is_command(".test_cmd"));
}

TEST_CASE("ExpressionEvaluator unknown command returns error", "[expression]")
{
    ExpressionEvaluator eval;
    auto result = eval.evaluate(".nonexistent_xyz");
    CHECK(result.is_error);
}

TEST_CASE("ExpressionEvaluator plain text is evaluated", "[expression]")
{
    ExpressionEvaluator eval;
    auto result = eval.evaluate("2 + 2");
    // Non-command text should be handled (may return result or echo)
    CHECK_FALSE(result.output.empty());
}

TEST_CASE("ExpressionEvaluator .clear returns non-error", "[expression]")
{
    ExpressionEvaluator eval;
    auto result = eval.evaluate(".clear");
    CHECK_FALSE(result.is_error);
}

TEST_CASE("ExpressionEvaluator empty expression is handled", "[expression]")
{
    ExpressionEvaluator eval;
    auto result = eval.evaluate("");
    // Should not crash; may return empty or echo
    CHECK_FALSE(result.is_error);
}
