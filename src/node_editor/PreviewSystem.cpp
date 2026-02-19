#include "PreviewSystem.h"

#include <algorithm>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// Preview requests
// ---------------------------------------------------------------------------

void PreviewSystem::request_preview(NodeId node_id)
{
    requested_.insert(node_id);
}

auto PreviewSystem::is_requested(NodeId node_id) const -> bool
{
    return requested_.contains(node_id);
}

void PreviewSystem::cancel_request(NodeId node_id)
{
    requested_.erase(node_id);
}

void PreviewSystem::update_preview(NodeId node_id, const PreviewData& data)
{
    previews_[node_id] = data;
    requested_.erase(node_id);
}

auto PreviewSystem::preview_data(NodeId node_id) const -> const PreviewData*
{
    const auto found = previews_.find(node_id);
    if (found == previews_.end())
    {
        return nullptr;
    }
    return &found->second;
}

// ---------------------------------------------------------------------------
// Pin management
// ---------------------------------------------------------------------------

void PreviewSystem::toggle_pin(NodeId node_id)
{
    if (pinned_.contains(node_id))
    {
        pinned_.erase(node_id);
    }
    else
    {
        pinned_.insert(node_id);
    }
}

auto PreviewSystem::is_pinned(NodeId node_id) const -> bool
{
    return pinned_.contains(node_id);
}

void PreviewSystem::pin(NodeId node_id)
{
    pinned_.insert(node_id);
}

void PreviewSystem::unpin(NodeId node_id)
{
    pinned_.erase(node_id);
}

auto PreviewSystem::pinned_nodes() const -> std::vector<NodeId>
{
    return {pinned_.begin(), pinned_.end()};
}

// ---------------------------------------------------------------------------
// Inline value display
// ---------------------------------------------------------------------------

void PreviewSystem::set_inline_value(SocketId socket_id, const std::string& text)
{
    inline_values_[socket_id] = text;
}

auto PreviewSystem::inline_value(SocketId socket_id) const -> std::string
{
    const auto found = inline_values_.find(socket_id);
    if (found == inline_values_.end())
    {
        return {};
    }
    return found->second;
}

auto PreviewSystem::has_inline_value(SocketId socket_id) const -> bool
{
    return inline_values_.contains(socket_id);
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void PreviewSystem::clear_all()
{
    previews_.clear();
    requested_.clear();
    pinned_.clear();
    inline_values_.clear();
}

void PreviewSystem::clear_stale(std::chrono::milliseconds max_age)
{
    const auto now = std::chrono::steady_clock::now();
    auto iter = previews_.begin();
    while (iter != previews_.end())
    {
        // Skip pinned previews.
        if (pinned_.contains(iter->first))
        {
            ++iter;
            continue;
        }

        const auto age =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - iter->second.timestamp);
        if (age > max_age)
        {
            iter = previews_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

auto PreviewSystem::preview_count() const -> std::size_t
{
    return previews_.size();
}

} // namespace markamp::node_editor
