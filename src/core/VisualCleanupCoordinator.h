#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// V22 Phase 20: Visual cleanup coordination service.
///
/// Provides icon migration tracking, hardcoded literal detection,
/// duplicate primitive consolidation, placeholder removal tracking,
/// and UI quality scoreboard metrics.
class VisualCleanupCoordinator
{
public:
    /// Cleanup category for tracking.
    enum class CleanupCategory : uint8_t
    {
        kIconMigration,
        kHardcodedLiterals,
        kDuplicatePrimitives,
        kPlaceholderVisuals,
        kObsoleteVariants,
    };

    /// Icon migration status.
    struct IconMigrationStatus
    {
        int total_icons{0};
        int migrated_icons{0};
        int legacy_icons_remaining{0};
        float migration_percent{0.0F};
    };

    /// Hardcoded literal audit result.
    struct LiteralAuditResult
    {
        int hardcoded_colors{0};
        int hardcoded_sizes{0};
        int hardcoded_fonts{0};
        int total_violations{0};
    };

    /// UI quality scoreboard.
    struct QualityScoreboard
    {
        float overall_score{0.0F};      /// 0–100 quality score
        int total_surfaces{0};
        int surfaces_passing{0};
        int blocking_issues{0};
        int warning_issues{0};
        int info_issues{0};
    };

    /// Exit criteria gate status.
    struct ExitCriteriaGate
    {
        std::string gate_name;
        bool passes{false};
        std::string reason;
    };

    VisualCleanupCoordinator() = default;

    /// Get icon migration status.
    [[nodiscard]] auto icon_migration_status() const -> IconMigrationStatus;

    /// Audit for hardcoded literals.
    [[nodiscard]] auto audit_hardcoded_literals() const -> LiteralAuditResult;

    /// Get overall quality scoreboard.
    [[nodiscard]] auto quality_scoreboard() const -> QualityScoreboard;

    /// Check all exit criteria gates.
    [[nodiscard]] auto check_exit_criteria() const -> std::vector<ExitCriteriaGate>;

private:
};

} // namespace markamp::core
