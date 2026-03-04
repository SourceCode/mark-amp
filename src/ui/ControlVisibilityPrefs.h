#pragma once

/**
 * @file ControlVisibilityPrefs.h
 * @brief Phase 37 Task 2: Per-control cluster visibility preferences.
 *
 * Toggle control cluster visibility, provide discoverability for hidden
 * controls, and profile-based visibility defaults.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Visibility state for a control cluster.
struct ClusterVisibility
{
    std::string cluster_id;
    std::string display_name;
    bool visible{true};
    bool user_modified{false}; ///< True if user explicitly changed visibility
};

/**
 * @brief Manages per-control cluster visibility preferences.
 */
class ControlVisibilityPrefs
{
public:
    ControlVisibilityPrefs() = default;

    // ── Cluster registration ───────────────────────────────────────

    /// Register a control cluster.
    void register_cluster(const std::string& cluster_id,
                          const std::string& display_name,
                          bool default_visible = true);

    /// Get cluster count.
    [[nodiscard]] auto cluster_count() const -> int;

    // ── Visibility ─────────────────────────────────────────────────

    /// Set visibility for a cluster.
    void set_visible(const std::string& cluster_id, bool visible);

    /// Toggle visibility for a cluster.
    void toggle(const std::string& cluster_id);

    /// Check if a cluster is visible.
    [[nodiscard]] auto is_visible(const std::string& cluster_id) const -> bool;

    /// Get all visible cluster IDs.
    [[nodiscard]] auto visible_clusters() const -> std::vector<std::string>;

    /// Get all hidden cluster IDs (for discoverability).
    [[nodiscard]] auto hidden_clusters() const -> std::vector<std::string>;

    /// Get all clusters (for settings UI).
    [[nodiscard]] auto all_clusters() const -> const std::vector<ClusterVisibility>&;

    // ── Reset ──────────────────────────────────────────────────────

    /// Reset all clusters to default visibility.
    void reset_to_defaults();

    /// Reset a single cluster to default visibility.
    void reset_cluster(const std::string& cluster_id);

    // ── User-modified tracking ─────────────────────────────────────

    /// Check if any clusters have been user-modified.
    [[nodiscard]] auto has_user_modifications() const -> bool;

    /// Get user-modified cluster count.
    [[nodiscard]] auto user_modified_count() const -> int;

private:
    struct ClusterDefault
    {
        std::string cluster_id;
        bool default_visible{true};
    };

    std::vector<ClusterVisibility> clusters_;
    std::vector<ClusterDefault> defaults_;

    auto find_cluster(const std::string& cluster_id) -> ClusterVisibility*;
    [[nodiscard]] auto find_cluster(const std::string& cluster_id) const
        -> const ClusterVisibility*;
    [[nodiscard]] auto find_default(const std::string& cluster_id) const -> bool;
};

} // namespace markamp::ui
