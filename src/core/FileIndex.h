#pragma once

/// @file FileIndex.h
/// @brief Phase 35 – Pre-sorted file index with trigram filtering for fast search.

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// Cached metadata for an indexed file.
struct IndexedFile
{
    std::string file_path;
    std::string filename;       ///< Filename only.
    std::string lowercase_path; ///< Lowercased full path for fast matching.
    std::size_t file_size{0};
};

/// A pre-built file index for fast workplace file search.
/// Uses trigrams for O(1) initial candidate filtering.
class FileIndex
{
public:
    /// Build the index from a list of file paths.
    void build(const std::vector<std::string>& file_paths);

    /// Add a single file to the index.
    void add(const std::string& file_path);

    /// Remove a file from the index.
    void remove(const std::string& file_path);

    /// Search for files matching a query, using trigram filtering.
    /// Returns candidate file paths (not scored, just filtered).
    [[nodiscard]] auto search(const std::string& query, std::size_t limit = 500) const
        -> std::vector<std::string>;

    /// Get all indexed files.
    [[nodiscard]] auto all_files() const -> std::vector<std::string>;

    /// Number of indexed files.
    [[nodiscard]] auto size() const -> std::size_t;

    /// Clear the index.
    void clear();

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, IndexedFile> files_;

    /// Trigram index: trigram -> set of file paths containing it.
    std::unordered_map<std::string, std::unordered_set<std::string>> trigram_index_;

    /// Extract trigrams from a lowercase string.
    [[nodiscard]] static auto extract_trigrams(const std::string& text) -> std::vector<std::string>;

    /// Add trigrams for a file.
    void add_trigrams(const std::string& file_path, const std::string& lowercase_path);

    /// Remove trigrams for a file.
    void remove_trigrams(const std::string& file_path, const std::string& lowercase_path);

    /// Convert to lowercase.
    [[nodiscard]] static auto to_lower(const std::string& text) -> std::string;
};

} // namespace markamp::core
