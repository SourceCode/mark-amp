/// SafeMode.cpp — V7 Phase 36: Multi-tier safe mode

#include "SafeMode.h"

namespace markamp::core
{

auto tier_name(SafeModeTier tier) -> std::string
{
    switch (tier)
    {
        case SafeModeTier::kNormal:
            return "Normal";
        case SafeModeTier::kNoExtensions:
            return "No Extensions";
        case SafeModeTier::kNoPreview:
            return "No Preview";
        case SafeModeTier::kMinimalUi:
            return "Minimal UI";
        case SafeModeTier::kFull:
            return "Full Safe Mode";
    }
    return "Unknown";
}

auto parse_safe_mode_flag(std::string_view flag) -> Result<SafeModeTier>
{
    if (flag == "--safe" || flag == "--safe-mode")
    {
        return SafeModeTier::kNoExtensions;
    }
    if (flag == "--safe-no-extensions")
    {
        return SafeModeTier::kNoExtensions;
    }
    if (flag == "--safe-no-preview")
    {
        return SafeModeTier::kNoPreview;
    }
    if (flag == "--safe-minimal-ui")
    {
        return SafeModeTier::kMinimalUi;
    }

    return std::unexpected(make_error(ErrorCode::InvalidArgument,
                                      SubsystemId::Core,
                                      "Unknown safe mode flag: " + std::string(flag)));
}

// ══════════════════════════════════════════════════════════════════════════════
// Crash Counter
// ══════════════════════════════════════════════════════════════════════════════

auto CrashCounter::record_crash() -> SafeModeTier
{
    auto now = std::chrono::steady_clock::now();
    crashes_.push_back(now);

    // Prune old crashes
    auto cutoff = now - window_;
    std::erase_if(crashes_, [cutoff](const auto& time_point) { return time_point < cutoff; });

    auto count = crashes_.size();
    if (count >= threshold_ * 2)
    {
        return SafeModeTier::kFull;
    }
    if (count >= threshold_)
    {
        return SafeModeTier::kNoExtensions;
    }
    return SafeModeTier::kNormal;
}

auto CrashCounter::recent_crash_count() const -> size_t
{
    auto now = std::chrono::steady_clock::now();
    auto cutoff = now - window_;
    size_t count = 0;
    for (const auto& crash_time : crashes_)
    {
        if (crash_time >= cutoff)
        {
            ++count;
        }
    }
    return count;
}

void CrashCounter::reset()
{
    crashes_.clear();
}

void CrashCounter::set_threshold(size_t threshold, std::chrono::seconds window)
{
    threshold_ = threshold;
    window_ = window;
}

// ══════════════════════════════════════════════════════════════════════════════
// Safe Mode Controller
// ══════════════════════════════════════════════════════════════════════════════

void SafeModeController::process_args(const std::vector<std::string>& args)
{
    for (const auto& arg : args)
    {
        auto result = parse_safe_mode_flag(arg);
        if (result.has_value())
        {
            // Take the most restrictive tier
            if (static_cast<uint8_t>(result.value()) > static_cast<uint8_t>(tier_))
            {
                tier_ = result.value();
            }
        }
    }
}

auto SafeModeController::on_crash() -> SafeModeTier
{
    auto recommended = crash_counter_.record_crash();
    if (static_cast<uint8_t>(recommended) > static_cast<uint8_t>(tier_))
    {
        tier_ = recommended;
    }
    return tier_;
}

} // namespace markamp::core
