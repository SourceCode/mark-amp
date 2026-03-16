#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// A keyboard command binding.
struct KeyBinding
{
    std::string command_id;
    std::string label;
    std::string shortcut; ///< e.g., "Ctrl+D", "V"
    std::string category; ///< e.g., "tool", "edit", "navigate", "arrange"
    bool active{true};

    // ── Round 5 Batch 7 (#66-69) ────────────────────────────────

    /// (#66) Whether a shortcut is bound.
    [[nodiscard]] auto has_shortcut() const noexcept -> bool
    {
        return !shortcut.empty();
    }

    /// (#67) Whether a label is set.
    [[nodiscard]] auto has_label() const noexcept -> bool
    {
        return !label.empty();
    }

    /// (#68) Whether this binding is active.
    [[nodiscard]] auto is_active() const noexcept -> bool
    {
        return active;
    }

    /// (#69) Whether a category is set.
    [[nodiscard]] auto has_category() const noexcept -> bool
    {
        return !category.empty();
    }
};

/// Nudge direction.
enum class NudgeDirection : uint8_t
{
    kUp,
    kDown,
    kLeft,
    kRight,
};

/// Testable model for Keyboard-First Canvas Commands (Phase 72).
///
/// Encapsulates:
/// - Command-to-shortcut binding registry
/// - Tool switching shortcut map
/// - Nudge with pixel/grid step
/// - Shortcut discoverability (tooltip hints)
/// - Conflict detection
class KeyboardCommandModel
{
public:
    // ── Bindings ────────────────────────────────────────────────────

    void set_bindings(std::vector<KeyBinding> bindings);
    [[nodiscard]] auto bindings() const -> const std::vector<KeyBinding>&;
    [[nodiscard]] auto bindings_in_category(const std::string& category) const
        -> std::vector<KeyBinding>;
    [[nodiscard]] auto find_binding(const std::string& command_id) const -> KeyBinding;

    // ── Conflicts ───────────────────────────────────────────────────

    [[nodiscard]] auto has_conflicts() const -> bool;
    [[nodiscard]] auto conflicts() const -> std::vector<std::string>;

    // ── Nudge ───────────────────────────────────────────────────────

    void set_nudge_step(double pixels);
    [[nodiscard]] auto nudge_step() const -> double;
    void set_grid_nudge(bool enabled);
    [[nodiscard]] auto grid_nudge() const -> bool;

private:
    std::vector<KeyBinding> bindings_;
    double nudge_step_{1.0};
    bool grid_nudge_{false};

    // ── Round 5 Batch 7 (#70) ───────────────────────────────────

    /// (#70) Number of registered bindings.
    [[nodiscard]] auto binding_count() const noexcept -> size_t
    {
        return bindings_.size();
    }
};

} // namespace markamp::canvas
