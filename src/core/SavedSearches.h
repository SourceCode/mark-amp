#pragma once

/// @file SavedSearches.h
/// @brief V9 Phase 10 – Saved search management with persistence.
///
/// Header-only implementation of saved search system:
///   - Named saved searches with pinning and color labels
///   - Capacity cap at 50 (oldest non-pinned evicted)
///   - Sorting by name, creation date, last used, or pinned-first
///   - Serialize/deserialize for persistence
///   - Prefix-based search by name

#include "Search.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace markamp::core
{

// ────────────────────────────────────────────────────────────
// Sort Order
// ────────────────────────────────────────────────────────────

/// Sort order for saved searches.
enum class SavedSearchSortOrder : std::uint8_t
{
    kName,     ///< Alphabetical by name
    kCreated,  ///< By creation date (newest first)
    kLastUsed, ///< By last used date (most recent first)
    kPinned    ///< Pinned first, then by name
};

// ────────────────────────────────────────────────────────────
// SavedSearch
// ────────────────────────────────────────────────────────────

/// A named, persisted search query.
struct SavedSearch
{
    /// Display name for the saved search.
    std::string name;

    /// The underlying search query.
    SearchQuery query;

    /// Optional description.
    std::string description;

    /// Optional color label for visual grouping (e.g., "red", "blue", "#FF0000").
    std::string color_label;

    /// Whether this search is pinned (protected from eviction).
    bool is_pinned{false};

    /// When this search was created.
    std::chrono::system_clock::time_point created_at;

    /// When this search was last executed.
    std::chrono::system_clock::time_point last_used_at;
};

// ────────────────────────────────────────────────────────────
// SavedSearchManager
// ────────────────────────────────────────────────────────────

/// Manages a collection of saved searches with persistence.
class SavedSearchManager
{
public:
    /// Maximum number of saved searches.
    static constexpr std::size_t kMaxSavedSearches = 50;

    SavedSearchManager() = default;

    // ── Core Operations ──────────────────────────────────────

    /// Add a new saved search. Returns false if name already exists.
    auto add(const std::string& search_name, const SearchQuery& query) -> bool
    {
        if (search_name.empty())
        {
            return false;
        }

        // Check for duplicate name
        if (find_index(search_name).has_value())
        {
            return false;
        }

        // Enforce capacity — evict oldest non-pinned if at limit
        if (searches_.size() >= kMaxSavedSearches)
        {
            evict_oldest_non_pinned();
            // If still at capacity (all pinned), reject
            if (searches_.size() >= kMaxSavedSearches)
            {
                return false;
            }
        }

        const auto now = std::chrono::system_clock::now();

        SavedSearch saved;
        saved.name = search_name;
        saved.query = query;
        saved.created_at = now;
        saved.last_used_at = now;

        searches_.push_back(std::move(saved));
        return true;
    }

    /// Remove a saved search by name. Returns false if not found.
    auto remove(const std::string& search_name) -> bool
    {
        auto idx = find_index(search_name);
        if (!idx.has_value())
        {
            return false;
        }
        searches_.erase(searches_.begin() + static_cast<std::ptrdiff_t>(idx.value()));
        return true;
    }

    /// Rename a saved search. Returns false if old name not found
    /// or new name already exists.
    auto rename(const std::string& old_name, const std::string& new_name) -> bool
    {
        if (new_name.empty())
        {
            return false;
        }

        auto old_idx = find_index(old_name);
        if (!old_idx.has_value())
        {
            return false;
        }

        // Check that new name doesn't conflict
        if (old_name != new_name && find_index(new_name).has_value())
        {
            return false;
        }

        searches_[old_idx.value()].name = new_name;
        return true;
    }

    /// Toggle the pinned state of a saved search.
    auto toggle_pin(const std::string& search_name) -> bool
    {
        auto idx = find_index(search_name);
        if (!idx.has_value())
        {
            return false;
        }
        searches_[idx.value()].is_pinned = !searches_[idx.value()].is_pinned;
        return true;
    }

    /// Set the description for a saved search.
    auto set_description(const std::string& search_name, const std::string& desc) -> bool
    {
        auto idx = find_index(search_name);
        if (!idx.has_value())
        {
            return false;
        }
        searches_[idx.value()].description = desc;
        return true;
    }

    /// Set the color label for a saved search.
    auto set_color_label(const std::string& search_name, const std::string& color) -> bool
    {
        auto idx = find_index(search_name);
        if (!idx.has_value())
        {
            return false;
        }
        searches_[idx.value()].color_label = color;
        return true;
    }

    /// Mark a saved search as recently used (updates last_used_at).
    void mark_used(const std::string& search_name)
    {
        auto idx = find_index(search_name);
        if (idx.has_value())
        {
            searches_[idx.value()].last_used_at = std::chrono::system_clock::now();
        }
    }

    // ── Accessors ────────────────────────────────────────────

    /// Get all saved searches.
    [[nodiscard]] auto get_all() const -> const std::vector<SavedSearch>&
    {
        return searches_;
    }

    /// Get the number of saved searches.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return searches_.size();
    }

    /// Check if empty.
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return searches_.empty();
    }

    /// Find a saved search by name.
    [[nodiscard]] auto find_by_name(const std::string& search_name) const
        -> std::optional<SavedSearch>
    {
        auto idx = find_index(search_name);
        if (!idx.has_value())
        {
            return std::nullopt;
        }
        return searches_[idx.value()];
    }

    // ── Filtering & Sorting ──────────────────────────────────

    /// Get only pinned searches.
    [[nodiscard]] auto get_pinned() const -> std::vector<SavedSearch>
    {
        std::vector<SavedSearch> result;
        for (const auto& saved : searches_)
        {
            if (saved.is_pinned)
            {
                result.push_back(saved);
            }
        }
        return result;
    }

    /// Get the N most recently used searches.
    [[nodiscard]] auto get_recent(std::size_t limit) const -> std::vector<SavedSearch>
    {
        auto sorted = searches_;
        std::sort(sorted.begin(),
                  sorted.end(),
                  [](const SavedSearch& lhs, const SavedSearch& rhs)
                  { return lhs.last_used_at > rhs.last_used_at; });

        if (sorted.size() > limit)
        {
            sorted.resize(limit);
        }
        return sorted;
    }

    /// Search saved searches by name prefix (case-insensitive).
    [[nodiscard]] auto search_by_name(const std::string& prefix) const -> std::vector<SavedSearch>
    {
        if (prefix.empty())
        {
            return searches_;
        }

        const auto lower_prefix = to_lower(prefix);
        std::vector<SavedSearch> matches;

        for (const auto& saved : searches_)
        {
            if (to_lower(saved.name).find(lower_prefix) != std::string::npos)
            {
                matches.push_back(saved);
            }
        }

        return matches;
    }

    /// Sort saved searches by the specified order.
    void sort_by(SavedSearchSortOrder order)
    {
        switch (order)
        {
            case SavedSearchSortOrder::kName:
                std::sort(searches_.begin(),
                          searches_.end(),
                          [](const SavedSearch& lhs, const SavedSearch& rhs)
                          { return lhs.name < rhs.name; });
                break;

            case SavedSearchSortOrder::kCreated:
                std::sort(searches_.begin(),
                          searches_.end(),
                          [](const SavedSearch& lhs, const SavedSearch& rhs)
                          { return lhs.created_at > rhs.created_at; });
                break;

            case SavedSearchSortOrder::kLastUsed:
                std::sort(searches_.begin(),
                          searches_.end(),
                          [](const SavedSearch& lhs, const SavedSearch& rhs)
                          { return lhs.last_used_at > rhs.last_used_at; });
                break;

            case SavedSearchSortOrder::kPinned:
                std::sort(searches_.begin(),
                          searches_.end(),
                          [](const SavedSearch& lhs, const SavedSearch& rhs)
                          {
                              if (lhs.is_pinned != rhs.is_pinned)
                              {
                                  return lhs.is_pinned;
                              }
                              return lhs.name < rhs.name;
                          });
                break;
        }
    }

    // ── Persistence ──────────────────────────────────────────

    /// Serialize all saved searches to a string format.
    [[nodiscard]] auto serialize() const -> std::string
    {
        std::ostringstream oss;
        oss << "SAVED_SEARCHES_V1\n";
        oss << "count=" << searches_.size() << "\n";

        for (const auto& saved : searches_)
        {
            const auto created_epoch = std::chrono::duration_cast<std::chrono::seconds>(
                                           saved.created_at.time_since_epoch())
                                           .count();
            const auto last_used_epoch = std::chrono::duration_cast<std::chrono::seconds>(
                                             saved.last_used_at.time_since_epoch())
                                             .count();

            // Line format:
            // name \t query_string \t method \t case_sensitive \t
            // is_pinned \t description \t color_label \t created \t last_used
            oss << escape_field(saved.name) << "\t" << escape_field(saved.query.query_string)
                << "\t" << static_cast<int>(saved.query.method) << "\t"
                << (saved.query.case_sensitive ? 1 : 0) << "\t" << (saved.is_pinned ? 1 : 0) << "\t"
                << escape_field(saved.description) << "\t" << escape_field(saved.color_label)
                << "\t" << created_epoch << "\t" << last_used_epoch << "\n";
        }

        return oss.str();
    }

    /// Deserialize saved searches from a string.
    /// Returns true if deserialization was successful.
    auto deserialize(const std::string& data) -> bool
    {
        searches_.clear();

        if (data.empty())
        {
            return false;
        }

        std::istringstream iss(data);
        std::string line;

        // Check header
        if (!std::getline(iss, line) || line != "SAVED_SEARCHES_V1")
        {
            return false;
        }

        // Read count
        if (!std::getline(iss, line) || line.find("count=") != 0)
        {
            return false;
        }

        int count = 0;
        try
        {
            count = std::stoi(line.substr(6));
        }
        catch (...)
        {
            return false;
        }

        // Read entries
        for (int idx = 0; idx < count && std::getline(iss, line); ++idx)
        {
            auto saved = parse_saved_line(line);
            if (saved.has_value())
            {
                searches_.push_back(std::move(saved.value()));
            }
        }

        return true;
    }

