/// @file FormulaEvaluator.h
/// @brief V9 Phase 48 — Expression evaluation with built-in functions.
#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Result of evaluating a formula.
struct FormulaResult
{
    double value{0.0};
    std::string text;
    bool is_error{false};
    std::string error_msg;
};

/// Evaluates expressions with variables and built-in functions (SUM, AVG, MIN, MAX, COUNT).
class FormulaEvaluator
{
public:
    using CustomFunction = std::function<double(const std::vector<double>&)>;

    FormulaEvaluator() = default;

    // ── Evaluation ────────────────────────────────────────────────────
    [[nodiscard]] auto evaluate(const std::string& expression) const -> FormulaResult;
    [[nodiscard]] auto validate(const std::string& expression) const -> bool;

    // ── Variables ─────────────────────────────────────────────────────
    void register_variable(const std::string& name, double value);
    [[nodiscard]] auto get_variable(const std::string& name) const -> double;
    [[nodiscard]] auto has_variable(const std::string& name) const -> bool;

    // ── Custom functions ──────────────────────────────────────────────
    void register_function(const std::string& name, CustomFunction func);

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto variable_count() const -> int;
    void clear_variables();

private:
    std::unordered_map<std::string, double> variables_;
    std::unordered_map<std::string, CustomFunction> custom_functions_;

    [[nodiscard]] auto evaluate_function(const std::string& func_name,
                                         const std::vector<double>& args) const -> FormulaResult;
    [[nodiscard]] auto parse_number(const std::string& token) const -> FormulaResult;
};

} // namespace markamp::core
