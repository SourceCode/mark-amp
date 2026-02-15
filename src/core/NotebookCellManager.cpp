/// @file NotebookCellManager.cpp
/// @brief V4 Phase 34 – Cell Metadata, Execution Tracking, Variable Inspector implementation.

#include "core/NotebookCellManager.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// CellMetadata helpers
// ============================================================================

auto CellMetadata::execution_label() const -> std::string
{
    if (execution_count > 0)
    {
        return "[" + std::to_string(execution_count) + "]";
    }
    return "[ ]";
}

auto CellMetadata::is_running() const -> bool
{
    return started_at.has_value() && !completed_at.has_value();
}

// ============================================================================
// NotebookCellManager – Constructor
// ============================================================================

NotebookCellManager::NotebookCellManager(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// ID generation
// ============================================================================

auto NotebookCellManager::generate_cell_id() -> std::string
{
    return "cell-" + std::to_string(next_cell_id_++);
}

// ============================================================================
// Cell lookup
// ============================================================================

auto NotebookCellManager::find_cell(const std::string& cell_id)
    -> std::vector<NotebookCell>::iterator
{
    return std::find_if(cells_.begin(),
                        cells_.end(),
                        [&cell_id](const NotebookCell& cell)
                        { return cell.metadata.cell_id == cell_id; });
}

auto NotebookCellManager::find_cell(const std::string& cell_id) const
    -> std::vector<NotebookCell>::const_iterator
{
    return std::find_if(cells_.begin(),
                        cells_.end(),
                        [&cell_id](const NotebookCell& cell)
                        { return cell.metadata.cell_id == cell_id; });
}

// ============================================================================
// Cell CRUD
// ============================================================================

auto NotebookCellManager::add_cell(CellType type, int position) -> std::string
{
    NotebookCell cell;
    cell.metadata.cell_id = generate_cell_id();
    cell.metadata.type = type;

    int insert_pos = 0;
    if (position < 0 || position >= static_cast<int>(cells_.size()))
    {
        insert_pos = static_cast<int>(cells_.size());
        cells_.push_back(std::move(cell));
    }
    else
    {
        insert_pos = position;
        cells_.insert(cells_.begin() + position, std::move(cell));
    }

    const auto& inserted_id = cells_[static_cast<size_t>(insert_pos)].metadata.cell_id;

    events::CellAddedEvent event;
    event.cell_id = inserted_id;
    event.position = insert_pos;
    event_bus_.publish(event);

    return inserted_id;
}

auto NotebookCellManager::remove_cell(const std::string& cell_id) -> void
{
    auto iter = find_cell(cell_id);
    if (iter == cells_.end())
    {
        return;
    }

    cells_.erase(iter);

    events::CellRemovedEvent event;
    event.cell_id = cell_id;
    event_bus_.publish(event);
}

auto NotebookCellManager::move_cell(const std::string& cell_id, int new_position) -> void
{
    auto iter = find_cell(cell_id);
    if (iter == cells_.end())
    {
        return;
    }

    const int old_position = static_cast<int>(std::distance(cells_.begin(), iter));
    if (old_position == new_position)
    {
        return;
    }

    NotebookCell cell = std::move(*iter);
    cells_.erase(iter);

    const int clamped = std::clamp(new_position, 0, static_cast<int>(cells_.size()));
    cells_.insert(cells_.begin() + clamped, std::move(cell));

    events::CellMovedEvent event;
    event.cell_id = cell_id;
    event.old_position = old_position;
    event.new_position = clamped;
    event_bus_.publish(event);
}

auto NotebookCellManager::set_cell_source(const std::string& cell_id, const std::string& source)
    -> void
{
    auto iter = find_cell(cell_id);
    if (iter != cells_.end())
    {
        iter->source = source;
    }
}

// ============================================================================
// Cell Queries
// ============================================================================

auto NotebookCellManager::get_cell(const std::string& cell_id) const -> std::optional<NotebookCell>
{
    auto iter = find_cell(cell_id);
    if (iter == cells_.end())
    {
        return std::nullopt;
    }
    return *iter;
}

auto NotebookCellManager::cells() const -> const std::vector<NotebookCell>&
{
    return cells_;
}

auto NotebookCellManager::cell_count() const -> int
{
    return static_cast<int>(cells_.size());
}

auto NotebookCellManager::cell_at(int position) const -> const NotebookCell&
{
    return cells_.at(static_cast<size_t>(position));
}

// ============================================================================
// Cell Metadata
// ============================================================================

auto NotebookCellManager::set_collapsed_input(const std::string& cell_id, bool collapsed) -> void
{
    auto iter = find_cell(cell_id);
    if (iter != cells_.end())
    {
        iter->metadata.collapsed_input = collapsed;
    }
}

auto NotebookCellManager::set_collapsed_output(const std::string& cell_id, bool collapsed) -> void
{
    auto iter = find_cell(cell_id);
    if (iter != cells_.end())
    {
        iter->metadata.collapsed_output = collapsed;
    }
}

auto NotebookCellManager::add_cell_tag(const std::string& cell_id, const std::string& tag) -> void
{
    auto iter = find_cell(cell_id);
    if (iter != cells_.end())
    {
        // Avoid duplicates.
        auto& tags = iter->metadata.tags;
        if (std::find(tags.begin(), tags.end(), tag) == tags.end())
        {
            tags.push_back(tag);
        }
    }
}

auto NotebookCellManager::record_execution(const std::string& cell_id, int count, double elapsed_ms)
    -> void
{
    auto iter = find_cell(cell_id);
    if (iter != cells_.end())
    {
        iter->metadata.execution_count = count;
        iter->metadata.execution_time_ms = elapsed_ms;
        iter->metadata.completed_at = std::chrono::system_clock::now();
    }

    execution_order_.push_back(cell_id);

    events::CellExecutionRecordedEvent event;
    event.cell_id = cell_id;
    event.execution_count = count;
    event.elapsed_ms = elapsed_ms;
    event_bus_.publish(event);
}

// ============================================================================
// Execution Tracking
// ============================================================================

auto NotebookCellManager::next_execution_count() -> int
{
    return ++execution_counter_;
}

auto NotebookCellManager::execution_order() const -> const std::vector<std::string>&
{
    return execution_order_;
}

// ============================================================================
// Serialization
// ============================================================================

auto NotebookCellManager::serialize_to_json() const -> std::string
{
    // Simplified Jupyter-like JSON format.
    std::ostringstream oss;
    oss << "{\"cells\":[";

    for (size_t idx = 0; idx < cells_.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ",";
        }

        const auto& cell = cells_[idx];
        oss << "{\"cell_id\":\"" << cell.metadata.cell_id << "\"";

        // Cell type.
        switch (cell.metadata.type)
        {
            case CellType::kCode:
                oss << ",\"cell_type\":\"code\"";
                break;
            case CellType::kMarkdown:
                oss << ",\"cell_type\":\"markdown\"";
                break;
            case CellType::kRaw:
                oss << ",\"cell_type\":\"raw\"";
                break;
        }

        // Source.
        oss << ",\"source\":\"";
        // Escape the source string.
        for (const char ch : cell.source)
        {
            if (ch == '"')
            {
                oss << "\\\"";
            }
            else if (ch == '\n')
            {
                oss << "\\n";
            }
            else if (ch == '\\')
            {
                oss << "\\\\";
            }
            else
            {
                oss << ch;
            }
        }
        oss << "\"";

        // Execution count.
        oss << ",\"execution_count\":" << cell.metadata.execution_count;

        // Tags.
        if (!cell.metadata.tags.empty())
        {
            oss << ",\"tags\":[";
            for (size_t t = 0; t < cell.metadata.tags.size(); ++t)
            {
                if (t > 0)
                {
                    oss << ",";
                }
                oss << "\"" << cell.metadata.tags[t] << "\"";
            }
            oss << "]";
        }

        oss << "}";
    }

    oss << "]}";
    return oss.str();
}

