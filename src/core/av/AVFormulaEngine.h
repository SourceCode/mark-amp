#pragma once

#include "AVTypes.h"
#include "AVValue.h"
#include "AttributeView.h"

#include <expected>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Formula Token
// ════════════════════════════════════════════════════════════

enum class AVFormulaTokenType : std::uint8_t
{
    Number,
    String,
    CellRef,    // e.g. "Price", references a key name
    Function,   // e.g. SUM, IF, CONCAT
    Operator,   // +, -, *, /
    Comparison, // =, !=, <, >, <=, >=
    LeftParen,
    RightParen,
    Comma,
    BoolTrue,
    BoolFalse,
    End
};

struct AVFormulaToken
{
    AVFormulaTokenType type{AVFormulaTokenType::End};
    std::string value;
    double number_value{0.0};
};

// ════════════════════════════════════════════════════════════
// Formula Result: typed result of evaluation
// ════════════════════════════════════════════════════════════

using AVFormulaResult = std::variant<std::monostate, double, std::string, bool>;

[[nodiscard]] auto formula_result_to_string(const AVFormulaResult& result) -> std::string;
[[nodiscard]] auto formula_result_to_double(const AVFormulaResult& result) -> double;
[[nodiscard]] auto formula_result_to_bool(const AVFormulaResult& result) -> bool;

// ════════════════════════════════════════════════════════════
// Parsed Formula
// ════════════════════════════════════════════════════════════

struct AVFormula
{
    std::string expression;
    std::vector<AVFormulaToken> tokens;
    std::unordered_set<std::string> dependent_key_ids;
    bool valid{false};
    std::string error;
};

// ════════════════════════════════════════════════════════════
// Built-in Function Signature
// ════════════════════════════════════════════════════════════

using AVFormulaFunc = std::function<AVFormulaResult(const std::vector<AVFormulaResult>&)>;

// ════════════════════════════════════════════════════════════
// Formula Engine
// ════════════════════════════════════════════════════════════

class AVFormulaEngine
{
public:
    AVFormulaEngine();

    /// Parse a formula expression (e.g. "=SUM(Price)") into tokenized form.
    [[nodiscard]] auto parse(const std::string& expression) -> AVFormula;

    /// Evaluate a parsed formula for a single row in an AttributeView.
    [[nodiscard]] auto evaluate(const AVFormula& formula,
                                const AttributeView& attribute_view,
                                const std::string& block_id) const
        -> std::expected<AVFormulaResult, std::string>;

    /// Evaluate a formula across all rows, returning one result per row.
    [[nodiscard]] auto evaluate_all(const AVFormula& formula,
                                    const AttributeView& attribute_view) const
        -> std::vector<std::pair<std::string, AVFormulaResult>>;

    /// Get the set of key IDs that a formula depends on.
    [[nodiscard]] auto get_dependencies(const AVFormula& formula) const
        -> std::unordered_set<std::string>;

    /// Register a custom formula function.
    void register_function(const std::string& name, AVFormulaFunc func);

    /// List all registered function names.
    [[nodiscard]] auto list_functions() const -> std::vector<std::string>;

private:
    std::unordered_map<std::string, AVFormulaFunc> functions_;

    /// Tokenize a formula string.
    [[nodiscard]] auto tokenize(const std::string& expression)
        -> std::expected<std::vector<AVFormulaToken>, std::string>;

    /// Recursive-descent evaluation of tokens.
    [[nodiscard]] auto eval_expression(const std::vector<AVFormulaToken>& tokens,
                                       size_t& pos,
                                       const AttributeView& attribute_view,
                                       const std::string& block_id) const
        -> std::expected<AVFormulaResult, std::string>;

    [[nodiscard]] auto eval_comparison(const std::vector<AVFormulaToken>& tokens,
                                       size_t& pos,
                                       const AttributeView& attribute_view,
                                       const std::string& block_id) const
        -> std::expected<AVFormulaResult, std::string>;

    [[nodiscard]] auto eval_additive(const std::vector<AVFormulaToken>& tokens,
                                     size_t& pos,
                                     const AttributeView& attribute_view,
                                     const std::string& block_id) const
        -> std::expected<AVFormulaResult, std::string>;

    [[nodiscard]] auto eval_multiplicative(const std::vector<AVFormulaToken>& tokens,
                                           size_t& pos,
                                           const AttributeView& attribute_view,
                                           const std::string& block_id) const
        -> std::expected<AVFormulaResult, std::string>;

    [[nodiscard]] auto eval_primary(const std::vector<AVFormulaToken>& tokens,
                                    size_t& pos,
                                    const AttributeView& attribute_view,
                                    const std::string& block_id) const
        -> std::expected<AVFormulaResult, std::string>;

    [[nodiscard]] auto eval_function_call(const std::string& func_name,
                                          const std::vector<AVFormulaToken>& tokens,
                                          size_t& pos,
                                          const AttributeView& attribute_view,
                                          const std::string& block_id) const
        -> std::expected<AVFormulaResult, std::string>;

    /// Resolve a cell reference (key name) to a value for a specific row.
    [[nodiscard]] auto resolve_cell_ref(const std::string& key_name,
                                        const AttributeView& attribute_view,
                                        const std::string& block_id) const -> AVFormulaResult;

    /// Collect all values for a key across all rows (for aggregate functions).
    [[nodiscard]] auto collect_column_values(const std::string& key_name,
                                             const AttributeView& attribute_view) const
        -> std::vector<AVFormulaResult>;

    /// Register all built-in functions (SUM, AVG, MIN, MAX, COUNT, IF, etc.).
    void register_built_in_functions();
};

} // namespace markamp::core::av
