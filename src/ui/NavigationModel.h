#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Navigation target type (for "Jump To").
enum class NavTargetType : uint8_t
{
    kFile,
    kHeading,
    kSymbol,
    kGraphNode,
    kPanel,
};

/// Navigation target for unified jump.
struct NavTarget
{
    std::string target_id;
    std::string label;
    NavTargetType type{NavTargetType::kFile};
    std::string surface; ///< "editor", "canvas", "graph"
    int use_count{0};    ///< MRU ranking
};

/// Navigation history entry.
struct NavHistoryEntry
{
    std::string target_id;
    std::string label;
    std::string surface;
};

/// Testable model for Cross-Surface Navigation (Phase 30).
///
/// Encapsulates:
/// - Back/forward navigation history stack
/// - Unified "Jump To" with type filter and MRU ranking
/// - Breadcrumb sync state
/// - Surface transition tracking
class NavigationModel
{
public:
    // ── History stack ────────────────────────────────────────────────

    /// Push a location onto history.
    void push_location(NavHistoryEntry entry);

    /// Navigate back, returns target or nullptr if at start.
    [[nodiscard]] auto go_back() -> const NavHistoryEntry*;

    /// Navigate forward, returns target or nullptr if at end.
    [[nodiscard]] auto go_forward() -> const NavHistoryEntry*;

    [[nodiscard]] auto can_go_back() const -> bool;
    [[nodiscard]] auto can_go_forward() const -> bool;

    /// Current location.
    [[nodiscard]] auto current() const -> const NavHistoryEntry*;

    // ── Jump targets ────────────────────────────────────────────────

    void set_targets(std::vector<NavTarget> targets);

    /// Filter targets by type.
    [[nodiscard]] auto targets_by_type(NavTargetType type) const -> std::vector<NavTarget>;

    /// Get all targets sorted by use_count descending (MRU).
    [[nodiscard]] auto mru_targets() const -> std::vector<NavTarget>;

    // ── Breadcrumb sync ─────────────────────────────────────────────

    void set_breadcrumb(const std::vector<std::string>& segments);
    [[nodiscard]] auto breadcrumb() const -> const std::vector<std::string>&;

    // ── Surface tracking ────────────────────────────────────────────

    void set_active_surface(const std::string& surface);
    [[nodiscard]] auto active_surface() const -> const std::string&;

private:
    std::vector<NavHistoryEntry> history_;
    int history_index_{-1};
    std::vector<NavTarget> targets_;
    std::vector<std::string> breadcrumb_;
    std::string active_surface_;
};

} // namespace markamp::ui
