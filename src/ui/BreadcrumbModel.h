#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Visual state of a breadcrumb segment (Phase 14 Task 3).
enum class BreadcrumbSegmentState : uint8_t
{
    kNormal,
    kHovered,
    kActive,   ///< Currently navigated-to segment
    kMenuOpen, ///< Segment with open sibling menu
};

/// A single breadcrumb segment.
struct BreadcrumbSegment
{
    std::string segment_id; ///< Unique ID (file path segment, symbol ID, etc.)
    std::string label;      ///< Display text
    BreadcrumbSegmentState state{BreadcrumbSegmentState::kNormal};
    std::vector<std::string> siblings; ///< Sibling names for jump menu
};

/// Testable model for Breadcrumb Navigation (Phase 14).
///
/// Encapsulates:
/// - Segment building from a file path
/// - Overflow/truncation with ellipsis
/// - Segment menu sibling listing
/// - Navigation history integration
class BreadcrumbModel
{
public:
    /// Set the segments from a full path.
    void set_path(std::vector<BreadcrumbSegment> segments);

    /// Get all segments.
    [[nodiscard]] auto segments() const -> const std::vector<BreadcrumbSegment>&;

    /// Segment count.
    [[nodiscard]] auto segment_count() const -> int;

    // ── Segment state ───────────────────────────────────────────────

    /// Set which segment is active (current location).
    void set_active(int index);

    /// Set hover state.
    void set_hovered(int index);

    /// Clear hover.
    void clear_hover();

    // ── Overflow ────────────────────────────────────────────────────

    /// Given a max visible count, return which segments should be visible.
    /// Uses middle-truncation: first segment + "…" + last N segments.
    [[nodiscard]] auto visible_segments(int max_visible) const -> std::vector<BreadcrumbSegment>;

    /// Check if truncation is needed.
    [[nodiscard]] auto needs_truncation(int max_visible) const -> bool;

    // ── Navigation ──────────────────────────────────────────────────

    /// Record a jump to a segment (adds to history).
    void navigate_to(int segment_index);

    /// Go back in history.
    auto go_back() -> bool;

    /// Go forward in history.
    auto go_forward() -> bool;

    /// Can go back?
    [[nodiscard]] auto can_go_back() const -> bool;

    /// Can go forward?
    [[nodiscard]] auto can_go_forward() const -> bool;

private:
    std::vector<BreadcrumbSegment> segments_;
    std::vector<int> history_; ///< Stack of navigated segment indices
    int history_position_{-1}; ///< Current position in history
};

} // namespace markamp::ui
