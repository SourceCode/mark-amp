/// @file PdfAnnotationManager.h
/// @brief V9 Phase 42 — PDF annotation support with highlights, comments, and bookmarks.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Types of PDF annotations.
enum class PdfAnnotationType : uint8_t
{
    kHighlight = 0, ///< Text highlight
    kUnderline = 1, ///< Text underline
    kStrikeout = 2, ///< Text strikethrough
    kComment = 3,   ///< Sticky note comment
    kFreeText = 4,  ///< Free text annotation
    kBookmark = 5,  ///< Position bookmark
};

/// A single PDF annotation.
struct PdfAnnotation
{
    std::string annotation_id;
    std::string document_id;
    int page_number{0};
    PdfAnnotationType annotation_type{PdfAnnotationType::kHighlight};
    std::string text;    ///< Selected/annotated text
    std::string comment; ///< User comment
    std::string color;   ///< Highlight color (hex)
    double pos_x{0.0};
    double pos_y{0.0};
    double width{0.0};
    double height{0.0};
    std::chrono::system_clock::time_point created_at;
};

/// Manages annotations for PDF documents.
///
/// Supports highlights, comments, bookmarks, and free text annotations.
/// Annotations are tracked per-document and can be exported.
class PdfAnnotationManager
{
public:
    PdfAnnotationManager() = default;

    // ── Annotation management ─────────────────────────────────────────
    auto add_annotation(PdfAnnotation annotation) -> std::string;
    auto remove_annotation(const std::string& annotation_id) -> bool;
    [[nodiscard]] auto find_annotation(const std::string& annotation_id) const
        -> const PdfAnnotation*;
    [[nodiscard]] auto annotation_count() const -> int;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto annotations_for_document(const std::string& document_id) const
        -> std::vector<const PdfAnnotation*>;
    [[nodiscard]] auto annotations_for_page(const std::string& document_id, int page) const
        -> std::vector<const PdfAnnotation*>;
    [[nodiscard]] auto annotations_by_type(PdfAnnotationType annotation_type) const
        -> std::vector<const PdfAnnotation*>;
    [[nodiscard]] auto all_annotations() const -> std::vector<const PdfAnnotation*>;

    // ── Export ─────────────────────────────────────────────────────────
    [[nodiscard]] auto export_annotations(const std::string& document_id) const -> std::string;

    void clear_all();

private:
    std::vector<PdfAnnotation> annotations_;
    int next_id_{1};
};

} // namespace markamp::core
