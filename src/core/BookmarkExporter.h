/// @file BookmarkExporter.h
/// @brief V9 Phase 37 — Bookmark export/import in multiple formats.
#pragma once

#include "Bookmark.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Export format for bookmarks.
enum class BookmarkExportFormat
{
    kMarkdown,
    kJson,
    kCsv,
    kOpml
};

/// Result of a bookmark import.
struct BookmarkImportResult
{
    std::vector<BookmarkEntry> entries;
    int imported_count{0};
    int skipped_count{0};
    std::string error; // Non-empty if the import failed
};

/// Exports and imports bookmarks in multiple formats.
class BookmarkExporter
{
public:
    BookmarkExporter() = default;

    /// Export all bookmark groups to a string in the specified format.
    [[nodiscard]] auto export_bookmarks(const std::vector<BookmarkGroup>& groups,
                                        BookmarkExportFormat format) const -> std::string;

    /// Export a single label group to a string in the specified format.
    [[nodiscard]] auto export_label(const BookmarkGroup& group, BookmarkExportFormat format) const
        -> std::string;

    /// Import bookmarks from a string in the specified format.
    [[nodiscard]] auto import_bookmarks(const std::string& data, BookmarkExportFormat format) const
        -> BookmarkImportResult;

    /// Get a human-readable format name.
    [[nodiscard]] static auto format_name(BookmarkExportFormat format) -> std::string;

    /// Get the file extension for a format (without dot).
    [[nodiscard]] static auto format_extension(BookmarkExportFormat format) -> std::string;

private:
    // Format-specific exporters
    [[nodiscard]] auto export_markdown(const std::vector<BookmarkGroup>& groups) const
        -> std::string;
    [[nodiscard]] auto export_json(const std::vector<BookmarkGroup>& groups) const -> std::string;
    [[nodiscard]] auto export_csv(const std::vector<BookmarkGroup>& groups) const -> std::string;
    [[nodiscard]] auto export_opml(const std::vector<BookmarkGroup>& groups) const -> std::string;

    // Format-specific importers
    [[nodiscard]] auto import_json(const std::string& data) const -> BookmarkImportResult;
    [[nodiscard]] auto import_csv(const std::string& data) const -> BookmarkImportResult;
};

} // namespace markamp::core
