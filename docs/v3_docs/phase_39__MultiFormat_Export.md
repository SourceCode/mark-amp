# Phase 39 — Multi-Format Export Engine (20+ Formats)

## Objective

Implement comprehensive document export capabilities, porting SiYuan's multi-format export system. Users can export individual documents or entire notebooks to 15+ output formats including Markdown, HTML, PDF, DOCX, EPUB, RTF, ODT, LaTeX, Textile, Org-Mode, AsciiDoc, MediaWiki, reStructuredText, OPML, plain text, and PNG image. The system uses a plugin-based architecture with an IExportFormat interface, allowing new export formats to be added by registering a single class.

Export formats are divided into three tiers. Built-in exporters (no external dependencies) handle Markdown, HTML, and plain text natively in C++. Pandoc-based exporters leverage the Pandoc universal document converter for DOCX, EPUB, RTF, ODT, LaTeX, Textile, Org-Mode, AsciiDoc, MediaWiki, reStructuredText, and OPML — these require Pandoc to be installed on the user's system. Specialized exporters handle PDF (via wxWidgets print framework or wkhtmltopdf) and PNG (via rendering the preview panel to a bitmap). The ExportService orchestrates the full export pipeline: resolving block references, gathering assets, applying themes, and dispatching to the appropriate format exporter.

The export dialog (wxDialog) provides a format selector dropdown, per-format options panel (e.g., page size for PDF, embed images for HTML), output path file picker, a progress bar for long exports, and an optional preview pane showing a sample of the export output. Notebook-level export produces a ZIP archive containing all documents in the selected format plus their associated assets.

## Prerequisites

- Phase 01 (Block Data Model Core)
- Phase 03 (Notebook Data Model)
- Phase 36 (Asset Management — for asset embedding/inclusion)

## SiYuan Source Reference

- `kernel/model/export.go` — ExportMarkdown, ExportHTML, ExportDocx, ExportEPUB, ExportPDF, Export2Liandi, ExportResources, ExportNotebookSY, ExportBlockRef
- `kernel/model/render.go` — Block tree to markdown/HTML conversion used by exporters
- `kernel/model/attribute_view.go` — Export of attribute view (database) data
- `app/src/protyle/export/` — Frontend export UI and format selection

## MarkAmp Integration Points

- New header: `src/core/ExportTypes.h`
- New header: `src/core/IExportFormat.h`
- New header: `src/core/ExportService.h`
- New source: `src/core/ExportService.cpp`
- New header: `src/core/MarkdownExporter.h`
- New source: `src/core/MarkdownExporter.cpp`
- New header: `src/core/HtmlExporter.h`
- New source: `src/core/HtmlExporter.cpp`
- New header: `src/core/PandocExporter.h`
- New source: `src/core/PandocExporter.cpp`
- New header: `src/core/PdfExporter.h`
- New source: `src/core/PdfExporter.cpp`
- New header: `src/ui/ExportDialog.h`
- New source: `src/ui/ExportDialog.cpp`
- Extends `Events.h` with export events
- Connects to Block model (Phase 01) and Asset system (Phase 36)
- ExportService added to PluginContext for extension access

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Supported export format identifiers.
// Each maps to a concrete IExportFormat implementation.
enum class ExportFormatId : uint8_t
{
    Markdown,           // Clean Markdown with resolved refs
    HTML,               // Standalone HTML with embedded CSS/images
    PlainText,          // Plain text, no markup
    PDF,                // PDF document
    DOCX,               // Microsoft Word
    EPUB,               // Electronic publication
    RTF,                // Rich Text Format
    ODT,                // OpenDocument Text
    LaTeX,              // LaTeX source
    Textile,            // Textile markup
    OrgMode,            // Emacs Org-Mode
    AsciiDoc,           // AsciiDoc markup
    MediaWiki,          // MediaWiki markup
    ReStructuredText,   // reStructuredText markup
    OPML,               // Outline Processor Markup Language
    PNG,                // Image capture of rendered preview
};