auto NotebookCellManager::deserialize_from_json(const std::string& json) -> void
{
    cells_.clear();
    execution_counter_ = 0;
    execution_order_.clear();

    // Simple parser: find each cell object.
    size_t pos = 0;
    while (true)
    {
        const size_t cell_start = json.find("{\"cell_id\"", pos);
        if (cell_start == std::string::npos)
        {
            break;
        }

        // Find the end of this cell object.
        int brace_depth = 0;
        size_t cell_end = cell_start;
        for (size_t idx = cell_start; idx < json.size(); ++idx)
        {
            if (json[idx] == '{')
            {
                ++brace_depth;
            }
            else if (json[idx] == '}')
            {
                --brace_depth;
                if (brace_depth == 0)
                {
                    cell_end = idx;
                    break;
                }
            }
        }

        const std::string cell_json = json.substr(cell_start, cell_end - cell_start + 1);

        // Extract cell_id.
        auto extract = [&cell_json](const std::string& key) -> std::string
        {
            const std::string search = "\"" + key + "\":\"";
            const size_t key_pos = cell_json.find(search);
            if (key_pos == std::string::npos)
            {
                return "";
            }
            const size_t start = key_pos + search.size();
            const size_t end = cell_json.find('"', start);
            if (end == std::string::npos)
            {
                return "";
            }
            return cell_json.substr(start, end - start);
        };

        NotebookCell cell;
        cell.metadata.cell_id = extract("cell_id");

        const auto type_str = extract("cell_type");
        if (type_str == "code")
        {
            cell.metadata.type = CellType::kCode;
        }
        else if (type_str == "markdown")
        {
            cell.metadata.type = CellType::kMarkdown;
        }
        else if (type_str == "raw")
        {
            cell.metadata.type = CellType::kRaw;
        }

        // Extract source (handle escaped characters).
        const auto raw_source = extract("source");
        std::string source;
        for (size_t idx = 0; idx < raw_source.size(); ++idx)
        {
            if (raw_source[idx] == '\\' && idx + 1 < raw_source.size())
            {
                if (raw_source[idx + 1] == 'n')
                {
                    source += '\n';
                    ++idx;
                }
                else if (raw_source[idx + 1] == '"')
                {
                    source += '"';
                    ++idx;
                }
                else if (raw_source[idx + 1] == '\\')
                {
                    source += '\\';
                    ++idx;
                }
                else
                {
                    source += raw_source[idx];
                }
            }
            else
            {
                source += raw_source[idx];
            }
        }
        cell.source = source;

        // Extract execution_count.
        const std::string ec_search = "\"execution_count\":";
        const size_t ec_pos = cell_json.find(ec_search);
        if (ec_pos != std::string::npos)
        {
            const size_t num_start = ec_pos + ec_search.size();
            std::string num_str;
            size_t idx = num_start;
            while (
                idx < cell_json.size() &&
                (std::isdigit(static_cast<unsigned char>(cell_json[idx])) || cell_json[idx] == '-'))
            {
                num_str += cell_json[idx];
                ++idx;
            }
            if (!num_str.empty())
            {
                cell.metadata.execution_count = std::stoi(num_str);
                if (cell.metadata.execution_count > execution_counter_)
                {
                    execution_counter_ = cell.metadata.execution_count;
                }
            }
        }

        // Update next_cell_id_ to avoid collisions.
        if (cell.metadata.cell_id.starts_with("cell-"))
        {
            const auto id_str = cell.metadata.cell_id.substr(5);
            if (!id_str.empty())
            {
                const int id_num = std::stoi(id_str);
                if (id_num >= next_cell_id_)
                {
                    next_cell_id_ = id_num + 1;
                }
            }
        }

        cells_.push_back(std::move(cell));
        pos = cell_end + 1;
    }
}

