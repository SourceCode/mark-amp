/// RenderingSafety.h — V7 Phase 27: Rendering pipeline safety
///
/// Wraps rendering operations in try/catch with fallback to plain text.
/// Enforces frame budgets and rendering timeouts.

#pragma once

#include "core/Result.h"
#include "core/StructuredLogger.h"

#include <chrono>
#include <functional>
#include <string>

namespace markamp::rendering
{

using markamp::core::Error;
using markamp::core::ErrorCode;
using markamp::core::Result;
using markamp::core::SubsystemId;

// ══════════════════════════════════════════════════════════════════════════════
// Frame Budget
// ══════════════════════════════════════════════════════════════════════════════

/// Default frame budget for rendering operations.
inline constexpr auto kDefaultFrameBudgetMs = std::chrono::milliseconds(16); // ~60 FPS
inline constexpr auto kMaxRenderTimeoutMs = std::chrono::milliseconds(5000);
inline constexpr size_t kMaxRenderOutputBytes = 10 * 1024 * 1024; // 10 MiB

// ══════════════════════════════════════════════════════════════════════════════
// Render Result
// ══════════════════════════════════════════════════════════════════════════════

/// Outcome of a safe render operation.
struct RenderOutcome
{
    std::string output;
    bool used_fallback{false};
    std::chrono::microseconds elapsed{0};
};

// ══════════════════════════════════════════════════════════════════════════════
// Rendering Safety
// ══════════════════════════════════════════════════════════════════════════════

/// Wraps rendering operations with safety guarantees.
class RenderingSafety
{
public:
    RenderingSafety() = default;

    /// Execute a rendering operation with fallback.
    /// If render_fn throws or exceeds the output size limit,
    /// fallback_fn is called instead.
    [[nodiscard]] auto safe_render(std::function<std::string()> render_fn,
                                   std::function<std::string()> fallback_fn) -> RenderOutcome;

    /// Check if a rendering output exceeds size limits.
    [[nodiscard]] static auto validate_output_size(const std::string& output) -> Result<void>;

    /// Get the number of times fallback was used.
    [[nodiscard]] auto fallback_count() const noexcept -> size_t
    {
        return fallback_count_;
    }

    /// Get the number of successful renders.
    [[nodiscard]] auto success_count() const noexcept -> size_t
    {
        return success_count_;
    }

    /// Reset counters.
    void reset_counters();

private:
    size_t fallback_count_{0};
    size_t success_count_{0};
    core::StructuredLogger logger_{"RenderingSafety"};
};

} // namespace markamp::rendering
