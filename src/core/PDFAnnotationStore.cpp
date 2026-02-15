// ============================================================================
// File: src/core/PDFAnnotationStore.cpp
// Phase 31: PDF Annotation System — Annotation store implementation
// ============================================================================
#include "PDFAnnotationStore.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>

// JSON serialization using nlohmann/json (assumed available)
#include <nlohmann/json.hpp>

namespace markamp::core
{

auto PDFAnnotationStore::initialize(const std::filesystem::path& workspace_root)
    -> std::expected<void, std::string>
{
    workspace_root_ = workspace_root;
    return {};
}

auto PDFAnnotationStore::add_annotation(PDFAnnotation annotation)
    -> std::expected<std::string, std::string>
{
    std::lock_guard lock(mutex_);

    if (annotation.id.empty())
    {
        // Generate a simple unique ID
        auto now = std::chrono::system_clock::now();
        auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        annotation.id = "ann-" + std::to_string(ms);
    }

    annotation.created = std::chrono::system_clock::now();
    annotation.modified = annotation.created;

    const auto ann_id = annotation.id;
    const auto doc_path = annotation.document_path;

    annotations_[ann_id] = std::move(annotation);
    doc_index_[doc_path].push_back(ann_id);

    return ann_id;
}

auto PDFAnnotationStore::update_annotation(const PDFAnnotation& annotation)
    -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);

    auto found = annotations_.find(annotation.id);
    if (found == annotations_.end())
    {
        return std::unexpected("Annotation not found: " + annotation.id);
    }

    auto updated = annotation;
    updated.modified = std::chrono::system_clock::now();
    found->second = std::move(updated);

    return {};
}

auto PDFAnnotationStore::delete_annotation(const std::string& annotation_id)
    -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);

    auto found = annotations_.find(annotation_id);
    if (found == annotations_.end())
    {
        return std::unexpected("Annotation not found: " + annotation_id);
    }

    const auto doc_path = found->second.document_path;
    annotations_.erase(found);

    // Remove from doc index
    auto& doc_anns = doc_index_[doc_path];
    doc_anns.erase(std::remove(doc_anns.begin(), doc_anns.end(), annotation_id), doc_anns.end());

    return {};
}

auto PDFAnnotationStore::get_annotation(const std::string& annotation_id) const
    -> std::expected<PDFAnnotation, std::string>
{
    std::lock_guard lock(mutex_);

    auto found = annotations_.find(annotation_id);
    if (found == annotations_.end())
    {
        return std::unexpected("Annotation not found: " + annotation_id);
    }

    return found->second;
}

auto PDFAnnotationStore::get_annotations_for_document(const std::string& pdf_path) const
    -> std::vector<PDFAnnotation>
{
    std::lock_guard lock(mutex_);

    std::vector<PDFAnnotation> result;
    auto found = doc_index_.find(pdf_path);
    if (found == doc_index_.end())
    {
        return result;
    }

    for (const auto& ann_id : found->second)
    {
        auto ann_it = annotations_.find(ann_id);
        if (ann_it != annotations_.end())
        {
            result.push_back(ann_it->second);
        }
    }

    // Sort by page number
    std::sort(result.begin(),
              result.end(),
              [](const PDFAnnotation& left, const PDFAnnotation& right)
              { return left.page_number < right.page_number; });

    return result;
}

auto PDFAnnotationStore::get_annotations_for_page(const std::string& pdf_path,
                                                  int page_number) const
    -> std::vector<PDFAnnotation>
{
    auto all_anns = get_annotations_for_document(pdf_path);

    std::vector<PDFAnnotation> page_anns;
    for (auto& ann : all_anns)
    {
        if (ann.page_number == page_number)
        {
            page_anns.push_back(std::move(ann));
        }
    }

    return page_anns;
}

auto PDFAnnotationStore::get_annotations_for_block(const std::string& block_id) const
    -> std::vector<PDFAnnotation>
{
    std::lock_guard lock(mutex_);

    std::vector<PDFAnnotation> result;
    for (const auto& [ann_id, ann] : annotations_)
    {
        if (ann.linked_block_id == block_id)
        {
            result.push_back(ann);
        }
    }

    return result;
}

