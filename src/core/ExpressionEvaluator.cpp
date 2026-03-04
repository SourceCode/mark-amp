#include "ExpressionEvaluator.h"

#include <sstream>

namespace markamp::core
{

ExpressionEvaluator::ExpressionEvaluator()
{
    register_built_in_commands();
}

auto ExpressionEvaluator::evaluate(const std::string& expression) -> EvalResult
{
    if (expression.empty())
    {
        return {.output = "", .is_error = false};
    }

    // Check for dot-commands
    if (expression[0] == '.')
    {
        auto cmd = extract_command(expression);
        auto args = extract_args(expression);

        auto handler_it = handlers_.find(cmd);
        if (handler_it != handlers_.end())
        {
            return handler_it->second(args);
        }

        return {.output = "Unknown command: " + cmd + ". Type .help for available commands.",
                .is_error = true};
    }

    // Generic expression evaluation — for now, echo back as result.
    // In a real debugger, this would evaluate against the debug session.
    return {.output = expression, .is_error = false};
}

void ExpressionEvaluator::register_handler(const std::string& command, EvalHandler handler)
{
    handlers_[command] = std::move(handler);
}

void ExpressionEvaluator::remove_handler(const std::string& command)
{
    handlers_.erase(command);
}

auto ExpressionEvaluator::registered_commands() const -> std::vector<std::string>
{
    std::vector<std::string> commands;
    commands.reserve(handlers_.size());
    for (const auto& [cmd, handler] : handlers_)
    {
        commands.push_back(cmd);
    }
    return commands;
}

auto ExpressionEvaluator::is_command(const std::string& expression) const -> bool
{
    if (expression.empty() || expression[0] != '.')
    {
        return false;
    }
    auto cmd = extract_command(expression);
    return handlers_.find(cmd) != handlers_.end();
}

void ExpressionEvaluator::register_built_in_commands()
{
    handlers_[".help"] = [](const std::string& /*args*/) -> EvalResult
    {
        std::ostringstream oss;
        oss << "Available Commands:\n"
            << "  .help      - Show this help message\n"
            << "  .clear     - Clear the console\n"
            << "  .version   - Show application version\n"
            << "  .env       - Show environment info\n"
            << "  .config    - Show current configuration\n"
            << "  .theme     - Show current theme info\n";
        return {.output = oss.str(), .is_error = false};
    };

    handlers_[".clear"] = [](const std::string& /*args*/) -> EvalResult
    {
        // Special return — caller should clear the console.
        return {.output = "__CLEAR__", .is_error = false};
    };

    handlers_[".version"] = [](const std::string& /*args*/) -> EvalResult
    { return {.output = "MarkAmp Studio v2.10.47", .is_error = false}; };

    handlers_[".env"] = [](const std::string& /*args*/) -> EvalResult
    {
        std::ostringstream oss;
        oss << "Platform: ";
#if defined(__APPLE__)
        oss << "macOS";
#elif defined(_WIN32)
        oss << "Windows";
#elif defined(__linux__)
        oss << "Linux";
#else
        oss << "Unknown";
#endif
        oss << "\nCompiler: ";
#if defined(__clang__)
        oss << "Clang " << __clang_major__ << "." << __clang_minor__;
#elif defined(__GNUC__)
        oss << "GCC " << __GNUC__ << "." << __GNUC_MINOR__;
#elif defined(_MSC_VER)
        oss << "MSVC " << _MSC_VER;
#else
        oss << "Unknown";
#endif
        oss << "\nC++ Standard: " << __cplusplus;
        return {.output = oss.str(), .is_error = false};
    };

    handlers_[".config"] = [](const std::string& /*args*/) -> EvalResult
    { return {.output = "Configuration: (use Settings panel for details)", .is_error = false}; };

    handlers_[".theme"] = [](const std::string& /*args*/) -> EvalResult
    { return {.output = "Theme: (current theme details from ThemeEngine)", .is_error = false}; };
}

auto ExpressionEvaluator::extract_command(const std::string& expression) -> std::string
{
    auto space_pos = expression.find(' ');
    if (space_pos != std::string::npos)
    {
        return expression.substr(0, space_pos);
    }
    return expression;
}

auto ExpressionEvaluator::extract_args(const std::string& expression) -> std::string
{
    auto space_pos = expression.find(' ');
    if (space_pos != std::string::npos && space_pos + 1 < expression.size())
    {
        return expression.substr(space_pos + 1);
    }
    return {};
}

} // namespace markamp::core
