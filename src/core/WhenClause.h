#pragma once

#include "ContextKeyService.h"

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace markamp::core
{

// ── AST Node Types ──

/// Type of a when-clause AST node.
enum class WhenClauseNodeKind
{
    kAnd,         // expr1 && expr2
    kOr,          // expr1 || expr2
    kNot,         // !expr
    kEquals,      // key == value
    kNotEquals,   // key != value
    kRegexMatch,  // key =~ /pattern/
    kHasKey,      // bare context key (truthy check)
    kLiteralTrue, // literal true
    kLiteralFalse // literal false
};

/// AST node for a parsed when-clause expression.
struct WhenClauseNode
{
    WhenClauseNodeKind kind;

    /// For kHasKey: the context key name.
    /// For kEquals/kNotEquals: the key (left side).
    /// For kRegexMatch: the key.
    std::string key;

    /// For kEquals/kNotEquals: the value (right side).
    /// For kRegexMatch: the regex pattern.
    std::string value;

    /// For kAnd, kOr: left and right children.
    /// For kNot: only left child.
    std::shared_ptr<WhenClauseNode> left;
    std::shared_ptr<WhenClauseNode> right;
};

// ── Parser ──

/// Parses a VS Code `when` expression string into an AST.
///
/// Supported syntax:
///   - Bare key: `editorTextFocus` (truthy check)
///   - Equality: `resourceScheme == file`
///   - Inequality: `resourceScheme != untitled`
///   - Negation: `!editorReadonly`
///   - Logical AND: `expr1 && expr2`
///   - Logical OR: `expr1 || expr2`
///   - Regex match: `resourceFilename =~ /^makefile$/i`
///   - Parentheses: `(expr1 || expr2) && expr3`
///   - Literals: `true`, `false`
///
/// Operator precedence: `!` > `==`/`!=`/`=~` > `&&` > `||`
class WhenClauseParser
{
public:
    /// Parse an expression string. Returns nullptr for empty/whitespace input.
    /// Throws std::runtime_error on malformed expressions.
    static auto parse(const std::string& expression) -> std::shared_ptr<WhenClauseNode>;

private:
    explicit WhenClauseParser(std::string expression);

    auto parse_or() -> std::shared_ptr<WhenClauseNode>;
    auto parse_and() -> std::shared_ptr<WhenClauseNode>;
    auto parse_unary() -> std::shared_ptr<WhenClauseNode>;
    auto parse_primary() -> std::shared_ptr<WhenClauseNode>;

    void skip_whitespace();
    [[nodiscard]] auto peek() const -> char;
    auto advance() -> char;
    [[nodiscard]] auto at_end() const -> bool;
    auto match(char expected) -> bool;
    auto read_token() -> std::string;
    auto read_regex_pattern() -> std::string;

    std::string input_;
    std::size_t pos_{0};
};

// ── Evaluator ──

/// Evaluates a parsed when-clause AST against a ContextKeyService.
class WhenClauseEvaluator
{
public:
    /// Evaluate an expression against context keys. Returns true if expression
    /// matches the current context. Returns true for null nodes (empty expression).
    static auto evaluate(const std::shared_ptr<WhenClauseNode>& node,
                         const ContextKeyService& context) -> bool;

    /// Convenience: parse and evaluate in one step.
    static auto matches(const std::string& expression, const ContextKeyService& context) -> bool;

private:
    static auto evaluate_regex(const std::string& text, const std::string& pattern) -> bool;

    static auto context_value_as_string(const ContextKeyService& context, const std::string& key)
        -> std::string;
};

} // namespace markamp::core
