#include "CanvasSearch.h"

#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/ConnectorData.h"
#include "canvas/FrameObject.h"
#include "canvas/SectionObject.h"
#include "canvas/ShapeData.h"
#include "canvas/StickyNote.h"
#include "canvas/TableObject.h"
#include "canvas/TextBox.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::canvas
{

namespace
{

auto to_lower(const std::string& str) -> std::string
{
    std::string result = str;
    std::transform(result.begin(),
                   result.end(),
                   result.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
    return result;
}

/// Build a context snippet around [offset, offset+length) in full_text.
auto build_context(const std::string& full_text, int offset, int length) -> std::string
{
    static constexpr int kContextChars = 30;
    const int ctx_start = std::max(0, offset - kContextChars);
    const int ctx_end =
        std::min(static_cast<int>(full_text.size()), offset + length + kContextChars);
    return full_text.substr(static_cast<size_t>(ctx_start),
                            static_cast<size_t>(ctx_end - ctx_start));
}

} // anonymous namespace

auto CanvasSearch::extract_text(const CanvasObject& obj) -> std::string
{
    switch (obj.type())
    {
        case CanvasObjectType::StickyNote:
        {
            const auto* sticky = dynamic_cast<const StickyNote*>(&obj);
            return sticky != nullptr ? sticky->text() : obj.name();
        }
        case CanvasObjectType::TextBox:
        {
            const auto* text_box = dynamic_cast<const TextBox*>(&obj);
            return text_box != nullptr ? text_box->text() : obj.name();
        }
        case CanvasObjectType::Shape:
        {
            const auto* shape = dynamic_cast<const ShapeObject*>(&obj);
            return shape != nullptr ? shape->text() : obj.name();
        }
        case CanvasObjectType::Connector:
        {
            const auto* connector = dynamic_cast<const ConnectorObject*>(&obj);
            return connector != nullptr ? connector->label() : obj.name();
        }
        case CanvasObjectType::Frame:
        {
            const auto* frame = dynamic_cast<const FrameObject*>(&obj);
            return frame != nullptr ? frame->title() : obj.name();
        }
        case CanvasObjectType::Section:
        {
            const auto* section = dynamic_cast<const SectionObject*>(&obj);
            return section != nullptr ? section->title() : obj.name();
        }
        case CanvasObjectType::Table:
        {
            const auto* table = dynamic_cast<const TableObject*>(&obj);
            if (table == nullptr)
            {
                return obj.name();
            }
            std::ostringstream text_stream;
            for (size_t col = 0; col < table->column_count(); ++col)
            {
                text_stream << table->column(col).header << " ";
            }
            for (size_t row = 0; row < table->row_count(); ++row)
            {
                for (size_t col = 0; col < table->column_count(); ++col)
                {
                    text_stream << table->get_cell(row, col).text << " ";
                }
            }
            return text_stream.str();
        }
        default:
            return obj.name();
    }
}

auto CanvasSearch::search(const Board& board, const std::string& query, bool case_sensitive)
    -> std::vector<SearchResult>
{
    if (query.empty())
    {
        return {};
    }

    std::vector<SearchResult> results;

    for (const auto& obj : board.objects())
    {
        if (!obj)
        {
            continue;
        }

        const std::string obj_text = extract_text(*obj);
        const std::string haystack = case_sensitive ? obj_text : to_lower(obj_text);
        const std::string needle = case_sensitive ? query : to_lower(query);

        size_t pos = 0;
        while ((pos = haystack.find(needle, pos)) != std::string::npos)
        {
            SearchResult result;
            result.object_id = obj->id();
            result.match_offset = static_cast<int>(pos);
            result.match_length = static_cast<int>(query.size());
            result.matched_text = obj_text.substr(pos, query.size());
            result.context = build_context(obj_text, result.match_offset, result.match_length);
            results.push_back(std::move(result));
            pos += query.size();
        }
    }

    return results;
}

// --- (#27) search_by_type ---

auto CanvasSearch::search_by_type(const Board& board, CanvasObjectType obj_type)
    -> std::vector<ObjectId>
{
    std::vector<ObjectId> ids;
    for (const auto& obj : board.objects())
    {
        if (obj && obj->type() == obj_type)
        {
            ids.push_back(obj->id());
        }
    }
    return ids;
}

// --- (#28) search_by_tag ---

auto CanvasSearch::search_by_tag(const Board& board, const std::string& tag)
    -> std::vector<ObjectId>
{
    std::vector<ObjectId> ids;
    for (const auto& obj : board.objects())
    {
        if (!obj)
        {
            continue;
        }
        for (const auto& obj_tag : obj->tags())
        {
            if (obj_tag == tag)
            {
                ids.push_back(obj->id());
                break;
            }
        }
    }
    return ids;
}

// --- (#29) replace_text ---

auto CanvasSearch::replace_text(Board& board,
                                const std::string& find_str,
                                const std::string& replace_str,
                                bool case_sensitive) -> int
{
    // Stub: iterate objects, find matching text, and replace.
    // Full implementation would mutate text on StickyNote, TextBox, etc.
    // For now, return 0 replacements.
    (void)board;
    (void)find_str;
    (void)replace_str;
    (void)case_sensitive;
    return 0;
}

} // namespace markamp::canvas
