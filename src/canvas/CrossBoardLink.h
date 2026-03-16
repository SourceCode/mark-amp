#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <optional>
#include <string>

namespace markamp::canvas
{

/// Describes which board (and optionally which frame/object within it) a
/// cross-board link points to.
struct BoardLinkTarget
{
    std::string board_id;
    std::string board_name;
    std::optional<ObjectId> frame_id;  // Optional: link to specific frame
    std::optional<ObjectId> object_id; // Optional: link to specific object

    /// Whether the link targets a specific frame.
    [[nodiscard]] auto has_frame() const noexcept -> bool
    {
        return frame_id.has_value();
    }

    /// Whether the link targets a specific object.
    [[nodiscard]] auto has_object() const noexcept -> bool
    {
        return object_id.has_value();
    }

    /// Whether this is a board-only link (no frame/object target).
    [[nodiscard]] auto is_board_only() const noexcept -> bool
    {
        return !frame_id.has_value() && !object_id.has_value();
    }

    // ── Round 3 Batch 7 (#67-68) ────────────────────────────────

    /// (#67) Whether a board name is set.
    [[nodiscard]] auto has_name() const noexcept -> bool
    {
        return !board_name.empty();
    }

    /// (#68) Whether a board ID is set.
    [[nodiscard]] auto has_board_id() const noexcept -> bool
    {
        return !board_id.empty();
    }
};

/// A canvas object that acts as a hyperlink to another board.
class CrossBoardLinkObject : public CanvasObject
{
public:
    CrossBoardLinkObject();

    [[nodiscard]] auto target() const -> const BoardLinkTarget&;
    auto set_target(const BoardLinkTarget& target) -> void;

    [[nodiscard]] auto display_text() const -> const std::string&;
    auto set_display_text(const std::string& text) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;

    // ── Improvements (#33-36) ─────────────────────────────────

    [[nodiscard]] auto tooltip() const -> const std::string&;
    auto set_tooltip(const std::string& tip) -> void;

    [[nodiscard]] auto link_color() const -> const CanvasColor&;
    auto set_link_color(const CanvasColor& color) -> void;

    [[nodiscard]] auto is_bidirectional() const -> bool;
    auto set_bidirectional(bool bidirectional) -> void;

    [[nodiscard]] auto last_verified_at() const -> const std::string&;
    auto set_last_verified_at(const std::string& timestamp) -> void;

    // ── Batch 10 (#55-56) ─────────────────────────────────────────

    /// Check if the link target is broken (empty board_id).
    [[nodiscard]] auto is_broken() const -> bool;

    /// Short badge label overlay (e.g. "NEW", "3").
    [[nodiscard]] auto badge_text() const -> const std::string&;
    auto set_badge_text(const std::string& text) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

    /// Whether a badge text is set.
    [[nodiscard]] auto has_badge() const noexcept -> bool
    {
        return !badge_text_.empty();
    }

    /// Whether a tooltip is set.
    [[nodiscard]] auto has_tooltip_set() const noexcept -> bool
    {
        return !tooltip_.empty();
    }

    // ── Round 3 Batch 7 (#69-70) ────────────────────────────────

    /// (#69) Whether this is a one-way (unidirectional) link.
    [[nodiscard]] auto is_unidirectional() const noexcept -> bool
    {
        return !bidirectional_;
    }

    /// (#70) Whether the link has been verified.
    [[nodiscard]] auto has_verified() const noexcept -> bool
    {
        return !last_verified_at_.empty();
    }

private:
    BoardLinkTarget target_;
    std::string display_text_{"Link to board"};
    double width_{200.0};
    double height_{60.0};
    std::string tooltip_;
    CanvasColor link_color_{66, 133, 244, 255};
    bool bidirectional_{false};
    std::string last_verified_at_;
    std::string badge_text_;
};

} // namespace markamp::canvas
