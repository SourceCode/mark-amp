#pragma once

#include "canvas/CanvasTypes.h"
#include "canvas/TagManager.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Panel displaying all tags with usage counts and providing tag-based
/// filtering of board objects.
class TagPanel
{
public:
    using OnFilterByTag = std::function<void(const std::string& tag)>;

    /// Populate the panel with tag data.
    auto set_tags(const std::vector<TagInfo>& tags) -> void;

    /// Clear the tag list.
    auto clear() -> void;

    /// Number of tags displayed.
    [[nodiscard]] auto tag_count() const -> size_t;

    /// Access a tag by index.
    [[nodiscard]] auto tag_at(size_t index) const -> const TagInfo&;

    /// Active tag filter (empty = no filter).
    [[nodiscard]] auto active_filter() const -> const std::string&;

    /// Set the active filter tag.
    auto set_active_filter(const std::string& tag) -> void;

    /// Clear the active filter.
    auto clear_filter() -> void;

    // ── Callbacks ──────────────────────────────────────────────────
    auto set_on_filter_by_tag(OnFilterByTag callback) -> void;

    // ── Visibility ─────────────────────────────────────────────────
    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

private:
    std::vector<TagInfo> tags_;
    std::string active_filter_;
    bool visible_{false};
    OnFilterByTag on_filter_by_tag_;
};

} // namespace markamp::canvas
