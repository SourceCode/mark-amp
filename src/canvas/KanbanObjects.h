#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Checklist item within a Kanban card.
struct ChecklistItem
{
    std::string text;
    bool done{false};
};

/// Sort order for cards within a Kanban column.
enum class KanbanSortOrder : uint8_t
{
    kManual,
    kPriority,
    kDueDate,
    kAssignee
};

/// A single card in a Kanban board with title, description, assignee, etc.
class KanbanCard : public CanvasObject
{
public:
    KanbanCard();

    [[nodiscard]] auto title() const -> const std::string&;
    auto set_title(const std::string& title) -> void;
    [[nodiscard]] auto description() const -> const std::string&;
    auto set_description(const std::string& desc) -> void;
    [[nodiscard]] auto card_color() const -> const CanvasColor&;
    auto set_card_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto assignee() const -> const std::string&;
    auto set_assignee(const std::string& name) -> void;
    [[nodiscard]] auto due_date() const -> const std::string&;
    auto set_due_date(const std::string& date) -> void;
    [[nodiscard]] auto priority() const -> int;
    auto set_priority(int priority) -> void;
    [[nodiscard]] auto column_id() const -> ObjectId;
    auto set_column_id(ObjectId id) -> void;
    [[nodiscard]] auto card_index() const -> int;
    auto set_card_index(int index) -> void;

    // ── Labels (#9) ─────────────────────────────────────────────
    [[nodiscard]] auto labels() const -> const std::vector<std::string>&;
    auto add_label(const std::string& label) -> void;
    auto remove_label(const std::string& label) -> void;
    auto clear_labels() -> void;

    // ── Checklist (#10) ─────────────────────────────────────────
    [[nodiscard]] auto checklist() const -> const std::vector<ChecklistItem>&;
    auto add_checklist_item(const std::string& text) -> void;
    auto toggle_checklist_item(size_t index) -> void;
    [[nodiscard]] auto checklist_progress() const -> double;

    // ── Story Points (#11) ─────────────────────────────────────
    [[nodiscard]] auto story_points() const -> int;
    auto set_story_points(int points) -> void;

    // ── Timestamps (#12) ───────────────────────────────────────
    [[nodiscard]] auto created_at() const -> const std::string&;
    auto set_created_at(const std::string& timestamp) -> void;
    [[nodiscard]] auto completed_at() const -> const std::string&;
    auto set_completed_at(const std::string& timestamp) -> void;

    // ── Attachment count (#31) ──────────────────────────────────
    [[nodiscard]] auto attachment_count() const -> int;
    auto set_attachment_count(int count) -> void;

    // ── Comment count (#32) ─────────────────────────────────────
    [[nodiscard]] auto comment_count() const -> int;
    auto set_comment_count(int count) -> void;

    // ── Archived (#33) ─────────────────────────────────────────
    [[nodiscard]] auto is_archived() const -> bool;
    auto set_archived(bool archived) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string title_{"New Card"};
    std::string description_;
    CanvasColor card_color_{255, 255, 255, 255};
    std::string assignee_;
    std::string due_date_;
    int priority_{0};
    ObjectId column_id_{kInvalidObjectId};
    int card_index_{0};
    std::vector<std::string> labels_;
    std::vector<ChecklistItem> checklist_;
    int story_points_{0};
    std::string created_at_;
    std::string completed_at_;
    int attachment_count_{0};
    int comment_count_{0};
    bool archived_{false};
};

/// A column in a Kanban board containing an ordered list of cards.
class KanbanColumn : public CanvasObject
{
public:
    KanbanColumn();

    [[nodiscard]] auto title() const -> const std::string&;
    auto set_title(const std::string& title) -> void;
    [[nodiscard]] auto column_color() const -> const CanvasColor&;
    auto set_column_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto wip_limit() const -> int;
    auto set_wip_limit(int limit) -> void;
    [[nodiscard]] auto card_ids() const -> const std::vector<ObjectId>&;
    auto add_card(ObjectId id) -> void;
    auto remove_card(ObjectId id) -> void;
    auto insert_card(ObjectId id, int index) -> void;
    [[nodiscard]] auto card_count() const -> int;
    [[nodiscard]] auto column_width() const -> double;
    auto set_column_width(double width) -> void;

    // ── Collapsed (#13) ────────────────────────────────────────
    [[nodiscard]] auto is_collapsed() const -> bool;
    auto set_collapsed(bool collapsed) -> void;

    // ── Sort Order (#14) ───────────────────────────────────────
    [[nodiscard]] auto sort_order() const -> KanbanSortOrder;
    auto set_sort_order(KanbanSortOrder order) -> void;

    // ── Batch 9 (#49-51) ──────────────────────────────────────────

    /// Column description text.
    [[nodiscard]] auto description() const -> const std::string&;
    auto set_description(const std::string& desc) -> void;

    /// Check if column is over its WIP limit.
    [[nodiscard]] auto is_over_wip_limit() const -> bool;

    /// Move a card within this column to a specific position.
    auto move_card_to_position(ObjectId card_id, int position) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string title_{"Column"};
    CanvasColor column_color_{240, 240, 240, 255};
    int wip_limit_{0}; // 0 = no limit
    std::vector<ObjectId> card_ids_;
    double column_width_{280.0};
    bool collapsed_{false};
    KanbanSortOrder sort_order_{KanbanSortOrder::kManual};
    std::string description_;
};

} // namespace markamp::canvas
