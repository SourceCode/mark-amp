#pragma once

/// ═══════════════════════════════════════════════════════
/// Portable compiler hint macros for hot-path optimization.
///
/// Pattern implemented: #40 Build and compiler optimization posture
/// ═══════════════════════════════════════════════════════

/// Branch prediction hints (C++20 attributes).
#define MARKAMP_LIKELY [[likely]]
#define MARKAMP_UNLIKELY [[unlikely]]

/// Force-inline for hot-path functions.
#if defined(__GNUC__) || defined(__clang__)
#define MARKAMP_FORCE_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#define MARKAMP_FORCE_INLINE __forceinline
#else
#define MARKAMP_FORCE_INLINE inline
#endif

/// Restrict pointer aliasing hint.
#if defined(__GNUC__) || defined(__clang__)
#define MARKAMP_RESTRICT __restrict__
#elif defined(_MSC_VER)
#define MARKAMP_RESTRICT __restrict
#else
#define MARKAMP_RESTRICT
#endif

/// Hot function — placed in hot text section for better code locality.
#if defined(__GNUC__) || defined(__clang__)
#define MARKAMP_HOT __attribute__((hot))
#else
#define MARKAMP_HOT
#endif

/// Cold function — placed away from hot code to reduce cache pollution.
#if defined(__GNUC__) || defined(__clang__)
#define MARKAMP_COLD __attribute__((cold))
#else
#define MARKAMP_COLD
#endif

/// No-inline for functions that should never be inlined (error paths, etc).
#if defined(__GNUC__) || defined(__clang__)
#define MARKAMP_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define MARKAMP_NOINLINE __declspec(noinline)
#else
#define MARKAMP_NOINLINE
#endif

/// Prefetch hint for data likely to be accessed soon.
#if defined(__GNUC__) || defined(__clang__)
#define MARKAMP_PREFETCH(addr) __builtin_prefetch(addr)
#else
#define MARKAMP_PREFETCH(addr) ((void)0)
#endif
