#pragma once

#include "core/EventBus.h"
#include "core/Events.h"

#include <chrono>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Performance budget violation severity.
enum class BudgetSeverity
{
    kWarning, // Near threshold
    kCritical // Exceeded threshold
};

/// A single budget violation record.
struct BudgetViolation
{
    std::string metric_name;    ///< e.g. "keystroke_latency_ms"
    double measured_value{0.0}; ///< Actual measured value
    double budget_value{0.0};   ///< Configured budget value
    BudgetSeverity severity{BudgetSeverity::kWarning};
};

/// Configurable thresholds for editor responsiveness.
struct EditorBudgetThresholds
{
    double keystroke_latency_ms{16.0}; ///< Max time from keypress → render (target: 1 frame)
    double syntax_highlight_ms{50.0};  ///< Max syntax highlight pass duration
    double content_change_debounce_ms{50.0}; ///< Max debounce before event publish
    double fold_scan_ms{100.0};              ///< Max fold region scan duration
    double theme_apply_ms{30.0};             ///< Max theme application duration
};

/// Tracks editor performance metrics and detects budget violations.
///
/// Usage:
///   EditorPerformanceBudget budget(event_bus);
///   budget.record_keystroke_latency(12.5);
///   auto violations = budget.check();
class EditorPerformanceBudget
{
public:
    explicit EditorPerformanceBudget(core::EventBus& event_bus)
        : event_bus_(event_bus)
    {
    }

    // ── Configuration ──

    void set_thresholds(const EditorBudgetThresholds& thresholds)
    {
        thresholds_ = thresholds;
    }

    [[nodiscard]] auto thresholds() const -> const EditorBudgetThresholds&
    {
        return thresholds_;
    }

    // ── Recording ──

    void record_keystroke_latency(double milliseconds)
    {
        last_keystroke_latency_ms_ = milliseconds;
        if (keystroke_samples_.size() >= kMaxSamples)
        {
            keystroke_samples_.erase(keystroke_samples_.begin());
        }
        keystroke_samples_.push_back(milliseconds);
    }

    void record_syntax_highlight_duration(double milliseconds)
    {
        last_syntax_highlight_ms_ = milliseconds;
    }

    void record_content_change_debounce(double milliseconds)
    {
        last_debounce_ms_ = milliseconds;
    }

    void record_fold_scan_duration(double milliseconds)
    {
        last_fold_scan_ms_ = milliseconds;
    }

    void record_theme_apply_duration(double milliseconds)
    {
        last_theme_apply_ms_ = milliseconds;
    }

    // ── Queries ──

    [[nodiscard]] auto last_keystroke_latency() const -> double
    {
        return last_keystroke_latency_ms_;
    }

    [[nodiscard]] auto last_syntax_highlight_duration() const -> double
    {
        return last_syntax_highlight_ms_;
    }

    [[nodiscard]] auto average_keystroke_latency() const -> double
    {
        if (keystroke_samples_.empty())
        {
            return 0.0;
        }
        double sum = 0.0;
        for (const double sample : keystroke_samples_)
        {
            sum += sample;
        }
        return sum / static_cast<double>(keystroke_samples_.size());
    }

    [[nodiscard]] auto sample_count() const -> std::size_t
    {
        return keystroke_samples_.size();
    }

    // ── Violation Detection ──

    /// Check all metrics against thresholds and return any violations.
    [[nodiscard]] auto check() const -> std::vector<BudgetViolation>
    {
        std::vector<BudgetViolation> violations;

        auto check_metric = [&](const std::string& name, double measured, double budget)
        {
            if (measured > budget)
            {
                const BudgetSeverity sev = (measured > budget * 2.0) ? BudgetSeverity::kCritical
                                                                     : BudgetSeverity::kWarning;
                violations.push_back({name, measured, budget, sev});
            }
        };

        check_metric(
            "keystroke_latency_ms", last_keystroke_latency_ms_, thresholds_.keystroke_latency_ms);
        check_metric(
            "syntax_highlight_ms", last_syntax_highlight_ms_, thresholds_.syntax_highlight_ms);
        check_metric("content_change_debounce_ms",
                     last_debounce_ms_,
                     thresholds_.content_change_debounce_ms);
        check_metric("fold_scan_ms", last_fold_scan_ms_, thresholds_.fold_scan_ms);
        check_metric("theme_apply_ms", last_theme_apply_ms_, thresholds_.theme_apply_ms);

        return violations;
    }

    /// Check and publish violations to EventBus if any found.
    void check_and_publish()
    {
        auto violations = check();
        for (const auto& violation : violations)
        {
            core::events::EditorBudgetViolationEvent evt;
            evt.metric_name = violation.metric_name;
            evt.measured_value = violation.measured_value;
            evt.budget_value = violation.budget_value;
            evt.is_critical = (violation.severity == BudgetSeverity::kCritical);
            event_bus_.publish(evt);
        }
    }

    /// Reset all recorded samples.
    void reset()
    {
        keystroke_samples_.clear();
        last_keystroke_latency_ms_ = 0.0;
        last_syntax_highlight_ms_ = 0.0;
        last_debounce_ms_ = 0.0;
        last_fold_scan_ms_ = 0.0;
        last_theme_apply_ms_ = 0.0;
    }

private:
    static constexpr std::size_t kMaxSamples = 100;

    core::EventBus& event_bus_;
    EditorBudgetThresholds thresholds_;

    std::vector<double> keystroke_samples_;
    double last_keystroke_latency_ms_{0.0};
    double last_syntax_highlight_ms_{0.0};
    double last_debounce_ms_{0.0};
    double last_fold_scan_ms_{0.0};
    double last_theme_apply_ms_{0.0};
};

} // namespace markamp::ui
