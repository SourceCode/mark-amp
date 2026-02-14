# Phase 36 — Asset Management & Content Indexing

## Objective

Implement comprehensive asset management for the knowledgebase, handling images, PDFs, documents, audio, and video files attached to notebooks. This phase provides a complete asset lifecycle — upload, storage, retrieval, deduplication, renaming, and deletion — along with unused asset detection and cleanup. Additionally, it implements content indexing: extracting searchable text from binary assets (PDFs via poppler/mupdf, DOCX/PPTX/XLSX via XML parsing of their zip archives, and optionally images via Tesseract OCR) and storing extracted text in an FTS5-enabled SQLite table for full-text search.

Assets are central to any knowledge management system. Users embed images in documents, attach reference PDFs, link to spreadsheets, and include audio/video recordings. Without proper asset management, orphaned files accumulate, duplicate files waste storage, and binary file contents remain invisible to search. This phase addresses all three concerns: asset lifecycle management prevents orphans, SHA-256 hash-based deduplication prevents duplicates, and content extraction makes binary assets searchable.

The system is modeled after SiYuan's asset management in `kernel/model/asset.go` and `kernel/model/asset_content.go`, which maintain an Asset database table for metadata and an AssetContent table for extracted text. The OCR capability (from `kernel/model/ocr.go`) is optional and gated behind a configuration flag, as it requires the Tesseract library to be installed on the system.

## Prerequisites

- Phase 01 (Block Data Model Core)
- Phase 03 (Notebook Data Model)
- Phase 04 (SQLite Storage Layer)

## SiYuan Source Reference

- `kernel/model/asset.go` — UploadAssets, GetAssets, RemoveUnusedAssets, RenameAsset, GetAssetAbsPath, FullReindexAssetContent
- `kernel/model/asset_content.go` — AssetContent struct, IndexAssetContent, GetAssetContent, QueryAssetContent, asset_contents SQL table
- `kernel/model/ocr.go` — OcrAsset, OcrAllAssets, GetOcrContent — Tesseract OCR integration
- `kernel/model/upload.go` — InsertLocalAssets, Upload handler for file attachment
- `kernel/sql/asset.go` — Asset struct (id, box, docpath, name, title, hash), AssetContent struct (id, name, ext, path, size, updated, content)
- `kernel/sql/asset_content.go` — SQL schema for asset_contents FTS5 table

## MarkAmp Integration Points

- New header: `src/core/AssetService.h`
- New source: `src/core/AssetService.cpp`
- New header: `src/core/AssetTypes.h`
- New header: `src/core/ContentExtractor.h`
- New source: `src/core/ContentExtractor.cpp`
- Extends `Events.h` with asset events
- Asset files stored in `<workspace>/data/assets/` directory
- Asset metadata and content index stored in SQLite database (Phase 04)
- Integrates with Block model for scanning block content for asset references

## Data Structures to Implement

