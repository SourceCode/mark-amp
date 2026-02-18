// ============================================================================
// File: src/canvas/CanvasAccessibility.cpp
// Phase 11: Canvas Workbench Shell — accessibility implementation
// ============================================================================
#include "CanvasAccessibility.h"

#include <algorithm>
#include <fmt/format.h>

namespace markamp::canvas
{

CanvasAccessibility::CanvasAccessibility() = default;

CanvasAccessibility::CanvasAccessibility(const DeleteConfirmationConfig& delete_config)
    : delete_config_(delete_config)
{
}

// ── Screen Reader Announcements ────────────────────────────────────

auto CanvasAccessibility::announce_selection(const CanvasObject& obj) -> AccessibilityAnnouncement
{
    auto ann =
        AccessibilityAnnouncement{fmt::format("Selected {} \"{}\"", object_label(obj), obj.name()),
                                  AnnouncementPriority::kPolite};
    record_announcement(ann);
    return ann;
}

auto CanvasAccessibility::announce_multi_selection(size_t count) -> AccessibilityAnnouncement
{
    auto ann = AccessibilityAnnouncement{fmt::format("{} objects selected", count),
                                         AnnouncementPriority::kPolite};
    record_announcement(ann);
    return ann;
}

auto CanvasAccessibility::announce_creation(const CanvasObject& obj) -> AccessibilityAnnouncement
{
    auto ann = AccessibilityAnnouncement{fmt::format("Created {}", object_label(obj)),
                                         AnnouncementPriority::kAssertive};
    record_announcement(ann);
    return ann;
}

auto CanvasAccessibility::announce_deletion(size_t count) -> AccessibilityAnnouncement
{
    auto ann =
        AccessibilityAnnouncement{fmt::format("Deleted {} object{}", count, count == 1 ? "" : "s"),
                                  AnnouncementPriority::kAssertive};
    record_announcement(ann);
    return ann;
}

auto CanvasAccessibility::announce_tool_change(const std::string& tool_name)
    -> AccessibilityAnnouncement
{
    auto ann = AccessibilityAnnouncement{fmt::format("{} tool active", tool_name),
                                         AnnouncementPriority::kPolite};
    record_announcement(ann);
    return ann;
}

auto CanvasAccessibility::announce_zoom(double zoom_percent) -> AccessibilityAnnouncement
{
    auto ann = AccessibilityAnnouncement{fmt::format("Zoom {:.0f}%", zoom_percent),
                                         AnnouncementPriority::kPolite};
    record_announcement(ann);
    return ann;
}

auto CanvasAccessibility::announce(const std::string& message, AnnouncementPriority priority)
    -> AccessibilityAnnouncement
{
    return {message, priority};
}

// ── Object Description ─────────────────────────────────────────────

auto CanvasAccessibility::describe_object(const CanvasObject& obj) -> std::string
{
    const auto& xform = obj.transform();
    const auto& kBounds = obj.world_bounds();

    return fmt::format("{} \"{}\" at position ({:.0f}, {:.0f}), size {:.0f} × {:.0f}{}{}",
                       object_label(obj),
                       obj.name(),
                       xform.tx,
                       xform.ty,
                       kBounds.width(),
                       kBounds.height(),
                       obj.is_locked() ? ", locked" : "",
                       !obj.is_visible() ? ", hidden" : "");
}

auto CanvasAccessibility::object_label(const CanvasObject& obj) -> std::string
{
    switch (obj.type())
    {
        case CanvasObjectType::StickyNote:
            return "Sticky Note";
        case CanvasObjectType::TextBox:
            return "Text Box";
        case CanvasObjectType::Shape:
            return "Shape";
        case CanvasObjectType::Connector:
            return "Connector";
        case CanvasObjectType::FreehandPath:
            return "Drawing";
        case CanvasObjectType::Image:
            return "Image";
        case CanvasObjectType::Frame:
            return "Frame";
        case CanvasObjectType::Section:
            return "Section";
        case CanvasObjectType::Group:
            return "Group";
        case CanvasObjectType::Table:
            return "Table";
        case CanvasObjectType::MindMapNode:
            return "Mind Map Node";
        case CanvasObjectType::KanbanColumn:
            return "Kanban Column";
        case CanvasObjectType::KanbanCard:
            return "Kanban Card";
        case CanvasObjectType::BookmarkCard:
            return "Bookmark";
        case CanvasObjectType::VideoEmbed:
            return "Video";
        case CanvasObjectType::PDFPage:
            return "PDF Page";
        case CanvasObjectType::Icon:
            return "Icon";
        case CanvasObjectType::Comment:
            return "Comment";
        case CanvasObjectType::DiagramShape:
            return "Diagram Shape";
        case CanvasObjectType::CrossBoardLink:
            return "Link";
        case CanvasObjectType::AppWidget:
            return "Widget";
    }
    return "Object";
}

// ── Keyboard Focus Ring ────────────────────────────────────────────

auto CanvasAccessibility::set_focused_object(ObjectId obj_id) -> void
{
    focused_id_ = obj_id;
}

auto CanvasAccessibility::focused_object() const -> ObjectId
{
    return focused_id_;
}

auto CanvasAccessibility::focus_next(const std::vector<ObjectId>& tab_order) -> ObjectId
{
    if (tab_order.empty())
    {
        focused_id_ = kInvalidObjectId;
        return focused_id_;
    }

    if (focused_id_ == kInvalidObjectId)
    {
        focused_id_ = tab_order.front();
        return focused_id_;
    }

    auto found = std::find(tab_order.begin(), tab_order.end(), focused_id_);
    if (found == tab_order.end() || std::next(found) == tab_order.end())
    {
        focused_id_ = tab_order.front(); // Wrap around
    }
    else
    {
        focused_id_ = *std::next(found);
    }

    return focused_id_;
}

auto CanvasAccessibility::focus_previous(const std::vector<ObjectId>& tab_order) -> ObjectId
{
    if (tab_order.empty())
    {
        focused_id_ = kInvalidObjectId;
        return focused_id_;
    }

    if (focused_id_ == kInvalidObjectId)
    {
        focused_id_ = tab_order.back();
        return focused_id_;
    }

    auto found = std::find(tab_order.begin(), tab_order.end(), focused_id_);
    if (found == tab_order.end() || found == tab_order.begin())
    {
        focused_id_ = tab_order.back(); // Wrap around
    }
    else
    {
        focused_id_ = *std::prev(found);
    }

    return focused_id_;
}

auto CanvasAccessibility::clear_focus() -> void
{
    focused_id_ = kInvalidObjectId;
}

// ── Delete Confirmation ────────────────────────────────────────────

auto CanvasAccessibility::check_delete_confirmation(
    const std::vector<const CanvasObject*>& objects) const -> DeleteConfirmationInfo
{
    DeleteConfirmationInfo info;
    info.object_count = objects.size();

    if (!delete_config_.enabled)
    {
        info.reason = DeleteConfirmation::kNoConfirmNeeded;
        return info;
    }

    // Count locked objects and groups
    for (const auto* obj : objects)
    {
        if (obj == nullptr)
        {
            continue;
        }
        if (obj->is_locked())
        {
            ++info.locked_count;
        }
        if (obj->type() == CanvasObjectType::Group)
        {
            ++info.group_count;
        }
    }

    // Check confirmation conditions
    if (delete_config_.confirm_locked && info.locked_count > 0)
    {
        info.reason = DeleteConfirmation::kConfirmLocked;
        info.message = fmt::format("Selection contains {} locked object{}. Delete anyway?",
                                   info.locked_count,
                                   info.locked_count == 1 ? "" : "s");
    }
    else if (delete_config_.confirm_groups && info.group_count > 0)
    {
        info.reason = DeleteConfirmation::kConfirmGroup;
        info.message = fmt::format("Delete {} group{} and all children?",
                                   info.group_count,
                                   info.group_count == 1 ? "" : "s");
    }
    else if (info.object_count >= delete_config_.threshold)
    {
        info.reason = DeleteConfirmation::kConfirmMultiple;
        info.message = fmt::format("Delete {} objects?", info.object_count);
    }
    else
    {
        info.reason = DeleteConfirmation::kNoConfirmNeeded;
    }

    return info;
}

auto CanvasAccessibility::delete_config() const -> const DeleteConfirmationConfig&
{
    return delete_config_;
}

auto CanvasAccessibility::set_delete_config(const DeleteConfirmationConfig& config) -> void
{
    delete_config_ = config;
}

// ── Announcement History ───────────────────────────────────────────

auto CanvasAccessibility::recent_announcements() const
    -> const std::vector<AccessibilityAnnouncement>&
{
    return announcement_history_;
}

auto CanvasAccessibility::record_announcement(const AccessibilityAnnouncement& ann) -> void
{
    announcement_history_.push_back(ann);
    if (announcement_history_.size() > kMaxAnnouncementHistory)
    {
        announcement_history_.erase(announcement_history_.begin());
    }
}

} // namespace markamp::canvas
