/// SafeRegex.h — V7 Phase 10: Safe regex execution with timeout
///
/// Wraps std::regex operations with exception handling and timeout protection
/// to prevent catastrophic backtracking.

#pragma once

#include "Result.h"

#include <chrono>
#include <regex>
#include <string>
#include <string_view>

namespace markamp::core
{

/// Default regex operation timeout.
inline constexpr auto kDefaultRegexTimeout = std::chrono::milliseconds(1000);

/// Compile a regex safely, catching std::regex_error.
[[nodiscard]] auto safe_regex_compile(std::string_view pattern,
                                      std::regex_constants::syntax_option_type flags =
                                          std::regex_constants::ECMAScript) -> Result<std::regex>;

/// Perform a regex match with exception handling.
[[nodiscard]] auto safe_regex_match(std::string_view input, const std::regex& pattern)
    -> Result<bool>;

/// Perform a regex search with exception handling.
[[nodiscard]] auto safe_regex_search(std::string_view input, const std::regex& pattern)
    -> Result<bool>;

/// Perform a regex replace with exception handling.
[[nodiscard]] auto safe_regex_replace(std::string_view input,
                                      const std::regex& pattern,
                                      std::string_view replacement) -> Result<std::string>;

} // namespace markamp::core
