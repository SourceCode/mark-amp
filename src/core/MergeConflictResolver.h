/// @file MergeConflictResolver.h
/// @brief Phase 25: Version Control Integration — Merge conflict resolution.
/// Parses <<<<<<< / ======= / >>>>>>> conflict markers, supports multiple
/// resolution strategies, and generates merged output.

#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Data structures
// ============================================================================

/// Resolution strategy for a conflict region.
enum class ResolutionStrategy : uint8_t
{
    kUseLocal,
    kUseRemote,
    kUseBase,
    kManual
};

/// A single conflicting region within a file.
struct ConflictRegion
{
    int start_line{0};
    int end_line{0};
    std::string base_content;     ///< Common ancestor (if available).
    std::string local_content;    ///< "Ours" side.
    std::string remote_content;   ///< "Theirs" side.
    std::string resolved_content; ///< Final resolved content.
    bool is_resolved{false};
};

/// A file containing one or more conflict regions.
struct ConflictFile
{
    std::string file_path;
    std::vector<ConflictRegion> regions;
    std::string original_content; ///< Full file content with conflict markers.

    /// Check whether every region has been resolved.
    [[nodiscard]] auto all_resolved() const -> bool
    {
        return std::ranges::all_of(regions,
                                   [](const ConflictRegion& region) { return region.is_resolved; });
    }

    /// Count unresolved regions.
    [[nodiscard]] auto unresolved_count() const -> int
    {
        int count = 0;
        for (const auto& region : regions)
        {
            if (!region.is_resolved)
            {
                ++count;
            }
        }
        return count;
    }
};

// ============================================================================
// MergeConflictResolver
// ============================================================================

/// Parses conflict markers in file content and provides resolution tools.
class MergeConflictResolver
{
public:
    explicit MergeConflictResolver(EventBus& event_bus);

    /// Parse conflict markers in content and return a ConflictFile.
    [[nodiscard]] auto parse_conflicts(const std::string& file_path, const std::string& content)
        -> ConflictFile;

    /// Resolve a specific conflict region by index using a strategy.
    [[nodiscard]] auto
    resolve_region(ConflictFile& file, int region_index, ResolutionStrategy strategy) -> bool;

    /// Resolve a specific conflict region with manual content.
    [[nodiscard]] auto
    resolve_region_manual(ConflictFile& file, int region_index, const std::string& content) -> bool;

    /// Resolve all regions using the same strategy.
    auto resolve_all(ConflictFile& file, ResolutionStrategy strategy) -> int;

    /// Generate the final merged content from resolved regions.
    [[nodiscard]] auto generate_merged_content(const ConflictFile& file) -> std::string;

    /// Check if content contains conflict markers.
    [[nodiscard]] static auto has_conflicts(const std::string& content) -> bool;

    /// Count the number of conflict regions in content.
    [[nodiscard]] static auto count_conflicts(const std::string& content) -> int;

private:
    EventBus& event_bus_;
};

} // namespace markamp::core
