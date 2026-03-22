/// @file SettingsCompletionAuditor.h
/// @brief V23 Phase 10 — Settings, config, deep link, and runtime application completion auditor.
///
/// Audits that settings ownership, schema, import/export, deep links, workspace
/// scope, and runtime-application behavior are finished and not split between
/// conflicting mutation models.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

enum class SettingsCapabilityArea : uint8_t
{
    kStateOwnership,      ///< Staged apply/cancel/revert authority
    kImportExport,        ///< Settings import/export
    kDeepLinks,           ///< Settings deep-link routing
    kWorkspaceScope,      ///< Workspace vs user scope
    kSchemaValidation,    ///< Settings schema and validation
    kRuntimeApplication,  ///< Live application of changed settings
    kProfileManagement,   ///< Named config profiles
    kMigration,           ///< Config version migration
};

[[nodiscard]] constexpr auto settings_capability_label(SettingsCapabilityArea area) -> const char*
{
    switch (area)
    {
    case SettingsCapabilityArea::kStateOwnership:    return "StateOwnership";
    case SettingsCapabilityArea::kImportExport:      return "ImportExport";
    case SettingsCapabilityArea::kDeepLinks:         return "DeepLinks";
    case SettingsCapabilityArea::kWorkspaceScope:    return "WorkspaceScope";
    case SettingsCapabilityArea::kSchemaValidation:  return "SchemaValidation";
    case SettingsCapabilityArea::kRuntimeApplication: return "RuntimeApplication";
    case SettingsCapabilityArea::kProfileManagement: return "ProfileManagement";
    case SettingsCapabilityArea::kMigration:         return "Migration";
    }
    return "Unknown";
}

struct SettingsCompletionItem
{
    SettingsCapabilityArea area{SettingsCapabilityArea::kStateOwnership};
    std::string feature_name;
    bool is_sole_authority{false};   ///< No conflicting ownership
    bool has_ui_sync{false};         ///< UI reflects changes
    bool has_error_handling{false};
    std::string evidence_file;
    int evidence_line{0};

    [[nodiscard]] auto is_complete() const noexcept -> bool
    { return is_sole_authority && has_error_handling; }
};

struct SettingsCompletionReport
{
    std::size_t total{0};
    std::size_t complete{0};
    std::size_t incomplete{0};

    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete) / static_cast<double>(total)) * 100.0 : 100.0; }
};

class SettingsCompletionAuditor
{
public:
    SettingsCompletionAuditor() = default;

    void add_item(SettingsCompletionItem item);
    void add_items(std::vector<SettingsCompletionItem> items);

    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(SettingsCapabilityArea area) const
        -> std::vector<const SettingsCompletionItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const SettingsCompletionItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const SettingsCompletionItem*>;

    [[nodiscard]] auto report() const -> SettingsCompletionReport;
    void clear();

    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;

private:
    std::vector<SettingsCompletionItem> items_;
};

} // namespace markamp::core
