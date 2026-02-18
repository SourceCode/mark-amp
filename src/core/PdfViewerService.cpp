/// @file PdfViewerService.cpp
/// @brief V9 Phase 42 — PdfViewerService implementation.

#include "PdfViewerService.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

auto PdfViewerService::load_document(const std::string& file_path) -> std::string
{
    PdfDocument doc;
    doc.document_id = "pdf_" + std::to_string(next_id_++);
    doc.file_path = file_path;

    // Extract title from filename
    auto slash = file_path.rfind('/');
    doc.title = (slash != std::string::npos) ? file_path.substr(slash + 1) : file_path;

    // Simulate a 10-page PDF with placeholder pages
    doc.total_pages = 10;
    doc.loaded = true;
    for (int page_num = 1; page_num <= doc.total_pages; ++page_num)
    {
        PdfPageInfo page;
        page.page_number = page_num;
        page.width_points = 612;  // US Letter width
        page.height_points = 792; // US Letter height
        page.extracted_text = "Page " + std::to_string(page_num) + " content";
        doc.pages.push_back(std::move(page));
    }

    auto doc_id = doc.document_id;
    documents_.push_back(std::move(doc));
    return doc_id;
}

auto PdfViewerService::close_document(const std::string& document_id) -> bool
{
    auto iter =
        std::remove_if(documents_.begin(),
                       documents_.end(),
                       [&](const PdfDocument& doc) { return doc.document_id == document_id; });
    if (iter == documents_.end())
    {
        return false;
    }
    documents_.erase(iter, documents_.end());
    return true;
}

auto PdfViewerService::find_document(const std::string& document_id) const -> const PdfDocument*
{
    for (const auto& doc : documents_)
    {
        if (doc.document_id == document_id)
        {
            return &doc;
        }
    }
    return nullptr;
}

auto PdfViewerService::document_count() const -> int
{
    return static_cast<int>(documents_.size());
}

auto PdfViewerService::go_to_page(const std::string& document_id, int page) -> bool
{
    auto* doc = find_mut(document_id);
    if (doc == nullptr || page < 1 || page > doc->total_pages)
    {
        return false;
    }
    doc->current_page = page;
    return true;
}

auto PdfViewerService::next_page(const std::string& document_id) -> bool
{
    auto* doc = find_mut(document_id);
    if (doc == nullptr || doc->current_page >= doc->total_pages)
    {
        return false;
    }
    doc->current_page++;
    return true;
}

auto PdfViewerService::previous_page(const std::string& document_id) -> bool
{
    auto* doc = find_mut(document_id);
    if (doc == nullptr || doc->current_page <= 1)
    {
        return false;
    }
    doc->current_page--;
    return true;
}

auto PdfViewerService::set_zoom(const std::string& document_id, double level) -> bool
{
    auto* doc = find_mut(document_id);
    if (doc == nullptr || level < 0.1 || level > 10.0)
    {
        return false;
    }
    doc->zoom_level = level;
    doc->zoom_mode = PdfZoomMode::kCustom;
    return true;
}

auto PdfViewerService::set_zoom_mode(const std::string& document_id, PdfZoomMode mode) -> bool
{
    auto* doc = find_mut(document_id);
    if (doc == nullptr)
    {
        return false;
    }
    doc->zoom_mode = mode;
    if (mode == PdfZoomMode::kActualSize)
    {
        doc->zoom_level = 1.0;
    }
    return true;
}

auto PdfViewerService::zoom_in(const std::string& document_id) -> bool
{
    auto* doc = find_mut(document_id);
    if (doc == nullptr || doc->zoom_level >= 5.0)
    {
        return false;
    }
    doc->zoom_level *= 1.25;
    doc->zoom_mode = PdfZoomMode::kCustom;
    return true;
}

auto PdfViewerService::zoom_out(const std::string& document_id) -> bool
{
    auto* doc = find_mut(document_id);
    if (doc == nullptr || doc->zoom_level <= 0.2)
    {
        return false;
    }
    doc->zoom_level *= 0.8;
    doc->zoom_mode = PdfZoomMode::kCustom;
    return true;
}

auto PdfViewerService::extract_text(const std::string& document_id, int page) const -> std::string
{
    const auto* doc = find_document(document_id);
    if (doc == nullptr || page < 1 || page > doc->total_pages)
    {
        return {};
    }
    return doc->pages[static_cast<size_t>(page - 1)].extracted_text;
}

auto PdfViewerService::extract_all_text(const std::string& document_id) const -> std::string
{
    const auto* doc = find_document(document_id);
    if (doc == nullptr)
    {
        return {};
    }
    std::ostringstream oss;
    for (const auto& page : doc->pages)
    {
        oss << page.extracted_text << "\n";
    }
    return oss.str();
}

auto PdfViewerService::set_render_quality(const std::string& document_id, PdfRenderQuality quality)
    -> bool
{
    auto* doc = find_mut(document_id);
    if (doc == nullptr)
    {
        return false;
    }
    doc->quality = quality;
    return true;
}

auto PdfViewerService::all_documents() const -> std::vector<const PdfDocument*>
{
    std::vector<const PdfDocument*> result;
    result.reserve(documents_.size());
    for (const auto& doc : documents_)
    {
        result.push_back(&doc);
    }
    return result;
}

auto PdfViewerService::open_document_ids() const -> std::vector<std::string>
{
    std::vector<std::string> ids;
    ids.reserve(documents_.size());
    for (const auto& doc : documents_)
    {
        ids.push_back(doc.document_id);
    }
    return ids;
}

void PdfViewerService::close_all()
{
    documents_.clear();
}

auto PdfViewerService::find_mut(const std::string& document_id) -> PdfDocument*
{
    for (auto& doc : documents_)
    {
        if (doc.document_id == document_id)
        {
            return &doc;
        }
    }
    return nullptr;
}

} // namespace markamp::core