// Metadata describing an export format.
struct FormatInfo
{
    ExportFormatId id{ExportFormatId::Markdown};
    std::string format_id;                       // String ID (e.g., "markdown", "html")
    std::string format_name;                     // Display name (e.g., "Markdown", "HTML")
    std::string file_extension;                  // File extension without dot (e.g., "md", "html")
    std::string description;                     // Brief description of the format
    bool requires_pandoc{false};                 // Whether Pandoc is needed
    bool supports_batch{true};                   // Whether notebook export is supported
    bool supports_assets{true};                  // Whether assets can be embedded/included

    [[nodiscard]] auto dotted_extension() const -> std::string
    {
        return "." + file_extension;
    }
};

// Options controlling export behavior.
struct ExportOptions
{
    bool include_assets{true};                   // Copy/embed associated assets
    bool embed_images{false};                    // Base64-embed images (for HTML)
    bool include_toc{false};                     // Generate table of contents
    bool include_frontmatter{false};             // Include YAML frontmatter
    std::string theme_id;                        // Theme for styled exports (HTML/PDF)
    std::string header_text;                     // Header text for PDF
    std::string footer_text;                     // Footer text for PDF
    std::string css_path;                        // Custom CSS file path (for HTML/PDF)

    // PDF-specific options
    std::string page_size{"A4"};                 // Page size: "A4", "Letter", "Legal"
    std::string orientation{"portrait"};         // "portrait" or "landscape"
    int margin_top_mm{20};                       // Top margin in mm
    int margin_bottom_mm{20};                    // Bottom margin in mm
    int margin_left_mm{25};                      // Left margin in mm
    int margin_right_mm{25};                     // Right margin in mm

    // EPUB-specific options
    std::string epub_title;                      // Override title for EPUB metadata
    std::string epub_author;                     // Author for EPUB metadata
    std::string epub_language{"en"};             // Language code

    // Markdown-specific options
    bool resolve_block_refs{true};               // Replace ((ref)) with content
    bool strip_ial{true};                        // Remove IAL annotations
};

// Result of an export operation.
struct ExportResult
{
    std::filesystem::path output_path;           // Path to the exported file
    std::string format_id;                       // Format that was used
    std::string file_name;                       // Name of the exported file
    int64_t size_bytes{0};                       // Size of the exported file
    int64_t elapsed_ms{0};                       // Export time
    int32_t pages{0};                            // Page count (for PDF)
    int32_t images_embedded{0};                  // Number of images embedded
    int32_t assets_copied{0};                    // Number of asset files copied
    std::vector<std::string> warnings;           // Non-fatal warnings

    [[nodiscard]] auto formatted_size() const -> std::string;
    [[nodiscard]] auto has_warnings() const -> bool
    {
        return !warnings.empty();
    }
};

// IExportFormat — interface for format-specific export implementations.
class IExportFormat
{
public:
    virtual ~IExportFormat() = default;

    // Get metadata about this format.
    [[nodiscard]] virtual auto format_info() const -> FormatInfo = 0;

    // Export a single document by its root block ID.
    [[nodiscard]] virtual auto export_document(
        const std::string& root_id,
        const std::filesystem::path& output_path,
        const ExportOptions& options)
        -> std::expected<ExportResult, std::string> = 0;

    // Export raw markdown content (for formats that work from markdown source).
    [[nodiscard]] virtual auto export_content(
        const std::string& markdown_content,
        const std::filesystem::path& output_path,
        const ExportOptions& options)
        -> std::expected<ExportResult, std::string> = 0;

    // Whether this format supports batch/notebook export.
    [[nodiscard]] virtual auto supports_batch() const -> bool
    {
        return true;
    }

    // Whether this format is currently available (dependencies present).
    [[nodiscard]] virtual auto is_available() const -> bool = 0;
};

// ExportService — manages export format registry and orchestrates exports.
class ExportService
{
public:
    ExportService(EventBus& event_bus, Config& config);

    // Register a format exporter.
    auto register_exporter(std::unique_ptr<IExportFormat> exporter) -> void;

    // Export a single document to a specified format.
    [[nodiscard]] auto export_document(
        const std::string& root_id,
        ExportFormatId format,
        const std::filesystem::path& output_path,
        const ExportOptions& options = {})
        -> std::expected<ExportResult, std::string>;

    // Export an entire notebook to a specified format (produces ZIP archive).
    [[nodiscard]] auto export_notebook(
        const std::string& notebook_id,
        ExportFormatId format,
        const std::filesystem::path& output_path,
        const ExportOptions& options = {})
        -> std::expected<ExportResult, std::string>;

