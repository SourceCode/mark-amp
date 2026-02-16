#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// A single node in a mind map tree, connected to its parent via a connector.
class MindMapNode : public CanvasObject
{
public:
    MindMapNode();

    [[nodiscard]] auto text() const -> const std::string&;
    auto set_text(const std::string& text) -> void;

    [[nodiscard]] auto parent_node_id() const -> ObjectId;
    auto set_parent_node_id(ObjectId id) -> void;

    [[nodiscard]] auto child_node_ids() const -> const std::vector<ObjectId>&;
    auto add_child_node_id(ObjectId id) -> void;
    auto remove_child_node_id(ObjectId id) -> void;

    [[nodiscard]] auto connector_id() const -> ObjectId;
    auto set_connector_id(ObjectId id) -> void;

    [[nodiscard]] auto node_color() const -> const CanvasColor&;
    auto set_node_color(const CanvasColor& color) -> void;

    [[nodiscard]] auto depth() const -> int;
    auto set_depth(int d) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto set_dimensions(double node_width, double node_height) -> void;

    // ── Extended properties (#19-23) ────────────────────────────

    [[nodiscard]] auto icon() const -> const std::string&;
    auto set_icon(const std::string& icon_name) -> void;

    [[nodiscard]] auto notes() const -> const std::string&;
    auto set_notes(const std::string& notes_text) -> void;

    [[nodiscard]] auto is_collapsed() const -> bool;
    auto set_collapsed(bool collapsed) -> void;

    /// Priority level (clamped 0-3).
    [[nodiscard]] auto priority() const -> int;
    auto set_priority(int level) -> void;

    /// Progress percentage (clamped 0-100).
    [[nodiscard]] auto progress() const -> int;
    auto set_progress(int percent) -> void;

    // ── Batch 9 (#54) ─────────────────────────────────────────────

    /// Returns true if this node has no children.
    [[nodiscard]] auto is_leaf() const -> bool;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string text_{"Topic"};
    ObjectId parent_node_id_{kInvalidObjectId};
    std::vector<ObjectId> child_node_ids_;
    ObjectId connector_id_{kInvalidObjectId};
    CanvasColor node_color_{66, 133, 244, 255};
    int depth_{0};
    double width_{120.0};
    double height_{40.0};
    std::string icon_;
    std::string notes_;
    bool collapsed_{false};
    int priority_{0};
    int progress_{0};
};

} // namespace markamp::canvas
