#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Visual state of a tree row (Phase 07 Task 1).
enum class TreeRowState : uint8_t
{
    kNormal,
    kHovered,
    kSelected,
    kFocused,
    kActive, ///< Currently opened file
};

/// Validation result for rename/create operations.
enum class RenameValidation : uint8_t
{
    kValid,
    kEmpty,        ///< Name is empty
    kConflict,     ///< Name already exists in parent
    kInvalidChars, ///< Contains illegal characters (/, \, :, *, ?, ", <, >, |)
};

/// A single tree item for testing (decoupled from core::FileNode).
struct TreeItemModel
{
    std::string node_id;
    std::string name;
    bool is_directory{false};
    bool is_expanded{false};
    bool is_selected{false};
    int depth{0};
};

/// Testable model for the File Tree control surface (Phase 07).
///
/// Encapsulates:
/// - Row affordance rules (hover/select/focus/active states)
/// - Inline rename/create validation
/// - Multi-select and bulk operations
/// - Type-ahead filtering
class FileTreeModel
{
public:
    /// Set the flat visible-items list (pre-flattened from a tree).
    void set_items(std::vector<TreeItemModel> items);

    /// Get visible items.
    [[nodiscard]] auto items() const -> const std::vector<TreeItemModel>&;

    /// Current item count.
    [[nodiscard]] auto item_count() const -> int;

    // ── Selection ───────────────────────────────────────────────────

    /// Select a single item (clears multi-select).
    void select(int index);

    /// Toggle selection on an item (Ctrl+Click).
    void toggle_select(int index);

    /// Range-select from anchor to index (Shift+Click).
    void range_select(int index);

    /// Get all selected indices.
    [[nodiscard]] auto selected_indices() const -> std::vector<int>;

    /// Number of selected items.
    [[nodiscard]] auto selection_count() const -> int;

    /// Clear all selection.
    void clear_selection();

    // ── Keyboard navigation ─────────────────────────────────────────

    /// Current keyboard focus index.
    [[nodiscard]] auto focus_index() const -> int;

    /// Move focus up/down.
    void focus_next();
    void focus_previous();

    /// Expand focused directory (Right arrow).
    void expand_focused();

    /// Collapse focused node or move to parent (Left arrow).
    void collapse_focused();

    // ── Inline rename/create ────────────────────────────────────────

    /// Validate a proposed name against sibling names.
    [[nodiscard]] static auto validate_name(const std::string& proposed_name,
                                            const std::vector<std::string>& sibling_names)
        -> RenameValidation;

    // ── Type-ahead filter ───────────────────────────────────────────

    /// Apply a type-ahead filter string. Returns matching indices.
    [[nodiscard]] auto filter(const std::string& query) const -> std::vector<int>;

    // ── Bulk operations ─────────────────────────────────────────────

    /// Get IDs of all selected items (for bulk delete/move).
    [[nodiscard]] auto selected_ids() const -> std::vector<std::string>;

private:
    std::vector<TreeItemModel> items_;
    int focus_index_{-1};
    int anchor_index_{-1}; ///< For range-select
};

} // namespace markamp::ui
