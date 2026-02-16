#pragma once

/// TracyIntegration.h — Zero-cost Tracy profiler macros
///
/// When MARKAMP_TRACY_ENABLED is defined, these macros forward to Tracy
/// instrumentation APIs. When disabled, they expand to nothing (zero cost).
///
/// Usage:
///   MARKAMP_TRACY_ZONE("MyFunction")        — scoped zone marker
///   MARKAMP_TRACY_ZONE_COLOR("Render", 0xFF0000)  — scoped zone with color
///   MARKAMP_TRACY_FRAME_MARK                — end-of-frame marker
///   MARKAMP_TRACY_PLOT("value", 42.0)       — named value plot
///   MARKAMP_TRACY_ALLOC(ptr, size)           — allocation tracking
///   MARKAMP_TRACY_FREE(ptr)                  — deallocation tracking
///   MARKAMP_TRACY_MESSAGE(text, len)         — message annotation

#ifdef MARKAMP_TRACY_ENABLED

#include <tracy/Tracy.hpp>

// NOLINTBEGIN(cppcoreguidelines-macro-usage) — Intentional macro API for zero-cost toggleability

#define MARKAMP_TRACY_ZONE(name) ZoneScopedN(name)
#define MARKAMP_TRACY_ZONE_COLOR(name, color) ZoneScopedNC(name, color)
#define MARKAMP_TRACY_FRAME_MARK FrameMark
#define MARKAMP_TRACY_PLOT(name, value) TracyPlot(name, value)
#define MARKAMP_TRACY_ALLOC(ptr, size) TracyAlloc(ptr, size)
#define MARKAMP_TRACY_FREE(ptr) TracyFree(ptr)
#define MARKAMP_TRACY_MESSAGE(text, len) TracyMessage(text, len)

// NOLINTEND(cppcoreguidelines-macro-usage)

#else

// Zero-cost no-op macros when Tracy is disabled
// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define MARKAMP_TRACY_ZONE(name) ((void)0)
#define MARKAMP_TRACY_ZONE_COLOR(name, color) ((void)0)
#define MARKAMP_TRACY_FRAME_MARK ((void)0)
#define MARKAMP_TRACY_PLOT(name, value) ((void)0)
#define MARKAMP_TRACY_ALLOC(ptr, size) ((void)0)
#define MARKAMP_TRACY_FREE(ptr) ((void)0)
#define MARKAMP_TRACY_MESSAGE(text, len) ((void)0)

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif
