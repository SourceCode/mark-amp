#pragma once

/**
 * @file JumpToController.h
 * @brief Phase 30 Task 2: Unified "Jump To" controller.
 *
 * Unified jump modal supporting files, headings, symbols, and canvas
 * nodes with MRU ranking and type filters.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Kind of jump target.
enum class JumpTargetKind : uint8_t
{
    kFile,
    kHeading,
    kSymbol,
    kCanvasNode,
    kBreadcrumb,
    kRecentLocation,
};

/// A single jump target result.
struct JumpTarget
{
    std::string label;       ///< Display label
    std::string description; ///< Secondary description (path, type info)
    std::string icon_name;   ///< Icon to display
    JumpTargetKind kind{JumpTargetKind::kFile};
    std::string target_id; ///< Unique identifier for the target
    int score{0};          ///< MRU/relevance score (higher = better)
    bool is_recent{false}; ///< Whether this is from recent history

    /// Human-readable kind label.
    [[nodiscard]] auto kind_label() const -> std::string;
};

/// Filter bitmask for jump targets.
struct JumpFilter
{
    bool include_files{true};
    bool include_headings{true};
    bool include_symbols{true};
    bool include_canvas_nodes{true};
    bool include_recent{true};

    /// Check if a kind is included.
    [[nodiscard]] auto includes(JumpTargetKind target_kind) const -> bool;

    /// Get the number of active filters.
    [[nodiscard]] auto active_count() const -> int;
};

/**
 * @brief Controller for the unified "Jump To" modal.
 *
 * Manages search, filtering, MRU ranking, and navigation execution.
 */
class JumpToController
{
public:
    JumpToController() = default;

    /// Add a target to the searchable index.
    void add_target(const JumpTarget& target);

    /// Remove all targets.
    void clear_targets();

    /// Search for matching targets with the given query and filter.
    [[nodiscard]] auto search(const std::string& query, const JumpFilter& filter = {}) const
        -> std::vector<JumpTarget>;

    /// Get recent targets (filtered and ranked by score).
    [[nodiscard]] auto recent_targets(int max_count = 10) const -> std::vector<JumpTarget>;

    /// Get the total number of targets.
    [[nodiscard]] auto target_count() const -> int;

    /// Record that a target was accessed (boosts MRU score).
    void record_access(const std::string& target_id);

    /// Get available filter kinds.
    [[nodiscard]] static auto available_kinds() -> std::vector<JumpTargetKind>;

private:
    std::vector<JumpTarget> targets_;

    /// Score a target against a query (fuzzy match).
    [[nodiscard]] static auto score_match(const JumpTarget& target, const std::string& query)
        -> int;
};

} // namespace markamp::ui
