#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <variant>
#include <vector>

namespace markamp::ui
{

/// Phase 45 Task 01: Drag payload types.
enum class DragPayloadType : uint8_t
{
    kTab,          ///< Editor tab being dragged
    kFile,         ///< File from explorer tree
    kPanel,        ///< Panel/tool window
    kExternalFile, ///< File from OS (external)
    kMultiSelect,  ///< Multiple selected items
};

/// Phase 45 Task 01: Drag payload data (type-safe union).
struct TabDragPayload
{
    int tab_index{0};
    int source_group_id{0};
    std::string file_path;
};

struct FileDragPayload
{
    std::string file_path;
    bool is_directory{false};
};

struct PanelDragPayload
{
    std::string panel_id;
    int source_dock_position{0};
};

struct MultiSelectPayload
{
    std::vector<std::string> file_paths;
};

using DragPayload =
    std::variant<TabDragPayload, FileDragPayload, PanelDragPayload, MultiSelectPayload>;

/// Phase 45: Drop zone position.
enum class DropPosition : uint8_t
{
    kNone,
    kBefore, ///< Insert before target
    kAfter,  ///< Insert after target
    kCenter, ///< Drop on target (replace/add)
    kLeft,   ///< Split left
    kRight,  ///< Split right
    kTop,    ///< Split top
    kBottom, ///< Split bottom
};

/// Phase 45: Drop zone descriptor.
struct DropZone
{
    std::string zone_id;
    DragPayloadType accepted_type;
    DropPosition position{DropPosition::kCenter};
    bool is_active{false};
};

/// Phase 45: Drag state machine phases.
enum class DragPhase : uint8_t
{
    kIdle,      ///< No drag in progress
    kPending,   ///< Mouse pressed, waiting for distance threshold
    kDragging,  ///< Active drag with ghost window
    kDropping,  ///< Drop animation in progress
    kCancelled, ///< Escape pressed, snap-back animation
};

/// Phase 45: Testable model for the drag controller.
///
/// Pure state machine managing drag lifecycle: threshold detection, zone
/// validation, drop resolution, and undo integration.
class DragControllerModel
{
public:
    /// Begin potential drag (mouse down).
    void begin_potential_drag(int pos_x, int pos_y, DragPayloadType type, DragPayload payload);

    /// Update drag position (mouse move).
    /// @return true if passed distance threshold and entered kDragging phase.
    auto update_position(int pos_x, int pos_y) -> bool;

    /// Complete the drop at current position.
    void complete_drop();

    /// Cancel the drag (Escape key).
    void cancel();

    /// Reset to idle state.
    void reset();

    // ── State queries ──────────────────────────────────────────

    [[nodiscard]] auto phase() const -> DragPhase;
    [[nodiscard]] auto payload_type() const -> DragPayloadType;
    [[nodiscard]] auto payload() const -> const DragPayload&;

    [[nodiscard]] auto start_x() const -> int;
    [[nodiscard]] auto start_y() const -> int;
    [[nodiscard]] auto current_x() const -> int;
    [[nodiscard]] auto current_y() const -> int;

    [[nodiscard]] auto drag_distance_squared() const -> int;
    [[nodiscard]] auto has_exceeded_threshold() const -> bool;

    // ── Drop zones ─────────────────────────────────────────────

    void register_drop_zone(DropZone zone);
    void clear_drop_zones();
    [[nodiscard]] auto active_drop_zone() const -> const DropZone*;
    void set_active_zone(const std::string& zone_id);
    void clear_active_zone();

    // ── Auto-scroll ────────────────────────────────────────────

    [[nodiscard]] auto needs_auto_scroll() const -> bool;
    [[nodiscard]] auto auto_scroll_direction() const -> int; ///< -1 = up, 0 = none, 1 = down
    void set_scroll_edge_margin(int margin_px);

    // ── Configuration ──────────────────────────────────────────

    void set_distance_threshold(int threshold_px);
    [[nodiscard]] auto distance_threshold() const -> int;

    static constexpr int kDefaultThreshold = 5;
    static constexpr int kDefaultAutoScrollMargin = 30;

private:
    DragPhase phase_{DragPhase::kIdle};
    DragPayloadType type_{DragPayloadType::kTab};
    DragPayload payload_;
    int start_x_{0};
    int start_y_{0};
    int current_x_{0};
    int current_y_{0};
    int distance_threshold_{kDefaultThreshold};
    int auto_scroll_margin_{kDefaultAutoScrollMargin};
    int viewport_height_{0};

    std::vector<DropZone> zones_;
    std::string active_zone_id_;
};

} // namespace markamp::ui