auto PDFAnnotationStore::export_as_markdown(const std::string& pdf_path) const -> std::string
{
    auto anns = get_annotations_for_document(pdf_path);

    std::ostringstream oss;
    oss << "# Annotations: " << pdf_path << "\n\n";

    int current_page = -1;
    for (const auto& ann : anns)
    {
        if (ann.page_number != current_page)
        {
            current_page = ann.page_number;
            oss << "## Page " << (current_page + 1) << "\n\n";
        }

        oss << "- ";
        if (!ann.text.empty())
        {
            oss << "**\"" << ann.text << "\"**";
        }
        if (!ann.comment.empty())
        {
            oss << " — " << ann.comment;
        }
        if (ann.has_linked_block())
        {
            oss << " [→ Block](siyuan://blocks/" << ann.linked_block_id << ")";
        }
        oss << "\n";
    }

    return oss.str();
}

auto PDFAnnotationStore::load_annotations(const std::string& pdf_path)
    -> std::expected<void, std::string>
{
    auto sya = sya_path_for(pdf_path);
    std::error_code error_code;

    if (!std::filesystem::exists(sya, error_code) || error_code)
    {
        return {}; // No annotations file yet; not an error
    }

    std::ifstream ifs(sya);
    if (!ifs.is_open())
    {
        return std::unexpected("Failed to open annotation file: " + sya.string());
    }

    try
    {
        auto json = nlohmann::json::parse(ifs);

        std::lock_guard lock(mutex_);
        for (const auto& entry : json)
        {
            PDFAnnotation ann;
            ann.id = entry.value("id", "");
            ann.document_path = pdf_path;
            ann.page_number = entry.value("page", 0);
            ann.type = static_cast<AnnotationType>(entry.value("type", 0));
            ann.color = static_cast<AnnotationColor>(entry.value("color", 0));
            ann.custom_color = entry.value("custom_color", "");
            ann.text = entry.value("text", "");
            ann.comment = entry.value("comment", "");
            ann.linked_block_id = entry.value("linked_block_id", "");

            if (entry.contains("rect"))
            {
                auto& rect_json = entry["rect"];
                ann.rect.x1 = rect_json.value("x1", 0.0);
                ann.rect.y1 = rect_json.value("y1", 0.0);
                ann.rect.x2 = rect_json.value("x2", 0.0);
                ann.rect.y2 = rect_json.value("y2", 0.0);
            }

            annotations_[ann.id] = ann;
            doc_index_[pdf_path].push_back(ann.id);
        }
    }
    catch (const nlohmann::json::parse_error& parse_err)
    {
        return std::unexpected("JSON parse error in " + sya.string() + ": " + parse_err.what());
    }

    return {};
}

auto PDFAnnotationStore::save_annotations(const std::string& pdf_path) const
    -> std::expected<void, std::string>
{
    auto sya = sya_path_for(pdf_path);
    auto anns = get_annotations_for_document(pdf_path);

    nlohmann::json json_array = nlohmann::json::array();
    for (const auto& ann : anns)
    {
        nlohmann::json entry;
        entry["id"] = ann.id;
        entry["page"] = ann.page_number;
        entry["type"] = static_cast<int>(ann.type);
        entry["color"] = static_cast<int>(ann.color);
        entry["custom_color"] = ann.custom_color;
        entry["text"] = ann.text;
        entry["comment"] = ann.comment;
        entry["linked_block_id"] = ann.linked_block_id;
        entry["rect"] = {
            {"x1", ann.rect.x1}, {"y1", ann.rect.y1}, {"x2", ann.rect.x2}, {"y2", ann.rect.y2}};
        json_array.push_back(std::move(entry));
    }

    // Create parent directory if needed
    std::error_code error_code;
    std::filesystem::create_directories(sya.parent_path(), error_code);

    std::ofstream ofs(sya);
    if (!ofs.is_open())
    {
        return std::unexpected("Failed to write annotation file: " + sya.string());
    }

    ofs << json_array.dump(2);
    return {};
}

auto PDFAnnotationStore::sya_path_for(const std::string& pdf_path) const -> std::filesystem::path
{
    // .sya file stored alongside the PDF
    std::filesystem::path pdf_fs_path(pdf_path);
    auto sya = pdf_fs_path;
    sya.replace_extension(".sya");
    return sya;
}

} // namespace markamp::core
