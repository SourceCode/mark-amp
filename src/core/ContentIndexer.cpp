// ============================================================================
// File: src/core/ContentIndexer.cpp
// Phase 36: Asset Management — ContentIndexer implementation
// ============================================================================

#include "ContentIndexer.h"

#include "Events.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace markamp::core
{

// ── ContentIndexer ──────────────────────────────────────────────────────────

ContentIndexer::ContentIndexer(EventBus& event_bus)
    : event_bus_(event_bus)
{
    register_builtins();
}

auto ContentIndexer::register_extractor(std::unique_ptr<IContentExtractor> extractor) -> void
{
    extractors_.push_back(std::move(extractor));
}

auto ContentIndexer::index_asset(const std::string& asset_id,
                                 const std::filesystem::path& file_path,
                                 const std::string& mime_type)
    -> std::expected<ContentIndexEntry, std::string>
{
    auto* extractor = find_extractor(mime_type);
    if (!extractor)
    {
        return std::unexpected("No extractor for MIME type: " + mime_type);
    }

    auto text_result = extractor->extract_text(file_path);
    if (!text_result)
    {
        return std::unexpected(text_result.error());
    }

    ContentIndexEntry entry;
    entry.asset_id = asset_id;
    entry.extracted_text = std::move(*text_result);
    entry.indexed_at = std::chrono::duration_cast<std::chrono::seconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();

    // Remove old entry if re-indexing.
    remove_from_index(asset_id);
    index_.push_back(entry);

    return entry;
}

auto ContentIndexer::search(const std::string& query, int limit) const
    -> std::vector<ContentIndexEntry>
{
    std::vector<ContentIndexEntry> results;
    auto lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    for (const auto& entry : index_)
    {
        auto lower_text = entry.extracted_text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        if (lower_text.find(lower_query) != std::string::npos)
        {
            results.push_back(entry);
            if (static_cast<int>(results.size()) >= limit)
                break;
        }
    }

    return results;
}

auto ContentIndexer::remove_from_index(const std::string& asset_id) -> void
{
    std::erase_if(index_, [&](const ContentIndexEntry& e) { return e.asset_id == asset_id; });
}

auto ContentIndexer::indexed_count() const -> int32_t
{
    return static_cast<int32_t>(index_.size());
}

auto ContentIndexer::can_extract(const std::string& mime_type) const -> bool
{
    return find_extractor(mime_type) != nullptr;
}

auto ContentIndexer::find_extractor(const std::string& mime_type) const -> IContentExtractor*
{
    for (const auto& ext : extractors_)
    {
        auto types = ext->supported_mime_types();
        if (std::find(types.begin(), types.end(), mime_type) != types.end())
        {
            return ext.get();
        }
    }
    return nullptr;
}

auto ContentIndexer::register_builtins() -> void
{
    register_extractor(std::make_unique<PlainTextExtractor>());
    register_extractor(std::make_unique<MarkdownTextExtractor>());
    register_extractor(std::make_unique<PdfTextExtractor>());
}

// ── PlainTextExtractor ──────────────────────────────────────────────────────

auto PlainTextExtractor::supported_mime_types() const -> std::vector<std::string>
{
    return {"text/plain",
            "text/csv",
            "text/css",
            "text/javascript",
            "text/html",
            "application/json",
            "application/xml"};
}

auto PlainTextExtractor::extract_text(const std::filesystem::path& file_path) const
    -> std::expected<std::string, std::string>
{
    std::ifstream file(file_path);
    if (!file)
        return std::unexpected("Cannot open file: " + file_path.string());

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// ── MarkdownTextExtractor ───────────────────────────────────────────────────

auto MarkdownTextExtractor::supported_mime_types() const -> std::vector<std::string>
{
    return {"text/markdown"};
}

auto MarkdownTextExtractor::extract_text(const std::filesystem::path& file_path) const
    -> std::expected<std::string, std::string>
{
    std::ifstream file(file_path);
    if (!file)
        return std::unexpected("Cannot open file: " + file_path.string());

    // Strip markdown formatting for indexing.
    std::string result;
    std::string line;
    while (std::getline(file, line))
    {
        // Strip heading markers.
        auto pos = line.find_first_not_of('#');
        if (pos != std::string::npos && pos > 0 && pos < line.size() && line[pos] == ' ')
        {
            line = line.substr(pos + 1);
        }

        // Strip bold/italic markers.
        std::string cleaned;
        for (size_t i = 0; i < line.size(); ++i)
        {
            if (line[i] == '*' || line[i] == '_' || line[i] == '`')
                continue;
            cleaned += line[i];
        }

        result += cleaned + "\n";
    }

    return result;
}

// ── PdfTextExtractor (stub) ─────────────────────────────────────────────────

auto PdfTextExtractor::supported_mime_types() const -> std::vector<std::string>
{
    return {"application/pdf"};
}

auto PdfTextExtractor::extract_text(const std::filesystem::path& file_path) const
    -> std::expected<std::string, std::string>
{
    // Basic PDF text extraction via stream parsing.
    // Scans for text operators (Tj/TJ) within stream sections.
    std::ifstream file(file_path, std::ios::binary);
    if (!file)
    {
        return std::unexpected("Cannot open PDF file: " + file_path.string());
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    // Verify PDF magic number.
    if (content.size() < 5 || content.substr(0, 5) != "%PDF-")
    {
        return std::unexpected("Not a valid PDF file");
    }

    std::string extracted_text;

    // Find text between stream/endstream markers and look for text operators.
    size_t pos = 0;
    while (pos < content.size())
    {
        auto stream_start = content.find("stream", pos);
        if (stream_start == std::string::npos)
        {
            break;
        }
        stream_start += 6; // skip "stream"
        if (stream_start < content.size() && content[stream_start] == '\r')
        {
            ++stream_start;
        }
        if (stream_start < content.size() && content[stream_start] == '\n')
        {
            ++stream_start;
        }

        auto stream_end = content.find("endstream", stream_start);
        if (stream_end == std::string::npos)
        {
            break;
        }

        std::string stream_data = content.substr(stream_start, stream_end - stream_start);

        // Extract text from parenthesized strings (used by Tj/TJ operators).
        size_t text_pos = 0;
        while (text_pos < stream_data.size())
        {
            auto paren_start = stream_data.find('(', text_pos);
            if (paren_start == std::string::npos)
            {
                break;
            }

            int depth = 1;
            size_t paren_end = paren_start + 1;
            while (paren_end < stream_data.size() && depth > 0)
            {
                if (stream_data[paren_end] == '\\')
                {
                    ++paren_end; // skip escaped char
                }
                else if (stream_data[paren_end] == '(')
                {
                    ++depth;
                }
                else if (stream_data[paren_end] == ')')
                {
                    --depth;
                }
                ++paren_end;
            }

            if (depth == 0)
            {
                std::string text_chunk =
                    stream_data.substr(paren_start + 1, paren_end - paren_start - 2);
                for (char text_char : text_chunk)
                {
                    if (text_char >= 32 && text_char < 127)
                    {
                        extracted_text += text_char;
                    }
                }
            }
            text_pos = paren_end;
        }

        pos = stream_end + 9; // skip "endstream"
    }

    if (extracted_text.empty())
    {
        return std::unexpected("No extractable text found in PDF (may use compressed streams)");
    }

    return extracted_text;
}

// (#180) Return the number of registered content extractors.
auto ContentIndexer::extractor_count() const -> std::size_t
{
    return extractors_.size();
}

// (#181) Return a list of all MIME types with registered extractors.
auto ContentIndexer::supported_mime_types() const -> std::vector<std::string>
{
    std::vector<std::string> all_types;
    for (const auto& ext : extractors_)
    {
        auto types = ext->supported_mime_types();
        all_types.insert(all_types.end(), types.begin(), types.end());
    }
    return all_types;
}

// (#182) Check if the content index is empty.
auto ContentIndexer::is_empty() const -> bool
{
    return index_.empty();
}

} // namespace markamp::core
