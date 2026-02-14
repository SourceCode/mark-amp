#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <string>

namespace markamp::core
{

/// Location where progress is displayed.
enum class ProgressLocation : std::uint8_t
{
    kNotification, ///< Show as a notification
    kStatusBar,    ///< Show in status bar
    kWindow        ///< Show as a window-level indicator
};

/// Options for showing progress.
struct ProgressOptions
{
    std::string title;
    ProgressLocation location{ProgressLocation::kNotification};
    bool cancellable{false};
};

/// Reports progress increments during a long-running task.
/// Mirrors VS Code's `Progress<{message, increment}>`.
class ProgressReporter
{
public:
    /// Report progress. `increment` is 0-100 (percentage points to add).
    void report(int increment, const std::string& message = {});

    /// Check if the user requested cancellation.
    [[nodiscard]] auto is_cancelled() const -> bool;

    /// Cancel the progress (called by UI when user clicks cancel).
    void cancel();

    /// Get current progress percentage (0-100).
    [[nodiscard]] auto percentage() const -> int;

    /// Get the last reported message.
    [[nodiscard]] auto message() const -> const std::string&;

    /// Reset the reporter to initial state.
    void reset();

private:
    std::atomic<int> percentage_{0};
    std::atomic<bool> cancelled_{false};
    std::string message_;
};

/// Service for showing progress indicators during long-running operations.
///
/// Mirrors VS Code's `window.withProgress()`.
///
/// Usage:
/// ```cpp
/// ctx.progress_service->with_progress(
///     {.title = "Indexing files...", .cancellable = true},
///     [](ProgressReporter& progress) {
///         for (int i = 0; i < 100; i += 10) {
///             progress.report(10, "Processing...");
///             if (progress.is_cancelled()) break;
///         }
///     });
/// ```
class ProgressService
{
public:
    using ProgressTask = std::function<void(ProgressReporter&)>;

    ProgressService() = default;

    /// Execute a task with progress reporting.
    void with_progress(const ProgressOptions& options, const ProgressTask& task);

    /// Check if a progress operation is currently active.
    [[nodiscard]] auto is_active() const -> bool;

    /// Get the current active progress reporter (for testing/UI integration).
    [[nodiscard]] auto current_reporter() -> ProgressReporter*;

private:
    bool active_{false};
    ProgressReporter current_reporter_;
};

} // namespace markamp::core
