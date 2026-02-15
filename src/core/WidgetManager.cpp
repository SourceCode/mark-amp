/// @file WidgetManager.cpp
/// @brief V4 Phase 33 – Interactive Widgets implementation.

#include "core/WidgetManager.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/KernelManager.h"

#include <sstream>

namespace markamp::core
{

// ============================================================================
// WidgetState accessors
// ============================================================================

auto WidgetState::get_string(const std::string& key) const -> std::string
{
    auto iter = properties.find(key);
    if (iter == properties.end())
    {
        return "";
    }
    if (const auto* str = std::get_if<std::string>(&iter->second))
    {
        return *str;
    }
    return "";
}

auto WidgetState::get_int(const std::string& key) const -> int
{
    auto iter = properties.find(key);
    if (iter == properties.end())
    {
        return 0;
    }
    if (const auto* val = std::get_if<int>(&iter->second))
    {
        return *val;
    }
    return 0;
}

auto WidgetState::get_double(const std::string& key) const -> double
{
    auto iter = properties.find(key);
    if (iter == properties.end())
    {
        return 0.0;
    }
    if (const auto* val = std::get_if<double>(&iter->second))
    {
        return *val;
    }
    return 0.0;
}

auto WidgetState::get_bool(const std::string& key) const -> bool
{
    auto iter = properties.find(key);
    if (iter == properties.end())
    {
        return false;
    }
    if (const auto* val = std::get_if<bool>(&iter->second))
    {
        return *val;
    }
    return false;
}

// ============================================================================
// Constructor
// ============================================================================

WidgetManager::WidgetManager(EventBus& event_bus, KernelManager& kernel_manager)
    : event_bus_(event_bus)
    , kernel_manager_(kernel_manager)
{
}

// ============================================================================
// JSON parsing helpers
// ============================================================================

auto WidgetManager::parse_widget_state(const std::string& json) -> WidgetState
{
    WidgetState state;

    // Simple key-value parser for testing: {"widget_id":"id","widget_type":"IntSlider",...}
    auto extract_string = [&](const std::string& key) -> std::string
    {
        const std::string search = "\"" + key + "\":\"";
        const size_t pos = json.find(search);
        if (pos == std::string::npos)
        {
            return "";
        }
        const size_t start = pos + search.size();
        const size_t end = json.find('"', start);
        if (end == std::string::npos)
        {
            return "";
        }
        return json.substr(start, end - start);
    };

    auto extract_int = [&](const std::string& key) -> std::optional<int>
    {
        const std::string search = "\"" + key + "\":";
        const size_t pos = json.find(search);
        if (pos == std::string::npos)
        {
            return std::nullopt;
        }
        const size_t start = pos + search.size();
        // Skip whitespace.
        size_t num_start = start;
        while (num_start < json.size() && json[num_start] == ' ')
        {
            ++num_start;
        }
        // Read digits.
        std::string num_str;
        while (
            num_start < json.size() &&
            (std::isdigit(static_cast<unsigned char>(json[num_start])) || json[num_start] == '-'))
        {
            num_str += json[num_start];
            ++num_start;
        }
        if (num_str.empty())
        {
            return std::nullopt;
        }
        return std::stoi(num_str);
    };

    state.widget_id = extract_string("widget_id");
    state.widget_type = extract_string("widget_type");
    state.comm_id = extract_string("comm_id");

    // Extract known numeric properties.
    if (const auto val = extract_int("value"))
    {
        state.properties["value"] = *val;
    }
    if (const auto min_val = extract_int("min"))
    {
        state.properties["min"] = *min_val;
    }
    if (const auto max_val = extract_int("max"))
    {
        state.properties["max"] = *max_val;
    }

    // Extract known string properties.
    const auto desc = extract_string("description");
    if (!desc.empty())
    {
        state.properties["description"] = desc;
    }

    return state;
}

auto WidgetManager::serialize_state_update(const std::string& property, const WidgetValue& value)
    -> std::string
{
    std::ostringstream oss;
    oss << "{\"" << property << "\":";

    std::visit(
        [&oss](const auto& val)
        {
            using ValType = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<ValType, int>)
            {
                oss << val;
            }
            else if constexpr (std::is_same_v<ValType, double>)
            {
                oss << val;
            }
            else if constexpr (std::is_same_v<ValType, std::string>)
            {
                oss << "\"" << val << "\"";
            }
            else if constexpr (std::is_same_v<ValType, bool>)
            {
                oss << (val ? "true" : "false");
            }
            else if constexpr (std::is_same_v<ValType, std::vector<std::string>>)
            {
                oss << "[";
                for (size_t idx = 0; idx < val.size(); ++idx)
                {
                    if (idx > 0)
                    {
                        oss << ",";
                    }
                    oss << "\"" << val[idx] << "\"";
                }
                oss << "]";
            }
        },
        value);