```cpp
namespace markamp::core
{

// MIME type categories for asset classification.
enum class AssetCategory : uint8_t
{
    Image,      // JPEG, PNG, GIF, WebP, SVG, BMP, ICO
    Document,   // PDF, DOCX, PPTX, XLSX, ODT, ODS, ODP
    Audio,      // MP3, WAV, OGG, FLAC, AAC, M4A
    Video,      // MP4, WebM, MKV, AVI, MOV
    Archive,    // ZIP, TAR, GZ, 7Z, RAR
    Code,       // Source code files attached as assets
    Other       // Unrecognized type
};

// Metadata record for a single asset file.
struct Asset
{
    std::string id;                              // Unique asset ID (SHA-256 hash prefix + timestamp)
    std::string box_id;                          // Notebook ID this asset belongs to
    std::string doc_path;                        // Path of the document referencing this asset
    std::string name;                            // Original filename (e.g., "photo.jpg")
    std::string title;                           // User-editable title/description
    std::string hash;                            // SHA-256 hash of file content
    std::string content_type;                    // MIME type (e.g., "image/jpeg")
    AssetCategory category{AssetCategory::Other};
    int64_t size_bytes{0};                       // File size in bytes
    int64_t created{0};                          // Upload time (Unix ms)
    int64_t updated{0};                          // Last modification time (Unix ms)
    std::filesystem::path absolute_path;         // Full filesystem path to the asset
    int32_t ref_count{0};                        // Number of blocks referencing this asset

    // Returns a human-readable file size string.
    [[nodiscard]] auto formatted_size() const -> std::string;

    // Returns the file extension (lowercase, without dot).
    [[nodiscard]] auto extension() const -> std::string;

    // Returns true if this asset is an image type.
    [[nodiscard]] auto is_image() const -> bool
    {
        return category == AssetCategory::Image;
    }

    // Returns true if this asset's content can be extracted for indexing.
    [[nodiscard]] auto is_indexable() const -> bool;

    // Returns true if no blocks reference this asset.
    [[nodiscard]] auto is_unused() const -> bool
    {
        return ref_count == 0;
    }
};

// Extracted text content from a binary asset for full-text search.
struct AssetContent
{
    std::string id;                              // Unique content record ID
    std::string asset_path;                      // Path to the source asset
    std::string asset_name;                      // Asset filename
    std::string extension;                       // File extension
    std::string content;                         // Extracted text content
    int64_t size_bytes{0};                       // Original asset file size
    int64_t updated{0};                          // When content was last extracted

    [[nodiscard]] auto is_empty() const -> bool
    {
        return content.empty();
    }

    // Truncate content to max_chars for preview display.
    [[nodiscard]] auto preview(int max_chars = 200) const -> std::string;
};

// Statistics from an asset cleanup operation.
struct AssetCleanupStats
{
    int32_t unused_found{0};                     // Number of unused assets found
    int32_t removed{0};                          // Number of assets actually removed
    int64_t bytes_freed{0};                      // Total disk space freed
    std::vector<std::string> removed_paths;      // Paths of removed assets
    std::vector<std::string> errors;             // Errors encountered during cleanup

    [[nodiscard]] auto formatted_freed() const -> std::string;
    [[nodiscard]] auto has_errors() const -> bool
    {
        return !errors.empty();
    }
};

// Result of a content extraction operation.
struct ExtractionResult
{
    std::string content;                         // Extracted text
    std::string extractor_name;                  // Which extractor was used
    int64_t elapsed_ms{0};                       // Extraction time
    bool success{false};
    std::string error_message;                   // Error if extraction failed
};

// Configuration for asset management.
struct AssetConfig
{
    std::filesystem::path assets_dir;            // Path to data/assets/ directory
    int64_t max_size_bytes{134217728};           // 128 MB default max upload size
    bool index_content{true};                    // Enable content extraction
    bool ocr_enabled{false};                     // Enable OCR for images
    std::string ocr_languages{"eng"};            // Tesseract language codes
    bool deduplicate{true};                      // Enable hash-based deduplication
};

// Interface for content extractors (one per file format).
class IContentExtractor
{
public:
    virtual ~IContentExtractor() = default;

    // Extract text content from the file at the given path.
    [[nodiscard]] virtual auto extract(const std::filesystem::path& file_path)
        -> ExtractionResult = 0;

    // File extensions this extractor handles (lowercase, without dot).
    [[nodiscard]] virtual auto supported_extensions() const
        -> std::vector<std::string> = 0;

    // Human-readable name of this extractor.
    [[nodiscard]] virtual auto name() const -> std::string = 0;

    // Whether this extractor is available (dependencies installed).
    [[nodiscard]] virtual auto is_available() const -> bool = 0;
};

// ContentExtractor — dispatches extraction to format-specific extractors.
class ContentExtractor
{
public:
    ContentExtractor();

    // Register a format-specific extractor.
    auto register_extractor(std::unique_ptr<IContentExtractor> extractor) -> void;

    // Extract text from a file, auto-detecting the appropriate extractor.
    [[nodiscard]] auto extract(const std::filesystem::path& file_path)
        -> ExtractionResult;

    // Check if extraction is supported for a given file extension.
    [[nodiscard]] auto supports_extension(const std::string& extension) const -> bool;

    // List all supported file extensions.
    [[nodiscard]] auto supported_extensions() const -> std::vector<std::string>;

private:
    std::vector<std::unique_ptr<IContentExtractor>> extractors_;

    // Find the appropriate extractor for a file extension.
    [[nodiscard]] auto find_extractor(const std::string& extension) const
        -> IContentExtractor*;

    // Register all built-in extractors.
    auto register_builtins() -> void;
};

// AssetService — manages asset lifecycle and content indexing.
class AssetService
{
public:
    AssetService(EventBus& event_bus, Config& config,
                 ContentExtractor& extractor);

    // Initialize the assets directory and content index.
    [[nodiscard]] auto initialize(const std::filesystem::path& workspace_path)
        -> std::expected<void, std::string>;

    // Upload a file as an asset, returning the created Asset record.
    // If deduplicate is enabled and the file hash matches an existing asset,
    // returns the existing asset instead of creating a duplicate.
    [[nodiscard]] auto upload_asset(const std::filesystem::path& file_path,
                                     const std::string& target_notebook)
        -> std::expected<Asset, std::string>;

    // Retrieve an asset by its path relative to the assets directory.
    [[nodiscard]] auto get_asset(const std::string& asset_path) const
        -> std::expected<Asset, std::string>;

    // List all assets in a notebook.
    [[nodiscard]] auto list_assets(const std::string& notebook_id) const
        -> std::vector<Asset>;

    // List all assets across all notebooks.
    [[nodiscard]] auto list_all_assets() const -> std::vector<Asset>;

    // Delete an asset file and its metadata.
    [[nodiscard]] auto delete_asset(const std::string& asset_path)
        -> std::expected<void, std::string>;

    // Rename an asset file and update all references in block content.
    [[nodiscard]] auto rename_asset(const std::string& old_path,
                                     const std::string& new_path)
        -> std::expected<void, std::string>;

    // Find assets with zero references from any block.
    [[nodiscard]] auto find_unused_assets() const -> std::vector<Asset>;

    // Delete all unused assets.
    [[nodiscard]] auto cleanup_unused_assets()
        -> std::expected<AssetCleanupStats, std::string>;

    // Get extracted text content for an asset.
    [[nodiscard]] auto get_asset_content(const std::string& asset_path) const
        -> std::expected<std::string, std::string>;

    // Re-extract and re-index content for an asset.
    [[nodiscard]] auto reindex_asset_content(const std::string& asset_path)
        -> std::expected<void, std::string>;

    // Re-index all asset content (full rebuild).
    [[nodiscard]] auto reindex_all_content()
        -> std::expected<int, std::string>;

    // Search asset content using FTS5 full-text search.
    [[nodiscard]] auto search_asset_content(const std::string& query) const
        -> std::vector<AssetContent>;

    // Get total disk usage of all assets.
    [[nodiscard]] auto total_asset_size() const -> int64_t;

    // Get total number of assets.
    [[nodiscard]] auto asset_count() const -> int;

private:
    EventBus& event_bus_;
    Config& config_;
    ContentExtractor& extractor_;
    AssetConfig asset_config_;
    std::filesystem::path workspace_path_;

    // Compute SHA-256 hash of a file.
    [[nodiscard]] auto compute_file_hash(const std::filesystem::path& path) const
        -> std::expected<std::string, std::string>;

    // Detect MIME type from file extension and/or magic bytes.
    [[nodiscard]] auto detect_content_type(const std::filesystem::path& path) const
        -> std::string;

    // Classify an asset by its MIME type.
    [[nodiscard]] auto classify_asset(const std::string& content_type) const
        -> AssetCategory;

    // Scan all block content for asset references, update ref_counts.
    auto update_reference_counts() -> void;

    // Find an existing asset with the same hash.
    [[nodiscard]] auto find_by_hash(const std::string& hash) const
        -> std::optional<Asset>;

    // Store asset metadata in SQLite.
    [[nodiscard]] auto store_asset_metadata(const Asset& asset)
        -> std::expected<void, std::string>;

    // Store extracted content in the asset_contents FTS5 table.
    [[nodiscard]] auto store_asset_content(const AssetContent& content)
        -> std::expected<void, std::string>;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. `AssetService::upload_asset()` — Validate file size against max_size_bytes. Compute SHA-256 hash. Check for duplicates if deduplication enabled (return existing asset if hash matches). Copy file to `data/assets/<notebook_id>/<filename>`, handling name collisions by appending a counter. Detect MIME type and category. Store metadata in SQLite. If content indexing is enabled and the file is indexable, extract content and store in FTS5 table. Publish AssetUploadedEvent.
2. `AssetService::find_unused_assets()` — Scan all block markdown/content fields for asset path references (pattern: `assets/...`). Build a set of referenced paths. Compare with all stored asset paths. Return assets whose paths are not in the referenced set.
3. `AssetService::cleanup_unused_assets()` — Call `find_unused_assets()`. For each unused asset, delete the file and metadata. Sum freed bytes. Publish AssetCleanupCompletedEvent. Return AssetCleanupStats.
4. `AssetService::rename_asset()` — Rename the file on disk using `std::filesystem::rename` with `std::error_code`. Update the metadata in SQLite. Scan all block content for old path references and replace with new path. Publish events.
5. `AssetService::compute_file_hash()` — Read file in chunks (64KB), feed to SHA-256 incremental hasher. Return hex-encoded hash string. Use `std::error_code` for file I/O.
6. `AssetService::update_reference_counts()` — Query all blocks, scan content/markdown fields for `assets/` path patterns, count occurrences per asset path, update ref_count in the asset metadata table.
7. `ContentExtractor::extract()` — Determine file extension, find registered extractor, call its `extract()` method. Return ExtractionResult with success/failure status.
8. `PdfExtractor::extract()` — Open PDF with poppler or mupdf library. Iterate pages, extract text from each page. Concatenate with page separators. Handle password-protected PDFs (return error).
9. `DocxExtractor::extract()` — Open DOCX as ZIP archive. Read `word/document.xml`. Parse XML, extract text content from `<w:t>` elements. Handle embedded images by ignoring them (text only).
10. `XlsxExtractor::extract()` — Open XLSX as ZIP. Read `xl/sharedStrings.xml` and `xl/worksheets/sheet*.xml`. Extract cell values. Format as tab-separated text.
11. `OcrExtractor::extract()` — Check if Tesseract is available. Load image. Run OCR with configured language. Return extracted text. Handle Tesseract errors gracefully.
12. `AssetService::search_asset_content()` — Execute FTS5 query against asset_contents table. Return matching AssetContent records ordered by rank.
13. `AssetService::detect_content_type()` — Check file extension mapping first (fast path). For ambiguous extensions, read first 8 bytes for magic number detection (JPEG: FF D8 FF, PNG: 89 50 4E 47, PDF: 25 50 44 46, ZIP/DOCX: 50 4B 03 04).
14. `Asset::is_indexable()` — Return true for categories Document and Image (if OCR enabled). Return false for Audio, Video, Archive, Code, Other.

## Events to Add (in Events.h)

```cpp
// ============================================================================
// Asset management events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetUploadedEvent)
std::string asset_path;
std::string content_type;
int64_t size_bytes{0};
bool deduplicated{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetDeletedEvent)
std::string asset_path;
int64_t size_bytes{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetRenamedEvent)
std::string old_path;
std::string new_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetContentIndexedEvent)
std::string asset_path;
int64_t content_length{0};
std::string extractor_name;
int64_t elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetCleanupCompletedEvent)
int32_t removed{0};
int64_t bytes_freed{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetReindexCompletedEvent)
int32_t indexed{0};
int32_t errors{0};
int64_t elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

