#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

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
    auto set_column_width(double w) -> void;

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
};

} // namespace markamp::canvas
