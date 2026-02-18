// ============================================================================
// File: src/canvas/RemoteSelectionOverlay.cpp
// Phase 13: Canvas Collaboration — remote selection highlighting overlay
// ============================================================================
#include "canvas/RemoteSelectionOverlay.h"

#include <algorithm>

namespace markamp::canvas
{

RemoteSelectionOverlay::RemoteSelectionOverlay() = default;

RemoteSelectionOverlay::RemoteSelectionOverlay(SelectionOverlayConfig config)
    : config_(config)
{
}

// ── Selection Updates ─────────────────────────────────────────────

auto RemoteSelectionOverlay::update_selection(const std::string& participant_id,
                                              const std::string& display_name,
                                              const std::vector<ObjectId>& selected_ids,
                                              const CanvasColor& color) -> void
{
    SelectionEntry entry;
    entry.participant_id = participant_id;
    entry.display_name = display_name;
    entry.color = color;
    entry.selected_ids = selected_ids;
    entry.border_width = config_.border_width;
    entry.show_badge = config_.show_badges;

    selections_[participant_id] = std::move(entry);
}

auto RemoteSelectionOverlay::clear_selection(const std::string& participant_id) -> void
{
    auto iter = selections_.find(participant_id);
    if (iter != selections_.end())
    {
        iter->second.selected_ids.clear();
    }
}

auto RemoteSelectionOverlay::remove_participant(const std::string& participant_id) -> void
{
    selections_.erase(participant_id);
}

auto RemoteSelectionOverlay::clear_all() -> void
{
    selections_.clear();
}

// ── Query ─────────────────────────────────────────────────────────

auto RemoteSelectionOverlay::selections() const
    -> const std::unordered_map<std::string, SelectionEntry>&
{
    return selections_;
}

auto RemoteSelectionOverlay::find_selection(const std::string& participant_id) const
    -> const SelectionEntry*
{
    auto iter = selections_.find(participant_id);
    if (iter != selections_.end())
    {
        return &iter->second;
    }
    return nullptr;
}

auto RemoteSelectionOverlay::is_remotely_selected(ObjectId obj_id) const -> bool
{
    for (const auto& [pid, entry] : selections_)
    {
        if (std::ranges::find(entry.selected_ids, obj_id) != entry.selected_ids.end())
        {
            return true;
        }
    }
    return false;
}

auto RemoteSelectionOverlay::participants_selecting(ObjectId obj_id) const
    -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& [pid, entry] : selections_)
    {
        if (std::ranges::find(entry.selected_ids, obj_id) != entry.selected_ids.end())
        {
            result.push_back(pid);
        }
    }
    return result;
}

auto RemoteSelectionOverlay::conflicts() const -> std::vector<SelectionConflict>
{
    const auto obj_map = build_object_participant_map();

    std::vector<SelectionConflict> result;
    for (const auto& [obj_id, participants] : obj_map)
    {
        if (participants.size() > 1)
        {
            SelectionConflict conflict;
            conflict.object_id = obj_id;
            conflict.participant_ids = participants;
            result.push_back(std::move(conflict));
        }
    }
    return result;
}

auto RemoteSelectionOverlay::active_selection_count() const -> size_t
{
    size_t active = 0;
    for (const auto& [pid, entry] : selections_)
    {
        if (!entry.selected_ids.empty())
        {
            ++active;
        }
    }
    return active;
}

auto RemoteSelectionOverlay::total_selected_objects() const -> size_t
{
    std::unordered_set<ObjectId> unique_ids;
    for (const auto& [pid, entry] : selections_)
    {
        for (const auto obj_id : entry.selected_ids)
        {
            unique_ids.insert(obj_id);
        }
    }
    return unique_ids.size();
}

// ── Configuration ─────────────────────────────────────────────────

auto RemoteSelectionOverlay::config() const -> const SelectionOverlayConfig&
{
    return config_;
}

auto RemoteSelectionOverlay::set_config(const SelectionOverlayConfig& config) -> void
{
    config_ = config;
}

auto RemoteSelectionOverlay::set_badges_visible(bool visible) -> void
{
    for (auto& [pid, entry] : selections_)
    {
        entry.show_badge = visible;
    }
}

auto RemoteSelectionOverlay::badges_visible() const -> bool
{
    return config_.show_badges;
}

// ── Private ───────────────────────────────────────────────────────

auto RemoteSelectionOverlay::build_object_participant_map() const
    -> std::unordered_map<ObjectId, std::vector<std::string>>
{
    std::unordered_map<ObjectId, std::vector<std::string>> result;
    for (const auto& [pid, entry] : selections_)
    {
        for (const auto obj_id : entry.selected_ids)
        {
            result[obj_id].push_back(pid);
        }
    }
    return result;
}

} // namespace markamp::canvas
