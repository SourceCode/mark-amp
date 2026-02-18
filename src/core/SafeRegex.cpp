/// SafeRegex.cpp — V7 Phase 10: Safe regex execution

#include "SafeRegex.h"

namespace markamp::core
{

auto safe_regex_compile(std::string_view pattern, std::regex_constants::syntax_option_type flags)
    -> Result<std::regex>
{
    try
    {
        return std::regex(std::string(pattern), flags);
    }
    catch (const std::regex_error& regex_err)
    {
        return std::unexpected(
            make_error(ErrorCode::RegexError,
                       SubsystemId::Core,
                       std::string("Invalid regex pattern: ") + regex_err.what()));
    }
}

auto safe_regex_match(std::string_view input, const std::regex& pattern) -> Result<bool>
{
    try
    {
        auto input_str = std::string(input);
        return std::regex_match(input_str, pattern);
    }
    catch (const std::regex_error& regex_err)
    {
        return std::unexpected(make_error(ErrorCode::RegexError,
                                          SubsystemId::Core,
                                          std::string("Regex match failed: ") + regex_err.what()));
    }
}

auto safe_regex_search(std::string_view input, const std::regex& pattern) -> Result<bool>
{
    try
    {
        auto input_str = std::string(input);
        return std::regex_search(input_str, pattern);
    }
    catch (const std::regex_error& regex_err)
    {
        return std::unexpected(make_error(ErrorCode::RegexError,
                                          SubsystemId::Core,
                                          std::string("Regex search failed: ") + regex_err.what()));
    }
}

auto safe_regex_replace(std::string_view input,
                        const std::regex& pattern,
                        std::string_view replacement) -> Result<std::string>
{
    try
    {
        auto input_str = std::string(input);
        return std::regex_replace(input_str, pattern, std::string(replacement));
    }
    catch (const std::regex_error& regex_err)
    {
        return std::unexpected(
            make_error(ErrorCode::RegexError,
                       SubsystemId::Core,
                       std::string("Regex replace failed: ") + regex_err.what()));
    }
}

} // namespace markamp::core
