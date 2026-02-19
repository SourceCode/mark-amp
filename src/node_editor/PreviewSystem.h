#pragma once

#include "NodeEditorTypes.h"

#include <chrono>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// PreviewData — thumbnail data for a node output
// ---------------------------------------------------------------------------

enum class PreviewFormat
{
    kNone,
    kRGBA8,   ///< 8-bit RGBA pixel data
    kFloat32, ///< Single-channel float (e.g. grayscale)
    kText     ///< Text representation of value
};

struct PreviewData
{
    NodeId node_id;
    int width{0};
    int height{0};
    PreviewFormat format{PreviewFormat::kNone};
    std::string text_value; ///< For kText format
    uint64_t generation{0}; ///< Incremented each update
    std::chrono::steady_clock::time_point timestamp;

    [[nodiscard]] auto is_valid() const -> bool
    {
        return format != PreviewFormat::kNone;
    }
};

// ---------------------------------------------------------------------------
// PreviewSystem — manages preview thumbnails and pin state
// ---------------------------------------------------------------------------

class PreviewSystem
{
public:
    PreviewSystem() = default;

    // --- Preview requests -------------------------------------------------

    /// Request preview generation for a node.
    void request_preview(NodeId node_id);

    /// Check if a preview has been requested.
    [[nodiscard]] auto is_requested(NodeId node_id) const -> bool;

    /// Cancel a pending preview request.
    void cancel_request(NodeId node_id);

    /// Store preview data (called after generation).
    void update_preview(NodeId node_id, const PreviewData& data);

    /// Get preview data for a node (returns nullptr if none).
    [[nodiscard]] auto preview_data(NodeId node_id) const -> const PreviewData*;

    // --- Pin management ---------------------------------------------------

    /// Toggle pin state for a node's preview.
    void toggle_pin(NodeId node_id);

    /// Check if a node's preview is pinned.
    [[nodiscard]] auto is_pinned(NodeId node_id) const -> bool;

    /// Pin a specific node's preview.
    void pin(NodeId node_id);

    /// Unpin a specific node's preview.
    void unpin(NodeId node_id);

    /// Get all pinned node IDs.
    [[nodiscard]] auto pinned_nodes() const -> std::vector<NodeId>;

    // --- Inline value display ---------------------------------------------

    /// Set inline value text for a socket.
    void set_inline_value(SocketId socket_id, const std::string& text);

    /// Get inline value text for a socket.
    [[nodiscard]] auto inline_value(SocketId socket_id) const -> std::string;

    /// Check if a socket has an inline value.
    [[nodiscard]] auto has_inline_value(SocketId socket_id) const -> bool;

    // --- Lifecycle --------------------------------------------------------

    /// Clear all previews and pins.
    void clear_all();

    /// Clear stale previews older than max_age.
    void clear_stale(std::chrono::milliseconds max_age);

    /// Total number of active previews.
    [[nodiscard]] auto preview_count() const -> std::size_t;

private:
    std::unordered_map<NodeId, PreviewData> previews_;
    std::unordered_set<NodeId> requested_;
    std::unordered_set<NodeId> pinned_;
    std::unordered_map<SocketId, std::string> inline_values_;
};

} // namespace markamp::node_editor