    // Export raw markdown content to a specified format.
    [[nodiscard]] auto export_content(
        const std::string& markdown_content,
        ExportFormatId format,
        const std::filesystem::path& output_path,
        const ExportOptions& options = {})
        -> std::expected<ExportResult, std::string>;

    // Get list of all registered and available export formats.
    [[nodiscard]] auto get_available_formats() const
        -> std::vector<FormatInfo>;

    // Get list of all registered formats (including unavailable).
    [[nodiscard]] auto get_all_formats() const
        -> std::vector<FormatInfo>;

    // Check if Pandoc is installed and accessible.
    [[nodiscard]] auto is_pandoc_available() const -> bool;

    // Get Pandoc version string if installed.
    [[nodiscard]] auto pandoc_version() const -> std::string;

private:
    EventBus& event_bus_;
    Config& config_;
    std::unordered_map<std::string, std::unique_ptr<IExportFormat>> exporters_;
    mutable std::optional<bool> pandoc_available_;
    mutable std::string pandoc_version_;

    // Find the exporter for a given format ID.
    [[nodiscard]] auto find_exporter(ExportFormatId format) const
        -> IExportFormat*;

    // Convert ExportFormatId enum to string ID.
    [[nodiscard]] static auto format_id_to_string(ExportFormatId format)
        -> std::string;

    // Register all built-in exporters.
    auto register_builtins() -> void;

    // Detect Pandoc installation and cache the result.
    auto detect_pandoc() const -> void;

    // Resolve block references in markdown content.
    [[nodiscard]] auto resolve_references(const std::string& markdown) const
        -> std::string;

    // Gather asset file paths referenced in the content.
    [[nodiscard]] auto gather_assets(const std::string& content) const
        -> std::vector<std::filesystem::path>;
};

// PandocExporter — base class for all Pandoc-based export formats.
// Subclasses only need to specify the format name and extension.
class PandocExporter : public IExportFormat
{
public:
    PandocExporter(const std::string& pandoc_format,
                   const FormatInfo& info);

    [[nodiscard]] auto format_info() const -> FormatInfo override;

    [[nodiscard]] auto export_document(
        const std::string& root_id,
        const std::filesystem::path& output_path,
        const ExportOptions& options)
        -> std::expected<ExportResult, std::string> override;

    [[nodiscard]] auto export_content(
        const std::string& markdown_content,
        const std::filesystem::path& output_path,
        const ExportOptions& options)
        -> std::expected<ExportResult, std::string> override;

    [[nodiscard]] auto is_available() const -> bool override;

private:
    std::string pandoc_format_;                  // Pandoc format name (e.g., "docx", "epub")
    FormatInfo info_;

    // Build the Pandoc command line arguments.
    [[nodiscard]] auto build_pandoc_args(
        const std::filesystem::path& input_path,
        const std::filesystem::path& output_path,
        const ExportOptions& options) const
        -> std::vector<std::string>;

