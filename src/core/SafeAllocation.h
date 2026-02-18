/// SafeAllocation.h — V7 Phase 16: Safe allocation and pointer discipline
///
/// Wraps memory allocation to convert std::bad_alloc into Result<T> errors
/// instead of crashing. Promotes RAII via safe smart pointer factories.

#pragma once

#include "Result.h"

#include <memory>
#include <new>
#include <string>
#include <utility>

namespace markamp::core
{

/// Safely create a unique_ptr, catching std::bad_alloc.
template <typename T, typename... Args>
[[nodiscard]] auto safe_make_unique(Args&&... args) -> Result<std::unique_ptr<T>>
{
    try
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    catch (const std::bad_alloc&)
    {
        return std::unexpected(make_error(ErrorCode::AllocationFailed,
                                          SubsystemId::Core,
                                          "Failed to allocate " + std::string(typeid(T).name())));
    }
}

/// Safely create a shared_ptr, catching std::bad_alloc.
template <typename T, typename... Args>
[[nodiscard]] auto safe_make_shared(Args&&... args) -> Result<std::shared_ptr<T>>
{
    try
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    catch (const std::bad_alloc&)
    {
        return std::unexpected(make_error(ErrorCode::AllocationFailed,
                                          SubsystemId::Core,
                                          "Failed to allocate " + std::string(typeid(T).name())));
    }
}

} // namespace markamp::core