    oss << "}";
    return oss.str();
}

// ============================================================================
// Comm protocol handlers
// ============================================================================

auto WidgetManager::handle_comm_open(const CommMessage& msg) -> void
{
    auto state = parse_widget_state(msg.data_json);
    if (state.comm_id.empty())
    {
        state.comm_id = msg.comm_id;
    }
    if (state.widget_id.empty())
    {
        state.widget_id = msg.comm_id; // Use comm_id as widget_id fallback.
    }

    const auto widget_id = state.widget_id;
    comm_to_widget_[msg.comm_id] = widget_id;
    widgets_[widget_id] = std::move(state);

    events::WidgetCreatedEvent event;
    event.widget_id = widget_id;
    event.widget_type = widgets_[widget_id].widget_type;
    event_bus_.publish(event);
}

auto WidgetManager::handle_comm_msg(const CommMessage& msg) -> void
{
    // Find the widget by comm_id.
    auto comm_iter = comm_to_widget_.find(msg.comm_id);
    if (comm_iter == comm_to_widget_.end())
    {
        return;
    }

    const auto& widget_id = comm_iter->second;
    auto widget_iter = widgets_.find(widget_id);
    if (widget_iter == widgets_.end())
    {
        return;
    }

    // Parse the update and merge properties.
    auto update = parse_widget_state(msg.data_json);
    std::string updated_property;

    for (auto& [key, val] : update.properties)
    {
        widget_iter->second.properties[key] = val;
        updated_property = key;
    }

    // Notify callbacks.
    auto cb_iter = callbacks_.find(widget_id);
    if (cb_iter != callbacks_.end())
    {
        for (const auto& callback : cb_iter->second)
        {
            callback(widget_iter->second);
        }
    }

    events::WidgetUpdatedEvent event;
    event.widget_id = widget_id;
    event.property = updated_property;
    event_bus_.publish(event);
}

auto WidgetManager::handle_comm_close(const CommMessage& msg) -> void
{
    auto comm_iter = comm_to_widget_.find(msg.comm_id);
    if (comm_iter == comm_to_widget_.end())
    {
        return;
    }

    const auto widget_id = comm_iter->second;

    events::WidgetDestroyedEvent event;
    event.widget_id = widget_id;
    event_bus_.publish(event);

    widgets_.erase(widget_id);
    callbacks_.erase(widget_id);
    comm_to_widget_.erase(comm_iter);
}

// ============================================================================
// Frontend -> Kernel
// ============================================================================

auto WidgetManager::send_state_update(const std::string& widget_id,
                                      const std::string& property,
                                      const WidgetValue& value) -> void
{
    auto iter = widgets_.find(widget_id);
    if (iter == widgets_.end())
    {
        return;
    }

    // Update local state.
    iter->second.properties[property] = value;

    // Serialize the update.
    last_outbound_ = serialize_state_update(property, value);

    // Notify callbacks.
    auto cb_iter = callbacks_.find(widget_id);
    if (cb_iter != callbacks_.end())
    {
        for (const auto& callback : cb_iter->second)
        {
            callback(iter->second);
        }
    }

    events::WidgetUpdatedEvent event;
    event.widget_id = widget_id;
    event.property = property;
    event_bus_.publish(event);
}

// ============================================================================
// Queries
// ============================================================================

auto WidgetManager::get_widget(const std::string& widget_id) const -> std::optional<WidgetState>
{
    auto iter = widgets_.find(widget_id);
    if (iter == widgets_.end())
    {
        return std::nullopt;
    }
    return iter->second;
}

auto WidgetManager::active_widgets() const -> std::vector<WidgetState>
{
    std::vector<WidgetState> result;
    result.reserve(widgets_.size());
    for (const auto& [id, state] : widgets_)
    {
        result.push_back(state);
    }
    return result;
}

auto WidgetManager::on_state_change(const std::string& widget_id,
                                    std::function<void(const WidgetState&)> callback) -> void
{
    callbacks_[widget_id].push_back(std::move(callback));
}

auto WidgetManager::clear_all() -> void
{
    widgets_.clear();
    comm_to_widget_.clear();
    callbacks_.clear();
}

auto WidgetManager::widget_count() const -> int
{
    return static_cast<int>(widgets_.size());
}

auto WidgetManager::last_outbound_message() const -> const std::string&
{
    return last_outbound_;
}

} // namespace markamp::core
