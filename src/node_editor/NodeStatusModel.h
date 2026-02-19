#pragma once

// V11 Phase 44: Output Problems And StatusBar Node Workflows
// Tracks per-node execution state for status-bar and output panel integration.

#include "NodeEditorTypes.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Node execution status level.
enum class NodeStatusLevel : uint8_t
{
    kIdle,
    kRunning,
    kSuccess,
    kWarning,
    kError
};

/// Per-node status entry.
struct NodeStatus
{
    NodeId node_id;
    NodeStatusLevel level{NodeStatusLevel::kIdle};
    std::string message;
    float progress{0.0F}; // 0.0–1.0
    std::chrono::steady_clock::time_point timestamp;
};

/// Aggregate status summary for the graph.
struct StatusSummary
{
    std::size_t total{0};
    std::size_t running{0};
    std::size_t success{0};
    std::size_t warning{0};
    std::size_t error{0};
};

/// Tracks node execution state and output logs.
class NodeStatusModel
{
public:
    NodeStatusModel() = default;

    // --- Status management ---
    void set_status(NodeId node_id, NodeStatusLevel level, const std::string& msg);
    [[nodiscard]] auto status(NodeId node_id) const -> const NodeStatus*;
    void set_progress(NodeId node_id, float progress_val);
    void clear_status(NodeId node_id);
    void clear_all();

    // --- Summary ---
    [[nodiscard]] auto status_summary() const -> StatusSummary;
    [[nodiscard]] auto status_count() const -> std::size_t;
    [[nodiscard]] auto status_bar_text() const -> std::string;

    // --- Output log ---
    void append_output(NodeId node_id, const std::string& line);
    [[nodiscard]] auto output_lines(NodeId node_id) const -> std::vector<std::string>;
    void clear_output(NodeId node_id);
    [[nodiscard]] auto output_line_count(NodeId node_id) const -> std::size_t;

private:
    std::unordered_map<uint64_t, NodeStatus> statuses_;
    std::unordered_map<uint64_t, std::vector<std::string>> output_logs_;
};

} // namespace markamp::node_editor
