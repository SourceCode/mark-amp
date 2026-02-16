/// ThreadBoundary.h — Phase 22: Typed exception boundary utilities
///
/// Replaces catch(...) patterns with typed exception chains.
/// Provides safe_call<R> wrapper and SafeCallResult<R> for error reporting.
///
/// Pattern implemented: #22 Exception boundaries at thread boundaries

#pragma once

#include <chrono>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

#ifdef NDEBUG
#define MARKAMP_ASSERT_MAIN_THREAD() ((void)0)
#else
#define MARKAMP_ASSERT_MAIN_THREAD() markamp::core::assert_main_thread_impl(__FILE__, __LINE__)
#endif

namespace markamp::core
{

/// Status of a safe_call invocation.
enum class SafeCallStatus : uint8_t
{
    Success,
    Exception,
    Timeout,
};

/// Result of a safe_call invocation with detailed error info.
template <typename T>
struct SafeCallResult
{
    SafeCallStatus status{SafeCallStatus::Success};
    T value{};
    std::string error_message;
    int64_t duration_us{0};

    [[nodiscard]] auto ok() const noexcept -> bool
    {
        return status == SafeCallStatus::Success;
    }
};

/// Specialization for void return type.
template <>
struct SafeCallResult<void>
{
    SafeCallStatus status{SafeCallStatus::Success};
    std::string error_message;
    int64_t duration_us{0};

    [[nodiscard]] auto ok() const noexcept -> bool
    {
        return status == SafeCallStatus::Success;
    }
};

/// Execute a callable with typed exception handling. Returns fallback on failure.
/// Exception chain: std::runtime_error → std::exception → "unknown exception".
template <typename R, typename Callable>
auto safe_call(Callable&& callable, R fallback) noexcept -> SafeCallResult<R>
{
    SafeCallResult<R> result;
    auto start = std::chrono::steady_clock::now();

    try
    {
        result.value = callable();
        result.status = SafeCallStatus::Success;
    }
    catch (const std::runtime_error& runtime_err)
    {
        result.status = SafeCallStatus::Exception;
        result.error_message = std::string("runtime_error: ") + runtime_err.what();
        result.value = std::move(fallback);
    }
    catch (const std::exception& exc)
    {
        result.status = SafeCallStatus::Exception;
        result.error_message = std::string("exception: ") + exc.what();
        result.value = std::move(fallback);
    }
    catch (...)
    {
        result.status = SafeCallStatus::Exception;
        result.error_message = "unknown exception (catch-all)";
        result.value = std::move(fallback);
    }

    auto end = std::chrono::steady_clock::now();
    result.duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return result;
}

/// Execute a void callable with typed exception handling.
template <typename Callable>
auto safe_call_void(Callable&& callable) noexcept -> SafeCallResult<void>
{
    SafeCallResult<void> result;
    auto start = std::chrono::steady_clock::now();

    try
    {
        callable();
        result.status = SafeCallStatus::Success;
    }
    catch (const std::runtime_error& runtime_err)
    {
        result.status = SafeCallStatus::Exception;
        result.error_message = std::string("runtime_error: ") + runtime_err.what();
    }
    catch (const std::exception& exc)
    {
        result.status = SafeCallStatus::Exception;
        result.error_message = std::string("exception: ") + exc.what();
    }
    catch (...)
    {
        result.status = SafeCallStatus::Exception;
        result.error_message = "unknown exception (catch-all)";
    }

    auto end = std::chrono::steady_clock::now();
    result.duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return result;
}

/// Main thread ID — set once during startup.
inline std::thread::id g_main_thread_id{};

/// Initialize the main thread ID. Call once from main().
inline void set_main_thread_id()
{
    g_main_thread_id = std::this_thread::get_id();
}

/// Assert that we're on the main thread (debug only).
inline void assert_main_thread_impl(const char* file, int line)
{
    if (g_main_thread_id != std::thread::id{} && std::this_thread::get_id() != g_main_thread_id)
    {
        // In debug mode, this is a programming error
        throw std::logic_error(std::string("Main-thread assertion failed at ") + file + ":" +
                               std::to_string(line));
    }
}

} // namespace markamp::core
