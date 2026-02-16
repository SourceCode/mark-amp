/// @file SurfaceLink.h
/// @brief V8 Phase 11 (Phase 29) — Unified Surface Link Contract.
/// One canonical deep-link model so any surface can open/reveal any other
/// surface without custom glue.

#pragma once

#include <cstdint>
#include <string>

namespace markamp::core
{

/// Which surface (workspace mode) a link originates from or targets.
enum class SurfaceKind : std::uint8_t
{
    kEditor,
    kPreview,
    kCanvas,
    kGraph,
    kNotebook
};

/// What kind of entity the link targets within a surface.
enum class EntityKind : std::uint8_t
{
    kDocument,
    kHeading,
    kCodeBlock,
    kCanvasObject,
    kNotebookCell
};

/// A precise anchor within any surface.
struct LinkAnchor
{
    SurfaceKind surface_kind{SurfaceKind::kEditor};
    EntityKind entity_kind{EntityKind::kDocument};
    std::string file_path;
    int line{0};
    int column{0};
    std::string board_id;
    std::string object_id;
    std::string cell_id;
    std::string heading_id;
};

/// A directed link from one surface point to another.
struct SurfaceLink
{
    LinkAnchor from;
    LinkAnchor to;
    std::string reason; ///< e.g. "debug", "review", "explore", "fix"
};

// =========================================================================
// V8 Phase 12 (Phase 35): Link Resolution
// =========================================================================

/// Outcome of resolving a surface link.
enum class LinkResolveStatus : std::uint8_t
{
    kResolved,    ///< Target found and navigable
    kNotFound,    ///< Target entity does not exist
    kUnsupported, ///< Surface combination not supported
    kAmbiguous    ///< Multiple candidates, needs disambiguation
};

/// Why a link resolution failed.
enum class LinkFailureReason : std::uint8_t
{
    kNone,             ///< No failure
    kSurfaceNotActive, ///< Target surface is not instantiated
    kEntityMissing,    ///< Entity ID / anchor not found in target
    kAnchorInvalid,    ///< Line, column, or ID out of range
    kModeLocked        ///< Current mode prevents navigation
};

/// Result of attempting to resolve a surface link.
struct LinkResolveResult
{
    LinkResolveStatus status{LinkResolveStatus::kNotFound};
    LinkFailureReason failure_reason{LinkFailureReason::kNone};
    LinkAnchor resolved_anchor; ///< Final anchor after normalization
    std::string diagnostic;     ///< Human-readable explanation
};

// Forward declarations
class EventBus;

/// Routes surface links to their targets deterministically.
/// Every cross-surface jump emits a resolved event or an explicit
/// failure event — never silent failure.
class SurfaceLinkRouter
{
public:
    explicit SurfaceLinkRouter(EventBus& event_bus);

    /// Resolve a surface link. Returns the result and emits
    /// SurfaceLinkResolvedEvent or SurfaceTraversalFailedEvent.
    auto resolve(const SurfaceLink& link) -> LinkResolveResult;

    /// Check if a surface pair is routable without executing.
    [[nodiscard]] auto can_route(SurfaceKind from, SurfaceKind to) const -> bool;

    /// Convert SurfaceKind to display string.
    [[nodiscard]] static auto surface_name(SurfaceKind kind) -> std::string_view;

private:
    EventBus& event_bus_;

    /// Validate that the target anchor fields are consistent.
    [[nodiscard]] static auto validate_anchor(const LinkAnchor& anchor) -> bool;
};

} // namespace markamp::core
