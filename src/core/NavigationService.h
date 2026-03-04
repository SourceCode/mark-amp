/// @file NavigationService.h
/// @brief V13 Phase 32 Task 1 — Centralized navigation history stack.
///
/// Maintains back/forward navigation with duplicate coalescing,
/// max stack depth, and NavigationChangedEvent emission.
#pragma once

#include "core/EventBus.h"

#include <chrono>
#include <string>
#include <vector>

namespace markamp::core
{

/// A single entry in the navigation history stack.
struct NavigationEntry
{
    std::string document_id; ///< File path or document identifier
    int line{0};             ///< 1-based line number
    int column{0};           ///< 0-based column
    std::chrono::steady_clock::time_point timestamp;
};

/// Centralized navigation service with back/forward history.
///
/// Manages a stack of NavigationEntry items. On go_to(), truncates
/// forward history and pushes a new entry. Emits NavigationChangedEvent
/// on every navigation. Stack max size: 100 (drops oldest).
///
/// Usage:
/// ```cpp
/// NavigationService nav(event_bus);
/// nav.go_to("file.md", 10, 0);
/// nav.go_to("other.md", 5, 0);
/// nav.go_back();  // returns to file.md:10
/// ```
class NavigationService
{
public:
    explicit NavigationService(EventBus& event_bus);

    /// Navigate to a specific location. Pushes onto the stack.
    void go_to(const std::string& document_id, int line, int column = 0);

    /// Navigate backward in the history stack.
    /// Returns true if navigation occurred.
    auto go_back() -> bool;

    /// Navigate forward in the history stack.
    /// Returns true if navigation occurred.
    auto go_forward() -> bool;

    /// Check if backward navigation is possible.
    [[nodiscard]] auto can_go_back() const -> bool;

    /// Check if forward navigation is possible.
    [[nodiscard]] auto can_go_forward() const -> bool;

    /// Get the current navigation location (nullptr if stack is empty).
    [[nodiscard]] auto current_location() const -> const NavigationEntry*;

    /// Get the full history stack (for display in NavigationHistoryPanel).
    [[nodiscard]] auto history() const -> const std::vector<NavigationEntry>&;

    /// Get the current stack index.
    [[nodiscard]] auto current_index() const -> int;

    /// Get the stack size.
    [[nodiscard]] auto stack_size() const -> size_t;

    /// Clear all history.
    void clear();

    /// Maximum stack depth.
    static constexpr int kMaxStackSize = 100;

private:
    EventBus& event_bus_;
    std::vector<NavigationEntry> stack_;
    int current_index_{-1};

    /// Emit NavigationChangedEvent for the entry at current_index_.
    void emit_navigation_event();

    /// Check if a new entry is a duplicate of the current entry.
    [[nodiscard]] auto is_duplicate(const std::string& document_id, int line, int column) const
        -> bool;
};

} // namespace markamp::core
