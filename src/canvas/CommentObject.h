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

private:
    std::vector<Comment> comments_;
    bool resolved_{false};
    std::string author_id_;
    std::string author_name_;
    bool pinned_{false};
    CommentPriority priority_{CommentPriority::kNormal};
};

} // namespace markamp::canvas
