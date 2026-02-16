/// PluginSafeCall.h — Phase 24: Plugin isolation with timeout and exception boundary
///
/// Wraps plugin callable invocations with exception handling and
/// wall-clock timeout detection. Uses ThreadBoundary.h under the hood.
///
/// Pattern implemented: #22 Exception boundaries at thread boundaries

#pragma once

#include "ThreadBoundary.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>

namespace markamp::core
{

/// Status of a plugin safe-call invocation.
enum class PluginCallStatus : uint8_t
{
    Success,
    Exception,
    Timeout,
};

/// Result of a plugin_safe_call invocation.
template <typename T>
struct PluginCallResult
{
    PluginCallStatus status{PluginCallStatus::Success};
    T value{};
    std::string error_message;
    std::string plugin_id;
    int64_t duration_us{0};

    [[nodiscard]] auto ok() const noexcept -> bool
    {
        return status == PluginCallStatus::Success;
    }
};

/// Specialization for void return type.
template <>
struct PluginCallResult<void>
{
    PluginCallStatus status{PluginCallStatus::Success};
    std::string error_message;
    std::string plugin_id;
    int64_t duration_us{0};

    [[nodiscard]] auto ok() const noexcept -> bool
    {
        return status == PluginCallStatus::Success;
    }
};

/// Execute a plugin callable with exception boundary and wall-clock timeout detection.
///
/// Note: This does NOT preemptively kill long-running tasks (C++ can't safely do that).
/// It measures execution time post-facto and reports timeout if exceeded.
template <typename R, typename Callable>
auto plugin_safe_call(std::string_view plugin_id_sv,
                      Callable&& callable,
                      int64_t timeout_ms,
                      R fallback) noexcept -> PluginCallResult<R>
{
    PluginCallResult<R> result;
    result.plugin_id = std::string(plugin_id_sv);

    auto start = std::chrono::steady_clock::now();

    try
    {
        result.value = callable();
        result.status = PluginCallStatus::Success;
    }
    catch (const std::runtime_error& runtime_err)
    {
        result.status = PluginCallStatus::Exception;
        result.error_message = std::string("runtime_error: ") + runtime_err.what();
        result.value = std::move(fallback);
    }
    catch (const std::exception& exc)
    {
        result.status = PluginCallStatus::Exception;
        result.error_message = std::string("exception: ") + exc.what();
        result.value = std::move(fallback);
    }
    catch (...)
    {
        result.status = PluginCallStatus::Exception;
        result.error_message = "unknown exception";
        result.value = std::move(fallback);
    }

    auto end = std::chrono::steady_clock::now();
    result.duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Post-facto timeout detection
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (timeout_ms > 0 && duration_ms > timeout_ms && result.status == PluginCallStatus::Success)
    {
        result.status = PluginCallStatus::Timeout;
        result.error_message = "Plugin exceeded timeout: " + std::to_string(duration_ms) + "ms > " +
                               std::to_string(timeout_ms) + "ms";
    }

    return result;
}

/// Void version of plugin_safe_call.
template <typename Callable>
auto plugin_safe_call_void(std::string_view plugin_id_sv,
                           Callable&& callable,
                           int64_t timeout_ms) noexcept -> PluginCallResult<void>
{
    PluginCallResult<void> result;
    result.plugin_id = std::string(plugin_id_sv);

    auto start = std::chrono::steady_clock::now();

    try
    {
        callable();
        result.status = PluginCallStatus::Success;
    }
    catch (const std::runtime_error& runtime_err)
    {
        result.status = PluginCallStatus::Exception;
        result.error_message = std::string("runtime_error: ") + runtime_err.what();
    }
    catch (const std::exception& exc)
    {
        result.status = PluginCallStatus::Exception;
        result.error_message = std::string("exception: ") + exc.what();
    }
    catch (...)
    {
        result.status = PluginCallStatus::Exception;
        result.error_message = "unknown exception";
    }

    auto end = std::chrono::steady_clock::now();
    result.duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (timeout_ms > 0 && duration_ms > timeout_ms && result.status == PluginCallStatus::Success)
    {
        result.status = PluginCallStatus::Timeout;
        result.error_message = "Plugin exceeded timeout: " + std::to_string(duration_ms) + "ms > " +
                               std::to_string(timeout_ms) + "ms";
    }

    return result;
}

} // namespace markamp::core
