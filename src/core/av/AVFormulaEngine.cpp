#include "AVFormulaEngine.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <numeric>
#include <sstream>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Formula result helpers
// ════════════════════════════════════════════════════════════

auto formula_result_to_string(const AVFormulaResult& result) -> std::string
{
    if (std::holds_alternative<std::monostate>(result))
    {
        return "";
    }
    if (const auto* num = std::get_if<double>(&result))
    {
        // Trim trailing zeros
        std::ostringstream oss;
        oss << *num;
        return oss.str();
    }
    if (const auto* str = std::get_if<std::string>(&result))
    {
        return *str;
    }
    if (const auto* flag = std::get_if<bool>(&result))
    {
        return *flag ? "true" : "false";
    }
    return "";
}

auto formula_result_to_double(const AVFormulaResult& result) -> double
{
    if (const auto* num = std::get_if<double>(&result))
    {
        return *num;
    }
    if (const auto* flag = std::get_if<bool>(&result))
    {
        return *flag ? 1.0 : 0.0;
    }
    if (const auto* str = std::get_if<std::string>(&result))
    {
        try
        {
            return std::stod(*str);
        }
        catch (...)
        {
            return 0.0;
        }
    }
    return 0.0;
}

auto formula_result_to_bool(const AVFormulaResult& result) -> bool
{
    if (const auto* flag = std::get_if<bool>(&result))
    {
        return *flag;
    }
    if (const auto* num = std::get_if<double>(&result))
    {
        return *num != 0.0;
    }
    if (const auto* str = std::get_if<std::string>(&result))
    {
        return !str->empty();
    }
    return false;
}

// ════════════════════════════════════════════════════════════
// Constructor
// ════════════════════════════════════════════════════════════

AVFormulaEngine::AVFormulaEngine()
{
    register_built_in_functions();
}

// ════════════════════════════════════════════════════════════
// Built-in functions
// ════════════════════════════════════════════════════════════

