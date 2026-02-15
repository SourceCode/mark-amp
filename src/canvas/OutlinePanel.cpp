#include "OutlinePanel.h"

#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/FrameObject.h"
#include "canvas/SectionObject.h"

#include <algorithm>
#include <unordered_set>

namespace markamp::canvas
{

// ═══════════════════════════════════════════════════════════════════════
// OutlineBuilder
// ═══════════════════════════════════════════════════════════════════════

auto OutlineBuilder::build_entries(const Board& board) -> std::vector<OutlineEntry>
{
    std::vector<OutlineEntry> entries;
    std::unordered_set<ObjectId> placed_ids; // Track objects already placed.

    // ── 1. Frames (sorted by z-index) with their contained objects ──

    struct FrameRecord
    {
        ObjectId frame_id;
        int z_index;
        std::string title;
    };

    std::vector<FrameRecord> frames;
    for (const auto& obj : board.objects())
    {
        if (!obj)
        {
            continue;
        }
        if (obj->type() == CanvasObjectType::Frame)
        {
            const auto* frame = dynamic_cast<const FrameObject*>(obj.get());
            std::string title = frame ? frame->title() : obj->name();
            frames.push_back({obj->id(), obj->z_index(), std::move(title)});
        }
    }
    std::sort(frames.begin(),
              frames.end(),
              [](const FrameRecord& lhs, const FrameRecord& rhs)
              { return lhs.z_index < rhs.z_index; });

    for (const auto& frame_rec : frames)
    {
        const auto* frame_obj = board.get_object(frame_rec.frame_id);
        if (!frame_obj)
        {
            continue;
        }

        const AABB frame_bounds = frame_obj->world_bounds();

        // Find objects contained within this frame.
        std::vector<ObjectId> children;
        for (const auto& obj : board.objects())
        {
            if (!obj || obj->id() == frame_rec.frame_id)
            {
                continue;
            }
            if (obj->type() == CanvasObjectType::Frame)
            {
                continue;
            }
            if (obj->type() == CanvasObjectType::Section)
            {
                continue;
            }

            const AABB obj_bounds = obj->world_bounds();
            // Simple containment check: object center within frame bounds.
            const double cx = (obj_bounds.min_x + obj_bounds.max_x) / 2.0;
            const double cy = (obj_bounds.min_y + obj_bounds.max_y) / 2.0;
            if (cx >= frame_bounds.min_x && cx <= frame_bounds.max_x && cy >= frame_bounds.min_y &&
                cy <= frame_bounds.max_y)
            {
                children.push_back(obj->id());
            }
        }

        OutlineEntry frame_entry;
        frame_entry.id = frame_rec.frame_id;
        frame_entry.label = frame_rec.title;
        frame_entry.type = CanvasObjectType::Frame;
        frame_entry.indent_level = 0;
        frame_entry.is_container = true;
        frame_entry.child_count = static_cast<int>(children.size());
        entries.push_back(std::move(frame_entry));
        placed_ids.insert(frame_rec.frame_id);

        for (const auto child_id : children)
        {
            const auto* child_obj = board.get_object(child_id);
            if (!child_obj)
            {
                continue;
            }

            OutlineEntry child_entry;
            child_entry.id = child_id;
            child_entry.label = child_obj->name();
            child_entry.type = child_obj->type();
            child_entry.indent_level = 1;
            entries.push_back(std::move(child_entry));
            placed_ids.insert(child_id);
        }
    }

    // ── 2. Sections ────────────────────────────────────────────────

    for (const auto& obj : board.objects())
    {
        if (!obj || obj->type() != CanvasObjectType::Section)
        {
            continue;
        }
        if (placed_ids.count(obj->id()) > 0)
        {
            continue;
        }

        const auto* section = dynamic_cast<const SectionObject*>(obj.get());
        OutlineEntry section_entry;
        section_entry.id = obj->id();
        section_entry.label = section ? section->title() : obj->name();
        section_entry.type = CanvasObjectType::Section;
        section_entry.indent_level = 0;
        section_entry.is_container = true;
        entries.push_back(std::move(section_entry));
        placed_ids.insert(obj->id());
    }

    // ── 3. Top-level (uncontained) objects ──────────────────────────

    for (const auto& obj : board.objects())
    {
        if (!obj)
        {
            continue;
        }
        if (placed_ids.count(obj->id()) > 0)
        {
            continue;
        }

        OutlineEntry top_entry;
        top_entry.id = obj->id();
        top_entry.label = obj->name();
        top_entry.type = obj->type();
        top_entry.indent_level = 0;
        entries.push_back(std::move(top_entry));
    }

    return entries;
}

// ═══════════════════════════════════════════════════════════════════════
// OutlinePanel
// ═══════════════════════════════════════════════════════════════════════

auto OutlinePanel::set_entries(const std::vector<OutlineEntry>& entries) -> void
{
    entries_ = entries;
}

auto OutlinePanel::entry_count() const -> size_t
{
    return entries_.size();
}

auto OutlinePanel::entry_at(size_t index) const -> const OutlineEntry&
{
    return entries_.at(index);
}

auto OutlinePanel::toggle_expand(ObjectId container_id) -> void
{
    for (auto& entry : entries_)
    {
        if (entry.id == container_id && entry.is_container)
        {
            entry.is_expanded = !entry.is_expanded;
            return;
        }
    }
}

auto OutlinePanel::visible_entries() const -> std::vector<const OutlineEntry*>
{
    std::vector<const OutlineEntry*> result;
    bool skip_children = false;
    int skip_below_level = -1;

    for (const auto& entry : entries_)
    {
        if (skip_children && entry.indent_level > skip_below_level)
        {
            continue;
        }
        skip_children = false;

        result.push_back(&entry);

        if (entry.is_container && !entry.is_expanded)
        {
            skip_children = true;
            skip_below_level = entry.indent_level;
        }
    }
    return result;
}

auto OutlinePanel::navigate_to(ObjectId target_id) -> void
{
    if (on_navigate_)
    {
        on_navigate_(target_id);
    }
}

auto OutlinePanel::set_on_navigate(OnNavigateCallback callback) -> void
{
    on_navigate_ = std::move(callback);
}

auto OutlinePanel::set_on_reorder(OnReorderCallback callback) -> void
{
    on_reorder_ = std::move(callback);
}

auto OutlinePanel::is_visible() const -> bool
{
    return visible_;
}
auto OutlinePanel::set_visible(bool visible) -> void
{
    visible_ = visible;
}

} // namespace markamp::canvas
