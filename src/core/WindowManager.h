/// @file WindowManager.h
/// @brief V9 Phase 46 — Application window lifecycle management.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Window display state.
enum class WindowState : uint8_t
{
    kNormal = 0,
    kMinimized = 1,
    kMaximized = 2,
    kFullscreen = 3,
};

/// Rectangle bounds for a window.
struct WindowBounds
{
    int pos_x{0};
    int pos_y{0};
    int width{1280};
    int height{720};
};

/// Information about a single application window.
struct WindowInfo
{
    std::string window_id;
    std::string title;
    WindowState state{WindowState::kNormal};
    WindowBounds bounds;
    bool is_focused{false};
    std::chrono::system_clock::time_point created_at;
};

/// Manages application window lifecycle, focus, and state.
class WindowManager
{
public:
    WindowManager() = default;

    // ── Lifecycle ─────────────────────────────────────────────────────
    auto create_window(const std::string& title = "MarkAmp") -> std::string;
    auto close_window(const std::string& window_id) -> bool;

    // ── Focus ─────────────────────────────────────────────────────────
    auto focus_window(const std::string& window_id) -> bool;
    [[nodiscard]] auto focused_window() const -> const WindowInfo*;

    // ── State ─────────────────────────────────────────────────────────
    auto set_window_state(const std::string& window_id, WindowState state) -> bool;
    auto set_window_bounds(const std::string& window_id, const WindowBounds& bounds) -> bool;
    [[nodiscard]] auto get_window(const std::string& window_id) const -> const WindowInfo*;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto all_windows() const -> std::vector<const WindowInfo*>;
    [[nodiscard]] auto window_count() const -> int;
    void clear_all();

private:
    std::vector<WindowInfo> windows_;
    int next_id_{1};

    auto find_window_mut(const std::string& window_id) -> WindowInfo*;
};

} // namespace markamp::core
