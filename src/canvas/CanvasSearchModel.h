#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Search result entry.
struct SearchResult
{
    std::string object_id;
    std::string label;
    std::string object_type;
    double center_x{0.0};
    double center_y{0.0};

    // ── Round 4 Batch 2 (#16-17) ────────────────────────────────

    /// (#16) Whether an object ID is set.
    [[nodiscard]] auto has_object_id() const noexcept -> bool
    {
        return !object_id.empty();
    }

    /// (#17) Whether a label is set.
    [[nodiscard]] auto has_label() const noexcept -> bool
    {
        return !label.empty();
    }
};

/// Filter chip.
struct FilterChip
{
    std::string key;
    std::string value;
    bool active{true};

    // ── Round 4 Batch 2 (#18-19) ────────────────────────────────

    /// (#18) Whether this filter is active.
    [[nodiscard]] auto is_active() const noexcept -> bool
    {
        return active;
    }

    /// (#19) Whether a key is set.
    [[nodiscard]] auto has_key() const noexcept -> bool
    {
        return !key.empty();
    }
};

/// Saved search preset.
struct SavedSearch
{
    std::string preset_name;
    std::string query;
    std::vector<FilterChip> filters;

    // ── Round 4 Batch 2-3 (#20-23) ──────────────────────────────

    /// (#20) Whether a preset name is set.
    [[nodiscard]] auto has_name() const noexcept -> bool
    {
        return !preset_name.empty();
    }

    /// (#21) Whether a query is set.
    [[nodiscard]] auto has_query() const noexcept -> bool
    {
        return !query.empty();
    }

    /// (#22) Whether filters are attached.
    [[nodiscard]] auto has_filters() const noexcept -> bool
    {
        return !filters.empty();
    }

    /// (#23) Number of filters.
    [[nodiscard]] auto filter_count() const noexcept -> size_t
    {
        return filters.size();
    }
};

/// Testable model for Canvas Search & Discovery (Phase 63).
///
/// Encapsulates:
/// - Search query with result list
/// - Filter chips by type/tag
/// - Jump-to-result (highlight object, center viewport)
/// - Saved search views
/// - Query history
class CanvasSearchModel
{
public:
    // ── Query ───────────────────────────────────────────────────────

    void set_query(const std::string& query);
    [[nodiscard]] auto query() const -> const std::string&;

    void set_results(std::vector<SearchResult> results);
    [[nodiscard]] auto results() const -> const std::vector<SearchResult>&;
    [[nodiscard]] auto result_count() const -> int;

    // ── Filters ─────────────────────────────────────────────────────

    void add_filter(FilterChip chip);
    void remove_filter(const std::string& key);
    void clear_filters();
    [[nodiscard]] auto active_filters() const -> std::vector<FilterChip>;

    // ── Jump ────────────────────────────────────────────────────────

    void set_highlighted(const std::string& object_id);
    [[nodiscard]] auto highlighted() const -> const std::string&;

    // ── Saved searches ──────────────────────────────────────────────

    void save_search(SavedSearch preset);
    void remove_saved(const std::string& preset_name);
    [[nodiscard]] auto saved_searches() const -> const std::vector<SavedSearch>&;

    // ── History ─────────────────────────────────────────────────────

    [[nodiscard]] auto query_history() const -> const std::vector<std::string>&;

private:
    std::string query_;
    std::vector<SearchResult> results_;
    std::vector<FilterChip> filters_;
    std::string highlighted_;
    std::vector<SavedSearch> saved_searches_;
    std::vector<std::string> query_history_;
    static constexpr int kMaxHistory = 10;

    // ── Round 4 Batch 3 (#24-28) ────────────────────────────────

    /// (#24) Whether a search query is set.
    [[nodiscard]] auto has_query() const noexcept -> bool
    {
        return !query_.empty();
    }

    /// (#25) Whether results are available.
    [[nodiscard]] auto has_results() const noexcept -> bool
    {
        return !results_.empty();
    }

    /// (#26) Whether an object is highlighted.
    [[nodiscard]] auto has_highlighted() const noexcept -> bool
    {
        return !highlighted_.empty();
    }

    /// (#27) Whether saved searches exist.
    [[nodiscard]] auto has_saved() const noexcept -> bool
    {
        return !saved_searches_.empty();
    }

    /// (#28) Whether query history exists.
    [[nodiscard]] auto has_history() const noexcept -> bool
    {
        return !query_history_.empty();
    }
};

} // namespace markamp::canvas
