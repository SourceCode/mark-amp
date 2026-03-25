/// @file NavigationService.h
/// @brief V4 Phase 20 – Contextual Pane Navigation (logic only).

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class EventBus;
class VaultService;
struct SurfaceLink;
struct LinkResolveResult;
} // namespace markamp::core

namespace markamp::ui
{

struct PaneId;

/// A single navigation entry in the history.
struct NavigationEntry
{
    std::string document_id;
    int scroll_position{0};
    int cursor_line{0};

    // V8 Phase 11 (Phase 29): cross-surface fields
    std::string from_surface; ///< Source surface kind (serialized)
    std::string to_surface;   ///< Target surface kind (serialized)
    std::string entity_id;    ///< Target entity unique ID
};

/// Per-pane back/forward navigation history.
class NavigationHistory
{
public:
    /// Push a new entry (truncates forward history).
    auto push(const NavigationEntry& entry) -> void;

    /// Go back. Returns the entry navigated to, or nullopt.
    [[nodiscard]] auto go_back() -> const NavigationEntry*;

    /// Go forward. Returns the entry navigated to, or nullopt.
    [[nodiscard]] auto go_forward() -> const NavigationEntry*;

    /// Can we go back?
    [[nodiscard]] auto can_go_back() const -> bool;

    /// Can we go forward?
    [[nodiscard]] auto can_go_forward() const -> bool;

    /// Current position in history.
    [[nodiscard]] auto current() const -> const NavigationEntry*;

    /// Number of entries in history.
    [[nodiscard]] auto size() const -> int;

    /// All entries (for breadcrumb display).
    [[nodiscard]] auto entries() const -> const std::vector<NavigationEntry>&;

private:
    std::vector<NavigationEntry> entries_;
    int current_index_{-1};
};

/// How a link should be opened.
enum class LinkOpenBehavior : uint8_t
{
    kSamePane,
    kAdjacentPane,
    kNewPane,
    kPopup
};

/// Service managing per-pane navigation.
class NavigationService
{
public:
    NavigationService(core::EventBus& event_bus, core::VaultService& vault_service);

    /// Navigate to a document in a specific pane, recording history.
    auto navigate_to(int pane_id,
                     const std::string& document_id,
                     LinkOpenBehavior behavior = LinkOpenBehavior::kSamePane) -> void;

    /// Follow a wikilink from the current document.
    [[nodiscard]] auto follow_wikilink(int pane_id, const std::string& link_target) -> bool;

    /// Go back in pane's history.
    [[nodiscard]] auto go_back(int pane_id) -> const NavigationEntry*;

    /// Go forward in pane's history.
    [[nodiscard]] auto go_forward(int pane_id) -> const NavigationEntry*;

    /// Get the history for a specific pane.
    [[nodiscard]] auto history_for(int pane_id) -> NavigationHistory&;

    /// Get breadcrumb trail for a pane.
    [[nodiscard]] auto breadcrumb_trail(int pane_id) const -> std::vector<std::string>;

    /// Check if a link target can be resolved.
    [[nodiscard]] auto can_resolve(const std::string& link_target) const -> bool;

    // V8 Phase 12 (Phase 35): Surface link routing

    /// Navigate via a surface link, resolving and recording cross-surface history.
    auto navigate_via_link(int pane_id, const core::SurfaceLink& link) -> core::LinkResolveResult;

    /// Resolve a surface link without navigating.
    [[nodiscard]] auto resolve_link(const core::SurfaceLink& link) -> core::LinkResolveResult;

    /// Global navigation timeline (cross-surface entries).
    [[nodiscard]] auto global_timeline() const -> const std::vector<NavigationEntry>&;

private:
    core::EventBus& event_bus_;
    core::VaultService& vault_service_;
    std::unordered_map<int, NavigationHistory> histories_;
    std::vector<NavigationEntry> global_timeline_; ///< Cross-surface traversal log
};

} // namespace markamp::ui
