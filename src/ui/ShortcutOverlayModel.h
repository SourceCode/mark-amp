#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A shortcut entry for the overlay (Phase 17).
struct ShortcutEntry
{
    std::string command_id; ///< Command identifier
    std::string label;      ///< Display label
    std::string shortcut;   ///< Key combination string
    std::string category;   ///< Category grouping (e.g., "Editor", "Navigation", "View")
    std::string context;    ///< Workbench mode this shortcut is relevant to
};

/// Testable model for the Shortcut Overlay (Phase 17).
///
/// Encapsulates:
/// - Search/filter with ranked results
/// - Context-aware subset filtering
/// - Category grouping
/// - Tooltip format standardization
class ShortcutOverlayModel
{
public:
    /// Load all shortcut entries.
    void set_entries(std::vector<ShortcutEntry> entries);

    /// All entries.
    [[nodiscard]] auto entries() const -> const std::vector<ShortcutEntry>&;

    // ── Search ──────────────────────────────────────────────────────

    /// Filter entries by query (matches label, shortcut, category).
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<ShortcutEntry>;

    // ── Context filtering ───────────────────────────────────────────

    /// Get entries relevant to a specific workbench mode.
    [[nodiscard]] auto for_context(const std::string& mode) const -> std::vector<ShortcutEntry>;

    // ── Categories ──────────────────────────────────────────────────

    /// Get unique category names (sorted).
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Get entries by category.
    [[nodiscard]] auto by_category(const std::string& category) const -> std::vector<ShortcutEntry>;

    // ── Tooltip formatting ──────────────────────────────────────────

    /// Format a tooltip: "Label (Shortcut)".
    [[nodiscard]] static auto format_tooltip(const std::string& label, const std::string& shortcut)
        -> std::string;

private:
    std::vector<ShortcutEntry> entries_;
};

} // namespace markamp::ui
