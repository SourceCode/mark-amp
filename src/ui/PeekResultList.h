// ============================================================================
// File: src/ui/PeekResultList.h
// Phase 47: Peek View System — Result list model
// ============================================================================
#pragma once

#include "../core/PeekProvider.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

/// Result list display mode.
enum class PeekResultDisplayMode : uint8_t
{
    FlatList,     ///< Simple flat list of results
    GroupedByFile ///< Results grouped by file path
};

/// A group of results from the same file.
struct PeekResultGroup
{
    std::string file_path;
    std::string file_name;              ///< Basename for display
    std::vector<size_t> result_indices; ///< Indices into the flat result list
    bool is_collapsed{false};
};

/// Model for the peek result list panel.
class PeekResultListModel
{
public:
    PeekResultListModel() = default;

    /// Set the results to display.
    void set_results(const std::vector<core::PeekLocation>& results);

    /// Get display mode.
    [[nodiscard]] auto display_mode() const -> PeekResultDisplayMode
    {
        return display_mode_;
    }

    /// Set display mode.
    void set_display_mode(PeekResultDisplayMode mode);

    /// Get flat result list.
    [[nodiscard]] auto results() const -> const std::vector<core::PeekLocation>&
    {
        return results_;
    }

    /// Get grouped results.
    [[nodiscard]] auto groups() const -> const std::vector<PeekResultGroup>&
    {
        return groups_;
    }

    /// Toggle collapse for a group.
    void toggle_group(size_t group_index);

    /// Get total result count.
    [[nodiscard]] auto count() const -> int
    {
        return static_cast<int>(results_.size());
    }

    /// Get group count.
    [[nodiscard]] auto group_count() const -> int
    {
        return static_cast<int>(groups_.size());
    }

private:
    std::vector<core::PeekLocation> results_;
    std::vector<PeekResultGroup> groups_;
    PeekResultDisplayMode display_mode_{PeekResultDisplayMode::GroupedByFile};

    void rebuild_groups();
    [[nodiscard]] static auto extract_filename(const std::string& path) -> std::string;
};

} // namespace markamp::ui
