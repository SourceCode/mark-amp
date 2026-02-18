/// @file CrossSurfaceRouter.h
/// @brief V9 Phase 17 Task 5 — Enhanced cross-surface routing with block/heading resolution.
///
/// Header-only. Wraps SurfaceLinkRouter with BlockReferenceResolver and
/// HeadingJumpProtocol to provide enriched routing across all 5×5 surface
/// combinations (Editor, Preview, Canvas, Graph, Notebook).

#pragma once

#include "BlockReferenceResolver.h"
#include "HeadingJumpProtocol.h"
#include "SurfaceLink.h"

#include <optional>
#include <string>
#include <utility>

namespace markamp::core
{

// ============================================================================
// RouteResult
// ============================================================================

/// Enriched result of cross-surface routing.
struct RouteResult
{
    LinkResolveStatus status{LinkResolveStatus::kNotFound};
    LinkFailureReason failure_reason{LinkFailureReason::kNone};
    LinkAnchor resolved_anchor;
    std::string diagnostic;

    /// Resolved heading info (if the link targets a heading).
    std::optional<HeadingTarget> resolved_heading;

    /// Resolved block reference info (if the link targets a ^block-id).
    std::optional<BlockReference> resolved_block;

    /// Human-readable breadcrumb label for the destination.
    /// e.g. "Editor › notes/daily.md › Section 3"
    std::string breadcrumb_label;

    /// Whether the route crossed surface boundaries.
    bool is_cross_surface{false};
};

// ============================================================================
// RoutePair — describes a from→to surface combination
// ============================================================================

/// Routing capability for a specific surface pair.
struct RoutePairCapability
{
    SurfaceKind from{SurfaceKind::kEditor};
    SurfaceKind to{SurfaceKind::kEditor};
    bool supported{false};
    std::string description;
};

// ============================================================================
// CrossSurfaceRouter
// ============================================================================

/// Unified cross-surface router integrating link resolution, block references,
/// and heading jumps.
///
/// Wraps `SurfaceLinkRouter` and enriches resolution with block-ref and heading
/// data. All 25 surface-pair combinations are supported.
///
/// Usage:
/// ```cpp
/// CrossSurfaceRouter router(event_bus);
/// RouteResult result = router.route(surface_link);
/// ```
class CrossSurfaceRouter
{
public:
    explicit CrossSurfaceRouter(EventBus& event_bus)
        : link_router_(event_bus)
    {
        build_routing_table();
    }

    /// Route a surface link with enriched resolution.
    [[nodiscard]] auto route(const SurfaceLink& link) -> RouteResult
    {
        RouteResult result;
        result.is_cross_surface = (link.from.surface_kind != link.to.surface_kind);

        // Check if this surface pair is routable
        if (!is_routable(link.from.surface_kind, link.to.surface_kind))
        {
            result.status = LinkResolveStatus::kUnsupported;
            result.failure_reason = LinkFailureReason::kModeLocked;
            result.diagnostic =
                "Route from " +
                std::string(SurfaceLinkRouter::surface_name(link.from.surface_kind)) + " to " +
                std::string(SurfaceLinkRouter::surface_name(link.to.surface_kind)) +
                " is not supported";
            return result;
        }

        // Base resolution via SurfaceLinkRouter
        auto base_result = link_router_.resolve(link);
        result.status = base_result.status;
        result.failure_reason = base_result.failure_reason;
        result.resolved_anchor = base_result.resolved_anchor;
        result.diagnostic = base_result.diagnostic;

        // Enrich with heading resolution
        if (!link.to.heading_id.empty())
        {
            auto heading_target = resolve_heading_for_link(link);
            if (heading_target.has_value())
            {
                result.resolved_heading = heading_target;
                result.resolved_anchor.heading_id = heading_target->heading_slug;
                result.resolved_anchor.line = heading_target->line_number;
            }
        }

        // Enrich with block reference resolution
        if (!link.to.object_id.empty() && link.to.object_id[0] == '^')
        {
            auto block_id = link.to.object_id.substr(1);
            auto block_ref = block_index_.resolve(link.to.file_path, block_id);
            if (!block_ref.has_value())
            {
                block_ref = block_index_.resolve_any(block_id);
            }
            if (block_ref.has_value())
            {
                result.resolved_block = block_ref;
                result.resolved_anchor.line = block_ref->line_number;
            }
        }

        // Build breadcrumb label
        result.breadcrumb_label = build_breadcrumb_label(link, result);

        return result;
    }

    /// Check if a surface-pair combination is routable.
    [[nodiscard]] auto is_routable(SurfaceKind from, SurfaceKind to) const -> bool
    {
        for (const auto& pair : routing_table_)
        {
            if (pair.from == from && pair.to == to)
            {
                return pair.supported;
            }
        }
        return false;
    }

    /// Get the routing table (all 25 combinations).
    [[nodiscard]] auto routing_table() const -> const std::vector<RoutePairCapability>&
    {
        return routing_table_;
    }

