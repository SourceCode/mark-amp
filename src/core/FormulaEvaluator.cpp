/// @file FormulaEvaluator.cpp
/// @brief V9 Phase 48 — FormulaEvaluator implementation.

#include "FormulaEvaluator.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <numeric>
#include <sstream>

namespace markamp::core
{

auto FormulaEvaluator::evaluate(const std::string& expression) const -> FormulaResult
{
    if (expression.empty())
    {
        return {0.0, "", true, "Empty expression"};
    }

    // Try parsing as a simple number
    auto num_result = parse_number(expression);
    if (!num_result.is_error)
    {
        return num_result;
    }

    // Try variable lookup
    if (has_variable(expression))
    {
        double val = get_variable(expression);
        FormulaResult result;
        result.value = val;
        result.text = std::to_string(val);
        return result;
    }

    // Try function call: FUNC(arg1, arg2, ...)
    auto paren_open = expression.find('(');
    auto paren_close = expression.rfind(')');
    if (paren_open != std::string::npos && paren_close != std::string::npos &&
        paren_close > paren_open)
    {
        std::string func_name = expression.substr(0, paren_open);
        // Trim and uppercase function name
        while (!func_name.empty() && func_name.back() == ' ')
        {
            func_name.pop_back();
        }

        std::string args_str = expression.substr(paren_open + 1, paren_close - paren_open - 1);

        // Parse comma-separated arguments
        std::vector<double> args;
        std::istringstream stream(args_str);
        std::string token;
        while (std::getline(stream, token, ','))
        {
            // Trim whitespace
            size_t start = token.find_first_not_of(' ');
            size_t end = token.find_last_not_of(' ');
            if (start == std::string::npos)
            {
                continue;
            }
            std::string trimmed = token.substr(start, end - start + 1);

            // Try as variable first, then as number
            if (has_variable(trimmed))
            {
                args.push_back(get_variable(trimmed));
            }
            else
            {
                auto arg_result = parse_number(trimmed);
                if (arg_result.is_error)
                {
                    return {0.0, "", true, "Invalid argument: " + trimmed};
                }
                args.push_back(arg_result.value);
            }
        }

        return evaluate_function(func_name, args);
    }

    // Simple binary operation: a + b, a - b, a * b, a / b
    for (char op_char : {'+', '-', '*', '/'})
    {
        auto op_pos = expression.rfind(op_char);
        if (op_pos != std::string::npos && op_pos > 0 && op_pos < expression.size() - 1)
        {
            auto left_result = evaluate(expression.substr(0, op_pos));
            auto right_result = evaluate(expression.substr(op_pos + 1));
            if (left_result.is_error || right_result.is_error)
            {
                continue;
            }
            double result_val = 0.0;
            switch (op_char)
            {
                case '+':
                    result_val = left_result.value + right_result.value;
                    break;
                case '-':
                    result_val = left_result.value - right_result.value;
                    break;
                case '*':
                    result_val = left_result.value * right_result.value;
                    break;
                case '/':
                    if (right_result.value == 0.0)
                    {
                        return {0.0, "", true, "Division by zero"};
                    }
                    result_val = left_result.value / right_result.value;
                    break;
                default:
                    break;
            }
            FormulaResult result;
            result.value = result_val;
            result.text = std::to_string(result_val);
            return result;
        }
    }

    return {0.0, "", true, "Unknown expression: " + expression};
}

auto FormulaEvaluator::validate(const std::string& expression) const -> bool
{
    auto result = evaluate(expression);
    return !result.is_error;
}

void FormulaEvaluator::register_variable(const std::string& name, double value)
{
    variables_[name] = value;
}

auto FormulaEvaluator::get_variable(const std::string& name) const -> double
{
    auto iter = variables_.find(name);
    if (iter != variables_.end())
    {
        return iter->second;
    }
    return 0.0;
}

auto FormulaEvaluator::has_variable(const std::string& name) const -> bool
{
    return variables_.find(name) != variables_.end();
}

void FormulaEvaluator::register_function(const std::string& name, CustomFunction func)
{
    custom_functions_[name] = std::move(func);
}

auto FormulaEvaluator::variable_count() const -> int
{
    return static_cast<int>(variables_.size());
}

void FormulaEvaluator::clear_variables()
{
    variables_.clear();
}

auto FormulaEvaluator::evaluate_function(const std::string& func_name,
                                         const std::vector<double>& args) const -> FormulaResult
{
    // Uppercase for comparison
    std::string upper_name = func_name;
    std::transform(upper_name.begin(),
                   upper_name.end(),
                   upper_name.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::toupper(chr)); });

    if (args.empty())
    {
        return {0.0, "", true, "No arguments for function " + func_name};
    }

    FormulaResult result;

    if (upper_name == "SUM")
    {
        result.value = std::accumulate(args.begin(), args.end(), 0.0);
    }
    else if (upper_name == "AVG")
    {
        result.value =
            std::accumulate(args.begin(), args.end(), 0.0) / static_cast<double>(args.size());
    }
    else if (upper_name == "MIN")
    {
        result.value = *std::min_element(args.begin(), args.end());
    }
    else if (upper_name == "MAX")
    {
        result.value = *std::max_element(args.begin(), args.end());
    }
    else if (upper_name == "COUNT")
    {
        result.value = static_cast<double>(args.size());
    }
    else
    {
        // Check custom functions
        auto custom_iter = custom_functions_.find(func_name);
        if (custom_iter != custom_functions_.end())
        {
            result.value = custom_iter->second(args);
        }
        else
        {
            return {0.0, "", true, "Unknown function: " + func_name};
        }
    }

    result.text = std::to_string(result.value);
    return result;
}

auto FormulaEvaluator::parse_number(const std::string& token) const -> FormulaResult
{
    // Trim whitespace
    size_t start = token.find_first_not_of(' ');
    size_t end = token.find_last_not_of(' ');
    if (start == std::string::npos)
    {
        return {0.0, "", true, "Empty token"};
    }
    std::string trimmed = token.substr(start, end - start + 1);

    try
    {
        size_t pos = 0;
        double val = std::stod(trimmed, &pos);
        if (pos == trimmed.size())
        {
            FormulaResult result;
            result.value = val;
            result.text = trimmed;
            return result;
        }
    }
    catch (...)
    {
        // Fall through
    }
    return {0.0, "", true, "Not a number: " + trimmed};
}

} // namespace markamp::core
