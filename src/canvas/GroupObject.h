#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <algorithm>
#include <memory>
#include <vector>

namespace markamp::canvas
{

class Board;

/// A container object that groups child objects together.
/// Children retain their own transforms; the group provides aggregate operations
/// (move/resize/delete act on all children). Groups can nest.
class GroupObject : public CanvasObject
{
public:
    GroupObject();

    // ── Children ──────────────────────────────────────────────

    auto add_child(ObjectId child_id) -> void;
    auto remove_child(ObjectId child_id) -> void;
    [[nodiscard]] auto children() const -> const std::vector<ObjectId>&;
    [[nodiscard]] auto child_count() const -> size_t;
    [[nodiscard]] auto contains_child(ObjectId child_id) const -> bool;

    // ── Bounds ────────────────────────────────────────────────

    /// Update the cached bounds from child world‐bounds.
    auto update_bounds(const Board& board) -> void;

    // ── CanvasObject overrides ────────────────────────────────

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;

    /// Whether the group has no children.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return children_ids_.empty();
    }

    /// Whether the group has children.
    [[nodiscard]] auto has_children() const noexcept -> bool
    {
        return !children_ids_.empty();
    }

    // ── Round 2 Batch 1 (#6-10) ───────────────────────────────────

    /// (#6) First child ObjectId, or kInvalidObjectId if empty.
    [[nodiscard]] auto first_child_id() const noexcept -> ObjectId
    {
        return children_ids_.empty() ? kInvalidObjectId : children_ids_.front();
    }

    /// (#7) Last child ObjectId, or kInvalidObjectId if empty.
    [[nodiscard]] auto last_child_id() const noexcept -> ObjectId
    {
        return children_ids_.empty() ? kInvalidObjectId : children_ids_.back();
    }

    /// (#8) Whether the group contains exactly one child.
    [[nodiscard]] auto is_single_child() const noexcept -> bool
    {
        return children_ids_.size() == 1;
    }

    /// (#9) Cached bounding box width.
    [[nodiscard]] auto cached_width() const noexcept -> double
    {
        return cached_bounds_.width();
    }

    /// (#10) Cached bounding box height.
    [[nodiscard]] auto cached_height() const noexcept -> double
    {
        return cached_bounds_.height();
    }

private:
    std::vector<ObjectId> children_ids_;
    AABB cached_bounds_{0.0, 0.0, 0.0, 0.0};
};

} // namespace markamp::canvas
