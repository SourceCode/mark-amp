#pragma once

#include "canvas/CanvasTypes.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::canvas
{

class Board;

/// A single entry in the hierarchical outline tree.
struct OutlineEntry
{
    ObjectId id{kInvalidObjectId};
    std::string label;
    CanvasObjectType type{CanvasObjectType::Shape};
    int indent_level{0};
    bool is_expanded{true};
    bool is_container{false}; ///< True for Frame or Section.
    int child_count{0};
};

/// Static utility that builds OutlineEntry lists from a Board.
class OutlineBuilder
{
public:
    /// Build an ordered list of outline entries from the board.
    /// Order: frames (z-sorted) → contained objects (indented) → sections → top-level objects.
    [[nodiscard]] static auto build_entries(const Board& board) -> std::vector<OutlineEntry>;
};

/// Panel displaying the hierarchical board structure as a navigable tree.
class OutlinePanel
{
public:
    using OnNavigateCallback = std::function<void(ObjectId)>;
    using OnReorderCallback = std::function<void(ObjectId, int new_index)>;

    // ── Data ───────────────────────────────────────────────────────
    auto set_entries(const std::vector<OutlineEntry>& entries) -> void;
    [[nodiscard]] auto entry_count() const -> size_t;
    [[nodiscard]] auto entry_at(size_t index) const -> const OutlineEntry&;

    /// Expand or collapse a container entry.
    auto toggle_expand(ObjectId container_id) -> void;

    /// Get only the entries that are currently visible (respecting collapsed parents).
    [[nodiscard]] auto visible_entries() const -> std::vector<const OutlineEntry*>;

    // ── Callbacks ──────────────────────────────────────────────────
    auto set_on_navigate(OnNavigateCallback callback) -> void;
    auto set_on_reorder(OnReorderCallback callback) -> void;

    /// Fire navigation callback for the given entry.
    auto navigate_to(ObjectId target_id) -> void;

    // ── Visibility ─────────────────────────────────────────────────
    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

private:
    std::vector<OutlineEntry> entries_;
    bool visible_{false};
    [[maybe_unused]] double scroll_offset_{0.0};
    OnNavigateCallback on_navigate_;
    OnReorderCallback on_reorder_;
};

} // namespace markamp::canvas
