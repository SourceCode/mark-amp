#include "NodeStatusModel.h"

#include <sstream>

namespace markamp::node_editor
{

void NodeStatusModel::set_status(NodeId node_id, NodeStatusLevel level, const std::string& msg)
{
    NodeStatus entry;
    entry.node_id = node_id;
    entry.level = level;
    entry.message = msg;
    entry.timestamp = std::chrono::steady_clock::now();
    statuses_[node_id.value] = std::move(entry);
}

auto NodeStatusModel::status(NodeId node_id) const -> const NodeStatus*
{
    auto iter = statuses_.find(node_id.value);
    if (iter == statuses_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

void NodeStatusModel::set_progress(NodeId node_id, float progress_val)
{
    auto iter = statuses_.find(node_id.value);
    if (iter != statuses_.end())
    {
        iter->second.progress = progress_val;
    }
}

void NodeStatusModel::clear_status(NodeId node_id)
{
    statuses_.erase(node_id.value);
}

void NodeStatusModel::clear_all()
{
    statuses_.clear();
    output_logs_.clear();
}

auto NodeStatusModel::status_summary() const -> StatusSummary
{
    StatusSummary summary;
    summary.total = statuses_.size();
    for (const auto& [_, entry] : statuses_)
    {
        switch (entry.level)
        {
            case NodeStatusLevel::kRunning:
                ++summary.running;
                break;
            case NodeStatusLevel::kSuccess:
                ++summary.success;
                break;
            case NodeStatusLevel::kWarning:
                ++summary.warning;
                break;
            case NodeStatusLevel::kError:
                ++summary.error;
                break;
            case NodeStatusLevel::kIdle:
                break;
        }
    }
    return summary;
}

auto NodeStatusModel::status_count() const -> std::size_t
{
    return statuses_.size();
}

auto NodeStatusModel::status_bar_text() const -> std::string
{
    auto summary = status_summary();
    std::ostringstream oss;
    oss << "Nodes: " << summary.total;
    if (summary.running > 0)
    {
        oss << " | Running: " << summary.running;
    }
    if (summary.error > 0)
    {
        oss << " | Errors: " << summary.error;
    }
    if (summary.warning > 0)
    {
        oss << " | Warnings: " << summary.warning;
    }
    return oss.str();
}

void NodeStatusModel::append_output(NodeId node_id, const std::string& line)
{
    output_logs_[node_id.value].push_back(line);
}

auto NodeStatusModel::output_lines(NodeId node_id) const -> std::vector<std::string>
{
    auto iter = output_logs_.find(node_id.value);
    if (iter == output_logs_.end())
    {
        return {};
    }
    return iter->second;
}

void NodeStatusModel::clear_output(NodeId node_id)
{
    output_logs_.erase(node_id.value);
}

auto NodeStatusModel::output_line_count(NodeId node_id) const -> std::size_t
{
    auto iter = output_logs_.find(node_id.value);
    if (iter == output_logs_.end())
    {
        return 0;
    }
    return iter->second.size();
}

} // namespace markamp::node_editor
