// ============================================================================
// File: src/core/ContentIndexer.h
// Phase 36: Asset Management — Content extraction and FTS indexing
// ============================================================================
#pragma once

#include "AssetTypes.h"

#include <expected>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;

// IContentExtractor — interface for extracting text from file formats.
class IContentExtractor
{
public:
    virtual ~IContentExtractor() = default;

    [[nodiscard]] virtual auto supported_mime_types() const -> std::vector<std::string> = 0;

    [[nodiscard]] virtual auto extract_text(const std::filesystem::path& file_path) const
        -> std::expected<std::string, std::string> = 0;
};

// ContentIndexer — extracts text from binary assets, indexes via FTS5.
class ContentIndexer
{
public:
    ContentIndexer(EventBus& event_bus);

    // Register a content extractor for specific MIME types.
    auto register_extractor(std::unique_ptr<IContentExtractor> extractor) -> void;

    // Index a single asset file.
    [[nodiscard]] auto index_asset(const std::string& asset_id,
                                   const std::filesystem::path& file_path,
                                   const std::string& mime_type)
        -> std::expected<ContentIndexEntry, std::string>;

    // Search indexed content.
    [[nodiscard]] auto search(const std::string& query, int limit = 50) const
        -> std::vector<ContentIndexEntry>;

    // Remove an asset from the index.
    auto remove_from_index(const std::string& asset_id) -> void;

    // Get index statistics.
    [[nodiscard]] auto indexed_count() const -> int32_t;

    // Check if a MIME type has a registered extractor.
    [[nodiscard]] auto can_extract(const std::string& mime_type) const -> bool;

    /// (#180) Return the number of registered content extractors.
    [[nodiscard]] auto extractor_count() const -> std::size_t;

    /// (#181) Return all MIME types with registered extractors.
    [[nodiscard]] auto supported_mime_types() const -> std::vector<std::string>;

    /// (#182) Check if the content index is empty.
    [[nodiscard]] auto is_empty() const -> bool;

private:
    EventBus& event_bus_;
    std::vector<std::unique_ptr<IContentExtractor>> extractors_;
    std::vector<ContentIndexEntry> index_; // In-memory index

    // Find extractor for a MIME type.
    [[nodiscard]] auto find_extractor(const std::string& mime_type) const -> IContentExtractor*;

    // Register built-in extractors.
    auto register_builtins() -> void;
};

// ── Built-in extractors ─────────────────────────────────────────────────────

// Extracts text from plain text files.
class PlainTextExtractor : public IContentExtractor
{
public:
    [[nodiscard]] auto supported_mime_types() const -> std::vector<std::string> override;
    [[nodiscard]] auto extract_text(const std::filesystem::path& file_path) const
        -> std::expected<std::string, std::string> override;
};

// Extracts text from Markdown files (strips formatting).
class MarkdownTextExtractor : public IContentExtractor
{
public:
    [[nodiscard]] auto supported_mime_types() const -> std::vector<std::string> override;
    [[nodiscard]] auto extract_text(const std::filesystem::path& file_path) const
        -> std::expected<std::string, std::string> override;
};

// Stub extractor for PDFs (requires external library).
class PdfTextExtractor : public IContentExtractor
{
public:
    [[nodiscard]] auto supported_mime_types() const -> std::vector<std::string> override;
    [[nodiscard]] auto extract_text(const std::filesystem::path& file_path) const
        -> std::expected<std::string, std::string> override;
};

} // namespace markamp::core
