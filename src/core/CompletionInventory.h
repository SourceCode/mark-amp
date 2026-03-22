/// @file CompletionInventory.h
/// @brief V23 Phase 01 — Canonical inventory of unfinished-work markers.
///
/// Catalogs explicit markers (TODO, FIXME, stub, placeholder, deferred, etc.)
/// across the first-party codebase.  Each item is classified by subsystem,
/// completion family, severity, and production-path exposure.  Query APIs
/// support filtering by subsystem, severity, family, and production-path flag.
/// Summary generation and blocker counting drive downstream release gate checks.
#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

// ============================================================================
// MarkerType — lexical category of the raw marker
// ============================================================================

enum class MarkerType
{
    kStub,
    kPlaceholder,
    kTodo,
    kFixme,
    kDeferred,
    kForNow,
    kMock,
    kFake,
    kNotYet,
    kPending,
    kRealImplementationWould,
    kHack,
};

/// Human-readable label for a MarkerType.
[[nodiscard]] constexpr auto marker_type_label(MarkerType t) noexcept -> std::string_view
{
    switch (t)
    {
    case MarkerType::kStub:                     return "Stub";
    case MarkerType::kPlaceholder:              return "Placeholder";
    case MarkerType::kTodo:                     return "Todo";
    case MarkerType::kFixme:                    return "Fixme";
    case MarkerType::kDeferred:                 return "Deferred";
    case MarkerType::kForNow:                   return "ForNow";
    case MarkerType::kMock:                     return "Mock";
    case MarkerType::kFake:                     return "Fake";
    case MarkerType::kNotYet:                   return "NotYet";
    case MarkerType::kPending:                  return "Pending";
    case MarkerType::kRealImplementationWould:  return "RealImplementationWould";
    case MarkerType::kHack:                     return "Hack";
    }
    return "Unknown";
}

// ============================================================================
// CompletionFamily — actionable grouping of markers
// ============================================================================

enum class CompletionFamily
{
    kServiceStub,           ///< Service layer fake/simulated behavior
    kWorkflowPartial,       ///< Partially implemented user workflow
    kPanelShell,            ///< Visible panel that is a thin shell
    kPlaceholderRendering,  ///< Placeholder content in render paths
    kMigrationSeam,         ///< Migration-era transitional code
    kTestGap,               ///< Test expecting stub behavior
    kTransportStub,         ///< Stubbed network/IPC transport
    kPersistenceStub,       ///< Stubbed save/load/recovery
    kUIPlaceholder,         ///< UI element with placeholder content
    kUnclassified,          ///< Not yet classified
};

[[nodiscard]] constexpr auto completion_family_label(CompletionFamily f) noexcept
    -> std::string_view
{
    switch (f)
    {
    case CompletionFamily::kServiceStub:          return "ServiceStub";
    case CompletionFamily::kWorkflowPartial:      return "WorkflowPartial";
    case CompletionFamily::kPanelShell:           return "PanelShell";
    case CompletionFamily::kPlaceholderRendering: return "PlaceholderRendering";
    case CompletionFamily::kMigrationSeam:        return "MigrationSeam";
    case CompletionFamily::kTestGap:              return "TestGap";
    case CompletionFamily::kTransportStub:        return "TransportStub";
    case CompletionFamily::kPersistenceStub:      return "PersistenceStub";
    case CompletionFamily::kUIPlaceholder:        return "UIPlaceholder";
    case CompletionFamily::kUnclassified:         return "Unclassified";
    }
    return "Unknown";
}

// ============================================================================
// CompletionSeverity — risk classification per item
// ============================================================================

enum class CompletionSeverity
{
    kCritical,   ///< Production-path fake behavior
    kHigh,       ///< Visible-but-incomplete functionality
    kMedium,     ///< Secondary workflow gaps
    kLow,        ///< Cleanup and cosmetic markers
};

[[nodiscard]] constexpr auto completion_severity_label(CompletionSeverity s) noexcept
    -> std::string_view
{
    switch (s)
    {
    case CompletionSeverity::kCritical: return "Critical";
    case CompletionSeverity::kHigh:     return "High";
    case CompletionSeverity::kMedium:   return "Medium";
    case CompletionSeverity::kLow:      return "Low";
    }
    return "Unknown";
}

// ============================================================================
// InventoryItem — one cataloged marker
// ============================================================================

