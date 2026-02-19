#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Selection mode modifier.
enum class SelectionModifier : uint8_t
{
    kNone,   ///< Replace selection
    kToggle, ///< Ctrl/Cmd click
    kExtend, ///< Shift click (range)
};

/// A selectable item.
struct SelectableItem
{
    std::string item_id;
    std::string label;
    bool is_selected{false};
};

/// Testable model for Multi-Select & Bulk Actions (Phase 33).
///
/// Encapsulates:
/// - Single/toggle/extend selection modes
/// - Select all / deselect all
/// - Contextual bulk action bar with selection count
/// - Safe destructive flow with confirmation state
class MultiSelectModel
{
public:
    void set_items(std::vector<SelectableItem> items);
    [[nodiscard]] auto items() const -> const std::vector<SelectableItem>&;

    // ── Selection ───────────────────────────────────────────────────

    void select(const std::string& item_id, SelectionModifier modifier = SelectionModifier::kNone);
    void select_all();
    void deselect_all();
    [[nodiscard]] auto selected_count() const -> int;
    [[nodiscard]] auto selected_ids() const -> std::vector<std::string>;

    // ── Bulk actions ────────────────────────────────────────────────

    /// Available actions based on current selection.
    [[nodiscard]] auto bulk_actions() const -> std::vector<std::string>;

    /// Status text (e.g. "3 items selected").
    [[nodiscard]] auto status_text() const -> std::string;

    // ── Safe destructive ────────────────────────────────────────────

    void request_destructive(const std::string& action);
    void confirm_destructive();
    void cancel_destructive();
    [[nodiscard]] auto pending_destructive() const -> const std::string&;
    [[nodiscard]] auto has_pending_destructive() const -> bool;

private:
    std::vector<SelectableItem> items_;
    int last_selected_index_{-1};
    std::string pending_destructive_;
};

} // namespace markamp::ui
