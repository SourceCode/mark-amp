#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Extension lifecycle state.
enum class ExtensionState : uint8_t
{
    kNotInstalled,
    kInstalling,
    kInstalled,
    kEnabled,
    kDisabled,
    kUpdateAvailable,
    kUpdating,
    kUninstalling,
    kError,
};

/// Extension filter scope.
enum class ExtensionFilter : uint8_t
{
    kAll,
    kInstalled,
    kEnabled,
    kDisabled,
    kUpdatesAvailable,
};

/// Extension card data.
struct ExtensionInfo
{
    std::string extension_id;
    std::string name;
    std::string author;
    std::string version;
    ExtensionState state{ExtensionState::kNotInstalled};
    bool is_selected{false}; ///< For bulk operations
};

/// Testable model for Extensions Browser (Phase 27).
///
/// Encapsulates:
/// - Extension list with state-based filtering
/// - Lifecycle action availability per state
/// - Bulk selection and operations
/// - Extension count by state
class ExtensionBrowserModel
{
public:
    void set_extensions(std::vector<ExtensionInfo> extensions);
    [[nodiscard]] auto all() const -> const std::vector<ExtensionInfo>&;

    // ── Filtering ───────────────────────────────────────────────────

    void set_filter(ExtensionFilter filter);
    [[nodiscard]] auto filter() const -> ExtensionFilter;
    [[nodiscard]] auto filtered() const -> std::vector<ExtensionInfo>;

    // ── Actions ─────────────────────────────────────────────────────

    /// Available actions for a given state (e.g., "Install", "Enable", "Update").
    [[nodiscard]] static auto actions_for_state(ExtensionState state) -> std::vector<std::string>;

    // ── Bulk operations ─────────────────────────────────────────────

    void toggle_selection(const std::string& extension_id);
    void select_all_visible();
    void clear_selection();
    [[nodiscard]] auto selected_count() const -> int;
    [[nodiscard]] auto selected_ids() const -> std::vector<std::string>;

    // ── Counts ──────────────────────────────────────────────────────

    [[nodiscard]] auto count_by_state(ExtensionState state) const -> int;

private:
    std::vector<ExtensionInfo> extensions_;
    ExtensionFilter filter_{ExtensionFilter::kAll};
};

} // namespace markamp::ui
