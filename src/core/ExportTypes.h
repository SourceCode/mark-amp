// ============================================================================
// File: src/core/ExportTypes.h
// Phase 39: Multi-Format Export — Types, config, format descriptors
// ============================================================================
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

// Export format identifier.
enum class ExportFormat : uint8_t
{
    Markdown,
    HTML,
    PlainText,
    PDF,
    DOCX,
    EPUB,
    LaTeX,
    RTF,
    ODT,
    PPTX,
    ConfluentWiki,
    MediaWiki,
    Textile,
    AsciiDoc,
    ReStructuredText,
    OPML,
    PNG, // Image-based export (e.g., screenshot)
    Custom
};

// Export scope.
enum class ExportScope : uint8_t
{
    CurrentBlock,    // Single block
    CurrentDocument, // Entire document
    SelectedBlocks,  // User selection
    Notebook,        // Entire notebook
    Workspace        // Everything
};

// Configuration for an export operation.
struct ExportOptions
{
    ExportFormat format{ExportFormat::Markdown};
    ExportScope scope{ExportScope::CurrentDocument};
    std::filesystem::path output_path; // Target file/directory path

    // Content options.
    bool include_frontmatter{true}; // Include YAML frontmatter
    bool include_toc{false};        // Generate table of contents
    bool include_metadata{false};   // Include doc metadata
    bool include_assets{true};      // Copy referenced assets
    bool embed_assets{false};       // Base64 embed images inline

    // Formatting options.
    std::string css_path;        // Custom CSS for HTML/PDF
    std::string template_name;   // Template name for rendering
    int heading_offset{0};       // Shift heading levels
    bool reference_links{false}; // Use reference-style MD links

    // Pandoc options (for Pandoc-based formats).
    std::filesystem::path pandoc_path; // Path to pandoc binary
    std::string pandoc_extra_args;     // Additional pandoc arguments

    // PDF-specific options.
    std::string pdf_engine{"wkhtmltopdf"}; // PDF renderer
    std::string page_size{"A4"};
    std::string margin{"20mm"};
    bool landscape{false};
    bool page_numbers{true};

    [[nodiscard]] auto is_pandoc_format() const -> bool
    {
        switch (format)
        {
            case ExportFormat::DOCX:
            case ExportFormat::EPUB:
            case ExportFormat::LaTeX:
            case ExportFormat::RTF:
            case ExportFormat::ODT:
            case ExportFormat::PPTX:
            case ExportFormat::ConfluentWiki:
            case ExportFormat::MediaWiki:
            case ExportFormat::Textile:
            case ExportFormat::AsciiDoc:
            case ExportFormat::ReStructuredText:
                return true;
            default:
                return false;
        }
    }
};

// Metadata about a supported export format.
struct ExportFormatDescriptor
{
    ExportFormat format{ExportFormat::Markdown};
    std::string name;      // Display name
    std::string extension; // File extension (e.g., ".md")
    std::string mime_type;
    std::string description;
    bool requires_pandoc{false};
    bool supports_assets{true};
    bool supports_toc{false};
};

// Result of an export operation.
struct ExportResult
{
    bool success{false};
    ExportFormat format{ExportFormat::Markdown};
    std::filesystem::path output_path;
    int64_t output_size_bytes{0};
    int32_t blocks_exported{0};
    int32_t assets_copied{0};
    int64_t elapsed_ms{0};
    std::string error_message;
    std::vector<std::string> warnings;
};

} // namespace markamp::core
