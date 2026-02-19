#pragma once

// V11 Phase 49: Release Hardening Migration And Backward Compatibility
// Migration engine: handles graph versioning and migration transformations.

#include "NodeEditorTypes.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Strong-typed migration step identifier.
struct MigrationStepId
{
    uint64_t value{0};

    constexpr MigrationStepId() = default;
    constexpr explicit MigrationStepId(uint64_t v_arg)
        : value(v_arg)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }
    constexpr auto operator==(const MigrationStepId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const MigrationStepId&) const noexcept = default;
};

/// Type of migration action.
enum class MigrationAction : uint8_t
{
    kRenameNode,
    kRemoveNode,
    kAddSocket,
    kRemoveSocket,
    kChangeType,
    kCustom
};

/// A single migration step transforming from one version to another.
struct MigrationStep
{
    MigrationStepId step_id;
    uint32_t from_version{0};
    uint32_t to_version{0};
    std::string description;
    MigrationAction action{MigrationAction::kCustom};
    std::string target_type;
    std::string new_value;
};

/// Result of attempting a migration.
struct MigrationResult
{
    bool success{false};
    std::string migrated_data;
    std::size_t steps_applied{0};
    std::vector<std::string> warnings;
};

/// Report on graph compatibility with the current runtime.
struct CompatibilityReport
{
    bool compatible{true};
    uint32_t current_version{0};
    uint32_t graph_version{0};
    std::size_t required_steps{0};
    std::vector<std::string> step_descriptions;
};

/// Deprecation entry: marks a type as deprecated.
struct DeprecationEntry
{
    std::string type_name;
    std::string replacement;
    uint32_t removal_version{0};
};

/// Migration engine managing graph version upgrades.
class NodeMigrationEngine
{
public:
    NodeMigrationEngine() = default;

    // --- Step registration ---
    void register_step(MigrationStep step);
    [[nodiscard]] auto step_count() const -> std::size_t;

    // --- Migration ---
    [[nodiscard]] auto migrate(const std::string& graph_data,
                               uint32_t from_version,
                               uint32_t to_version) const -> MigrationResult;
    [[nodiscard]] auto can_migrate(uint32_t from_version, uint32_t to_version) const -> bool;
    [[nodiscard]] auto migration_path(uint32_t from_version, uint32_t to_version) const
        -> std::vector<std::string>;

    // --- Version ---
    [[nodiscard]] auto current_version() const -> uint32_t;
    void set_current_version(uint32_t version);

    // --- Compatibility ---
    [[nodiscard]] auto check_compatibility(const std::string& graph_data,
                                           uint32_t graph_version) const -> CompatibilityReport;

    // --- Deprecation ---
    void register_deprecation(const std::string& type_name,
                              const std::string& replacement,
                              uint32_t removal_version);
    [[nodiscard]] auto deprecated_types() const -> std::vector<DeprecationEntry>;
    [[nodiscard]] auto deprecation_count() const -> std::size_t;
    [[nodiscard]] auto is_deprecated(const std::string& type_name) const -> bool;

    // --- Bulk ---
    void clear();

private:
    uint64_t next_step_id_{1};
    uint32_t current_version_{1};
    std::vector<MigrationStep> steps_;
    std::vector<DeprecationEntry> deprecations_;
};

} // namespace markamp::node_editor
