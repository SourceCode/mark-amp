#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Analytics event entry.
struct AnalyticsEntry
{
    std::string event_name;
    std::string category;
    std::string timestamp;

    [[nodiscard]] auto has_category() const noexcept -> bool
    {
        return !category.empty();
    }
};

/// Analytics flush state.
enum class AnalyticsFlushState : uint8_t
{
    kIdle,
    kFlushing,
    kFailed,
};

/// Testable model for Canvas Analytics (Phase 01 W20).
///
/// Encapsulates:
/// - Event recording with category tagging
/// - Flush lifecycle tracking
/// - Opt-out control
/// - Category-based event counts
class CanvasAnalyticsModel
{
public:
    // ── Recording ───────────────────────────────────────────────────

    void record_event(const std::string& event_name, const std::string& category);
    [[nodiscard]] auto events() const -> const std::vector<AnalyticsEntry>&;
    [[nodiscard]] auto pending_count() const -> int;

    // ── Flush ───────────────────────────────────────────────────────

    void start_flush();
    void complete_flush();
    void fail_flush();
    [[nodiscard]] auto flush_state() const -> AnalyticsFlushState;

    // ── Opt-out ─────────────────────────────────────────────────────

    void set_opted_out(bool opted_out);
    [[nodiscard]] auto is_opted_out() const -> bool;

    // ── Category counts ─────────────────────────────────────────────

    [[nodiscard]] auto count_by_category(const std::string& category) const -> int;

    // ── Convenience ─────────────────────────────────────────────────

    [[nodiscard]] auto has_events() const noexcept -> bool
    {
        return !events_.empty();
    }

    [[nodiscard]] auto is_idle() const noexcept -> bool
    {
        return flush_state_ == AnalyticsFlushState::kIdle;
    }

    [[nodiscard]] auto is_flushing() const noexcept -> bool
    {
        return flush_state_ == AnalyticsFlushState::kFlushing;
    }

private:
    std::vector<AnalyticsEntry> events_;
    AnalyticsFlushState flush_state_{AnalyticsFlushState::kIdle};
    bool opted_out_{false};
};

} // namespace markamp::canvas
