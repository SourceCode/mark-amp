#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Context menu scope (what was right-clicked).
enum class ContextScope : uint8_t
{
    kEmptySpace,
    kSingleObject,
    kMultiSelect,
};

/// A context action.
struct ContextAction
{
    std::string action_id;
    std::string label;
    std::string group; ///< e.g., "edit", "style", "arrange", "create"
    bool applicable{true};

    // ── Round 5 Batch 1 (#1-3) ──────────────────────────────────

    /// (#1) Whether a label is set.
    [[nodiscard]] auto has_label() const noexcept -> bool
    {
        return !label.empty();
    }

    /// (#2) Whether a group is set.
    [[nodiscard]] auto has_group() const noexcept -> bool
    {
        return !group.empty();
    }

    /// (#3) Whether this action is applicable.
    [[nodiscard]] auto is_applicable() const noexcept -> bool
    {
        return applicable;
    }
};

/// Testable model for Canvas Context Menus (Phase 62).
///
/// Encapsulates:
/// - Scope-based action taxonomy (empty/single/multi)
/// - Object-type specific quick actions
/// - Multi-select bulk actions
/// - Empty-space create shortcuts
class ContextMenuModel
{
public:
    // ── Scope ───────────────────────────────────────────────────────

    void set_scope(ContextScope scope);
    [[nodiscard]] auto scope() const -> ContextScope;

    // ── Actions ─────────────────────────────────────────────────────

    void set_actions(std::vector<ContextAction> actions);
    [[nodiscard]] auto actions() const -> const std::vector<ContextAction>&;
    [[nodiscard]] auto applicable_actions() const -> std::vector<ContextAction>;
    [[nodiscard]] auto actions_in_group(const std::string& group) const
        -> std::vector<ContextAction>;

    // ── Object type ─────────────────────────────────────────────────

    void set_object_type(const std::string& object_type);
    [[nodiscard]] auto object_type() const -> const std::string&;

private:
    ContextScope scope_{ContextScope::kEmptySpace};
    std::vector<ContextAction> actions_;
    std::string object_type_;

    // ── Round 5 Batch 1 (#4-7) ──────────────────────────────────

    /// (#4) Whether scope is empty space.
    [[nodiscard]] auto is_empty_space() const noexcept -> bool
    {
        return scope_ == ContextScope::kEmptySpace;
    }

    /// (#5) Whether scope is single object.
    [[nodiscard]] auto is_single_object() const noexcept -> bool
    {
        return scope_ == ContextScope::kSingleObject;
    }

    /// (#6) Whether scope is multi-select.
    [[nodiscard]] auto is_multi_select() const noexcept -> bool
    {
        return scope_ == ContextScope::kMultiSelect;
    }

    /// (#7) Number of registered actions.
    [[nodiscard]] auto action_count() const noexcept -> size_t
    {
        return actions_.size();
    }
};

} // namespace markamp::canvas
