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

// ── Labels (#9) ─────────────────────────────────────────────

auto KanbanCard::labels() const -> const std::vector<std::string>&
{
    return labels_;
}
auto KanbanCard::add_label(const std::string& label) -> void
{
    labels_.push_back(label);
    mark_dirty();
}
auto KanbanCard::remove_label(const std::string& label) -> void
{
    labels_.erase(std::remove(labels_.begin(), labels_.end(), label), labels_.end());
    mark_dirty();
}
auto KanbanCard::clear_labels() -> void
{
    labels_.clear();
    mark_dirty();
}

// ── Checklist (#10) ─────────────────────────────────────────

auto KanbanCard::checklist() const -> const std::vector<ChecklistItem>&
{
    return checklist_;
}
auto KanbanCard::add_checklist_item(const std::string& text) -> void
{
    checklist_.push_back({text, false});
    mark_dirty();
}
auto KanbanCard::toggle_checklist_item(size_t index) -> void
{
    if (index < checklist_.size())
    {
        checklist_[index].done = !checklist_[index].done;
        mark_dirty();
    }
}
auto KanbanCard::checklist_progress() const -> double
{
    if (checklist_.empty())
    {
        return 0.0;
    }
    const auto done_count = std::count_if(
        checklist_.begin(), checklist_.end(), [](const ChecklistItem& item) { return item.done; });
    return static_cast<double>(done_count) / static_cast<double>(checklist_.size());
}

// ── Story Points (#11) ─────────────────────────────────────

auto KanbanCard::story_points() const -> int
{
    return story_points_;
}
auto KanbanCard::set_story_points(int points) -> void
{
    story_points_ = points;
    mark_dirty();
}

// ── Timestamps (#12) ───────────────────────────────────────

auto KanbanCard::created_at() const -> const std::string&
{
    return created_at_;
}
auto KanbanCard::set_created_at(const std::string& timestamp) -> void
{
    created_at_ = timestamp;
    mark_dirty();
}
auto KanbanCard::completed_at() const -> const std::string&
{
    return completed_at_;
}
auto KanbanCard::set_completed_at(const std::string& timestamp) -> void
{
    completed_at_ = timestamp;
    mark_dirty();
}

// ── Attachment count (#31) ──────────────────────────────────

auto KanbanCard::attachment_count() const -> int
{
    return attachment_count_;
}
auto KanbanCard::set_attachment_count(int count) -> void
{
    attachment_count_ = count;
    mark_dirty();
}

// ── Comment count (#32) ─────────────────────────────────────

auto KanbanCard::comment_count() const -> int
{
    return comment_count_;
}
auto KanbanCard::set_comment_count(int count) -> void
{
    comment_count_ = count;
    mark_dirty();
}

// ── Archived (#33) ─────────────────────────────────────────

auto KanbanCard::is_archived() const -> bool
{
    return archived_;
}
auto KanbanCard::set_archived(bool archived) -> void
{
    archived_ = archived;
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
        << ",\"card_index\":" << card_index_ << ",\"story_points\":" << story_points_
        << ",\"created_at\":\"" << created_at_ << "\""
        << ",\"completed_at\":\"" << completed_at_ << "\"";

    // Labels.
    oss << ",\"labels\":[";
    for (size_t idx = 0; idx < labels_.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ",";
        }
        oss << "\"" << labels_[idx] << "\"";
    }
    oss << "]";

    oss << "}";
    return oss.str();
}

