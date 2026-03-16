#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// A single comment/reply within a thread.
struct Comment
{
    std::string comment_id;
    std::string author_id;
    std::string author_name;
    std::string body;       ///< Markdown body text
    std::string created_at; ///< ISO-8601 timestamp
    bool is_resolved{false};
    std::vector<std::string> mentions; ///< @mentioned participant IDs

    // ── Round 2 Batch 3 (#26-29) ──────────────────────────────────

    /// (#26) Whether the comment body is empty.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return body.empty();
    }

    /// (#27) Whether the comment body has content.
    [[nodiscard]] auto has_body() const noexcept -> bool
    {
        return !body.empty();
    }

    /// (#28) Whether this comment has @mentions.
    [[nodiscard]] auto has_mention_list() const noexcept -> bool
    {
        return !mentions.empty();
    }

    /// (#29) Number of @mentions in this comment.
    [[nodiscard]] auto mention_count() const noexcept -> size_t
    {
        return mentions.size();
    }
};

/// Priority level for comment threads.
enum class CommentPriority : uint8_t
{
    kNormal,
    kHigh,
    kCritical
};

/// A comment thread anchored to a position on the canvas.
/// Treated as a first-class canvas object so it moves, scales, and
/// participates in selection/z-ordering like any other element.
class CommentObject : public CanvasObject
{
public:
    CommentObject();
    ~CommentObject() override = default;

    // Non-copyable, movable (matches base)
    CommentObject(const CommentObject&) = delete;
    auto operator=(const CommentObject&) -> CommentObject& = delete;
    CommentObject(CommentObject&&) = default;
    auto operator=(CommentObject&&) -> CommentObject& = default;

    /// Return "comment" as the object type name.
    [[nodiscard]] auto type_name() const -> std::string;

    /// Bounding box for the comment icon (fixed 24×24).
    [[nodiscard]] auto local_bounds() const -> AABB override;

    /// Deep clone.
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

    // ── Thread management ──────────────────────────────────────────

    /// Add a new comment to the thread.
    auto add_comment(const Comment& comment) -> void;

    /// Resolve the entire thread.
    auto resolve() -> void;

    /// Reopen a resolved thread.
    auto unresolve() -> void;

    [[nodiscard]] auto is_resolved() const -> bool;
    [[nodiscard]] auto comments() const -> const std::vector<Comment>&;
    [[nodiscard]] auto comment_count() const -> size_t;

    /// The initial/root comment of the thread.
    [[nodiscard]] auto root_comment() const -> const Comment*;

    // ── Metadata ───────────────────────────────────────────────────

    /// Thread creator.
    auto set_author(const std::string& author_id, const std::string& author_name) -> void;
    [[nodiscard]] auto author_id() const -> const std::string&;
    [[nodiscard]] auto author_name() const -> const std::string&;

    // ── Enhancements (#19-24) ─────────────────────────────────

    [[nodiscard]] auto is_pinned() const -> bool;
    auto set_pinned(bool pinned) -> void;

    [[nodiscard]] auto priority() const -> CommentPriority;
    auto set_priority(CommentPriority priority) -> void;

    [[nodiscard]] auto unresolved_count() const -> size_t;
    [[nodiscard]] auto last_activity_at() const -> const std::string&;

    auto edit_comment(const std::string& comment_id, const std::string& new_body) -> bool;
    auto delete_comment(const std::string& comment_id) -> bool;

    // ── Batch 8 (#43-45) ──────────────────────────────────────────

    /// Returns true if any comment in the thread contains @mentions.
    [[nodiscard]] auto has_mentions() const -> bool;

    /// Collect all unique @mentions across the thread.
    [[nodiscard]] auto all_mentions() const -> std::vector<std::string>;

    /// Lookup a specific comment in the thread by its ID.
    [[nodiscard]] auto comment_by_id(const std::string& comment_id) const -> const Comment*;

    // ── Round 2 Batch 3 (#21-25, #30) ─────────────────────────────

    /// (#21) Whether the thread has more than one comment.
    [[nodiscard]] auto is_thread() const noexcept -> bool
    {
        return comments_.size() > 1;
    }

    /// (#22) Whether the thread has replies.
    [[nodiscard]] auto has_replies() const noexcept -> bool
    {
        return comments_.size() > 1;
    }

    /// (#23) Whether this is a high or critical priority thread.
    [[nodiscard]] auto is_high_priority() const noexcept -> bool
    {
        return priority_ == CommentPriority::kHigh || priority_ == CommentPriority::kCritical;
    }

    /// (#24) Whether this is a critical priority thread.
    [[nodiscard]] auto is_critical_priority() const noexcept -> bool
    {
        return priority_ == CommentPriority::kCritical;
    }

    /// (#25) Whether the thread is open (not resolved).
    [[nodiscard]] auto is_open() const noexcept -> bool
    {
        return !resolved_;
    }

    /// (#30) Pointer to the last comment, or nullptr if empty.
    [[nodiscard]] auto last_comment() const noexcept -> const Comment*
    {
        return comments_.empty() ? nullptr : &comments_.back();
    }

    // ── Serialization ─────────────────────────────────────────────

    /// Populate fields from a JSON string.
    auto from_json(const std::string& json) -> void override;

private:
    std::vector<Comment> comments_;
    bool resolved_{false};
    std::string author_id_;
    std::string author_name_;
    bool pinned_{false};
    CommentPriority priority_{CommentPriority::kNormal};
};

} // namespace markamp::canvas
