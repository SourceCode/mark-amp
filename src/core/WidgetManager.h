/// @file WidgetManager.h
/// @brief V4 Phase 33 – Interactive Widgets and Plot Backends (pure-logic engine).
/// ipywidgets-compatible comm channel for bidirectional state sync.

#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace markamp::core
{

class EventBus;
class KernelManager;

// ============================================================================
// Widget value type (variant of supported types)
// ============================================================================

using WidgetValue = std::variant<int, double, std::string, bool, std::vector<std::string>>;

// ============================================================================
// WidgetState – current state of a single widget
// ============================================================================

struct WidgetState
{
    std::string widget_id;
    std::string widget_type; // "IntSlider", "Dropdown", "Text", "Button", "Output"
    std::unordered_map<std::string, WidgetValue> properties;
    std::string comm_id; // ZeroMQ comm channel ID

    [[nodiscard]] auto get_string(const std::string& key) const -> std::string;
    [[nodiscard]] auto get_int(const std::string& key) const -> int;
    [[nodiscard]] auto get_double(const std::string& key) const -> double;
    [[nodiscard]] auto get_bool(const std::string& key) const -> bool;
};

// ============================================================================
// CommMessage – represents a comm-channel message
// ============================================================================

struct CommMessage
{
    std::string comm_id;
    std::string target_name; // "jupyter.widget"
    std::string msg_type;    // "comm_open", "comm_msg", "comm_close"
    std::string data_json;
};

// ============================================================================
// WidgetManager – pure-logic widget lifecycle manager
// ============================================================================

class WidgetManager
{
public:
    WidgetManager(EventBus& event_bus, KernelManager& kernel_manager);

    /// Handle a comm_open message (kernel creates a widget).
    auto handle_comm_open(const CommMessage& msg) -> void;

    /// Handle a comm_msg (kernel updates widget state).
    auto handle_comm_msg(const CommMessage& msg) -> void;

    /// Handle a comm_close (kernel destroys widget).
    auto handle_comm_close(const CommMessage& msg) -> void;

    /// Send a state update from frontend to kernel.
    auto send_state_update(const std::string& widget_id,
                           const std::string& property,
                           const WidgetValue& value) -> void;

    /// Get the current state of a widget.
    [[nodiscard]] auto get_widget(const std::string& widget_id) const -> std::optional<WidgetState>;

    /// Get all active widgets.
    [[nodiscard]] auto active_widgets() const -> std::vector<WidgetState>;

    /// Register a callback for widget state changes.
    auto on_state_change(const std::string& widget_id,
                         std::function<void(const WidgetState&)> callback) -> void;

    /// Clear all widgets (on kernel restart).
    auto clear_all() -> void;

    /// Get widget count.
    [[nodiscard]] auto widget_count() const -> int;

    /// Get the last outbound message (for testing).
    [[nodiscard]] auto last_outbound_message() const -> const std::string&;

private:
    EventBus& event_bus_;
    [[maybe_unused]] KernelManager& kernel_manager_;

    std::unordered_map<std::string, WidgetState> widgets_;
    std::unordered_map<std::string, std::string> comm_to_widget_; // comm_id -> widget_id
    std::unordered_map<std::string, std::vector<std::function<void(const WidgetState&)>>>
        callbacks_;
    std::string last_outbound_;

    /// Parse widget state from a simple JSON-like format.
    [[nodiscard]] static auto parse_widget_state(const std::string& json) -> WidgetState;

    /// Serialize a property update to JSON.
    [[nodiscard]] static auto serialize_state_update(const std::string& property,
                                                     const WidgetValue& value) -> std::string;
};

} // namespace markamp::core
