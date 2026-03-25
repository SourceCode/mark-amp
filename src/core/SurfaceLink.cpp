/// @file SurfaceLink.cpp
/// @brief V8 Phase 12 (Phase 35) — Surface Link Router implementation.
/// Deterministic cross-surface traversal with explicit resolution/failure events.

#include "core/SurfaceLink.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/Logger.h"

namespace markamp::core
{

SurfaceLinkRouter::SurfaceLinkRouter(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto SurfaceLinkRouter::resolve(const SurfaceLink& link) -> LinkResolveResult
{
    LinkResolveResult result;

    // Validate the target anchor
    if (!validate_anchor(link.to))
    {
        result.status = LinkResolveStatus::kNotFound;
        result.failure_reason = LinkFailureReason::kAnchorInvalid;
        result.diagnostic = "Target anchor has invalid fields (negative line/column or "
                            "missing required IDs for surface kind)";

        // Emit failure event
        events::SurfaceTraversalFailedEvent fail_evt;
        fail_evt.target = link.to;
        fail_evt.error_message = result.diagnostic;
        event_bus_.publish(fail_evt);

        MARKAMP_LOG_WARN("Surface link resolution failed: {}", result.diagnostic);
        return result;
    }

    // Check if the surface pair is routable
    if (!can_route(link.from.surface_kind, link.to.surface_kind))
    {
        result.status = LinkResolveStatus::kUnsupported;
        result.failure_reason = LinkFailureReason::kSurfaceNotActive;
        result.diagnostic = std::string("Route not supported: ") +
                            std::string(surface_name(link.from.surface_kind)) + " -> " +
                            std::string(surface_name(link.to.surface_kind));

        events::SurfaceTraversalFailedEvent fail_evt;
        fail_evt.target = link.to;
        fail_evt.error_message = result.diagnostic;
        event_bus_.publish(fail_evt);

        MARKAMP_LOG_WARN("Surface link resolution failed: {}", result.diagnostic);
        return result;
    }

    // Resolution succeeded — normalize and return
    result.status = LinkResolveStatus::kResolved;
    result.failure_reason = LinkFailureReason::kNone;
    result.resolved_anchor = link.to;
    result.diagnostic = std::string("Resolved: ") +
                        std::string(surface_name(link.from.surface_kind)) + " -> " +
                        std::string(surface_name(link.to.surface_kind));

    // Emit success event
    events::SurfaceLinkResolvedEvent resolved_evt;
    resolved_evt.link = link;
    resolved_evt.success = true;
    event_bus_.publish(resolved_evt);

    MARKAMP_LOG_INFO("Surface link resolved: {} -> {}",
                     std::string(surface_name(link.from.surface_kind)),
                     std::string(surface_name(link.to.surface_kind)));
    return result;
}

auto SurfaceLinkRouter::can_route(SurfaceKind from, SurfaceKind to_surface) const -> bool
{
    // All same-surface routes are valid
    if (from == to_surface)
    {
        return true;
    }

    // Valid cross-surface pairs (bidirectional)
    // Editor <-> Preview, Editor <-> Graph,
    // Preview <-> Graph
    switch (from)
    {
        case SurfaceKind::kEditor:
            return true; // Editor can route to any surface
        case SurfaceKind::kPreview:
            return to_surface == SurfaceKind::kEditor || to_surface == SurfaceKind::kGraph;
        case SurfaceKind::kGraph:
            return true; // Graph can route to any surface
    }
    return false;
}

auto SurfaceLinkRouter::surface_name(SurfaceKind kind) -> std::string_view
{
    switch (kind)
    {
        case SurfaceKind::kEditor:
            return "Editor";
        case SurfaceKind::kPreview:
            return "Preview";
        case SurfaceKind::kGraph:
            return "Graph";
    }
    return "Unknown";
}

auto SurfaceLinkRouter::validate_anchor(const LinkAnchor& anchor) -> bool
{
    // Lines and columns must be non-negative
    if (anchor.line < 0 || anchor.column < 0)
    {
        return false;
    }

    // Headings need a heading_id or file_path
    if (anchor.entity_kind == EntityKind::kHeading && anchor.heading_id.empty() &&
        anchor.file_path.empty())
    {
        return false;
    }

    return true;
}

} // namespace markamp::core
