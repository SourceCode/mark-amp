#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Result of evaluating a debug console expression.
struct EvalResult
{
    std::string output;
    bool is_error{false};
    bool is_object{false}; ///< True if result is a structured object
    std::string type_name; ///< Type description for objects
};

/// Handler for a registered evaluation command.
using EvalHandler = std::function<EvalResult(const std::string& args)>;

/// Evaluator for debug console REPL expressions.
/// Supports built-in commands (.help, .clear, .version, .env, .config, .theme)
/// and extensible handlers for custom commands.
class ExpressionEvaluator
{
public:
    ExpressionEvaluator();

    /// Evaluate an expression or command.
    [[nodiscard]] auto evaluate(const std::string& expression) -> EvalResult;

    /// Register a custom command handler (e.g., ".mycommand").
    void register_handler(const std::string& command, EvalHandler handler);

    /// Remove a registered handler.
    void remove_handler(const std::string& command);

    /// Get all registered command names.
    [[nodiscard]] auto registered_commands() const -> std::vector<std::string>;

    /// Check if a string is a known command.
    [[nodiscard]] auto is_command(const std::string& expression) const -> bool;

private:
    std::unordered_map<std::string, EvalHandler> handlers_;

    void register_built_in_commands();

    /// Extract command name from expression (e.g., ".help" from ".help topic").
    static auto extract_command(const std::string& expression) -> std::string;

    /// Extract arguments after the command name.
    static auto extract_args(const std::string& expression) -> std::string;
};

} // namespace markamp::core