auto KanbanCard::from_json(const std::string& json) -> void
{
    // Simple field extractor (production would use nlohmann/json).
    auto extract_str = [&json](const std::string& key) -> std::string
    {
        const std::string search = "\"" + key + "\"";
        auto key_pos = json.find(search);
        if (key_pos == std::string::npos)
        {
            return {};
        }
        auto colon = json.find(':', key_pos + search.size());
        if (colon == std::string::npos)
        {
            return {};
        }
        auto quote_start = json.find('"', colon + 1);
        if (quote_start == std::string::npos)
        {
            return {};
        }
        auto quote_end = json.find('"', quote_start + 1);
        if (quote_end == std::string::npos)
        {
            return {};
        }
        return json.substr(quote_start + 1, quote_end - quote_start - 1);
    };

    auto extract_int = [&json](const std::string& key) -> int
    {
        const std::string search = "\"" + key + "\"";
        auto key_pos = json.find(search);
        if (key_pos == std::string::npos)
        {
            return 0;
        }
        auto colon = json.find(':', key_pos + search.size());
        if (colon == std::string::npos)
        {
            return 0;
        }
        auto val_start = json.find_first_not_of(" \t", colon + 1);
        if (val_start == std::string::npos)
        {
            return 0;
        }
        try { return std::stoi(json.substr(val_start)); }
        catch (...) { return 0; }
    };

    title_ = extract_str("title");
    description_ = extract_str("description");
    assignee_ = extract_str("assignee");
    due_date_ = extract_str("due_date");
    created_at_ = extract_str("created_at");
    completed_at_ = extract_str("completed_at");
    priority_ = extract_int("priority");
    column_id_ = static_cast<ObjectId>(extract_int("column_id"));
    card_index_ = extract_int("card_index");
    story_points_ = extract_int("story_points");

    // Parse labels array: ["label1", "label2", ...]
    labels_.clear();
    auto labels_pos = json.find("\"labels\"");
    if (labels_pos != std::string::npos)
    {
        auto arr_start = json.find('[', labels_pos);
        auto arr_end = json.find(']', arr_start);
        if (arr_start != std::string::npos && arr_end != std::string::npos)
        {
            auto arr_content = json.substr(arr_start + 1, arr_end - arr_start - 1);
            size_t pos = 0;
            while (pos < arr_content.size())
            {
                auto qs = arr_content.find('"', pos);
                if (qs == std::string::npos) { break; }
                auto qe = arr_content.find('"', qs + 1);
                if (qe == std::string::npos) { break; }
                labels_.push_back(arr_content.substr(qs + 1, qe - qs - 1));
                pos = qe + 1;
            }
        }
    }
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
    copy->set_story_points(story_points_);
    copy->set_created_at(created_at_);
    copy->set_completed_at(completed_at_);
    for (const auto& label : labels_)
    {
        copy->add_label(label);
    }
    for (const auto& item : checklist_)
    {
        copy->add_checklist_item(item.text);
        if (item.done)
        {
            copy->toggle_checklist_item(copy->checklist().size() - 1);
        }
    }
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
auto KanbanColumn::set_column_width(double width) -> void
{
    column_width_ = width;
    mark_dirty();
}

// ── Collapsed (#13) ────────────────────────────────────────

auto KanbanColumn::is_collapsed() const -> bool
{
    return collapsed_;
}
auto KanbanColumn::set_collapsed(bool collapsed) -> void
{
    collapsed_ = collapsed;
    mark_dirty();
}

// ── Sort Order (#14) ───────────────────────────────────────

auto KanbanColumn::sort_order() const -> KanbanSortOrder
{
    return sort_order_;
}
auto KanbanColumn::set_sort_order(KanbanSortOrder order) -> void
{
    sort_order_ = order;
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
        << ",\"wip_limit\":" << wip_limit_ << ",\"width\":" << column_width_
        << ",\"collapsed\":" << (collapsed_ ? "true" : "false")
        << ",\"sort_order\":" << static_cast<int>(sort_order_) << ",\"cards\":[";
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

auto KanbanColumn::from_json(const std::string& json) -> void
{
    // Simple field extractor.
    auto extract_str = [&json](const std::string& key) -> std::string
    {
        const std::string search = "\"" + key + "\"";
        auto key_pos = json.find(search);
        if (key_pos == std::string::npos) { return {}; }
        auto colon = json.find(':', key_pos + search.size());
        if (colon == std::string::npos) { return {}; }
        auto quote_start = json.find('"', colon + 1);
        if (quote_start == std::string::npos) { return {}; }
        auto quote_end = json.find('"', quote_start + 1);
        if (quote_end == std::string::npos) { return {}; }
        return json.substr(quote_start + 1, quote_end - quote_start - 1);
    };

    auto extract_int = [&json](const std::string& key) -> int
    {
        const std::string search = "\"" + key + "\"";
        auto key_pos = json.find(search);
        if (key_pos == std::string::npos) { return 0; }
        auto colon = json.find(':', key_pos + search.size());
        if (colon == std::string::npos) { return 0; }
        auto val_start = json.find_first_not_of(" \t", colon + 1);
        if (val_start == std::string::npos) { return 0; }
        try { return std::stoi(json.substr(val_start)); }
        catch (...) { return 0; }
    };

    auto extract_double = [&json](const std::string& key) -> double
    {
        const std::string search = "\"" + key + "\"";
        auto key_pos = json.find(search);
        if (key_pos == std::string::npos) { return 0.0; }
        auto colon = json.find(':', key_pos + search.size());
        if (colon == std::string::npos) { return 0.0; }
        auto val_start = json.find_first_not_of(" \t", colon + 1);
        if (val_start == std::string::npos) { return 0.0; }
        try { return std::stod(json.substr(val_start)); }
        catch (...) { return 0.0; }
    };

    auto extract_bool = [&json](const std::string& key) -> bool
    {
        const std::string search = "\"" + key + "\"";
        auto key_pos = json.find(search);
        if (key_pos == std::string::npos) { return false; }
        auto colon = json.find(':', key_pos + search.size());
        if (colon == std::string::npos) { return false; }
        auto val_start = json.find_first_not_of(" \t", colon + 1);
        if (val_start == std::string::npos) { return false; }
        return json.substr(val_start, 4) == "true";
    };

    title_ = extract_str("title");
    description_ = extract_str("description");
    wip_limit_ = extract_int("wip_limit");
    column_width_ = extract_double("width");
    if (column_width_ <= 0.0) { column_width_ = 280.0; }
    collapsed_ = extract_bool("collapsed");
    sort_order_ = static_cast<KanbanSortOrder>(extract_int("sort_order"));

    // Parse card IDs array: [id1, id2, ...]
    card_ids_.clear();
    auto cards_pos = json.find("\"cards\"");
    if (cards_pos != std::string::npos)
    {
        auto arr_start = json.find('[', cards_pos);
        auto arr_end = json.find(']', arr_start);
        if (arr_start != std::string::npos && arr_end != std::string::npos)
        {
            auto arr_content = json.substr(arr_start + 1, arr_end - arr_start - 1);
            std::istringstream stream(arr_content);
            std::string token;
            while (std::getline(stream, token, ','))
            {
                try
                {
                    auto trimmed = token;
                    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
                    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
                    if (!trimmed.empty())
                    {
                        card_ids_.push_back(static_cast<ObjectId>(std::stoul(trimmed)));
                    }
                }
                catch (...) {}
            }
        }
    }
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
    copy->set_collapsed(collapsed_);
    copy->set_sort_order(sort_order_);
    copy->set_name(name());
    return copy;
}

// ── Batch 9 (#49-51) ──────────────────────────────────────────────

auto KanbanColumn::description() const -> const std::string&
{
    return description_;
}

auto KanbanColumn::set_description(const std::string& desc) -> void
{
    description_ = desc;
    mark_dirty();
}

auto KanbanColumn::is_over_wip_limit() const -> bool
{
    if (wip_limit_ <= 0)
    {
        return false;
    }
    return static_cast<int>(card_ids_.size()) > wip_limit_;
}

auto KanbanColumn::move_card_to_position(ObjectId card_id, int position) -> void
{
    // Remove the card from its current position.
    auto iter = std::find(card_ids_.begin(), card_ids_.end(), card_id);
    if (iter == card_ids_.end())
    {
        return;
    }
    card_ids_.erase(iter);

    // Insert at the new position (clamped).
    const int clamped_pos = std::clamp(position, 0, static_cast<int>(card_ids_.size()));
    card_ids_.insert(card_ids_.begin() + clamped_pos, card_id);
    mark_dirty();
}

} // namespace markamp::canvas
