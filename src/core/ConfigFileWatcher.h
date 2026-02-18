#pragma once

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <functional>

namespace markamp::core
{

/// Watches the config file for external modifications.
/// Uses polling via `std::filesystem::last_write_time` to detect changes.
/// Calls a user-provided callback when a change is detected.
class ConfigFileWatcher
{
public:
    using Callback = std::function<void()>;

    /// Construct watcher for the given file path with a change callback.
    explicit ConfigFileWatcher(std::filesystem::path config_path, Callback on_change);

    /// Poll for changes. Call this periodically (e.g. from OnIdle).
    /// Returns true if a change was detected.
    auto poll() -> bool;

    /// Return the number of times a change was detected.
    [[nodiscard]] auto change_count() const -> std::size_t
    {
        return change_count_;
    }

    /// Return whether the watcher is active.
    [[nodiscard]] auto is_active() const -> bool
    {
        return active_;
    }

    /// Start/stop the watcher.
    void start();
    void stop();

private:
    std::filesystem::path config_path_;
    Callback on_change_;
    std::filesystem::file_time_type last_write_time_{};
    std::size_t change_count_{0};
    bool active_{false};
};

} // namespace markamp::core