struct InventoryItem
{
    std::string  file_path;
    std::size_t  line_number{0};
    MarkerType   marker_type{MarkerType::kTodo};
    std::string  raw_text;
    std::string  subsystem_tag;
    CompletionSeverity   severity{CompletionSeverity::kMedium};
    CompletionFamily     family{CompletionFamily::kUnclassified};
    bool         is_production_path{false};
    std::string  blocked_feature;   ///< Feature chain this blocks (if any)
    std::string  notes;

    /// True if this item blocks a named feature.
    [[nodiscard]] auto blocks_feature() const noexcept -> bool
    {
        return !blocked_feature.empty();
    }

    /// True if classified as a release blocker.
    [[nodiscard]] auto is_blocker() const noexcept -> bool
    {
        return severity == CompletionSeverity::kCritical && is_production_path;
    }
};

// ============================================================================
// BlockedFeatureChain — links markers to the feature they hold back
// ============================================================================

struct BlockedFeatureChain
{
    std::string feature_name;
    std::string command_chain;
    std::string persistence_chain;
    std::string transport_chain;
    std::string test_chain;
    std::vector<std::size_t> blocking_item_indices;

    [[nodiscard]] auto blocker_count() const noexcept -> std::size_t
    {
        return blocking_item_indices.size();
    }
};

// ============================================================================
// InventorySummary — aggregate counts
// ============================================================================

struct InventorySummary
{
    std::size_t total_items{0};
    std::size_t critical{0};
    std::size_t high{0};
    std::size_t medium{0};
    std::size_t low{0};
    std::size_t blockers{0};
    std::size_t production_path{0};
    std::size_t test_only{0};

    [[nodiscard]] auto blocker_rate_pct() const noexcept -> double
    {
        return total_items > 0
            ? static_cast<double>(blockers) / static_cast<double>(total_items) * 100.0
            : 0.0;
    }

    [[nodiscard]] auto has_blockers() const noexcept -> bool
    {
        return blockers > 0;
    }
};

// ============================================================================
// CompletionInventory — the canonical unfinished-work catalog
// ============================================================================

class CompletionInventory
{
public:
    CompletionInventory() = default;

    // ── Registration ──

    void add_item(InventoryItem item);
    void add_items(std::vector<InventoryItem> items);
    void clear();

    // ── Classification ──

    /// Classify a raw marker type into a completion family based on subsystem
    /// and production-path context.
    [[nodiscard]] static auto classify_family(MarkerType type,
                                              std::string_view subsystem,
                                              bool is_production) -> CompletionFamily;

    /// Auto-tag subsystem from a file path.
    [[nodiscard]] static auto subsystem_from_path(std::string_view file_path)
        -> std::string;

    /// Determine severity from marker type and production-path context.
    [[nodiscard]] static auto severity_for(MarkerType type,
                                           bool is_production) -> CompletionSeverity;

    // ── Queries ──

    [[nodiscard]] auto item_count() const noexcept -> std::size_t;

    [[nodiscard]] auto items_by_subsystem(std::string_view subsystem) const
        -> std::vector<const InventoryItem*>;

    [[nodiscard]] auto items_by_severity(CompletionSeverity severity) const
        -> std::vector<const InventoryItem*>;

    [[nodiscard]] auto items_by_family(CompletionFamily family) const
        -> std::vector<const InventoryItem*>;

    [[nodiscard]] auto production_path_items() const
        -> std::vector<const InventoryItem*>;

    [[nodiscard]] auto test_only_items() const
        -> std::vector<const InventoryItem*>;

    [[nodiscard]] auto blocker_items() const
        -> std::vector<const InventoryItem*>;

    [[nodiscard]] auto all_items() const
        -> std::vector<const InventoryItem*>;

    [[nodiscard]] auto all_subsystems() const -> std::vector<std::string>;

    // ── Feature-Chain Linkage ──

    void add_feature_chain(BlockedFeatureChain chain);

    [[nodiscard]] auto feature_chain_count() const noexcept -> std::size_t;

    [[nodiscard]] auto feature_chain_for(std::string_view feature_name) const
        -> const BlockedFeatureChain*;

    [[nodiscard]] auto all_feature_chains() const
        -> std::vector<const BlockedFeatureChain*>;

    // ── Summary ──

    [[nodiscard]] auto summary() const -> InventorySummary;

    // ── Export ──

    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;

private:
    std::vector<InventoryItem>        items_;
    std::vector<BlockedFeatureChain>  feature_chains_;
};

} // namespace markamp::core
