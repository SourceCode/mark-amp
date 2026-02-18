/// NumericGuards.h — V7 Phase 11: Numeric range enforcement
///
/// Prevents integer overflow/underflow and enforces config value ranges.

#pragma once

#include "Result.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Config Clamping
// ══════════════════════════════════════════════════════════════════════════════

/// Clamp a config value to [min, max] range.
template <typename T>
[[nodiscard]] constexpr auto clamp_config(T value, T min_val, T max_val) noexcept -> T
{
    return std::clamp(value, min_val, max_val);
}

// ══════════════════════════════════════════════════════════════════════════════
// Checked Arithmetic
// ══════════════════════════════════════════════════════════════════════════════

/// Add two values, returning an error on overflow.
template <typename T>
[[nodiscard]] auto checked_add(T lhs, T rhs) -> Result<T>
{
    static_assert(std::is_integral_v<T>, "checked_add requires integral types");

    if constexpr (std::is_signed_v<T>)
    {
        // Use compiler built-in if available
#if defined(__GNUC__) || defined(__clang__)
        T result{};
        if (__builtin_add_overflow(lhs, rhs, &result))
        {
            return std::unexpected(
                make_validation_error("Integer overflow in addition: " + std::to_string(lhs) +
                                          " + " + std::to_string(rhs),
                                      ErrorCode::IntegerOverflow));
        }
        return result;
#else
        // Manual overflow check
        if ((rhs > 0 && lhs > std::numeric_limits<T>::max() - rhs) ||
            (rhs < 0 && lhs < std::numeric_limits<T>::min() - rhs))
        {
            return std::unexpected(
                make_validation_error("Integer overflow in addition", ErrorCode::IntegerOverflow));
        }
        return static_cast<T>(lhs + rhs);
#endif
    }
    else
    {
#if defined(__GNUC__) || defined(__clang__)
        T result{};
        if (__builtin_add_overflow(lhs, rhs, &result))
        {
            return std::unexpected(
                make_validation_error("Unsigned overflow in addition: " + std::to_string(lhs) +
                                          " + " + std::to_string(rhs),
                                      ErrorCode::IntegerOverflow));
        }
        return result;
#else
        if (lhs > std::numeric_limits<T>::max() - rhs)
        {
            return std::unexpected(
                make_validation_error("Unsigned overflow in addition", ErrorCode::IntegerOverflow));
        }
        return static_cast<T>(lhs + rhs);
#endif
    }
}

/// Multiply two values, returning an error on overflow.
template <typename T>
[[nodiscard]] auto checked_multiply(T lhs, T rhs) -> Result<T>
{
    static_assert(std::is_integral_v<T>, "checked_multiply requires integral types");

#if defined(__GNUC__) || defined(__clang__)
    T result{};
    if (__builtin_mul_overflow(lhs, rhs, &result))
    {
        return std::unexpected(
            make_validation_error("Integer overflow in multiplication: " + std::to_string(lhs) +
                                      " * " + std::to_string(rhs),
                                  ErrorCode::IntegerOverflow));
    }
    return result;
#else
    if (lhs != 0 && rhs != 0)
    {
        if constexpr (std::is_signed_v<T>)
        {
            if ((lhs > 0 && rhs > 0 && lhs > std::numeric_limits<T>::max() / rhs) ||
                (lhs < 0 && rhs < 0 && lhs < std::numeric_limits<T>::max() / rhs) ||
                (lhs > 0 && rhs < 0 && rhs < std::numeric_limits<T>::min() / lhs) ||
                (lhs < 0 && rhs > 0 && lhs < std::numeric_limits<T>::min() / rhs))
            {
                return std::unexpected(make_validation_error("Integer overflow in multiplication",
                                                             ErrorCode::IntegerOverflow));
            }
        }
        else
        {
            if (lhs > std::numeric_limits<T>::max() / rhs)
            {
                return std::unexpected(make_validation_error("Unsigned overflow in multiplication",
                                                             ErrorCode::IntegerOverflow));
            }
        }
    }
    return static_cast<T>(lhs * rhs);
#endif
}

/// Subtract two unsigned values, returning an error on underflow.
template <typename T>
[[nodiscard]] auto safe_subtract(T lhs, T rhs) -> Result<T>
{
    static_assert(std::is_integral_v<T>, "safe_subtract requires integral types");

    if constexpr (std::is_unsigned_v<T>)
    {
        if (rhs > lhs)
        {
            return std::unexpected(
                make_validation_error("Unsigned underflow in subtraction: " + std::to_string(lhs) +
                                          " - " + std::to_string(rhs),
                                      ErrorCode::IntegerUnderflow));
        }
        return static_cast<T>(lhs - rhs);
    }
    else
    {
#if defined(__GNUC__) || defined(__clang__)
        T result{};
        if (__builtin_sub_overflow(lhs, rhs, &result))
        {
            return std::unexpected(
                make_validation_error("Integer overflow in subtraction: " + std::to_string(lhs) +
                                          " - " + std::to_string(rhs),
                                      ErrorCode::IntegerOverflow));
        }
        return result;
#else
        if ((rhs > 0 && lhs < std::numeric_limits<T>::min() + rhs) ||
            (rhs < 0 && lhs > std::numeric_limits<T>::max() + rhs))
        {
            return std::unexpected(make_validation_error("Integer overflow in subtraction",
                                                         ErrorCode::IntegerOverflow));
        }
        return static_cast<T>(lhs - rhs);
#endif
    }
}

/// Safe cast from one integral type to another with range checking.
template <typename To, typename From>
[[nodiscard]] auto safe_cast(From value) -> Result<To>
{
    static_assert(std::is_integral_v<From> && std::is_integral_v<To>,
                  "safe_cast requires integral types");

    if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>)
    {
        if (value < 0)
        {
            return std::unexpected(make_validation_error("Negative value " + std::to_string(value) +
                                                             " cannot be cast to unsigned type",
                                                         ErrorCode::InvalidRange));
        }
    }

    if (static_cast<std::common_type_t<From, To>>(value) >
        static_cast<std::common_type_t<From, To>>(std::numeric_limits<To>::max()))
    {
        return std::unexpected(
            make_validation_error("Value " + std::to_string(value) + " exceeds target type range",
                                  ErrorCode::IntegerOverflow));
    }

    if constexpr (std::is_signed_v<To>)
    {
        if constexpr (std::is_signed_v<From>)
        {
            if (value < std::numeric_limits<To>::min())
            {
                return std::unexpected(make_validation_error("Value " + std::to_string(value) +
                                                                 " below target type minimum",
                                                             ErrorCode::IntegerUnderflow));
            }
        }
    }

    return static_cast<To>(value);
}

} // namespace markamp::core