- `knowledgebase.assets.path` — Relative path to assets directory within workspace (default: `"data/assets/"`)
- `knowledgebase.assets.max_size_mb` — Maximum upload file size in megabytes (default: `128`)
- `knowledgebase.assets.index_content` — Enable content extraction and full-text indexing of asset contents (default: `true`)
- `knowledgebase.assets.ocr_enabled` — Enable Tesseract OCR for image text extraction (default: `false`)
- `knowledgebase.assets.ocr_languages` — Comma-separated Tesseract language codes (default: `"eng"`)
- `knowledgebase.assets.deduplicate` — Enable SHA-256 hash-based deduplication (default: `true`)
- `knowledgebase.assets.auto_cleanup_on_doc_delete` — Automatically clean up unused assets when documents are deleted (default: `false`)

## Test Cases (Catch2)

File: `tests/unit/test_asset_service.cpp`

1. **Upload asset creates file and metadata** — Upload a test image file. Verify: asset file exists in `data/assets/` directory, Asset record has correct name, content_type, size_bytes, hash is non-empty, category is Image.
2. **Hash-based deduplication** — Upload the same file twice. Verify the second upload returns the existing Asset (same ID) instead of creating a duplicate. Verify only one file exists on disk.
3. **Detect unused assets** — Upload 3 assets. Create mock blocks referencing 2 of them. Call `find_unused_assets()`. Verify exactly 1 unused asset returned with ref_count == 0.
4. **Cleanup unused assets** — Upload 3 assets, 1 unreferenced. Call `cleanup_unused_assets()`. Verify stats: unused_found == 1, removed == 1, bytes_freed > 0. Verify the unused file is deleted. Verify the other 2 files remain.
5. **Rename asset updates references** — Upload an asset, create a mock block referencing it. Rename the asset. Verify: file exists at new path, old path does not exist, block content contains the new path.
6. **Delete asset removes file and metadata** — Upload an asset. Delete it. Verify: file does not exist on disk, `get_asset()` returns error, `asset_count()` decreased.
7. **Content extraction from plain text file** — Upload a `.txt` file with known content. Verify `get_asset_content()` returns the file's text content. Verify `search_asset_content("keyword")` finds it.
8. **MIME type detection** — Test detection for JPEG (image/jpeg), PNG (image/png), PDF (application/pdf), DOCX (application/vnd.openxmlformats-officedocument.wordprocessingml.document). Verify each returns correct MIME type.
9. **Asset category classification** — Verify classify_asset maps image/* to Image, application/pdf to Document, audio/* to Audio, video/* to Video. Verify unknown types map to Other.
10. **File size limit enforcement** — Configure max_size_mb to 1. Attempt to upload a file larger than 1 MB. Verify upload returns error. Verify no file was created in the assets directory.
11. **Full text search across assets** — Upload 3 text files with different content. Call `search_asset_content("specific-keyword")` where only one file contains that keyword. Verify exactly 1 result returned.
12. **Reindex all content** — Upload 3 indexable assets. Call `reindex_all_content()`. Verify returns 3 indexed count. Verify all 3 have non-empty content in the asset_contents table.

## Acceptance Criteria

- [ ] AssetService uploads files to `data/assets/<notebook>/` with correct metadata
- [ ] SHA-256 deduplication prevents duplicate file storage
- [ ] Unused asset detection correctly identifies assets with zero block references
- [ ] Cleanup removes unused assets and frees disk space
- [ ] Asset renaming updates both filesystem and block content references
- [ ] ContentExtractor dispatches to format-specific extractors
- [ ] PDF text extraction produces searchable text from PDF files
- [ ] DOCX/XLSX extraction parses Office Open XML format
- [ ] FTS5 full-text search works across asset content
- [ ] All 12 test cases pass

## Files to Create/Modify

- CREATE: `src/core/AssetTypes.h`
- CREATE: `src/core/AssetService.h`
- CREATE: `src/core/AssetService.cpp`
- CREATE: `src/core/ContentExtractor.h`
- CREATE: `src/core/ContentExtractor.cpp`
- MODIFY: `src/core/Events.h` (add AssetUploadedEvent, AssetDeletedEvent, AssetRenamedEvent, AssetContentIndexedEvent, AssetCleanupCompletedEvent, AssetReindexCompletedEvent)
- MODIFY: `src/core/PluginContext.h` (add `AssetService*` pointer)
- MODIFY: `src/app/MarkAmpApp.cpp` (instantiate ContentExtractor, AssetService, wire to PluginContext)
- MODIFY: `src/CMakeLists.txt` (add AssetService.cpp, ContentExtractor.cpp to source list; add poppler/mupdf, minizip/libzip link libraries)
- CREATE: `tests/unit/test_asset_service.cpp`
- MODIFY: `tests/CMakeLists.txt` (add test_asset_service target)
