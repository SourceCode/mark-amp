/// @file NavigationHistory.h
/// @brief V24 P10-T04: Back/forward navigation with breadcrumb trail.
///
/// Tracks artifact-level and position-level navigation for implementing
/// browser-style back/forward in the editor.
#pragma once

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// A navigation entry recording a visited location.
struct NavigationEntry
{
    std::string artifact_id;
    std::string display_name;
    std::optional<std::string> file_path;
    int line{0};
    int column{0};
    std::string view_type;   ///< "editor", "notebook", "canvas", "settings"

    [[nodiscard]] auto matches_artifact(const std::string& id) const -> bool
    {
        return artifact_id == id;
    }
};

/// Manages back/forward navigation stack with breadcrumb trail.
class NavigationHistory
{
public:
    NavigationHistory() = default;

    /// Push a new entry (clears any forward history).
    void push(NavigationEntry entry);

    /// Navigate back. Returns the entry navigated to, or nullopt.
    [[nodiscard]] auto go_back() -> std::optional<NavigationEntry>;

    /// Navigate forward. Returns the entry navigated to, or nullopt.
    [[nodiscard]] auto go_forward() -> std::optional<NavigationEntry>;

    /// Can navigate back?
    [[nodiscard]] auto can_go_back() const noexcept -> bool;

    /// Can navigate forward?
    [[nodiscard]] auto can_go_forward() const noexcept -> bool;

    /// Get current entry.
    [[nodiscard]] auto current() const -> const NavigationEntry*;

    /// Get breadcrumb trail (all entries up to current).
    [[nodiscard]] auto breadcrumbs() const -> std::vector<const NavigationEntry*>;

    /// Get full history.
    [[nodiscard]] auto full_history() const -> const std::vector<NavigationEntry>&
    {
        return entries_;
    }

    /// Total entry count.
    [[nodiscard]] auto entry_count() const noexcept -> int
    {
        return static_cast<int>(entries_.size());
    }

    /// Current position in the history stack.
    [[nodiscard]] auto current_index() const noexcept -> int
    {
        return current_index_;
    }

    /// Clear all history.
    void clear();

    /// Set max history size (default 100).
    void set_max_size(int max_size);

private:
    std::vector<NavigationEntry> entries_;
    int current_index_{-1};
    int max_size_{100};
};

} // namespace markamp::core
