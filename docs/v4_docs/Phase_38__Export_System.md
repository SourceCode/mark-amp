# Phase 38 -- Export System (HTML, PDF, Markdown, LaTeX, Slides)

## Objective

Implement a multi-format export system: export individual documents or entire vaults to HTML (static site), PDF, clean Markdown, LaTeX, and slide decks. Each format has specific options (HTML: standalone or with assets, PDF: page size, LaTeX: document class, Slides: reveal.js). Supports batch export of multiple documents.

## Prerequisites

- Existing HtmlRenderer, MarkdownParser
- Phase 18 (LatexEngine -- LaTeX rendering)
- Phase 27 (PresentationPanel -- slide parsing)
- Phase 10 (EmbedResolver -- resolve embeds for export)

## Feature References (PRD)

- PRD Notebook #13: Export to Multiple Formats

## Data Structures to Implement

### File: `src/core/ExportService.h`

```cpp
#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class VaultService;

enum class ExportFormat : uint8_t
{
    Html,
    HtmlStandalone,  // Single file with inlined assets
    Pdf,
    Markdown,         // Clean markdown (resolve embeds, strip vault-specific syntax)
    Latex,
    Slides,           // Reveal.js HTML slides
    Docx              // Future: Word document
};

struct ExportOptions
{
    ExportFormat format{ExportFormat::Html};
    std::filesystem::path output_path;
    bool include_frontmatter{false};
    bool resolve_embeds{true};
    bool resolve_wikilinks{true};    // Convert [[link]] to [link](path.md)
    bool include_css{true};
    std::string css_theme;           // Which theme to apply

    // PDF options
    std::string page_size{"A4"};     // A4, Letter, etc.
    bool landscape{false};

    // LaTeX options
    std::string document_class{"article"};
    std::string preamble;

    // Slides options
    std::string slide_theme{"black"};
    std::string slide_transition{"slide"};

    // Batch options
    bool export_vault{false};        // Export entire vault
    bool create_index{true};         // Create index.html for vault export
};

struct ExportResult
{
    std::filesystem::path output_path;
    ExportFormat format;
    int documents_exported{0};
    int64_t total_bytes{0};
    double elapsed_ms{0.0};
    std::vector<std::string> errors;
    bool success{false};
};

class ExportService
{
public:
    ExportService(EventBus& event_bus, Config& config, VaultService& vault_service);

    /// Export a single document.
    [[nodiscard]] auto export_document(const std::string& document_id,
                                        const ExportOptions& options)
        -> std::expected<ExportResult, std::string>;

    /// Export multiple documents.
    [[nodiscard]] auto export_documents(const std::vector<std::string>& document_ids,
                                         const ExportOptions& options)
        -> std::expected<ExportResult, std::string>;

    /// Export the entire vault as a static site.
    [[nodiscard]] auto export_vault(const ExportOptions& options)
        -> std::expected<ExportResult, std::string>;

    /// Get export preview (first page of rendered output).
    [[nodiscard]] auto preview_export(const std::string& document_id,
                                       const ExportOptions& options) const
        -> std::string;

private:
    EventBus& event_bus_;
    Config& config_;
    VaultService& vault_service_;

    [[nodiscard]] auto export_to_html(const std::string& markdown,
                                       const ExportOptions& options) const -> std::string;
    [[nodiscard]] auto export_to_pdf(const std::string& html,
                                      const ExportOptions& options) const
        -> std::expected<std::vector<uint8_t>, std::string>;
    [[nodiscard]] auto export_to_latex(const std::string& markdown,
                                        const ExportOptions& options) const -> std::string;
    [[nodiscard]] auto export_to_clean_markdown(const std::string& markdown,
                                                 const ExportOptions& options) const -> std::string;
    [[nodiscard]] auto export_to_slides(const std::string& markdown,
                                         const ExportOptions& options) const -> std::string;

    [[nodiscard]] auto resolve_embeds_in_markdown(const std::string& markdown) const -> std::string;
    [[nodiscard]] auto convert_wikilinks_to_markdown(const std::string& content) const -> std::string;
    [[nodiscard]] auto generate_vault_index(const std::vector<std::string>& doc_ids,
                                              const ExportOptions& options) const -> std::string;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`export_document(document_id, options)`** -- Load document. Optionally resolve embeds and wikilinks. Dispatch to format-specific exporter. Write output file. Return ExportResult.

2. **`export_to_html(markdown, options)`** -- Render markdown to HTML. Wrap in HTML5 document. Include CSS (inline for standalone, external for multi-file). Generate navigation if vault export.

3. **`export_to_pdf(html, options)`** -- Use the HTML rendering and print it to PDF via platform printing API (or wkhtmltopdf-style conversion). Set page size and orientation.

4. **`export_to_latex(markdown, options)`** -- Convert markdown to LaTeX: headings to \section, bold to \textbf, code to lstlisting, tables to tabular, math passthrough.

5. **`export_to_slides(markdown, options)`** -- Parse slide breaks. Generate reveal.js HTML with configured theme and transitions.

6. **`export_to_clean_markdown(markdown, options)`** -- Resolve embeds inline. Convert `[[wikilinks]]` to `[text](path.md)`. Strip vault-specific metadata if configured.

7. **`export_vault(options)`** -- Export every document. Generate index.html with links to all documents. Copy assets (images, etc.) to output directory.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExportCompletedEvent)
std::string output_path;
ExportFormat format;
int documents{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExportProgressEvent)
int current{0};
int total{0};
std::string current_document;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_export_service.cpp`

1. **Export HTML** -- Export document to HTML. Verify valid HTML with content.
2. **Export standalone HTML** -- Verify CSS is inlined, single file.
3. **Export clean markdown** -- Wikilinks converted to standard markdown links.
4. **Export LaTeX** -- Headings become \section. Bold becomes \textbf.
5. **Export slides** -- 3-slide document produces reveal.js HTML.
6. **Resolve embeds** -- Document with ![[embed]]. Export resolves inline.
7. **Convert wikilinks** -- [[Note]] becomes [Note](Note.md).
8. **Vault export** -- 5 documents. Export all. Verify index.html and 5 HTML files.
9. **Export with theme** -- Apply specific CSS theme to HTML export.
10. **Batch export** -- Export 3 documents. Verify all 3 output files created.

## Acceptance Criteria

- [ ] HTML export produces valid, styled HTML documents
- [ ] Standalone HTML inlines CSS and assets
- [ ] LaTeX export converts markdown syntax to LaTeX
- [ ] Slides export generates reveal.js presentation
- [ ] Clean markdown resolves embeds and wikilinks
- [ ] Vault export creates index + all documents
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/ExportService.h` | ExportService, ExportOptions, ExportResult |
| CREATE | `src/core/ExportService.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 2 export events |
| MODIFY | `src/core/PluginContext.h` | Add `ExportService* export_service{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add ExportService.cpp |
| CREATE | `tests/unit/test_export_service.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_export_service target |

## Estimated Complexity

**XL** -- 5 export format converters, embed/wikilink resolution, vault-wide batch, 10 tests.
