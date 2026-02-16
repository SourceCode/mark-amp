/// Watchdog.h — Phase 31: UI Stall & Deadlock Detection
///
/// Monitors heartbeat signals from the main thread. If no heartbeat
/// arrives within the configured timeout, fires a UIStallEvent.
///
/// Pattern implemented: #28 Watchdog / stall detection

#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace markamp::core
{

/// Stall severity levels.
enum class StallSeverity : uint8_t
{
    Warning,  // > threshold, < 2x threshold
    Critical, // > 2x threshold
};

/// Information about a detected stall.
struct StallEvent
{
    StallSeverity severity{StallSeverity::Warning};
    std::chrono::milliseconds stall_duration{0};
    std::chrono::milliseconds threshold{0};
    int64_t timestamp_ms{0};
};

/// Callback type for stall notifications.
using StallCallback = std::function<void(const StallEvent&)>;

/// Watchdog monitors the main thread heartbeat and detects stalls.
///
/// Usage:
///   Watchdog watchdog;
///   watchdog.set_threshold(std::chrono::milliseconds(100));
///   watchdog.on_stall([](const StallEvent& e) { log(e); });
///   watchdog.start();       // spawns monitor thread
///   // In main loop:
///   watchdog.heartbeat();   // call every frame
///   // On shutdown:
///   watchdog.stop();
class Watchdog
{
public:
    static constexpr auto kDefaultThreshold = std::chrono::milliseconds(200);
    static constexpr auto kDefaultCheckInterval = std::chrono::milliseconds(50);

    Watchdog() = default;

    ~Watchdog()
    {
        stop();
    }

    Watchdog(const Watchdog&) = delete;
    auto operator=(const Watchdog&) -> Watchdog& = delete;
    Watchdog(Watchdog&&) = delete;
    auto operator=(Watchdog&&) -> Watchdog& = delete;

    /// Set the stall detection threshold.
    void set_threshold(std::chrono::milliseconds threshold)
    {
        threshold_ = threshold;
    }

    /// Register a stall callback.
    void on_stall(StallCallback callback)
    {
        callbacks_.push_back(std::move(callback));
    }

    /// Signal that the main thread is alive.
    void heartbeat()
    {
        last_heartbeat_.store(std::chrono::steady_clock::now().time_since_epoch().count(),
                              std::memory_order_release);
        ++heartbeat_count_;
    }

    /// Start the watchdog monitor thread.
    void start()
    {
        if (running_.load(std::memory_order_acquire))
        {
            return;
        }
        running_.store(true, std::memory_order_release);
        heartbeat(); // initial heartbeat
        monitor_thread_ = std::thread([this] { monitor_loop(); });
    }

    /// Stop the watchdog.
    void stop()
    {
        running_.store(false, std::memory_order_release);
        if (monitor_thread_.joinable())
        {
            monitor_thread_.join();
        }
    }

    /// Check if the watchdog is running.
    [[nodiscard]] auto is_running() const noexcept -> bool
    {
        return running_.load(std::memory_order_acquire);
    }

    /// Total heartbeats received.
    [[nodiscard]] auto heartbeat_count() const noexcept -> uint64_t
    {
        return heartbeat_count_.load(std::memory_order_acquire);
    }

    /// Total stalls detected.
    [[nodiscard]] auto stall_count() const noexcept -> uint64_t
    {
        return stall_count_.load(std::memory_order_acquire);
    }

    /// Time since last heartbeat.
    [[nodiscard]] auto time_since_heartbeat() const -> std::chrono::milliseconds
    {
        auto last = std::chrono::steady_clock::time_point(
            std::chrono::steady_clock::duration(last_heartbeat_.load(std::memory_order_acquire)));
        auto elapsed = std::chrono::steady_clock::now() - last;
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
    }

private:
    std::chrono::milliseconds threshold_{kDefaultThreshold};
    std::atomic<int64_t> last_heartbeat_{0};
    std::atomic<uint64_t> heartbeat_count_{0};
    std::atomic<uint64_t> stall_count_{0};
    std::atomic<bool> running_{false};
    std::thread monitor_thread_;
    std::vector<StallCallback> callbacks_;

    void monitor_loop()
    {
        while (running_.load(std::memory_order_acquire))
        {
            std::this_thread::sleep_for(kDefaultCheckInterval);

            auto elapsed = time_since_heartbeat();
            if (elapsed > threshold_)
            {
                StallEvent event;
                event.stall_duration = elapsed;
                event.threshold = threshold_;
                event.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                         std::chrono::system_clock::now().time_since_epoch())
                                         .count();

                if (elapsed > threshold_ * 2)
                {
                    event.severity = StallSeverity::Critical;
                }
                else
                {
                    event.severity = StallSeverity::Warning;
                }

                ++stall_count_;

                for (const auto& callback : callbacks_)
                {
                    callback(event);
                }
            }
        }
    }
};

} // namespace markamp::core