// ============================================================================
// VariableInspector
// ============================================================================

VariableInspector::VariableInspector(EventBus& event_bus, KernelManager& kernel_manager)
    : event_bus_(event_bus)
    , kernel_manager_(kernel_manager)
{
}

auto VariableInspector::refresh(const std::string& kernel_id)
    -> std::expected<std::vector<VariableInfo>, std::string>
{
    // Check that the kernel exists.
    auto info = kernel_manager_.get_kernel_info(kernel_id);
    if (!info.has_value())
    {
        return std::unexpected("Kernel not found: " + kernel_id);
    }

    // In a real implementation, this would send introspection code to the kernel.
    // For the testable layer, return the current variables.

    events::VariablesRefreshedEvent event;
    event.variable_count = static_cast<int>(variables_.size());
    event_bus_.publish(event);

    return variables_;
}

auto VariableInspector::variables() const -> const std::vector<VariableInfo>&
{
    return variables_;
}

auto VariableInspector::inspect_variable(const std::string& kernel_id,
                                         const std::string& var_name) const
    -> std::expected<MimeBundle, std::string>
{
    auto info = kernel_manager_.get_kernel_info(kernel_id);
    if (!info.has_value())
    {
        return std::unexpected("Kernel not found: " + kernel_id);
    }

    // Find the variable.
    for (const auto& var : variables_)
    {
        if (var.name == var_name)
        {
            MimeBundle bundle;
            bundle.data["text/plain"] = var.value_repr;
            return bundle;
        }
    }

    return std::unexpected("Variable not found: " + var_name);
}

auto VariableInspector::delete_variable(const std::string& kernel_id, const std::string& var_name)
    -> std::expected<void, std::string>
{
    auto info = kernel_manager_.get_kernel_info(kernel_id);
    if (!info.has_value())
    {
        return std::unexpected("Kernel not found: " + kernel_id);
    }

    auto iter = std::find_if(variables_.begin(),
                             variables_.end(),
                             [&var_name](const VariableInfo& v) { return v.name == var_name; });
    if (iter == variables_.end())
    {
        return std::unexpected("Variable not found: " + var_name);
    }

    variables_.erase(iter);
    return {};
}

auto VariableInspector::add_variable(const VariableInfo& info) -> void
{
    variables_.push_back(info);
}

auto VariableInspector::clear() -> void
{
    variables_.clear();
}

} // namespace markamp::core
