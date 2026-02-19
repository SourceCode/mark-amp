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
};

/// Filter chip.
struct FilterChip
{
    std::string key;
    std::string value;
    bool active{true};
};

/// Saved search preset.
struct SavedSearch
{
    std::string preset_name;
    std::string query;
    std::vector<FilterChip> filters;
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
};

} // namespace markamp::canvas