private:
    std::vector<SavedSearch> searches_;

    /// Find the index of a saved search by name.
    [[nodiscard]] auto find_index(const std::string& search_name) const
        -> std::optional<std::size_t>
    {
        for (std::size_t idx = 0; idx < searches_.size(); ++idx)
        {
            if (searches_[idx].name == search_name)
            {
                return idx;
            }
        }
        return std::nullopt;
    }

    /// Evict the oldest non-pinned search.
    void evict_oldest_non_pinned()
    {
        std::optional<std::size_t> oldest_idx;
        auto oldest_time = std::chrono::system_clock::time_point::max();

        for (std::size_t idx = 0; idx < searches_.size(); ++idx)
        {
            if (!searches_[idx].is_pinned && searches_[idx].last_used_at < oldest_time)
            {
                oldest_time = searches_[idx].last_used_at;
                oldest_idx = idx;
            }
        }

        if (oldest_idx.has_value())
        {
            searches_.erase(searches_.begin() + static_cast<std::ptrdiff_t>(oldest_idx.value()));
        }
    }

    /// Case-insensitive helper.
    [[nodiscard]] static auto to_lower(const std::string& str) -> std::string
    {
        std::string result = str;
        std::transform(result.begin(),
                       result.end(),
                       result.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
        return result;
    }

    /// Escape field for tab-separated serialization.
    [[nodiscard]] static auto escape_field(const std::string& str) -> std::string
    {
        std::string result;
        result.reserve(str.size());
        for (char chr : str)
        {
            if (chr == '\t')
            {
                result += "\\t";
            }
            else if (chr == '\\')
            {
                result += "\\\\";
            }
            else if (chr == '\n')
            {
                result += "\\n";
            }
            else
            {
                result += chr;
            }
        }
        return result;
    }

    /// Unescape field from tab-separated serialization.
    [[nodiscard]] static auto unescape_field(const std::string& str) -> std::string
    {
        std::string result;
        result.reserve(str.size());
        for (std::size_t idx = 0; idx < str.size(); ++idx)
        {
            if (str[idx] == '\\' && idx + 1 < str.size())
            {
                if (str[idx + 1] == 't')
                {
                    result += '\t';
                    ++idx;
                }
                else if (str[idx + 1] == '\\')
                {
                    result += '\\';
                    ++idx;
                }
                else if (str[idx + 1] == 'n')
                {
                    result += '\n';
                    ++idx;
                }
                else
                {
                    result += str[idx];
                }
            }
            else
            {
                result += str[idx];
            }
        }
        return result;
    }

    /// Split a line by tab delimiter.
    [[nodiscard]] static auto split_tabs(const std::string& line) -> std::vector<std::string>
    {
        std::vector<std::string> fields;
        std::size_t start = 0;
        for (std::size_t pos = 0; pos <= line.size(); ++pos)
        {
            if (pos == line.size() || line[pos] == '\t')
            {
                fields.push_back(line.substr(start, pos - start));
                start = pos + 1;
            }
        }
        return fields;
    }

    /// Parse a single serialized saved search line.
    [[nodiscard]] static auto parse_saved_line(const std::string& line)
        -> std::optional<SavedSearch>
    {
        auto fields = split_tabs(line);
        if (fields.size() < 9)
        {
            return std::nullopt;
        }

        SavedSearch saved;
        saved.name = unescape_field(fields[0]);
        saved.query.query_string = unescape_field(fields[1]);

        try
        {
            saved.query.method = static_cast<SearchMethod>(std::stoi(fields[2]));
            saved.query.case_sensitive = (std::stoi(fields[3]) != 0);
            saved.is_pinned = (std::stoi(fields[4]) != 0);
            saved.description = unescape_field(fields[5]);
            saved.color_label = unescape_field(fields[6]);

            const auto created_secs = std::stoll(fields[7]);
            const auto last_used_secs = std::stoll(fields[8]);

            saved.created_at =
                std::chrono::system_clock::time_point(std::chrono::seconds(created_secs));
            saved.last_used_at =
                std::chrono::system_clock::time_point(std::chrono::seconds(last_used_secs));
        }
        catch (...)
        {
            return std::nullopt;
        }

        return saved;
    }
};

} // namespace markamp::core
