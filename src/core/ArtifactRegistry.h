/// @file ArtifactRegistry.h
/// @brief Unified artifact registry and unsaved artifact model.
///
/// Introduces the canonical artifact identity layer used by all creation,
/// lifecycle, persistence, and shell-integration code. Every file
/// is represented by an ArtifactRecord in the registry.
#pragma once

#include "EventBus.h"

#include <chrono>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Core enumerations
// ============================================================================

/// Discriminates first-class artifact families.
enum class ArtifactKind
{
    kTextFile
};

/// Lifecycle state machine for an artifact.
enum class ArtifactLifecycleState
{
    kUnsaved,    ///< Newly created, never persisted
    kSaved,      ///< Successfully persisted to disk
    kDirty,      ///< Modified since last save
    kSaving,     ///< Save in progress
    kSaveError,  ///< Last save attempt failed
    kDeleted     ///< Marked for removal
};

// ============================================================================
// Value types
// ============================================================================

/// Unique identity for an artifact. Wraps a UUID string.
struct ArtifactId
{
    std::string value;

    [[nodiscard]] auto empty() const noexcept -> bool { return value.empty(); }

    auto operator==(const ArtifactId& other) const -> bool = default;
    auto operator!=(const ArtifactId& other) const -> bool = default;
};

/// Core record describing a single artifact in the registry.
struct ArtifactRecord
{
    ArtifactId id;
    ArtifactKind kind{ArtifactKind::kTextFile};
    ArtifactLifecycleState state{ArtifactLifecycleState::kUnsaved};

    std::string display_name;                        ///< User-visible name
    std::optional<std::string> file_path;            ///< Disk path (empty for unsaved)
    std::string language_id;                         ///< Language identifier (e.g. "cpp", "markdown")
    std::string source;                              ///< Entry point that created this artifact

    std::chrono::steady_clock::time_point created_at{std::chrono::steady_clock::now()};
    std::chrono::steady_clock::time_point modified_at{std::chrono::steady_clock::now()};

    // ── Convenience queries ──

    [[nodiscard]] auto is_unsaved() const noexcept -> bool
    {
        return state == ArtifactLifecycleState::kUnsaved;
    }

    [[nodiscard]] auto is_dirty() const noexcept -> bool
    {
        return state == ArtifactLifecycleState::kDirty;
    }

    [[nodiscard]] auto is_saved() const noexcept -> bool
    {
        return state == ArtifactLifecycleState::kSaved;
    }

    [[nodiscard]] auto has_path() const noexcept -> bool
    {
        return file_path.has_value() && !file_path->empty();
    }

    [[nodiscard]] auto is_text_file() const noexcept -> bool
    {
        return kind == ArtifactKind::kTextFile;
    }
};

// ============================================================================
// ArtifactRegistry
// ============================================================================

/// Central registry for all open artifacts.
///
/// Provides CRUD operations, find-by-ID/path, active-artifact tracking,
/// counting by kind/state, and event publishing on mutations.
class ArtifactRegistry
{
public:
    explicit ArtifactRegistry(EventBus& bus);

    // ── Registration ──

    /// Register a new artifact. Returns the assigned ID.
    [[nodiscard]] auto register_artifact(ArtifactRecord record) -> ArtifactId;

    /// Remove an artifact from the registry.
    [[nodiscard]] auto unregister(const ArtifactId& id) -> bool;

    // ── Lookup ──

    /// Find an artifact by its unique ID. Returns nullptr if not found.
    [[nodiscard]] auto find(const ArtifactId& id) const -> const ArtifactRecord*;

    /// Find an artifact by its file path. Returns nullptr if not found.
    [[nodiscard]] auto find_by_path(const std::string& path) const -> const ArtifactRecord*;

    // ── Update ──

    /// Replace the record for an existing artifact.
    [[nodiscard]] auto update(const ArtifactId& id, ArtifactRecord record) -> bool;

    /// Transition an artifact's lifecycle state.
    [[nodiscard]] auto set_state(const ArtifactId& id, ArtifactLifecycleState new_state) -> bool;

    /// Promote an unsaved artifact to saved with a file path.
    [[nodiscard]] auto promote_to_saved(const ArtifactId& id, const std::string& path) -> bool;

    /// Rename an artifact's display name.
    [[nodiscard]] auto rename(const ArtifactId& id, const std::string& new_name) -> bool;

    /// V24 P02-T03: Standardize initial metadata on an artifact.
    /// Sets language_id, source, and display_name if not already set.
    [[nodiscard]] auto set_initial_metadata(const ArtifactId& id,
                                             const std::string& language_id,
                                             const std::string& source,
                                             const std::string& display_name = {}) -> bool;

    // ── Enumeration ──

    /// All registered artifacts.
    [[nodiscard]] auto all_artifacts() const -> std::vector<ArtifactRecord>;

    /// Artifacts filtered by kind.
    [[nodiscard]] auto artifacts_by_kind(ArtifactKind kind) const -> std::vector<ArtifactRecord>;

    /// Artifacts filtered by lifecycle state.
    [[nodiscard]] auto artifacts_by_state(ArtifactLifecycleState state) const
        -> std::vector<ArtifactRecord>;

    // ── Active artifact ──

    /// Get the currently active artifact ID (may be empty).
    [[nodiscard]] auto active_artifact() const -> const ArtifactId& { return active_id_; }

    /// Set the active artifact. Publishes ArtifactActivatedEvent.
    void set_active_artifact(const ArtifactId& id);

    // ── Counting ──

    /// Total number of registered artifacts.
    [[nodiscard]] auto count() const -> int { return static_cast<int>(records_.size()); }

    /// Count by kind.
    [[nodiscard]] auto count_by_kind(ArtifactKind kind) const -> int;

    /// Count by state.
    [[nodiscard]] auto count_by_state(ArtifactLifecycleState state) const -> int;

    // ── ID generation ──

    /// Generate a new unique artifact ID.
    [[nodiscard]] static auto generate_id() -> ArtifactId;

    // ── Clearing ──

    /// Clear all artifacts (for testing / workspace close).
    void clear();

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, ArtifactRecord> records_;
    ArtifactId active_id_;
};

} // namespace markamp::core
