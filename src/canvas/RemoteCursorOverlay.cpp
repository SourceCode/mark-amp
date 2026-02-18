// ============================================================================
// File: src/canvas/RemoteCursorOverlay.cpp
// Phase 13: Canvas Collaboration — remote cursor rendering overlay
// ============================================================================
#include "canvas/RemoteCursorOverlay.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace markamp::canvas
{

RemoteCursorOverlay::RemoteCursorOverlay() = default;

RemoteCursorOverlay::RemoteCursorOverlay(CursorOverlayConfig config)
    : config_(config)
    , labels_visible_(config.show_labels_by_default)
{
}

// ── Cursor Updates ────────────────────────────────────────────────

auto RemoteCursorOverlay::update_cursor(const std::string& participant_id,
                                        const std::string& display_name,
                                        const Point2D& position,
                                        const CanvasColor& color) -> void
{
    auto iter = cursors_.find(participant_id);
    if (iter == cursors_.end())
    {
        CursorEntry entry;
        entry.participant_id = participant_id;
        entry.display_name = display_name;
        entry.color = color;
        entry.position = position;
        entry.target_position = position;
        entry.show_label = labels_visible_;
        entry.opacity = 1.0;
        entry.last_update = std::chrono::steady_clock::now();
        cursors_.emplace(participant_id, entry);
    }
    else
    {
        iter->second.target_position = position;
        iter->second.display_name = display_name;
        iter->second.color = color;
        iter->second.opacity = 1.0;
        iter->second.last_update = std::chrono::steady_clock::now();
    }
}

auto RemoteCursorOverlay::update_tool(const std::string& participant_id, RemoteToolType tool)
    -> void
{
    auto iter = cursors_.find(participant_id);
    if (iter != cursors_.end())
    {
        iter->second.tool = tool;
    }
}

auto RemoteCursorOverlay::remove_cursor(const std::string& participant_id) -> void
{
    cursors_.erase(participant_id);
}

auto RemoteCursorOverlay::clear() -> void
{
    cursors_.clear();
}

// ── Frame Update ──────────────────────────────────────────────────

auto RemoteCursorOverlay::tick(double /*delta_seconds*/) -> void
{
    for (auto& [id, entry] : cursors_)
    {
        // Interpolate position
        lerp_position(entry.position, entry.target_position, config_.interpolation_speed);

        // Update idle opacity
        entry.opacity = compute_idle_opacity(entry);
    }
}

// ── Query ─────────────────────────────────────────────────────────

auto RemoteCursorOverlay::cursors() const -> const std::unordered_map<std::string, CursorEntry>&
{
    return cursors_;
}

auto RemoteCursorOverlay::visible_cursors() const -> std::vector<const CursorEntry*>
{
    std::vector<const CursorEntry*> result;
    for (const auto& [id, entry] : cursors_)
    {
        if (entry.opacity > 0.01)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto RemoteCursorOverlay::find_cursor(const std::string& participant_id) const -> const CursorEntry*
{
    auto iter = cursors_.find(participant_id);
    if (iter != cursors_.end())
    {
        return &iter->second;
    }
    return nullptr;
}

auto RemoteCursorOverlay::cursor_count() const -> size_t
{
    return cursors_.size();
}

// ── Configuration ─────────────────────────────────────────────────

auto RemoteCursorOverlay::config() const -> const CursorOverlayConfig&
{
    return config_;
}

auto RemoteCursorOverlay::set_config(const CursorOverlayConfig& config) -> void
{
    config_ = config;
}

auto RemoteCursorOverlay::set_labels_visible(bool visible) -> void
{
    labels_visible_ = visible;
    for (auto& [id, entry] : cursors_)
    {
        entry.show_label = visible;
    }
}

auto RemoteCursorOverlay::labels_visible() const -> bool
{
    return labels_visible_;
}

// ── Utility ───────────────────────────────────────────────────────

auto RemoteCursorOverlay::tool_name(RemoteToolType tool) -> std::string
{
    switch (tool)
    {
        case RemoteToolType::kPointer:
            return "Pointer";
        case RemoteToolType::kPan:
            return "Pan";
        case RemoteToolType::kDraw:
            return "Draw";
        case RemoteToolType::kText:
            return "Text";
        case RemoteToolType::kShape:
            return "Shape";
        case RemoteToolType::kEraser:
            return "Eraser";
        case RemoteToolType::kLaser:
            return "Laser";
    }
    return "Unknown";
}

auto RemoteCursorOverlay::participant_color(size_t participant_index) -> CanvasColor
{
    // 8 distinct, high-contrast collaboration colors
    static const std::array<CanvasColor, 8> kPalette = {{
        {0, 120, 215, 255},  // Blue
        {232, 65, 24, 255},  // Red
        {39, 174, 96, 255},  // Green
        {142, 68, 173, 255}, // Purple
        {243, 156, 18, 255}, // Orange
        {0, 184, 148, 255},  // Teal
        {214, 48, 49, 255},  // Crimson
        {108, 92, 231, 255}, // Indigo
    }};
    return kPalette[participant_index % kPalette.size()];
}

// ── Private ───────────────────────────────────────────────────────

auto RemoteCursorOverlay::lerp_position(Point2D& current, const Point2D& target, double factor)
    -> void
{
    current.x += (target.x - current.x) * factor;
    current.y += (target.y - current.y) * factor;
}

auto RemoteCursorOverlay::compute_idle_opacity(const CursorEntry& entry) const -> double
{
    const auto now = std::chrono::steady_clock::now();
    const double elapsed = std::chrono::duration<double>(now - entry.last_update).count();

    if (elapsed < config_.idle_timeout_seconds)
    {
        return 1.0;
    }

    const double fade_elapsed = elapsed - config_.idle_timeout_seconds;
    if (fade_elapsed >= config_.fade_duration_seconds)
    {
        return 0.0;
    }

    return 1.0 - (fade_elapsed / config_.fade_duration_seconds);
}

} // namespace markamp::canvas
