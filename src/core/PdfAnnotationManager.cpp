/// @file PdfAnnotationManager.cpp
/// @brief V9 Phase 42 — PdfAnnotationManager implementation.

#include "PdfAnnotationManager.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

auto PdfAnnotationManager::add_annotation(PdfAnnotation annotation) -> std::string
{
    if (annotation.annotation_id.empty())
    {
        annotation.annotation_id = "ann_" + std::to_string(next_id_++);
    }
    annotation.created_at = std::chrono::system_clock::now();
    auto ann_id = annotation.annotation_id;
    annotations_.push_back(std::move(annotation));
    return ann_id;
}

auto PdfAnnotationManager::remove_annotation(const std::string& annotation_id) -> bool
{
    auto iter = std::remove_if(annotations_.begin(),
                               annotations_.end(),
                               [&](const PdfAnnotation& ann)
                               { return ann.annotation_id == annotation_id; });
    if (iter == annotations_.end())
    {
        return false;
    }
    annotations_.erase(iter, annotations_.end());
    return true;
}

auto PdfAnnotationManager::find_annotation(const std::string& annotation_id) const
    -> const PdfAnnotation*
{
    for (const auto& ann : annotations_)
    {
        if (ann.annotation_id == annotation_id)
        {
            return &ann;
        }
    }
    return nullptr;
}

auto PdfAnnotationManager::annotation_count() const -> int
{
    return static_cast<int>(annotations_.size());
}

auto PdfAnnotationManager::annotations_for_document(const std::string& document_id) const
    -> std::vector<const PdfAnnotation*>
{
    std::vector<const PdfAnnotation*> result;
    for (const auto& ann : annotations_)
    {
        if (ann.document_id == document_id)
        {
            result.push_back(&ann);
        }
    }
    return result;
}

auto PdfAnnotationManager::annotations_for_page(const std::string& document_id, int page) const
    -> std::vector<const PdfAnnotation*>
{
    std::vector<const PdfAnnotation*> result;
    for (const auto& ann : annotations_)
    {
        if (ann.document_id == document_id && ann.page_number == page)
        {
            result.push_back(&ann);
        }
    }
    return result;
}

auto PdfAnnotationManager::annotations_by_type(PdfAnnotationType annotation_type) const
    -> std::vector<const PdfAnnotation*>
{
    std::vector<const PdfAnnotation*> result;
    for (const auto& ann : annotations_)
    {
        if (ann.annotation_type == annotation_type)
        {
            result.push_back(&ann);
        }
    }
    return result;
}

auto PdfAnnotationManager::all_annotations() const -> std::vector<const PdfAnnotation*>
{
    std::vector<const PdfAnnotation*> result;
    result.reserve(annotations_.size());
    for (const auto& ann : annotations_)
    {
        result.push_back(&ann);
    }
    return result;
}

auto PdfAnnotationManager::export_annotations(const std::string& document_id) const -> std::string
{
    std::ostringstream oss;
    oss << "# Annotations\n\n";
    for (const auto& ann : annotations_)
    {
        if (ann.document_id == document_id)
        {
            oss << "## Page " << ann.page_number << "\n";
            if (!ann.text.empty())
            {
                oss << "> " << ann.text << "\n";
            }
            if (!ann.comment.empty())
            {
                oss << "\n" << ann.comment << "\n";
            }
            oss << "\n";
        }
    }
    return oss.str();
}

void PdfAnnotationManager::clear_all()
{
    annotations_.clear();
}

} // namespace markamp::core
