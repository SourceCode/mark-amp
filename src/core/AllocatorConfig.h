#pragma once

/// @file AllocatorConfig.h
/// @brief Compile-time allocator configuration and diagnostics.
///
/// When MARKAMP_MIMALLOC_ENABLED is defined (via cmake/Allocator.cmake),
/// mimalloc replaces the default allocator globally. This header provides:
///   - allocator_name() — identifies active allocator at runtime
///   - print_allocator_stats() — dumps mimalloc stats (no-op with default)
///   - allocator_is_mimalloc() — constexpr check
///
/// Phase 10: Custom Allocator Integration

#include <cstddef>
#include <string_view>

#if defined(MARKAMP_MIMALLOC_ENABLED)
#include <mimalloc.h>
#endif

namespace markamp::core
{

/// Returns the name of the active global allocator.
[[nodiscard]] constexpr auto allocator_name() noexcept -> std::string_view
{
#if defined(MARKAMP_MIMALLOC_ENABLED)
    return "mimalloc";
#else
    return "system";
#endif
}

/// Whether mimalloc is the active global allocator (compile-time check).
[[nodiscard]] constexpr auto allocator_is_mimalloc() noexcept -> bool
{
#if defined(MARKAMP_MIMALLOC_ENABLED)
    return true;
#else
    return false;
#endif
}

/// Print allocator statistics to stderr. No-op with system allocator.
inline void print_allocator_stats()
{
#if defined(MARKAMP_MIMALLOC_ENABLED)
    mi_stats_print(nullptr);
#endif
}

/// Collect allocator heap usage in bytes. Returns 0 with system allocator.
[[nodiscard]] inline auto allocator_heap_usage() noexcept -> std::size_t
{
#if defined(MARKAMP_MIMALLOC_ENABLED)
    return mi_process_info(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
#else
    return 0;
#endif
}

} // namespace markamp::core
