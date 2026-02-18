#pragma once

/// @file CanvasToolHost.h
/// @brief Phase 14: Extension-contributed canvas tool hosting.
///
/// Manages both built-in and extension-contributed tools in a unified
/// registry. Extension tools register via their manifest and are
/// activated/deactivated through the same interface as built-in tools.

#include "canvas/CanvasTool.h"
#include "core/EventBus.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// Callback invoked when an extension tool is activated/deactivated.
using ToolActivationCallback = std::function<void(bool is_active)>;

/// Descriptor for a hosted canvas tool (built-in or extension).
struct HostedTool
{
    std::string tool_id;                  ///< Unique tool identifier
    std::string label;                    ///< Human-readable name
    std::string icon;                     ///< Icon path
    std::string category;                 ///< "draw", "annotate", "navigate", "extension"
    std::string extension_id;             ///< Empty for built-in tools
    int priority{0};                      ///< Ordering within category (lower = first)
    bool is_built_in{false};              ///< True for engine-provided tools
    ToolActivationCallback on_activation; ///< Called on activate/deactivate
};

/// Result of tool activation.
struct ToolActivationResult
{
    bool success{false};
    std::string error_message;
    std::string deactivated_tool_id; ///< Previously active tool (if any)
};

/// Unified host for built-in and extension canvas tools.
///
/// Provides registration, activation lifecycle, category grouping,
/// and priority-sorted tool enumeration for toolbar construction.
class CanvasToolHost
{
public:
    explicit CanvasToolHost(core::EventBus& event_bus);

    // ── Registration ──────────────────────────────────────────────

    /// Register a tool (built-in or extension).
    auto register_tool(const HostedTool& tool) -> bool;

    /// Unregister a tool by ID.
    auto unregister_tool(const std::string& tool_id) -> bool;

    /// Unregister all tools from a specific extension.
    auto unregister_extension_tools(const std::string& extension_id) -> size_t;

    // ── Activation ────────────────────────────────────────────────

    /// Activate a tool by ID. Deactivates the current tool first.
    auto activate_tool(const std::string& tool_id) -> ToolActivationResult;

    /// Deactivate the current tool (if any).
    auto deactivate_current() -> void;

    /// Currently active tool ID (empty if none).
    [[nodiscard]] auto active_tool_id() const -> const std::string&;

    /// Currently active tool descriptor.
    [[nodiscard]] auto active_tool() const -> const HostedTool*;

    // ── Query ─────────────────────────────────────────────────────

    /// Find a tool by ID.
    [[nodiscard]] auto find_tool(const std::string& tool_id) const -> const HostedTool*;

    /// All registered tools (unordered).
    [[nodiscard]] auto registered_tools() const
        -> const std::unordered_map<std::string, HostedTool>&;

    /// Tools in a given category, sorted by priority.
    [[nodiscard]] auto tools_in_category(const std::string& category) const
        -> std::vector<const HostedTool*>;

    /// All unique tool categories.
    [[nodiscard]] auto tool_categories() const -> std::vector<std::string>;

    /// Available tools sorted by category + priority.
    [[nodiscard]] auto available_tools() const -> std::vector<const HostedTool*>;

    /// Number of registered tools.
    [[nodiscard]] auto tool_count() const -> size_t;

    /// Number of extension-contributed tools.
    [[nodiscard]] auto extension_tool_count() const -> size_t;

    // ── Cleanup ───────────────────────────────────────────────────

    auto clear() -> void;

private:
    core::EventBus& event_bus_;
    std::unordered_map<std::string, HostedTool> tools_;
    std::string active_tool_id_;
};

} // namespace markamp::canvas
