// ============================================================================
// File: src/core/ImportTypes.h
// Phase 40: Import & AI Integration — Import types and configuration
// ============================================================================
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

// Import source format.
enum class ImportFormat : uint8_t
{
    Markdown,
    HTML,
    DOCX,
    EPUB,
    OPML,
    CSV,
    PlainText,
    LaTeX,
    RTF,
    SiYuanPackage, // .sy.zip SiYuan export package
    Custom
};

// How to handle conflicts during import.
enum class ImportConflict : uint8_t
{
    Skip,      // Skip if document already exists
    Overwrite, // Replace existing
    Rename,    // Create with modified name
    Merge      // Attempt to merge (blocks)
};

// Configuration for an import operation.
struct ImportOptions
{
    ImportFormat format{ImportFormat::Markdown};
    std::filesystem::path source_path; // File or directory to import
    std::string target_notebook_id;    // Target notebook
    ImportConflict conflict_handling{ImportConflict::Rename};

    // Content options.
    bool import_assets{true};       // Import referenced images/files
    bool preserve_structure{true};  // Maintain directory hierarchy
    bool convert_links{true};       // Convert file links to block refs
    bool extract_frontmatter{true}; // Parse YAML frontmatter

    // Pandoc options.
    std::filesystem::path pandoc_path;
    std::string pandoc_extra_args;
};

// Result of an import operation.
struct ImportResult
{
    bool success{false};
    ImportFormat format{ImportFormat::Markdown};
    int32_t documents_imported{0};
    int32_t blocks_created{0};
    int32_t assets_imported{0};
    int32_t skipped{0};
    int32_t errors_count{0};
    int64_t elapsed_ms{0};
    std::vector<std::string> imported_doc_ids;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// Metadata about a supported import format.
struct ImportFormatDescriptor
{
    ImportFormat format{ImportFormat::Markdown};
    std::string name;
    std::string extension;                   // Primary file extension
    std::vector<std::string> all_extensions; // All supported extensions
    std::string description;
    bool requires_pandoc{false};
};

} // namespace markamp::core
