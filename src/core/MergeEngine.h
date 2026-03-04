// ============================================================================
// File: src/core/MergeEngine.h
// Phase 46: Diff Editor and Merge Tool — 3-way merge engine
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Merge line origin.
enum class MergeOrigin : uint8_t
{
    Base,    ///< From the common ancestor
    Theirs,  ///< From the incoming (theirs) version
    Yours,   ///< From the local (yours) version
    Both,    ///< Changed identically in both versions
    Conflict ///< Conflicting changes from both sides
};

/// Resolution for a conflict region.
enum class ConflictResolution : uint8_t
{
    Unresolved,   ///< Not yet resolved
    AcceptTheirs, ///< Use theirs version
    AcceptYours,  ///< Use yours version
    AcceptBoth,   ///< Include both sequentially
    Manual        ///< Custom hand-edited resolution
};

/// A single line in the merged output.
struct MergeLine
{
    std::string content;
    MergeOrigin origin{MergeOrigin::Base};
    int base_line{-1};   ///< Line number in base (-1 if not from base)
    int theirs_line{-1}; ///< Line number in theirs
    int yours_line{-1};  ///< Line number in yours
};

/// A conflict region where both sides changed the same area.
struct MergeConflict
{
    int start_line{0};                     ///< Start in merged output
    int end_line{0};                       ///< End in merged output (exclusive)
    std::vector<std::string> base_lines;   ///< Original base content
    std::vector<std::string> theirs_lines; ///< Theirs content
    std::vector<std::string> yours_lines;  ///< Yours content
    ConflictResolution resolution{ConflictResolution::Unresolved};
    std::vector<std::string> resolved_lines; ///< Custom resolution content
};

/// Result of a 3-way merge.
struct MergeResult
{
    std::vector<MergeLine> lines;         ///< Merged output lines
    std::vector<MergeConflict> conflicts; ///< Conflict regions
    int auto_merged_regions{0};           ///< Regions merged automatically
    bool has_conflicts{false};            ///< Whether any conflicts exist

    /// Count of unresolved conflicts.
    [[nodiscard]] auto unresolved_count() const -> int
    {
        int count = 0;
        for (const auto& c : conflicts)
        {
            if (c.resolution == ConflictResolution::Unresolved)
            {
                ++count;
            }
        }
        return count;
    }

    /// Whether all conflicts are resolved.
    [[nodiscard]] auto is_fully_resolved() const -> bool
    {
        return unresolved_count() == 0;
    }

    /// Get the final merged text (only valid when fully resolved).
    [[nodiscard]] auto merged_text() const -> std::string;
};

/// 3-way merge engine.
/// Takes base, theirs, and yours versions and produces a merged result.
/// Non-overlapping changes are auto-merged; overlapping changes create conflicts.
class MergeEngine
{
public:
    MergeEngine() = default;

    /// Perform a 3-way merge.
    [[nodiscard]] auto merge(std::string_view base,
                             std::string_view theirs,
                             std::string_view yours) const -> MergeResult;

    /// Resolve a conflict by index.
    static void
    resolve_conflict(MergeResult& result, size_t conflict_index, ConflictResolution resolution);

    /// Apply a manual resolution with custom content.
    static void resolve_conflict_manual(MergeResult& result,
                                        size_t conflict_index,
                                        std::vector<std::string> resolved_lines);

    /// Split text into lines.
    [[nodiscard]] static auto split_lines(std::string_view text) -> std::vector<std::string>;

private:
    /// Compute line-level LCS between two sets of lines.
    [[nodiscard]] static auto lcs_lines(const std::vector<std::string>& a,
                                        const std::vector<std::string>& b)
        -> std::vector<std::pair<int, int>>;

    /// A change region relative to the base.
    struct ChangeRegion
    {
        int base_start{0};
        int base_end{0}; // exclusive
        int other_start{0};
        int other_end{0}; // exclusive
        enum class Source : uint8_t
        {
            Theirs,
            Yours
        } source{Source::Theirs};
    };

    /// Extract change regions from LCS matches.
    [[nodiscard]] static auto extract_changes(const std::vector<std::string>& base,
                                              const std::vector<std::string>& other,
                                              const std::vector<std::pair<int, int>>& matches,
                                              ChangeRegion::Source source)
        -> std::vector<ChangeRegion>;
};

} // namespace markamp::core
