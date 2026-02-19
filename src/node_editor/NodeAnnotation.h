#pragma once

// V11 Phase 29: Node Frames Comments Annotations And Backdrops
// Annotation layer: frames, comments, and text labels on the canvas.

#include "NodeEditorTypes.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Type of annotation.
enum class AnnotationType : uint8_t
{
    Frame,
    Comment,
    TextLabel
};

/// An annotation is a visual-only canvas decoration: frame, comment, or label.
struct Annotation
{
    FrameId frame_id;
    AnnotationType type{AnnotationType::Frame};
    Rect bounds;
    std::string label;
    std::string color_hex{"#808080"};
    std::vector<NodeId> contained_nodes; // Nodes inside this frame
    bool visible{true};
    int z_order{0};
};

/// Manages the annotation layer for a node graph.
class NodeAnnotation
{
public:
    NodeAnnotation() = default;

    // --- CRUD ---
    auto add_frame(Rect bounds, const std::string& label) -> FrameId;
    auto add_comment(Vec2 position, const std::string& text) -> FrameId;
    auto add_text_label(Vec2 position, const std::string& text) -> FrameId;
    auto remove(FrameId annotation_id) -> bool;

    // --- Queries ---
    [[nodiscard]] auto find(FrameId annotation_id) const -> const Annotation*;
    auto find_mut(FrameId annotation_id) -> Annotation*;
    [[nodiscard]] auto count() const -> std::size_t;
    [[nodiscard]] auto all_ids() const -> std::vector<FrameId>;
    [[nodiscard]] auto frames_at(Vec2 world_pos) const -> std::vector<FrameId>;

    // --- Containment ---
    void update_containment(FrameId annotation_id, const std::vector<NodeId>& contained);
    [[nodiscard]] auto containing_frame(Vec2 node_center) const -> FrameId;

    // --- Bulk ---
    void clear();

private:
    uint64_t next_id_{1};
    std::unordered_map<FrameId, Annotation> annotations_;

    [[nodiscard]] auto generate_id() -> FrameId;
};

} // namespace markamp::node_editor
