#pragma once

#include "EventBus.h"
#include "Outline.h"

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class Config;

/// Service for document outline (table of contents) navigation.
/// Extracts heading structure, caches results, and provides scroll-sync data.
class OutlineService
{
public:
    OutlineService(EventBus& event_bus, Config& config);

    /// Return the hierarchical outline for a document. Cached, rebuilds on invalidation.
    [[nodiscard]] auto get_outline(const std::string& root_id) -> DocumentOutline;

    /// Flatten the outline into a linear list with depth/indent info.
    [[nodiscard]] auto get_flat_outline(const std::string& root_id) -> std::vector<OutlineEntry>;

    /// Publish OutlineScrollToEvent so the editor scrolls to the heading.
    void scroll_to_heading(const std::string& block_id);

    /// Find the heading closest above the given line number.
    [[nodiscard]] auto get_heading_at_line(const std::string& root_id, int line)
        -> std::optional<OutlineEntry>;

    /// Return the heading ancestry path for the given line (for breadcrumb).
    [[nodiscard]] auto get_breadcrumb_path(const std::string& root_id, int line)
        -> std::vector<OutlineEntry>;

    /// Mark the cached outline for a document as stale.
    void invalidate(const std::string& root_id);

    /// Filter outline entries by query string (case-insensitive substring).
    [[nodiscard]] auto search_headings(const std::string& root_id, const std::string& query)
        -> std::vector<OutlineEntry>;

    /// Handle new content: parse, rebuild outline, diff, emit event.
    void on_content_changed(const std::string& content, const std::string& root_id);

    /// Handle cursor movement: determine active heading, emit event if changed.
    void on_cursor_moved(int line, const std::string& root_id);

    /// Return heading statistics.
    [[nodiscard]] auto get_statistics(const std::string& root_id) -> DocumentOutline;

private:
    EventBus& event_bus_;
    Config& config_;
    OutlineBuilder builder_;

    mutable std::mutex cache_mutex_;
    std::unordered_map<std::string, DocumentOutline> outline_cache_;
    std::unordered_map<std::string, int> active_heading_line_;

    /// Flatten a vector of OutlineNodes recursively, populating entries.
    void flatten_nodes(const std::vector<OutlineNode>& nodes,
                       std::vector<OutlineEntry>& entries,
                       int& index) const;

    /// Find the deepest heading ancestry path containing the line.
    void find_ancestry(const std::vector<OutlineNode>& nodes,
                       int target_line,
                       std::vector<OutlineEntry>& path) const;
};

} // namespace markamp::core
