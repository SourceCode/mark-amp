#include "ProgressService.h"

namespace markamp::core
{

// ── ProgressReporter ──

void ProgressReporter::report(int increment, const std::string& message)
{
    auto current = percentage_.load();
    auto updated = std::min(100, current + increment);
    percentage_.store(updated);

    if (!message.empty())
    {
        message_ = message;
    }
}

auto ProgressReporter::is_cancelled() const -> bool
{
    return cancelled_.load();
}

void ProgressReporter::cancel()
{
    cancelled_.store(true);
}

auto ProgressReporter::percentage() const -> int
{
    return percentage_.load();
}

auto ProgressReporter::message() const -> const std::string&
{
    return message_;
}

void ProgressReporter::reset()
{
    percentage_.store(0);
    cancelled_.store(false);
    message_.clear();
}

// ── ProgressService ──

void ProgressService::with_progress(const ProgressOptions& /*options*/, const ProgressTask& task)
{
    active_ = true;
    current_reporter_.reset();

    if (task)
    {
        task(current_reporter_);
    }

    active_ = false;
}

auto ProgressService::is_active() const -> bool
{
    return active_;
}

auto ProgressService::current_reporter() -> ProgressReporter*
{
    return &current_reporter_;
}

} // namespace markamp::core
