#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Container selection mode.
enum class ContainerSelectMode : uint8_t
{
    kContainerOnly,  ///< Select the frame itself
    kSelectChildren, ///< Select children within
};

/// Frame style preset.
struct FrameStyle
{
    std::string title;
    std::string tint_color{"#E8E8E8"};
    std::string border_style{"solid"};
    double border_width{1.0};
};

/// A child in a container.
struct ContainedObject
{
    std::string object_id;
    double rel_x{0.0}; ///< Relative to container origin
    double rel_y{0.0};
};

/// Testable model for Frames, Sections & Containers (Phase 51).
///
/// Encapsulates:
/// - Containment (add/remove children, maintain relative positions)
/// - Frame title and styling
/// - Frame navigation (prev/next)
/// - Container-aware selection mode
class FrameContainerModel
{
public:
    // ── Containment ─────────────────────────────────────────────────

    void set_children(std::vector<ContainedObject> children);
    [[nodiscard]] auto children() const -> const std::vector<ContainedObject>&;
    void add_child(ContainedObject child);
    void remove_child(const std::string& object_id);
    [[nodiscard]] auto contains(const std::string& object_id) const -> bool;
    [[nodiscard]] auto child_count() const -> int;

    // ── Styling ─────────────────────────────────────────────────────

    void set_style(FrameStyle style);
    [[nodiscard]] auto style() const -> const FrameStyle&;

    // ── Navigation ──────────────────────────────────────────────────

    void set_frame_ids(std::vector<std::string> ids);
    void set_active_frame(const std::string& frame_id);
    [[nodiscard]] auto active_frame() const -> const std::string&;
    [[nodiscard]] auto next_frame() const -> std::string;
    [[nodiscard]] auto prev_frame() const -> std::string;

    // ── Selection mode ──────────────────────────────────────────────

    void set_select_mode(ContainerSelectMode mode);
    [[nodiscard]] auto select_mode() const -> ContainerSelectMode;

private:
    std::vector<ContainedObject> children_;
    FrameStyle style_;
    std::vector<std::string> frame_ids_;
    std::string active_frame_;
    ContainerSelectMode select_mode_{ContainerSelectMode::kContainerOnly};
};

} // namespace markamp::canvas
