#pragma once

#include "canvas/CanvasTypes.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::canvas
{

class Board;

/// Describes a unique tag with its color and usage count.
struct TagInfo
{
    std::string name;
    CanvasColor color{100, 100, 100, 255};
    int usage_count{0};

    /// Whether this tag has zero usages.
    [[nodiscard]] auto is_unused() const noexcept -> bool
    {
        return usage_count == 0;
    }
};

/// Indexes all tags across board objects, provides per-tag object lookup,
/// configurable tag colors, and prefix-based autocomplete.
class TagManager
{
public:
    /// Rebuild the full tag index from all objects in the board.
    auto rebuild_index(const Board& board) -> void;

    /// Get all known tags sorted by usage count (descending).
    [[nodiscard]] auto all_tags() const -> std::vector<TagInfo>;

    /// Get object IDs that carry a specific tag.
    [[nodiscard]] auto objects_with_tag(const std::string& tag) const -> std::vector<ObjectId>;

    /// Set a custom color for a tag.
    auto set_tag_color(const std::string& tag, const CanvasColor& color) -> void;

    /// Get the color for a tag (default grey if not customized).
    [[nodiscard]] auto tag_color(const std::string& tag) const -> CanvasColor;

    /// Return tag names that start with the given prefix.
    [[nodiscard]] auto suggest_tags(const std::string& prefix) const -> std::vector<std::string>;

    /// Number of distinct tags in the index.
    [[nodiscard]] auto tag_count() const -> size_t;

    // ── Batch 8 (#43-46) ──────────────────────────────────────────

    /// Rename a tag across all objects in the index.
    auto rename_tag(const std::string& old_name, const std::string& new_name) -> void;

    /// Remove a tag from the index and all objects.
    auto delete_tag(const std::string& tag_name) -> void;

    /// Merge source tag into destination tag (source is removed).
    auto merge_tags(const std::string& source, const std::string& destination) -> void;

    /// Return the top-N tags by usage_count (descending).
    [[nodiscard]] auto most_used_tags(size_t top_n) const -> std::vector<TagInfo>;

    /// (#94) Check if a tag exists in the index.
    [[nodiscard]] auto has_tag(const std::string& tag_name) const -> bool;

    /// Whether the tag index is empty.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return tags_.empty();
    }

private:
    std::unordered_map<std::string, TagInfo> tags_;
    std::unordered_map<std::string, std::unordered_set<ObjectId>> tag_to_objects_;
};

} // namespace markamp::canvas
