#pragma once

/// @file BacklinksPanel.h
/// @brief V4 Phase 06 – Backlinks Panel and Local Graph Panel data models.

#include "../core/BacklinkIndex.h"
#include "../core/EventBus.h"

#include <string>
#include <vector>

namespace markamp::core
{

class VaultService;
class ThemeEngine;
class GraphEngine;

// ============================================================================
// Backlinks Panel Data Structures
// ============================================================================

/// Display state for a single backlink mention row.
struct BacklinkMentionItem
{
    std::string source_document_id;
    std::string source_document_title;
    std::string context_snippet; // Surrounding text for preview
    int line_number{0};
    bool is_linked{true}; // true = [[wiki]] link, false = plain text mention
};

/// Display group: all mentions from one source document.
struct BacklinkSourceGroup
{
    std::string document_id;
    std::string document_title;
    std::string relative_path;
    std::vector<BacklinkMentionItem> mentions;
};

/// Filter state for the Backlinks Panel.
struct BacklinkFilterState
{
    std::string filter_text;
    bool show_linked{true};
    bool show_unlinked{true};

    enum class SortOrder : uint8_t
    {
        kByDocument,
        kByLineNumber,
        kByRecent
    };
    SortOrder sort_order{SortOrder::kByDocument};
};

/// Computed data for the Backlinks Panel.
struct BacklinksPanelData
{
    std::string document_id;
    std::string document_title;
    std::vector<BacklinkSourceGroup> linked_groups;
    std::vector<BacklinkSourceGroup> unlinked_groups;
    int total_linked{0};
    int total_unlinked{0};
};

// ============================================================================
// Local Graph Panel Data Structures
// ============================================================================

/// A node in the local graph visualization.
struct LocalGraphNode
{
    std::string document_id;
    std::string label;
    double angle{0.0};  // Position angle on circle (radians)
    double radius{6.0}; // Display radius
    bool is_center{false};
    bool is_hovered{false};
    int link_count{0};
};

/// An edge in the local graph visualization.
struct LocalGraphEdge
{
    int source_index{0};
    int target_index{0};
    bool is_bidirectional{false};
};

/// Computed data for the Local Graph Panel.
struct LocalGraphData
{
    std::string center_document_id;
    std::vector<LocalGraphNode> nodes;
    std::vector<LocalGraphEdge> edges;
    double layout_radius{120.0}; // Radius of the circular layout
};

// ============================================================================
// Backlinks Panel Controller
// ============================================================================

/// Non-UI controller for the Backlinks Panel.
/// Queries BacklinkIndex and prepares display data.
class BacklinksPanelController
{
public:
    BacklinksPanelController(EventBus& event_bus,
                             VaultService& vault_service,
                             BacklinkIndex& backlink_index);

    /// Compute backlinks data for a document.
    [[nodiscard]] auto compute_backlinks(const std::string& document_id) const
        -> BacklinksPanelData;

    /// Apply filter to existing data.
    [[nodiscard]] auto apply_filter(const BacklinksPanelData& data,
                                    const BacklinkFilterState& filter) const -> BacklinksPanelData;

    /// Convert an unlinked mention to a wiki link.
    /// Returns true if the conversion was applied to the source document.
    auto convert_to_link(const std::string& source_document_id,
                         int line_number,
                         const std::string& mention_text) -> bool;

    /// Build the local graph for a document (circular layout).
    [[nodiscard]] auto compute_local_graph(const std::string& document_id, int depth = 1) const
        -> LocalGraphData;

    /// Get the currently active document ID.
    [[nodiscard]] auto active_document_id() const -> const std::string&;

    /// Set active document and publish refresh event.
    auto set_active_document(const std::string& document_id) -> void;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;
    BacklinkIndex& backlink_index_;

    std::string active_document_id_;

    /// Build mention items from BacklinkEntry data.
    [[nodiscard]] auto build_mention_items(const std::vector<BacklinkEntry>& entries,
                                           bool is_linked) const
        -> std::vector<BacklinkSourceGroup>;

    Subscription doc_switched_sub_;
};

} // namespace markamp::core
