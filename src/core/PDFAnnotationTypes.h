// ============================================================================
// File: src/core/PDFAnnotationTypes.h
// Phase 31: PDF Annotation System — Types and data structures
// ============================================================================
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Type of PDF annotation.
enum class AnnotationType : uint8_t
{
    kHighlight, // Text highlight
    kRectangle, // Rectangular area
    kUnderline, // Text underline
    kStrikeout, // Text strikethrough
    kFreetext   // Free text note on the page
};

/// Color presets for annotations.
enum class AnnotationColor : uint8_t
{
    kYellow,
    kRed,
    kGreen,
    kBlue,
    kPurple,
    kOrange,
    kCustom
};

/// A normalized rectangle (coordinates 0.0–1.0 relative to page dimensions).
struct NormalizedRect
{
    double x1{0.0}; // Left
    double y1{0.0}; // Top
    double x2{0.0}; // Right
    double y2{0.0}; // Bottom

    [[nodiscard]] auto width() const -> double
    {
        return x2 - x1;
    }
    [[nodiscard]] auto height() const -> double
    {
        return y2 - y1;
    }
    [[nodiscard]] auto area() const -> double
    {
        return width() * height();
    }

    [[nodiscard]] auto contains(double pnt_x, double pnt_y) const -> bool
    {
        return pnt_x >= x1 && pnt_x <= x2 && pnt_y >= y1 && pnt_y <= y2;
    }

    [[nodiscard]] auto is_valid() const -> bool
    {
        return x1 >= 0.0 && y1 >= 0.0 && x2 > x1 && y2 > y1 && x2 <= 1.0 && y2 <= 1.0;
    }
};

/// A single annotation on a PDF page.
struct PDFAnnotation
{
    std::string id;            // Unique annotation ID (UUID)
    std::string document_path; // Path to the PDF file
    int page_number{0};        // 0-based page index
    AnnotationType type{AnnotationType::kHighlight};
    AnnotationColor color{AnnotationColor::kYellow};
    std::string custom_color;    // Hex color string for kCustom
    NormalizedRect rect;         // Annotation position
    std::string text;            // Annotated text content
    std::string comment;         // User comment/note
    std::string linked_block_id; // Optional linked Markdown block ID
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point modified;

    [[nodiscard]] auto has_comment() const -> bool
    {
        return !comment.empty();
    }

    [[nodiscard]] auto has_linked_block() const -> bool
    {
        return !linked_block_id.empty();
    }

    /// Get the reference syntax for embedding in Markdown.
    /// Format: <<assets/file.pdf/annotation-id "page N">>
    [[nodiscard]] auto reference_syntax() const -> std::string
    {
        return "<<" + document_path + "/" + id + " \"page " + std::to_string(page_number + 1) +
               "\">>";
    }
};

/// Summary info for a PDF document.
struct PDFDocumentInfo
{
    std::string path; // Path to the PDF file
    int page_count{0};
    std::string title;
    std::string author;
    int annotation_count{0};
    int64_t file_size_bytes{0};
};

/// Color utility functions.
struct AnnotationColorUtil
{
    [[nodiscard]] static auto to_hex(AnnotationColor color) -> std::string
    {
        switch (color)
        {
            case AnnotationColor::kYellow:
                return "#FFFF00";
            case AnnotationColor::kRed:
                return "#FF0000";
            case AnnotationColor::kGreen:
                return "#00FF00";
            case AnnotationColor::kBlue:
                return "#0000FF";
            case AnnotationColor::kPurple:
                return "#800080";
            case AnnotationColor::kOrange:
                return "#FFA500";
            case AnnotationColor::kCustom:
                return "#FFFF00";
        }
        return "#FFFF00";
    }

    [[nodiscard]] static auto display_name(AnnotationColor color) -> std::string_view
    {
        switch (color)
        {
            case AnnotationColor::kYellow:
                return "Yellow";
            case AnnotationColor::kRed:
                return "Red";
            case AnnotationColor::kGreen:
                return "Green";
            case AnnotationColor::kBlue:
                return "Blue";
            case AnnotationColor::kPurple:
                return "Purple";
            case AnnotationColor::kOrange:
                return "Orange";
            case AnnotationColor::kCustom:
                return "Custom";
        }
        return "Yellow";
    }
};

} // namespace markamp::core
