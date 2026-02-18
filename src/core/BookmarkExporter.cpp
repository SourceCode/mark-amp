/// @file BookmarkExporter.cpp
/// @brief V9 Phase 37 — BookmarkExporter implementation.

#include "BookmarkExporter.h"

#include <sstream>

namespace markamp::core
{

// ── Public API ──────────────────────────────────────────────────────────────

auto BookmarkExporter::export_bookmarks(const std::vector<BookmarkGroup>& groups,
                                        BookmarkExportFormat format) const -> std::string
{
    switch (format)
    {
        case BookmarkExportFormat::kMarkdown:
            return export_markdown(groups);
        case BookmarkExportFormat::kJson:
            return export_json(groups);
        case BookmarkExportFormat::kCsv:
            return export_csv(groups);
        case BookmarkExportFormat::kOpml:
            return export_opml(groups);
    }
    return "";
}

auto BookmarkExporter::export_label(const BookmarkGroup& group, BookmarkExportFormat format) const
    -> std::string
{
    return export_bookmarks({group}, format);
}

auto BookmarkExporter::import_bookmarks(const std::string& data, BookmarkExportFormat format) const
    -> BookmarkImportResult
{
    switch (format)
    {
        case BookmarkExportFormat::kJson:
            return import_json(data);
        case BookmarkExportFormat::kCsv:
            return import_csv(data);
        case BookmarkExportFormat::kMarkdown:
        case BookmarkExportFormat::kOpml:
            return BookmarkImportResult{.error = "Import not supported for this format"};
    }
    return BookmarkImportResult{.error = "Unknown format"};
}

auto BookmarkExporter::format_name(BookmarkExportFormat format) -> std::string
{
    switch (format)
    {
        case BookmarkExportFormat::kMarkdown:
            return "Markdown";
        case BookmarkExportFormat::kJson:
            return "JSON";
        case BookmarkExportFormat::kCsv:
            return "CSV";
        case BookmarkExportFormat::kOpml:
            return "OPML";
    }
    return "Unknown";
}

auto BookmarkExporter::format_extension(BookmarkExportFormat format) -> std::string
{
    switch (format)
    {
        case BookmarkExportFormat::kMarkdown:
            return "md";
        case BookmarkExportFormat::kJson:
            return "json";
        case BookmarkExportFormat::kCsv:
            return "csv";
        case BookmarkExportFormat::kOpml:
            return "opml";
    }
    return "txt";
}

// ── Markdown ────────────────────────────────────────────────────────────────

auto BookmarkExporter::export_markdown(const std::vector<BookmarkGroup>& groups) const
    -> std::string
{
    std::ostringstream oss;
    oss << "# Bookmarks\n\n";

    for (const auto& group : groups)
    {
        oss << "## " << group.label << "\n\n";
        for (const auto& entry : group.entries)
        {
            oss << "- **" << entry.doc_title << "** — " << entry.content_snippet << "\n";
        }
        oss << "\n";
    }

    return oss.str();
}

// ── JSON ────────────────────────────────────────────────────────────────────

auto BookmarkExporter::export_json(const std::vector<BookmarkGroup>& groups) const -> std::string
{
    std::ostringstream oss;
    oss << "{\n  \"bookmarks\": [\n";

    bool first_group = true;
    for (const auto& group : groups)
    {
        for (const auto& entry : group.entries)
        {
            if (!first_group)
            {
                oss << ",\n";
            }
            first_group = false;
            oss << "    {\n";
            oss << "      \"label\": \"" << entry.label << "\",\n";
            oss << "      \"block_id\": \"" << entry.block_id.value << "\",\n";
            oss << "      \"root_id\": \"" << entry.root_id << "\",\n";
            oss << "      \"doc_title\": \"" << entry.doc_title << "\",\n";
            oss << "      \"block_type\": \"" << entry.block_type << "\",\n";
            oss << "      \"content_snippet\": \"" << entry.content_snippet << "\"\n";
            oss << "    }";
        }
    }

    oss << "\n  ]\n}\n";
    return oss.str();
}

// ── CSV ─────────────────────────────────────────────────────────────────────

auto BookmarkExporter::export_csv(const std::vector<BookmarkGroup>& groups) const -> std::string
{
    std::ostringstream oss;
    oss << "label,block_id,root_id,doc_title,block_type,content_snippet\n";

    for (const auto& group : groups)
    {
        for (const auto& entry : group.entries)
        {
            oss << "\"" << entry.label << "\","
                << "\"" << entry.block_id.value << "\","
                << "\"" << entry.root_id << "\","
                << "\"" << entry.doc_title << "\","
                << "\"" << entry.block_type << "\","
                << "\"" << entry.content_snippet << "\"\n";
        }
    }

    return oss.str();
}

// ── OPML ────────────────────────────────────────────────────────────────────

auto BookmarkExporter::export_opml(const std::vector<BookmarkGroup>& groups) const -> std::string
{
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<opml version=\"2.0\">\n";
    oss << "  <head>\n    <title>MarkAmp Bookmarks</title>\n  </head>\n";
    oss << "  <body>\n";

    for (const auto& group : groups)
    {
        oss << "    <outline text=\"" << group.label << "\">\n";
        for (const auto& entry : group.entries)
        {
            oss << "      <outline text=\"" << entry.doc_title << "\" description=\""
                << entry.content_snippet << "\" type=\"" << entry.block_type << "\" />\n";
        }
        oss << "    </outline>\n";
    }

    oss << "  </body>\n</opml>\n";
    return oss.str();
}

// ── JSON Import ─────────────────────────────────────────────────────────────

auto BookmarkExporter::import_json(const std::string& data) const -> BookmarkImportResult
{
    BookmarkImportResult result;

    // Simple JSON parsing: look for bookmark entries
    std::string::size_type pos = 0;
    while ((pos = data.find("\"label\":", pos)) != std::string::npos)
    {
        BookmarkEntry entry;

        // Extract label
        auto label_start = data.find('"', pos + 8);
        if (label_start == std::string::npos)
        {
            break;
        }
        auto label_end = data.find('"', label_start + 1);
        if (label_end == std::string::npos)
        {
            break;
        }
        entry.label = data.substr(label_start + 1, label_end - label_start - 1);

        // Extract block_id
        auto bid_key = data.find("\"block_id\":", pos);
        if (bid_key != std::string::npos && bid_key < pos + 500)
        {
            auto bid_start = data.find('"', bid_key + 11);
            auto bid_end = data.find('"', bid_start + 1);
            if (bid_start != std::string::npos && bid_end != std::string::npos)
            {
                entry.block_id.value = data.substr(bid_start + 1, bid_end - bid_start - 1);
            }
        }

        // Extract doc_title
        auto dt_key = data.find("\"doc_title\":", pos);
        if (dt_key != std::string::npos && dt_key < pos + 500)
        {
            auto dt_start = data.find('"', dt_key + 12);
            auto dt_end = data.find('"', dt_start + 1);
            if (dt_start != std::string::npos && dt_end != std::string::npos)
            {
                entry.doc_title = data.substr(dt_start + 1, dt_end - dt_start - 1);
            }
        }

        // Extract content_snippet
        auto cs_key = data.find("\"content_snippet\":", pos);
        if (cs_key != std::string::npos && cs_key < pos + 500)
        {
            auto cs_start = data.find('"', cs_key + 18);
            auto cs_end = data.find('"', cs_start + 1);
            if (cs_start != std::string::npos && cs_end != std::string::npos)
            {
                entry.content_snippet = data.substr(cs_start + 1, cs_end - cs_start - 1);
            }
        }

        result.entries.push_back(std::move(entry));
        result.imported_count++;
        pos = label_end + 1;
    }

    if (result.entries.empty() && !data.empty())
    {
        result.error = "No bookmark entries found in JSON";
    }

    return result;
}

// ── CSV Import ──────────────────────────────────────────────────────────────

auto BookmarkExporter::import_csv(const std::string& data) const -> BookmarkImportResult
{
    BookmarkImportResult result;
    std::istringstream iss(data);
    std::string line;

    // Skip header
    if (!std::getline(iss, line))
    {
        result.error = "Empty CSV data";
        return result;
    }

    while (std::getline(iss, line))
    {
        if (line.empty())
        {
            continue;
        }

        BookmarkEntry entry;

        // Simple CSV parsing: read quoted fields
        std::vector<std::string> fields;
        std::string field;
        bool in_quotes = false;

        for (char character : line)
        {
            if (character == '"')
            {
                in_quotes = !in_quotes;
            }
            else if (character == ',' && !in_quotes)
            {
                fields.push_back(field);
                field.clear();
            }
            else
            {
                field += character;
            }
        }
        fields.push_back(field);

        if (fields.size() >= 6)
        {
            entry.label = fields[0];
            entry.block_id.value = fields[1];
            entry.root_id = fields[2];
            entry.doc_title = fields[3];
            entry.block_type = fields[4];
            entry.content_snippet = fields[5];
            result.entries.push_back(std::move(entry));
            result.imported_count++;
        }
        else
        {
            result.skipped_count++;
        }
    }

    return result;
}

} // namespace markamp::core
