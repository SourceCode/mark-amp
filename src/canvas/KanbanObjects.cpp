#include "KanbanObjects.h"

#include <algorithm>
#include <sstream>

namespace markamp::canvas
{

// ──────────────────────────── KanbanCard ────────────────────────────

KanbanCard::KanbanCard()
    : CanvasObject(CanvasObjectType::KanbanCard)
{
    set_name("Kanban Card");
}

auto KanbanCard::title() const -> const std::string&
{
    return title_;
}
auto KanbanCard::set_title(const std::string& title) -> void
{
    title_ = title;
    mark_dirty();
}
auto KanbanCard::description() const -> const std::string&
{
    return description_;
}
auto KanbanCard::set_description(const std::string& desc) -> void
{
    description_ = desc;
    mark_dirty();
}
auto KanbanCard::card_color() const -> const CanvasColor&
{
    return card_color_;
}
auto KanbanCard::set_card_color(const CanvasColor& color) -> void
{
    card_color_ = color;
    mark_dirty();
}
auto KanbanCard::assignee() const -> const std::string&
{
    return assignee_;
}
auto KanbanCard::set_assignee(const std::string& name) -> void
{
    assignee_ = name;
    mark_dirty();
}
auto KanbanCard::due_date() const -> const std::string&
{
    return due_date_;
}
auto KanbanCard::set_due_date(const std::string& date) -> void
{
    due_date_ = date;
    mark_dirty();
}
auto KanbanCard::priority() const -> int
{
    return priority_;
}
auto KanbanCard::set_priority(int priority) -> void
{
    priority_ = priority;
    mark_dirty();
}
auto KanbanCard::column_id() const -> ObjectId
{
    return column_id_;
}
auto KanbanCard::set_column_id(ObjectId id) -> void
{
    column_id_ = id;
    mark_dirty();
}
auto KanbanCard::card_index() const -> int
{
    return card_index_;
}
auto KanbanCard::set_card_index(int index) -> void
{
    card_index_ = index;
    mark_dirty();
}

auto KanbanCard::local_bounds() const -> AABB
{
    return {0.0, 0.0, 260.0, 80.0};
}

auto KanbanCard::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"KanbanCard\""
        << ",\"title\":\"" << title_ << "\""
        << ",\"description\":\"" << description_ << "\""
        << ",\"assignee\":\"" << assignee_ << "\""
        << ",\"due_date\":\"" << due_date_ << "\""
        << ",\"priority\":" << priority_ << ",\"column_id\":" << column_id_
        << ",\"card_index\":" << card_index_ << "}";
    return oss.str();
}

auto KanbanCard::from_json(const std::string& /*json*/) -> void
{ /* stub */
}

auto KanbanCard::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<KanbanCard>();
    copy->set_title(title_);
    copy->set_description(description_);
    copy->set_card_color(card_color_);
    copy->set_assignee(assignee_);
    copy->set_due_date(due_date_);
    copy->set_priority(priority_);
    copy->set_column_id(column_id_);
    copy->set_card_index(card_index_);
    copy->set_name(name());
    return copy;
}

// ──────────────────────────── KanbanColumn ────────────────────────────

KanbanColumn::KanbanColumn()
    : CanvasObject(CanvasObjectType::KanbanColumn)
{
    set_name("Kanban Column");
}

auto KanbanColumn::title() const -> const std::string&
{
    return title_;
}
auto KanbanColumn::set_title(const std::string& title) -> void
{
    title_ = title;
    mark_dirty();
}
auto KanbanColumn::column_color() const -> const CanvasColor&
{
    return column_color_;
}
auto KanbanColumn::set_column_color(const CanvasColor& color) -> void
{
    column_color_ = color;
    mark_dirty();
}
auto KanbanColumn::wip_limit() const -> int
{
    return wip_limit_;
}
auto KanbanColumn::set_wip_limit(int limit) -> void
{
    wip_limit_ = limit;
    mark_dirty();
}

auto KanbanColumn::card_ids() const -> const std::vector<ObjectId>&
{
    return card_ids_;
}

auto KanbanColumn::add_card(ObjectId id) -> void
{
    card_ids_.push_back(id);
    mark_dirty();
}

auto KanbanColumn::remove_card(ObjectId id) -> void
{
    card_ids_.erase(std::remove(card_ids_.begin(), card_ids_.end(), id), card_ids_.end());
    mark_dirty();
}

auto KanbanColumn::insert_card(ObjectId id, int index) -> void
{
    const auto pos = std::min(static_cast<size_t>(index), card_ids_.size());
    card_ids_.insert(card_ids_.begin() + static_cast<ptrdiff_t>(pos), id);
    mark_dirty();
}

auto KanbanColumn::card_count() const -> int
{
    return static_cast<int>(card_ids_.size());
}
auto KanbanColumn::column_width() const -> double
{
    return column_width_;
}
auto KanbanColumn::set_column_width(double w) -> void
{
    column_width_ = w;
    mark_dirty();
}

auto KanbanColumn::local_bounds() const -> AABB
{
    constexpr double kHeaderHeight = 50.0;
    constexpr double kCardHeight = 90.0;
    const double total_height = kHeaderHeight + static_cast<double>(card_ids_.size()) * kCardHeight;
    return {0.0, 0.0, column_width_, std::max(200.0, total_height)};
}

auto KanbanColumn::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"KanbanColumn\""
        << ",\"title\":\"" << title_ << "\""
        << ",\"wip_limit\":" << wip_limit_ << ",\"width\":" << column_width_ << ",\"cards\":[";
    for (size_t idx = 0; idx < card_ids_.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ",";
        }
        oss << card_ids_[idx];
    }
    oss << "]}";
    return oss.str();
}

auto KanbanColumn::from_json(const std::string& /*json*/) -> void
{ /* stub */
}

auto KanbanColumn::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<KanbanColumn>();
    copy->set_title(title_);
    copy->set_column_color(column_color_);
    copy->set_wip_limit(wip_limit_);
    for (const auto& card_id : card_ids_)
    {
        copy->add_card(card_id);
    }
    copy->set_column_width(column_width_);
    copy->set_name(name());
    return copy;
}

} // namespace markamp::canvas
