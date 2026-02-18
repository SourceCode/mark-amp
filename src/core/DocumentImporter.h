/// @file DocumentImporter.h
/// @brief V9 Phase 42 — Multi-format document import with conversion to Markdown.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Supported import formats.
enum class ImportFormat : uint8_t
{
    kDocx = 0,  ///< Microsoft Word
    kHtml = 1,  ///< HTML documents
    kTxt = 2,   ///< Plain text
    kCsv = 3,   ///< CSV spreadsheets
    kRtf = 4,   ///< Rich Text Format
    kEpub = 5,  ///< EPUB e-books
    kLatex = 6, ///< LaTeX source
    kOrg = 7,   ///< Org-mode
};

/// Status of an import operation.
enum class ImportStatus : uint8_t
{
    kPending = 0,
    kInProgress = 1,
    kCompleted = 2,
    kFailed = 3,
    kCancelled = 4,
};

/// Convert format enum to display name.
[[nodiscard]] auto import_format_name(ImportFormat format) -> std::string;

/// Result of a single import operation.
struct ImportResult
{
    std::string import_id;
    std::string source_path;
    std::string output_path;
    ImportFormat format{ImportFormat::kTxt};
    ImportStatus status{ImportStatus::kPending};
    std::string error_message;
    std::string converted_markdown; ///< Result markdown content
    int source_size_bytes{0};
    int output_size_bytes{0};
    std::chrono::system_clock::time_point started_at;
    std::chrono::system_clock::time_point completed_at;
};

/// Options for an import operation.
struct ImportOptions
{
    bool preserve_formatting{true};
    bool extract_images{true};
    bool convert_tables{true};
    bool include_metadata{false};
    std::string output_directory;
    std::string filename_template; ///< e.g. "{title}.md"
};

/// Service for importing documents from various formats into Markdown.
///
/// Handles format detection, conversion, and tracking of import operations.
class DocumentImporter
{
public:
    DocumentImporter() = default;

    // ── Import operations ─────────────────────────────────────────────
    auto import_file(const std::string& file_path,
                     ImportFormat format,
                     const ImportOptions& options = {}) -> std::string;
    auto cancel_import(const std::string& import_id) -> bool;

    // ── Status ────────────────────────────────────────────────────────
    [[nodiscard]] auto find_import(const std::string& import_id) const -> const ImportResult*;
    [[nodiscard]] auto import_count() const -> int;
    [[nodiscard]] auto pending_count() const -> int;
    [[nodiscard]] auto completed_count() const -> int;

    // ── Format detection ──────────────────────────────────────────────
    [[nodiscard]] static auto detect_format(const std::string& file_path) -> ImportFormat;
    [[nodiscard]] static auto supported_extensions() -> std::vector<std::string>;

    // ── History ───────────────────────────────────────────────────────
    [[nodiscard]] auto all_imports() const -> std::vector<const ImportResult*>;
    [[nodiscard]] auto imports_by_status(ImportStatus status) const
        -> std::vector<const ImportResult*>;
    void clear_history();

private:
    std::vector<ImportResult> imports_;
    int next_id_{1};

    auto find_mut(const std::string& import_id) -> ImportResult*;
    static auto convert_to_markdown(const std::string& file_path,
                                    ImportFormat format,
                                    const ImportOptions& options) -> std::string;
};

} // namespace markamp::core
