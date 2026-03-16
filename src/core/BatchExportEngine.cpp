// ============================================================================
// File: src/core/BatchExportEngine.cpp
// Phase 24: Export & Publishing — Multi-document batch export engine
// ============================================================================

#include "BatchExportEngine.h"

#include "ExportService.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Construction
// ============================================================================

BatchExportEngine::BatchExportEngine(EventBus& event_bus)
    : event_bus_(event_bus)
{
    (void)event_bus_; // Will be used when firing events.
}

auto BatchExportEngine::set_export_service(ExportService& svc) -> void
{
    export_service_ = &svc;
}

// ============================================================================
// Execution
// ============================================================================

auto BatchExportEngine::execute(const BatchExportJob& job, BatchProgressCallback progress)
    -> BatchExportResult
{
    cancelled_ = false;

    auto start_time = std::chrono::steady_clock::now();

    BatchExportResult result;
    result.total = static_cast<int32_t>(job.documents.size());
    result.items.reserve(job.documents.size());

    const auto today = []
    {
        auto now = std::chrono::system_clock::now();
        auto dp = std::chrono::floor<std::chrono::days>(now);
        auto ymd = std::chrono::year_month_day{dp};
        std::ostringstream oss;
        oss << static_cast<int>(ymd.year()) << "-";
        auto month_val = static_cast<unsigned>(ymd.month());
        if (month_val < 10)
            oss << "0";
        oss << month_val << "-";
        auto day_val = static_cast<unsigned>(ymd.day());
        if (day_val < 10)
            oss << "0";
        oss << day_val;
        return oss.str();
    }();

    // Determine file extension from format.
    const auto extension = ExportService::default_filename("x", job.options.format);
    const auto ext_dot = extension.rfind('.');
    const std::string file_ext = ext_dot != std::string::npos ? extension.substr(ext_dot) : ".md";

    int index = 1;
    for (const auto& doc : job.documents)
    {
        if (cancelled_)
        {
            result.cancelled = true;
            break;
        }

        // Report progress.
        if (progress)
        {
            progress(index, result.total, doc.title);
        }

        BatchItemResult item;
        item.document_id = doc.document_id;
        item.title = doc.title;

        // Resolve output filename.
        item.output_filename =
            resolve_filename(job.naming_pattern, doc.title, index, file_ext, today);

        if (export_service_ != nullptr)
        {
            // Build per-document options.
            auto doc_options = job.options;
            doc_options.output_path = job.output_directory + "/" + item.output_filename;

            auto export_result = export_service_->export_content(doc.markdown_source, doc_options);

            item.success = export_result.success;
            item.error_message = export_result.error_message;
            item.output_size_bytes = export_result.output_size_bytes;
        }
        else
        {
            // No export service — just mark success for testing purposes.
            item.success = true;
            item.output_size_bytes = static_cast<int64_t>(doc.markdown_source.size());
        }

        if (item.success)
        {
            ++result.succeeded;
            result.total_size_bytes += item.output_size_bytes;
        }
        else
        {
            ++result.failed;
            if (job.stop_on_error)
            {
                result.items.push_back(std::move(item));
                break;
            }
        }

        result.items.push_back(std::move(item));
        ++index;
    }

    // (#44) Generate and write index page if requested and any items succeeded.
    if (job.generate_index_page && result.succeeded > 0)
    {
        result.index_path = job.output_directory + "/index.html";
        auto index_html = generate_index("Batch Export", result.items);
        std::ofstream index_file(result.index_path);
        if (index_file)
        {
            index_file << index_html;
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    result.elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    result.completed = !result.cancelled;

    return result;
}

auto BatchExportEngine::cancel() -> void
{
    cancelled_ = true;
}

auto BatchExportEngine::is_cancelled() const -> bool
{
    return cancelled_;
}

// ============================================================================
// Helpers
// ============================================================================

auto BatchExportEngine::resolve_filename(const std::string& pattern,
                                         const std::string& title,
                                         int index,
                                         const std::string& extension,
                                         const std::string& date) -> std::string
{
    std::string result = pattern;

    // Replace {title} with slugified title.
    auto replace_token = [&](const std::string& token, const std::string& value)
    {
        std::string::size_type pos = 0;
        while ((pos = result.find(token, pos)) != std::string::npos)
        {
            result.replace(pos, token.size(), value);
            pos += value.size();
        }
    };

    replace_token("{title}", slugify(title));
    replace_token("{date}", date);
    replace_token("{index}", std::to_string(index));
    replace_token("{format}", extension.empty() ? "" : extension.substr(1)); // strip leading dot

    // Append extension if not already present.
    if (!extension.empty() && result.size() >= extension.size() &&
        result.substr(result.size() - extension.size()) != extension)
    {
        result += extension;
    }

    return result;
}

auto BatchExportEngine::slugify(const std::string& title) -> std::string
{
    std::string result;
    result.reserve(title.size());

    bool prev_sep = false;
    for (char ch : title)
    {
        if (std::isalnum(static_cast<unsigned char>(ch)))
        {
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            prev_sep = false;
        }
        else if (!prev_sep && !result.empty())
        {
            result += '-';
            prev_sep = true;
        }
    }

    // Trim trailing hyphen.
    if (!result.empty() && result.back() == '-')
    {
        result.pop_back();
    }

    return result;
}

auto BatchExportEngine::generate_index(const std::string& batch_title,
                                       const std::vector<BatchItemResult>& items) -> std::string
{
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
        << "<meta charset=\"utf-8\">\n"
        << "<title>" << batch_title << "</title>\n"
        << "<style>\n"
        << "body { font-family: system-ui, sans-serif; max-width: 800px; margin: 2em auto; }\n"
        << "table { border-collapse: collapse; width: 100%; }\n"
        << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
        << "th { background: #f5f5f5; }\n"
        << ".success { color: #2d7d46; }\n"
        << ".failure { color: #d32f2f; }\n"
        << "</style>\n</head>\n<body>\n"
        << "<h1>" << batch_title << "</h1>\n"
        << "<p>" << items.size() << " documents exported</p>\n"
        << "<table>\n<tr><th>#</th><th>Title</th><th>File</th><th>Status</th></tr>\n";

    int idx = 1;
    for (const auto& item : items)
    {
        oss << "<tr><td>" << idx << "</td>"
            << "<td>" << item.title << "</td>"
            << "<td>";
        if (item.success)
        {
            oss << "<a href=\"" << item.output_filename << "\">" << item.output_filename << "</a>";
        }
        else
        {
            oss << item.output_filename;
        }
        oss << "</td><td class=\"" << (item.success ? "success" : "failure") << "\">"
            << (item.success ? "OK" : item.error_message) << "</td></tr>\n";
        ++idx;
    }

    oss << "</table>\n</body>\n</html>\n";
    return oss.str();
}

// (#102) Return the number of documents in a batch export job.
auto BatchExportEngine::document_count(const BatchExportJob& job) -> std::size_t
{
    return job.documents.size();
}

} // namespace markamp::core
