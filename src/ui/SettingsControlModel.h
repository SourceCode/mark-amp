#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace markamp::ui
{

/// Supported setting value types (Phase 12 Task 1).
enum class SettingValueType : uint8_t
{
    kBoolean,
    kInteger,
    kDouble,
    kString,
    kChoice,
    kColor,
    kKeybinding,
};

/// Validation result for a setting value (Phase 12 Task 2).
struct SettingValidation
{
    bool is_valid{true};
    std::string error_message;   ///< Empty when valid
    std::string corrective_hint; ///< Suggested fix (e.g., "Must be between 8 and 72")
};

/// Represents a single staged change (Phase 12 Task 3).
struct StagedChange
{
    std::string setting_id;
    std::string old_value;
    std::string new_value;
};

/// Testable model for Settings Control Primitives (Phase 12).
///
/// Encapsulates:
/// - Per-type value validation and normalization
/// - Inline validation with error messages and hints
/// - Staged change tracking with summary
/// - Per-setting revert (local undo)
class SettingsControlModel
{
public:
    /// Constraint definition for a setting.
    struct SettingConstraint
    {
        std::string setting_id;
        SettingValueType value_type{SettingValueType::kString};
        std::string default_value;
        int int_min{0};
        int int_max{0};
        double double_min{0.0};
        double double_max{0.0};
        std::vector<std::string> choices; ///< Valid choices for kChoice type
    };

    /// Register a setting constraint.
    void add_constraint(SettingConstraint constraint);

    // ── Validation ──────────────────────────────────────────────────

    /// Validate a proposed value against the setting's constraints.
    [[nodiscard]] auto validate(const std::string& setting_id,
                                const std::string& proposed_value) const -> SettingValidation;

    /// Clamp an integer value to the setting's range.
    [[nodiscard]] auto clamp_int(const std::string& setting_id, int value) const -> int;

    /// Clamp a double value to the setting's range.
    [[nodiscard]] auto clamp_double(const std::string& setting_id, double value) const -> double;

    // ── Staged changes ──────────────────────────────────────────────

    /// Stage a change (records old → new).
    void stage_change(const std::string& setting_id,
                      const std::string& old_value,
                      const std::string& new_value);

    /// Get all staged changes.
    [[nodiscard]] auto staged_changes() const -> const std::vector<StagedChange>&;

    /// Number of pending changes.
    [[nodiscard]] auto pending_count() const -> int;

    /// Discard all staged changes.
    void discard_all();

    // ── Per-setting revert ──────────────────────────────────────────

    /// Revert a single staged change (removes from pending).
    void revert(const std::string& setting_id);

    /// Check if a specific setting has a pending change.
    [[nodiscard]] auto has_pending(const std::string& setting_id) const -> bool;

    /// Get the default value for a setting.
    [[nodiscard]] auto default_value(const std::string& setting_id) const -> std::string;

private:
    std::vector<SettingConstraint> constraints_;
    std::vector<StagedChange> staged_;

    [[nodiscard]] auto find_constraint(const std::string& setting_id) const
        -> const SettingConstraint*;
};

} // namespace markamp::ui