void AVFormulaEngine::register_built_in_functions()
{
    functions_["SUM"] = [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
    {
        double total = 0.0;
        for (const auto& arg : args)
        {
            total += formula_result_to_double(arg);
        }
        return total;
    };

    functions_["AVG"] = [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
    {
        if (args.empty())
        {
            return 0.0;
        }
        double total = 0.0;
        for (const auto& arg : args)
        {
            total += formula_result_to_double(arg);
        }
        return total / static_cast<double>(args.size());
    };

    functions_["MIN"] = [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
    {
        if (args.empty())
        {
            return 0.0;
        }
        double result = formula_result_to_double(args[0]);
        for (size_t idx = 1; idx < args.size(); ++idx)
        {
            result = std::min(result, formula_result_to_double(args[idx]));
        }
        return result;
    };

    functions_["MAX"] = [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
    {
        if (args.empty())
        {
            return 0.0;
        }
        double result = formula_result_to_double(args[0]);
        for (size_t idx = 1; idx < args.size(); ++idx)
        {
            result = std::max(result, formula_result_to_double(args[idx]));
        }
        return result;
    };

    functions_["COUNT"] = [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
    {
        int count = 0;
        for (const auto& arg : args)
        {
            if (!std::holds_alternative<std::monostate>(arg))
            {
                ++count;
            }
        }
        return static_cast<double>(count);
    };

    functions_["IF"] = [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
    {
        if (args.size() < 3)
        {
            return std::monostate{};
        }
        bool condition = formula_result_to_bool(args[0]);
        return condition ? args[1] : args[2];
    };

    functions_["CONCAT"] = [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
    {
        std::string result;
        for (const auto& arg : args)
        {
            result += formula_result_to_string(arg);
        }
        return result;
    };

    functions_["LEN"] = [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
    {
        if (args.empty())
        {
            return 0.0;
        }
        return static_cast<double>(formula_result_to_string(args[0]).length());
    };

    functions_["ABS"] = [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
    {
        if (args.empty())
        {
            return 0.0;
        }
        return std::abs(formula_result_to_double(args[0]));
    };

    functions_["ROUND"] = [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
    {
        if (args.empty())
        {
            return 0.0;
        }
        double value = formula_result_to_double(args[0]);
        int decimals = args.size() > 1 ? static_cast<int>(formula_result_to_double(args[1])) : 0;
        double factor = std::pow(10.0, decimals);
        return std::round(value * factor) / factor;
    };

    functions_["NOT"] = [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
    {
        if (args.empty())
        {
            return true;
        }
        return !formula_result_to_bool(args[0]);
    };
}

// ════════════════════════════════════════════════════════════
// Registration & listing
// ════════════════════════════════════════════════════════════

void AVFormulaEngine::register_function(const std::string& name, AVFormulaFunc func)
{
    std::string upper_name = name;
    std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);
    functions_[upper_name] = std::move(func);
}

auto AVFormulaEngine::list_functions() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(functions_.size());
    for (const auto& [name, _] : functions_)
    {
        names.push_back(name);
    }
    std::sort(names.begin(), names.end());
    return names;
}

// ════════════════════════════════════════════════════════════
// Tokenizer
// ════════════════════════════════════════════════════════════

auto AVFormulaEngine::tokenize(const std::string& expression)
    -> std::expected<std::vector<AVFormulaToken>, std::string>
{
    std::vector<AVFormulaToken> tokens;
    size_t pos = 0;
    const size_t len = expression.size();

    while (pos < len)
    {
        char current = expression[pos];

        // Skip whitespace
        if (std::isspace(static_cast<unsigned char>(current)) != 0)
        {
            ++pos;
            continue;
        }

        // Number
        if (std::isdigit(static_cast<unsigned char>(current)) != 0 || current == '.')
        {
            size_t start = pos;
            bool has_dot = false;
            while (pos < len && (std::isdigit(static_cast<unsigned char>(expression[pos])) != 0 ||
                                 expression[pos] == '.'))
            {
                if (expression[pos] == '.')
                {
                    if (has_dot)
                    {
                        break;
                    }
                    has_dot = true;
                }
                ++pos;
            }
            std::string num_str = expression.substr(start, pos - start);
            AVFormulaToken token;
            token.type = AVFormulaTokenType::Number;
            token.value = num_str;
            token.number_value = std::stod(num_str);
            tokens.push_back(token);
            continue;
        }

        // String literal
        if (current == '"')
        {
            ++pos;
            std::string str_val;
            while (pos < len && expression[pos] != '"')
            {
                if (expression[pos] == '\\' && pos + 1 < len)
                {
                    ++pos;
                }
                str_val += expression[pos];
                ++pos;
            }
            if (pos >= len)
            {
                return std::unexpected("Unterminated string literal");
            }
            ++pos; // skip closing quote
            AVFormulaToken token;
            token.type = AVFormulaTokenType::String;
            token.value = str_val;
            tokens.push_back(token);
            continue;
        }

        // Identifier (function name or cell reference)
        if (std::isalpha(static_cast<unsigned char>(current)) != 0 || current == '_')
        {
            size_t start = pos;
            while (pos < len && (std::isalnum(static_cast<unsigned char>(expression[pos])) != 0 ||
                                 expression[pos] == '_'))
            {
                ++pos;
            }
            std::string ident = expression.substr(start, pos - start);

            // Check for boolean keywords
            std::string upper_ident = ident;
            std::transform(upper_ident.begin(), upper_ident.end(), upper_ident.begin(), ::toupper);

            if (upper_ident == "TRUE")
            {
                AVFormulaToken token;
                token.type = AVFormulaTokenType::BoolTrue;
                token.value = "true";
                tokens.push_back(token);
            }
            else if (upper_ident == "FALSE")
            {
                AVFormulaToken token;
                token.type = AVFormulaTokenType::BoolFalse;
                token.value = "false";
                tokens.push_back(token);
            }
            else if (functions_.contains(upper_ident) && pos < len && expression[pos] == '(')
            {
                AVFormulaToken token;
                token.type = AVFormulaTokenType::Function;
                token.value = upper_ident;
                tokens.push_back(token);
            }
            else
            {
                AVFormulaToken token;
                token.type = AVFormulaTokenType::CellRef;
                token.value = ident;
                tokens.push_back(token);
            }
            continue;
        }

        // Operators and punctuation
        if (current == '+' || current == '-' || current == '*' || current == '/')
        {
            AVFormulaToken token;
            token.type = AVFormulaTokenType::Operator;
            token.value = std::string(1, current);
            tokens.push_back(token);
            ++pos;
            continue;
        }

        if (current == '(')
        {
            AVFormulaToken token;
            token.type = AVFormulaTokenType::LeftParen;
            token.value = "(";
            tokens.push_back(token);
            ++pos;
            continue;
        }

        if (current == ')')
        {
            AVFormulaToken token;
            token.type = AVFormulaTokenType::RightParen;
            token.value = ")";
            tokens.push_back(token);
            ++pos;
            continue;
        }

        if (current == ',')
        {
            AVFormulaToken token;
            token.type = AVFormulaTokenType::Comma;
            token.value = ",";
            tokens.push_back(token);
            ++pos;
            continue;
        }

        // Comparison operators
        if (current == '=' || current == '!' || current == '<' || current == '>')
        {
            std::string comp_op(1, current);
            ++pos;
            if (pos < len && expression[pos] == '=')
            {
                comp_op += '=';
                ++pos;
            }
            AVFormulaToken token;
            token.type = AVFormulaTokenType::Comparison;
            token.value = comp_op;
            tokens.push_back(token);
            continue;
        }

        return std::unexpected("Unexpected character: " + std::string(1, current));
    }

    AVFormulaToken end_token;
    end_token.type = AVFormulaTokenType::End;
    tokens.push_back(end_token);

    return tokens;
}

// ════════════════════════════════════════════════════════════
// Parse
// ════════════════════════════════════════════════════════════

auto AVFormulaEngine::parse(const std::string& expression) -> AVFormula
{
    AVFormula formula;
    formula.expression = expression;

    // Strip leading '='
    std::string expr = expression;
    if (!expr.empty() && expr[0] == '=')
    {
        expr = expr.substr(1);
    }

    auto tokens_result = tokenize(expr);
    if (!tokens_result)
    {
        formula.valid = false;
        formula.error = tokens_result.error();
        return formula;
    }

    formula.tokens = std::move(*tokens_result);
    formula.valid = true;

    // Extract dependencies
    for (const auto& token : formula.tokens)
    {
        if (token.type == AVFormulaTokenType::CellRef)
        {
            formula.dependent_key_ids.insert(token.value);
        }
    }

    return formula;
}

// ════════════════════════════════════════════════════════════
// Cell reference resolution
// ════════════════════════════════════════════════════════════

auto AVFormulaEngine::resolve_cell_ref(const std::string& key_name,
                                       const AttributeView& attribute_view,
                                       const std::string& block_id) const -> AVFormulaResult
{
    // Find key by name
    for (const auto& kv : attribute_view.key_values)
    {
        if (kv.key.name == key_name || kv.key.id == key_name)
        {
            for (const auto& value : kv.values)
            {
                if (value.block_id == block_id)
                {
                    if (const auto* num = value.as_number())
                    {
                        return num->content;
                    }
                    if (const auto* txt = value.as_text())
                    {
                        return txt->content;
                    }
                    if (const auto* chk = value.as_checkbox())
                    {
                        return chk->checked;
                    }
                    return value.to_display_string();
                }
            }
            return std::monostate{};
        }
    }
    return std::monostate{};
}

auto AVFormulaEngine::collect_column_values(const std::string& key_name,
                                            const AttributeView& attribute_view) const
    -> std::vector<AVFormulaResult>
{
    std::vector<AVFormulaResult> results;
    for (const auto& kv : attribute_view.key_values)
    {
        if (kv.key.name == key_name || kv.key.id == key_name)
        {
            for (const auto& value : kv.values)
            {
                if (const auto* num = value.as_number())
                {
                    results.emplace_back(num->content);
                }
                else if (const auto* txt = value.as_text())
                {
                    results.emplace_back(txt->content);
                }
                else if (const auto* chk = value.as_checkbox())
                {
                    results.emplace_back(chk->checked);
                }
                else if (!value.is_empty())
                {
                    results.emplace_back(value.to_display_string());
                }
                else
                {
                    results.emplace_back(std::monostate{});
                }
            }
            break;
        }
    }
    return results;
}

// ════════════════════════════════════════════════════════════
// Recursive-descent evaluator
// ════════════════════════════════════════════════════════════

auto AVFormulaEngine::eval_expression(const std::vector<AVFormulaToken>& tokens,
                                      size_t& pos,
                                      const AttributeView& attribute_view,
                                      const std::string& block_id) const
    -> std::expected<AVFormulaResult, std::string>
{
    return eval_comparison(tokens, pos, attribute_view, block_id);
}

auto AVFormulaEngine::eval_comparison(const std::vector<AVFormulaToken>& tokens,
                                      size_t& pos,
                                      const AttributeView& attribute_view,
                                      const std::string& block_id) const
    -> std::expected<AVFormulaResult, std::string>
{
    auto left = eval_additive(tokens, pos, attribute_view, block_id);
    if (!left)
    {
        return left;
    }

    while (pos < tokens.size() && tokens[pos].type == AVFormulaTokenType::Comparison)
    {
        std::string op_val = tokens[pos].value;
        ++pos;
        auto right = eval_additive(tokens, pos, attribute_view, block_id);
        if (!right)
        {
            return right;
        }

        double left_num = formula_result_to_double(*left);
        double right_num = formula_result_to_double(*right);

        // String comparison for = and !=
        std::string left_str = formula_result_to_string(*left);
        std::string right_str = formula_result_to_string(*right);

        bool result = false;
        if (op_val == "=")
        {
            result = left_str == right_str;
        }
        else if (op_val == "!=")
        {
            result = left_str != right_str;
        }
        else if (op_val == "<")
        {
            result = left_num < right_num;
        }
        else if (op_val == ">")
        {
            result = left_num > right_num;
        }
        else if (op_val == "<=")
        {
            result = left_num <= right_num;
        }
        else if (op_val == ">=")
        {
            result = left_num >= right_num;
        }
        left = AVFormulaResult{result};
    }

    return left;
}

auto AVFormulaEngine::eval_additive(const std::vector<AVFormulaToken>& tokens,
                                    size_t& pos,
                                    const AttributeView& attribute_view,
                                    const std::string& block_id) const
    -> std::expected<AVFormulaResult, std::string>
{
    auto left = eval_multiplicative(tokens, pos, attribute_view, block_id);
    if (!left)
    {
        return left;
    }

    while (pos < tokens.size() && tokens[pos].type == AVFormulaTokenType::Operator &&
           (tokens[pos].value == "+" || tokens[pos].value == "-"))
    {
        std::string op_val = tokens[pos].value;
        ++pos;
        auto right = eval_multiplicative(tokens, pos, attribute_view, block_id);
        if (!right)
        {
            return right;
        }

        // String concatenation with +
        if (op_val == "+" && (std::holds_alternative<std::string>(*left) ||
                              std::holds_alternative<std::string>(*right)))
        {
            left =
                AVFormulaResult{formula_result_to_string(*left) + formula_result_to_string(*right)};
        }
        else
        {
            double lval = formula_result_to_double(*left);
            double rval = formula_result_to_double(*right);
            left = AVFormulaResult{op_val == "+" ? lval + rval : lval - rval};
        }
    }

    return left;
}

auto AVFormulaEngine::eval_multiplicative(const std::vector<AVFormulaToken>& tokens,
                                          size_t& pos,
                                          const AttributeView& attribute_view,
                                          const std::string& block_id) const
    -> std::expected<AVFormulaResult, std::string>
{
    auto left = eval_primary(tokens, pos, attribute_view, block_id);
    if (!left)
    {
        return left;
    }

    while (pos < tokens.size() && tokens[pos].type == AVFormulaTokenType::Operator &&
           (tokens[pos].value == "*" || tokens[pos].value == "/"))
    {
        std::string op_val = tokens[pos].value;
        ++pos;
        auto right = eval_primary(tokens, pos, attribute_view, block_id);
        if (!right)
        {
            return right;
        }

        double lval = formula_result_to_double(*left);
        double rval = formula_result_to_double(*right);

        if (op_val == "/" && rval == 0.0)
        {
            return std::unexpected("Division by zero");
        }

        left = AVFormulaResult{op_val == "*" ? lval * rval : lval / rval};
    }

    return left;
}

auto AVFormulaEngine::eval_primary(const std::vector<AVFormulaToken>& tokens,
                                   size_t& pos,
                                   const AttributeView& attribute_view,
                                   const std::string& block_id) const
    -> std::expected<AVFormulaResult, std::string>
{
    if (pos >= tokens.size())
    {
        return std::unexpected("Unexpected end of expression");
    }

    const auto& token = tokens[pos];

    switch (token.type)
    {
        case AVFormulaTokenType::Number:
            ++pos;
            return AVFormulaResult{token.number_value};

        case AVFormulaTokenType::String:
            ++pos;
            return AVFormulaResult{token.value};

        case AVFormulaTokenType::BoolTrue:
            ++pos;
            return AVFormulaResult{true};

        case AVFormulaTokenType::BoolFalse:
            ++pos;
            return AVFormulaResult{false};

        case AVFormulaTokenType::CellRef:
            ++pos;
            return AVFormulaResult{resolve_cell_ref(token.value, attribute_view, block_id)};

        case AVFormulaTokenType::Function:
            return eval_function_call(token.value, tokens, pos, attribute_view, block_id);

        case AVFormulaTokenType::LeftParen:
        {
            ++pos;
            auto result = eval_expression(tokens, pos, attribute_view, block_id);
            if (!result)
            {
                return result;
            }
            if (pos < tokens.size() && tokens[pos].type == AVFormulaTokenType::RightParen)
            {
                ++pos;
            }
            return result;
        }

        case AVFormulaTokenType::Operator:
            // Unary minus
            if (token.value == "-")
            {
                ++pos;
                auto operand = eval_primary(tokens, pos, attribute_view, block_id);
                if (!operand)
                {
                    return operand;
                }
                return AVFormulaResult{-formula_result_to_double(*operand)};
            }
            return std::unexpected("Unexpected operator: " + token.value);

        default:
            return std::unexpected("Unexpected token: " + token.value);
    }
}

auto AVFormulaEngine::eval_function_call(const std::string& func_name,
                                         const std::vector<AVFormulaToken>& tokens,
                                         size_t& pos,
                                         const AttributeView& attribute_view,
                                         const std::string& block_id) const
    -> std::expected<AVFormulaResult, std::string>
{
    ++pos; // skip function name
    if (pos >= tokens.size() || tokens[pos].type != AVFormulaTokenType::LeftParen)
    {
        return std::unexpected("Expected '(' after function name");
    }
    ++pos; // skip '('

    std::vector<AVFormulaResult> args;

    // Check for aggregate functions that take a column name as argument
    static const std::unordered_set<std::string> kAggregateSet = {
        "SUM", "AVG", "MIN", "MAX", "COUNT"};
    bool is_aggregate = kAggregateSet.contains(func_name);

    if (is_aggregate && pos < tokens.size() && tokens[pos].type == AVFormulaTokenType::CellRef)
    {
        // Aggregate: collect all values for the column
        std::string col_name = tokens[pos].value;
        ++pos;
        args = collect_column_values(col_name, attribute_view);
    }
    else
    {
        // Regular function: evaluate each argument
        if (pos < tokens.size() && tokens[pos].type != AVFormulaTokenType::RightParen)
        {
            auto arg = eval_expression(tokens, pos, attribute_view, block_id);
            if (!arg)
            {
                return arg;
            }
            args.push_back(*arg);

            while (pos < tokens.size() && tokens[pos].type == AVFormulaTokenType::Comma)
            {
                ++pos; // skip comma
                arg = eval_expression(tokens, pos, attribute_view, block_id);
                if (!arg)
                {
                    return arg;
                }
                args.push_back(*arg);
            }
        }
    }

    if (pos >= tokens.size() || tokens[pos].type != AVFormulaTokenType::RightParen)
    {
        return std::unexpected("Expected ')' after function arguments");
    }
    ++pos; // skip ')'

    auto func_iter = functions_.find(func_name);
    if (func_iter == functions_.end())
    {
        return std::unexpected("Unknown function: " + func_name);
    }

    return func_iter->second(args);
}

// ════════════════════════════════════════════════════════════
// Public evaluation methods
// ════════════════════════════════════════════════════════════

auto AVFormulaEngine::evaluate(const AVFormula& formula,
                               const AttributeView& attribute_view,
                               const std::string& block_id) const
    -> std::expected<AVFormulaResult, std::string>
{
    if (!formula.valid)
    {
        return std::unexpected("Invalid formula: " + formula.error);
    }

    if (formula.tokens.empty())
    {
        return AVFormulaResult{std::monostate{}};
    }

    size_t pos = 0;
    return eval_expression(formula.tokens, pos, attribute_view, block_id);
}

auto AVFormulaEngine::evaluate_all(const AVFormula& formula,
                                   const AttributeView& attribute_view) const
    -> std::vector<std::pair<std::string, AVFormulaResult>>
{
    std::vector<std::pair<std::string, AVFormulaResult>> results;
    auto block_ids = attribute_view.row_block_ids();

    for (const auto& bid : block_ids)
    {
        auto result = evaluate(formula, attribute_view, bid);
        if (result)
        {
            results.emplace_back(bid, *result);
        }
        else
        {
            results.emplace_back(bid, std::monostate{});
        }
    }

    return results;
}

auto AVFormulaEngine::get_dependencies(const AVFormula& formula) const
    -> std::unordered_set<std::string>
{
    return formula.dependent_key_ids;
}

} // namespace markamp::core::av
