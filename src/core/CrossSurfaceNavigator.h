/// @file CrossSurfaceNavigator.h
/// @brief P07-T05: Cross-surface navigation orchestrator.
///
/// Orchestrates navigation among editor and graph surfaces.
/// Resolves target, switches mode, updates history and focus.
#pragma once

#include <string>

namespace markamp::core
{
class EventBus;

/// Target surface for cross-surface navigation.
enum class NavigationSurface
{
    kEditor,
    kGraph,
};

/// A cross-surface navigation target.
struct NavigationTarget
{
    NavigationSurface surface{NavigationSurface::kEditor};
    std::string resource_id; ///< File path, board ID, or graph node ID
    int line{0};             ///< Line number for editor targets
    std::string context;     ///< Optional context (e.g., section heading)
};

/// Orchestrates navigation among editor and graph.
class CrossSurfaceNavigator
{
public:
    explicit CrossSurfaceNavigator(EventBus& bus);

    /// Navigate to a target, switching surface if necessary.
    void navigate_to(const NavigationTarget& target);

    /// Navigate to a file in the editor.
    void navigate_to_file(const std::string& path, int line = 0);

    /// Navigate to a node in the graph.
    void navigate_to_graph_node(const std::string& node_id);

    /// Get the current surface.
    [[nodiscard]] auto current_surface() const -> NavigationSurface { return current_; }

private:
    EventBus& event_bus_;
    NavigationSurface current_{NavigationSurface::kEditor};
};

} // namespace markamp::core