    /// Get a description of a specific route.
    [[nodiscard]] auto route_description(SurfaceKind from, SurfaceKind to) const -> std::string
    {
        for (const auto& pair : routing_table_)
        {
            if (pair.from == from && pair.to == to)
            {
                return pair.description;
            }
        }
        return "Unknown route";
    }

    /// Get underlying block reference index for direct indexing.
    [[nodiscard]] auto block_index() -> BlockReferenceIndex&
    {
        return block_index_;
    }

    /// Get underlying heading index for direct indexing.
    [[nodiscard]] auto heading_index() -> HeadingIndex&
    {
        return heading_index_;
    }

    /// Index a document for both block references and headings.
    void index_document(const std::string& document_id, const std::string& content)
    {
        block_index_.index_document(document_id, content);
        auto headings = HeadingJumpProtocol::find_all_headings(content);
        heading_index_.add_document(document_id, headings);
    }

    /// Count of supported routes.
    [[nodiscard]] auto supported_route_count() const -> int
    {
        int supported_count = 0;
        for (const auto& pair : routing_table_)
        {
            if (pair.supported)
            {
                ++supported_count;
            }
        }
        return supported_count;
    }

private:
    SurfaceLinkRouter link_router_;
    BlockReferenceIndex block_index_;
    HeadingIndex heading_index_;
    std::vector<RoutePairCapability> routing_table_;

    /// Build the 5×5 routing capability table.
    void build_routing_table()
    {
        routing_table_.clear();

        const SurfaceKind surfaces[] = {SurfaceKind::kEditor,
                                        SurfaceKind::kPreview,
                                        SurfaceKind::kCanvas,
                                        SurfaceKind::kGraph,
                                        SurfaceKind::kNotebook};

        for (auto from_surface : surfaces)
        {
            for (auto to_surface : surfaces)
            {
                RoutePairCapability pair;
                pair.from = from_surface;
                pair.to = to_surface;
                pair.supported = compute_support(from_surface, to_surface);
                pair.description = compute_description(from_surface, to_surface);
                routing_table_.push_back(pair);
            }
        }
    }

    /// Determine if a surface pair is supported.
    [[nodiscard]] static auto compute_support(SurfaceKind from, SurfaceKind to) -> bool
    {
        // All same-surface routes are supported
        if (from == to)
        {
            return true;
        }

        // All cross-surface routes are supported
        // (Editor, Preview, Canvas, Graph, Notebook can all link to each other)
        return true;
    }

    /// Generate a human-readable description for a route.
    [[nodiscard]] static auto compute_description(SurfaceKind from, SurfaceKind to) -> std::string
    {
        const auto from_name = std::string(SurfaceLinkRouter::surface_name(from));
        const auto to_name = std::string(SurfaceLinkRouter::surface_name(to));

        if (from == to)
        {
            return "Navigate within " + from_name + " (same surface)";
        }

        return "Navigate from " + from_name + " to " + to_name;
    }

    /// Resolve heading for a link target.
    [[nodiscard]] auto resolve_heading_for_link(const SurfaceLink& link) const
        -> std::optional<HeadingTarget>
    {
        // Try the heading index first
        auto result = heading_index_.find(link.to.file_path, link.to.heading_id);
        if (result.has_value())
        {
            return result;
        }

        // If not indexed, try to find by slug across all documents
        auto all_matches = heading_index_.search(link.to.heading_id);
        if (!all_matches.empty())
        {
            // Prefer match in the target document
            for (const auto& match : all_matches)
            {
                if (match.document_id == link.to.file_path)
                {
                    return match;
                }
            }
            // Otherwise return first match
            return all_matches.front();
        }

        return std::nullopt;
    }

    /// Build breadcrumb label for a route result.
    [[nodiscard]] static auto build_breadcrumb_label(const SurfaceLink& link,
                                                     const RouteResult& result) -> std::string
    {
        std::string label;

        // Surface name
        label += std::string(SurfaceLinkRouter::surface_name(link.to.surface_kind));

        // Document/target path
        if (!link.to.file_path.empty())
        {
            label += " \xE2\x80\xBA "; // " › " (UTF-8 encoded)
            // Use just the filename portion
            auto slash_pos = link.to.file_path.rfind('/');
            if (slash_pos != std::string::npos)
            {
                label += link.to.file_path.substr(slash_pos + 1);
            }
            else
            {
                label += link.to.file_path;
            }
        }

        // Heading if resolved
        if (result.resolved_heading.has_value())
        {
            label += " \xE2\x80\xBA "; // " › "
            label += result.resolved_heading->heading_text;
        }

        // Block ref if resolved
        if (result.resolved_block.has_value())
        {
            label += " \xE2\x80\xBA ^";
            label += result.resolved_block->block_id;
        }

        return label;
    }
};

} // namespace markamp::core
