/// SafeMode.h — V7 Phase 36: Multi-tier safe mode
///
/// 5 tiers: Normal → NoExtensions → NoPreview → MinimalUI → Full
/// Crash counter persistence, auto-trigger after repeated crashes,
/// command-line flag parsing.

#pragma once

#include "Result.h"

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Safe Mode Tiers
// ══════════════════════════════════════════════════════════════════════════════

/// Safe mode levels, from least to most restrictive.
enum class SafeModeTier : uint8_t
{
    kNormal = 0,       // All features enabled
    kNoExtensions = 1, // Extensions disabled
    kNoPreview = 2,    // Extensions + preview disabled
    kMinimalUi = 3,    // Bare-bones UI
    kFull = 4,         // Maximum safety: editor-only mode
};

/// Get display name for a safe mode tier.
[[nodiscard]] auto tier_name(SafeModeTier tier) -> std::string;

/// Parse a command-line flag to a safe mode tier.
/// Recognizes: --safe, --safe-no-extensions, --safe-no-preview, --safe-minimal-ui
[[nodiscard]] auto parse_safe_mode_flag(std::string_view flag) -> Result<SafeModeTier>;

// ══════════════════════════════════════════════════════════════════════════════
// Crash Counter
// ══════════════════════════════════════════════════════════════════════════════

/// Persists crash timestamps and auto-triggers safe mode.
class CrashCounter
{
public:
    CrashCounter() = default;

    /// Record a crash. Returns recommended safe mode tier.
    [[nodiscard]] auto record_crash() -> SafeModeTier;

    /// Get the current crash count within the time window.
    [[nodiscard]] auto recent_crash_count() const -> size_t;

    /// Reset the crash counter.
    void reset();

    /// Set the threshold and window for auto-trigger.
    void set_threshold(size_t threshold, std::chrono::seconds window);

    /// Get the auto-trigger threshold.
    [[nodiscard]] auto threshold() const noexcept -> size_t
    {
        return threshold_;
    }

private:
    std::vector<std::chrono::steady_clock::time_point> crashes_;
    size_t threshold_{3};
    std::chrono::seconds window_{300}; // 5 minutes
};

// ══════════════════════════════════════════════════════════════════════════════
// Safe Mode Controller
// ══════════════════════════════════════════════════════════════════════════════

/// Manages the application's safe mode state.
class SafeModeController
{
public:
    SafeModeController() = default;

    /// Get the current safe mode tier.
    [[nodiscard]] auto current_tier() const noexcept -> SafeModeTier
    {
        return tier_;
    }

    /// Set the safe mode tier.
    void set_tier(SafeModeTier tier)
    {
        tier_ = tier;
    }

    /// Check if extensions are allowed.
    [[nodiscard]] auto extensions_allowed() const noexcept -> bool
    {
        return tier_ == SafeModeTier::kNormal;
    }

    /// Check if preview is allowed.
    [[nodiscard]] auto preview_allowed() const noexcept -> bool
    {
        return tier_ <= SafeModeTier::kNoExtensions;
    }

    /// Check if full UI is allowed.
    [[nodiscard]] auto full_ui_allowed() const noexcept -> bool
    {
        return tier_ <= SafeModeTier::kNoPreview;
    }

    /// Process command-line arguments for safe mode flags.
    void process_args(const std::vector<std::string>& args);

    /// Record a crash and potentially escalate safe mode.
    [[nodiscard]] auto on_crash() -> SafeModeTier;

    /// Reset to normal mode.
    void reset()
    {
        tier_ = SafeModeTier::kNormal;
        crash_counter_.reset();
    }

    /// Access the crash counter.
    [[nodiscard]] auto crash_counter() -> CrashCounter&
    {
        return crash_counter_;
    }

private:
    SafeModeTier tier_{SafeModeTier::kNormal};
    CrashCounter crash_counter_;
};

} // namespace markamp::core
