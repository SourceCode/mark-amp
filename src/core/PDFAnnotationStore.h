// ============================================================================
// File: src/core/PDFAnnotationStore.h
// Phase 31: PDF Annotation System — Annotation persistence (.sya files)
// ============================================================================
#pragma once

#include "PDFAnnotationTypes.h"

#include <expected>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Manages PDF annotations with .sya file persistence.
/// Each PDF file has a corresponding .sya (SiYuan Annotation) file
/// stored alongside it, containing all annotations as JSON.
class PDFAnnotationStore
{
public:
    PDFAnnotationStore() = default;

    /// Initialize the store for a workspace directory.
    [[nodiscard]] auto initialize(const std::filesystem::path& workspace_root)
        -> std::expected<void, std::string>;

    // ── CRUD Operations ──

    /// Add a new annotation.
    [[nodiscard]] auto add_annotation(PDFAnnotation annotation)
        -> std::expected<std::string, std::string>;

    /// Update an existing annotation.
    [[nodiscard]] auto update_annotation(const PDFAnnotation& annotation)
        -> std::expected<void, std::string>;

    /// Delete an annotation by ID.
    [[nodiscard]] auto delete_annotation(const std::string& annotation_id)
        -> std::expected<void, std::string>;

    /// Get a specific annotation by ID.
    [[nodiscard]] auto get_annotation(const std::string& annotation_id) const
        -> std::expected<PDFAnnotation, std::string>;

    // ── Query Operations ──

    /// Get all annotations for a PDF document.
    [[nodiscard]] auto get_annotations_for_document(const std::string& pdf_path) const
        -> std::vector<PDFAnnotation>;

    /// Get annotations for a specific page of a PDF.
    [[nodiscard]] auto get_annotations_for_page(const std::string& pdf_path, int page_number) const
        -> std::vector<PDFAnnotation>;

    /// Get all annotations linked to a specific Markdown block.
    [[nodiscard]] auto get_annotations_for_block(const std::string& block_id) const
        -> std::vector<PDFAnnotation>;

    /// Export all annotations for a PDF as Markdown.
    [[nodiscard]] auto export_as_markdown(const std::string& pdf_path) const -> std::string;

    // ── Persistence ──

    /// Load annotations from the .sya file for a PDF.
    [[nodiscard]] auto load_annotations(const std::string& pdf_path)
        -> std::expected<void, std::string>;

    /// Save annotations to the .sya file for a PDF.
    [[nodiscard]] auto save_annotations(const std::string& pdf_path) const
        -> std::expected<void, std::string>;

    /// Get the .sya file path for a PDF path.
    [[nodiscard]] auto sya_path_for(const std::string& pdf_path) const -> std::filesystem::path;

private:
    std::filesystem::path workspace_root_;
    mutable std::mutex mutex_;

    /// All annotations indexed by annotation ID.
    std::unordered_map<std::string, PDFAnnotation> annotations_;

    /// Document path → set of annotation IDs.
    std::unordered_map<std::string, std::vector<std::string>> doc_index_;
};

} // namespace markamp::core