    // Execute Pandoc as a subprocess.
    [[nodiscard]] auto run_pandoc(const std::vector<std::string>& args) const
        -> std::expected<int, std::string>;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. `ExportService::register_builtins()` — Create and register MarkdownExporter, HtmlExporter, PlainTextExporter (built-in). If Pandoc is detected, register PandocExporter instances for DOCX, EPUB, RTF, ODT, LaTeX, Textile, OrgMode, AsciiDoc, MediaWiki, ReStructuredText, OPML. Register PdfExporter and PngExporter (specialized).
2. `ExportService::export_document()` — Find exporter for the format. Load document block tree by root_id. Convert block tree to markdown. If options.resolve_block_refs, resolve all `((block-id))` references to their content. If options.include_assets, gather asset paths. Dispatch to exporter.export_content(). Publish ExportCompletedEvent.
3. `ExportService::export_notebook()` — List all documents in the notebook. Create a temporary directory. Export each document to the selected format in the temp directory. If options.include_assets, copy all notebook assets to an `assets/` subdirectory. Create a ZIP archive of the temp directory. Publish ExportCompletedEvent.
4. `MarkdownExporter::export_content()` — Resolve block references. Optionally strip IAL annotations. Optionally include/exclude YAML frontmatter. Write clean markdown to output file. Return ExportResult with size and timing.
5. `HtmlExporter::export_content()` — Use HtmlRenderer to convert markdown to HTML. Wrap in full HTML document with `<head>`, theme CSS, and optional custom CSS. If embed_images, base64-encode all referenced images inline. If include_toc, generate a table of contents from headings. Write to output file.
6. `PandocExporter::export_content()` — Write markdown content to a temp `.md` file. Build Pandoc command args: `pandoc input.md -f markdown -t <format> -o output.<ext>`. Add format-specific args (EPUB metadata, PDF engine, etc.). Run Pandoc subprocess. Check exit code. Return ExportResult.
7. `PandocExporter::run_pandoc()` — Use `popen()` or `std::system()` to execute the Pandoc command. Capture stdout and stderr. Return exit code. Timeout after 60 seconds for large documents.
8. `PdfExporter::export_content()` — Render markdown to HTML. Use wxWidgets print framework (wxHtmlEasyPrinting) to render HTML to PDF, or invoke wkhtmltopdf/weasyprint as a subprocess. Apply page size, margins, and header/footer from options.
9. `PngExporter::export_content()` — Render markdown to HTML in an off-screen wxWebView or wxHtmlWindow. Capture the rendered bitmap. Save as PNG file. This requires a wxWidgets UI context.
10. `ExportService::resolve_references()` — Scan markdown for `((block-id "anchor text"))` patterns. For each match, look up the block by ID. Replace the reference with the block's content or markdown. Handle missing references by leaving the original pattern or inserting an error marker.
11. `ExportService::gather_assets()` — Scan content for `assets/` path patterns and image references. Resolve to absolute paths. Return deduplicated list of asset file paths.
12. `ExportService::is_pandoc_available()` — Run `pandoc --version`, capture output. Cache result. Return true if exit code is 0.
13. `ExportDialog` constructor — Create wxChoice for format selection. Create wxPanel for per-format options (dynamically shown/hidden). Create wxFilePickerCtrl for output path. Create wxGauge for progress. Wire event handlers for format change (show/hide relevant options).
14. `FormatInfo::dotted_extension()` — Return `"." + file_extension` for use in file dialogs.
15. `ExportService::format_id_to_string()` — Switch on ExportFormatId, return string ID: Markdown->"markdown", HTML->"html", PlainText->"plaintext", PDF->"pdf", DOCX->"docx", etc.

## Events to Add (in Events.h)

```cpp
// ============================================================================
// Export events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExportStartedEvent)
std::string format_id;
std::string root_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExportCompletedEvent)
std::string format_id;
std::string output_path;
int64_t size_bytes{0};
int64_t elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExportFailedEvent)
std::string format_id;
std::string error_message;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExportProgressEvent)
std::string format_id;
int progress_percent{0};
std::string current_item;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(ExportDialogRequestEvent);
```

## Config Keys to Add

- `knowledgebase.export.default_format` — Default export format (default: `"markdown"`)
- `knowledgebase.export.pandoc_path` — Path to Pandoc executable; empty = search PATH (default: `""`)
- `knowledgebase.export.pdf_engine` — PDF engine: `"wxwidgets"`, `"wkhtmltopdf"`, `"weasyprint"` (default: `"wxwidgets"`)
- `knowledgebase.export.include_assets` — Include assets in export by default (default: `true`)
- `knowledgebase.export.embed_images` — Embed images as base64 in HTML export (default: `false`)
- `knowledgebase.export.default_page_size` — Default PDF page size (default: `"A4"`)
- `knowledgebase.export.resolve_refs` — Resolve block references during export (default: `true`)
- `knowledgebase.export.last_output_dir` — Remember last export output directory (default: `""`)

## Test Cases (Catch2)

File: `tests/unit/test_export_service.cpp`

1. **Markdown export produces clean output** — Export a document with block references, IAL, and standard markdown. Verify output is valid markdown. Verify block references are resolved to content. Verify IAL annotations are stripped.
2. **HTML export produces standalone document** — Export a document to HTML. Verify output contains `<!DOCTYPE html>`, `<head>`, `<body>`. Verify CSS is included inline. Verify headings, paragraphs, and code blocks are present.
3. **HTML export with embedded images** — Create content referencing 2 images. Export with `embed_images=true`. Verify output contains `data:image/png;base64,` or `data:image/jpeg;base64,` for each image. Verify no `<img src="assets/..."` references remain.
4. **Plain text export strips all markup** — Export markdown with bold, italic, headings, links, images. Verify output contains only plain text with no HTML tags or markdown syntax characters.
5. **Pandoc availability detection** — Mock the `pandoc --version` call. When available, verify `is_pandoc_available()` returns true. When not available, verify returns false and Pandoc-based formats are not in `get_available_formats()`.
6. **Format registry lists correct formats** — Register all built-in exporters. Verify `get_all_formats()` returns at least 3 (Markdown, HTML, PlainText). Verify each FormatInfo has non-empty format_id, format_name, file_extension.
7. **Export with table of contents** — Export a document with 5 headings (H1-H3) with `include_toc=true`. Verify HTML output contains a `<nav>` or `<div class="toc">` section with links to each heading.
8. **Notebook export produces ZIP** — Create a mock notebook with 3 documents and 2 assets. Export as HTML. Verify output is a ZIP file. Verify ZIP contains 3 HTML files and an `assets/` directory with 2 files.
9. **Block reference resolution** — Content contains `((block-123 "display text"))`. Mock block-123 lookup returns "The actual content". Verify exported content replaces the reference with "The actual content".
10. **Export options page size (PDF path)** — Create ExportOptions with page_size="Letter", orientation="landscape". Verify PdfExporter builds correct command arguments. Verify margins are passed correctly.
11. **ExportResult contains accurate stats** — Export a document with 3 images. Verify ExportResult: size_bytes > 0, elapsed_ms >= 0, images_embedded == 3 (if embed enabled), output_path matches provided path.
12. **Export format ID to string conversion** — Verify all ExportFormatId enum values produce correct string IDs: Markdown->"markdown", HTML->"html", PDF->"pdf", DOCX->"docx", EPUB->"epub", LaTeX->"latex", etc.

## Acceptance Criteria

- [ ] Built-in exporters (Markdown, HTML, PlainText) work without external dependencies
- [ ] Pandoc-based exporters detected and registered when Pandoc is installed
- [ ] HTML export produces self-contained documents with embedded CSS
- [ ] Image embedding converts asset references to base64 inline data
- [ ] Block reference resolution replaces ((id)) patterns with actual content
- [ ] Notebook export produces ZIP archives with all documents and assets
- [ ] ExportDialog provides format selection, options, and file picker
- [ ] Progress events published during long exports
- [ ] All 12 test cases pass
- [ ] Uses `[[nodiscard]]` on all query and export methods

## Files to Create/Modify

- CREATE: `src/core/ExportTypes.h`
- CREATE: `src/core/IExportFormat.h`
- CREATE: `src/core/ExportService.h`
- CREATE: `src/core/ExportService.cpp`
- CREATE: `src/core/MarkdownExporter.h`
- CREATE: `src/core/MarkdownExporter.cpp`
- CREATE: `src/core/HtmlExporter.h`
- CREATE: `src/core/HtmlExporter.cpp`
- CREATE: `src/core/PandocExporter.h`
- CREATE: `src/core/PandocExporter.cpp`
- CREATE: `src/core/PdfExporter.h`
- CREATE: `src/core/PdfExporter.cpp`
- CREATE: `src/core/PngExporter.h`
- CREATE: `src/core/PngExporter.cpp`
- CREATE: `src/ui/ExportDialog.h`
- CREATE: `src/ui/ExportDialog.cpp`
- MODIFY: `src/core/Events.h` (add ExportStartedEvent, ExportCompletedEvent, ExportFailedEvent, ExportProgressEvent, ExportDialogRequestEvent)
- MODIFY: `src/core/PluginContext.h` (add `ExportService*` pointer)
- MODIFY: `src/app/MarkAmpApp.cpp` (instantiate ExportService, register exporters, wire to PluginContext)
- MODIFY: `src/CMakeLists.txt` (add all exporter source files to source list)
- CREATE: `tests/unit/test_export_service.cpp`
- MODIFY: `tests/CMakeLists.txt` (add test_export_service target)
