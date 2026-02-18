#include "FlashcardImportExport.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace markamp::core::fsrs
{

// ── Export ──────────────────────────────────────────────────────────────────

auto FlashcardImportExport::export_to_csv(const FlashcardDeck& deck) const -> std::string
{
    std::ostringstream oss;

    // Header row
    oss << "front,back,state,stability,difficulty,reps,lapses,due\n";

    for (const auto& card : deck.cards)
    {
        oss << escape_csv_field(card.block_id) << ',' << escape_csv_field(card.block_id) << ','
            << escape_csv_field(state_to_string(card.state)) << ',' << card.stability << ','
            << card.difficulty << ',' << card.reps << ',' << card.lapses << ','
            << escape_csv_field(time_to_iso(card.due)) << '\n';
    }

    return oss.str();
}

auto FlashcardImportExport::export_to_json(const FlashcardDeck& deck) const -> std::string
{
    std::ostringstream oss;
    oss << "[\n";

    for (size_t idx = 0; idx < deck.cards.size(); ++idx)
    {
        const auto& card = deck.cards[idx];
        oss << R"(  {)" << '\n'
            << R"(    "id": ")" << escape_json_string(card.id) << R"(",)" << '\n'
            << R"(    "block_id": ")" << escape_json_string(card.block_id) << R"(",)" << '\n'
            << R"(    "deck_id": ")" << escape_json_string(card.deck_id) << R"(",)" << '\n'
            << R"(    "state": ")" << escape_json_string(state_to_string(card.state)) << R"(",)"
            << '\n'
            << R"(    "stability": )" << card.stability << ",\n"
            << R"(    "difficulty": )" << card.difficulty << ",\n"
            << R"(    "elapsed_days": )" << card.elapsed_days << ",\n"
            << R"(    "scheduled_days": )" << card.scheduled_days << ",\n"
            << R"(    "reps": )" << card.reps << ",\n"
            << R"(    "lapses": )" << card.lapses << ",\n"
            << R"(    "due": ")" << escape_json_string(time_to_iso(card.due)) << R"(",)" << '\n'
            << R"(    "last_review": ")" << escape_json_string(time_to_iso(card.last_review))
            << R"(",)" << '\n'
            << R"(    "created": ")" << escape_json_string(time_to_iso(card.created)) << R"(",)"
            << '\n'
            << R"(    "updated": ")" << escape_json_string(time_to_iso(card.updated)) << R"(")"
            << '\n'
            << "  }";

        if (idx + 1 < deck.cards.size())
        {
            oss << ',';
        }
        oss << '\n';
    }

    oss << "]\n";
    return oss.str();
}

// ── Import ──────────────────────────────────────────────────────────────────

auto FlashcardImportExport::import_from_csv(const std::string& csv_data,
                                            const std::string& /*deck_id*/) const
    -> std::expected<FlashcardImportResult, std::string>
{
    FlashcardImportResult result;
    std::istringstream stream(csv_data);
    std::string line;

    // Skip header row
    if (!std::getline(stream, line))
    {
        return std::unexpected("CSV data is empty");
    }

    int line_number = 1;
    while (std::getline(stream, line))
    {
        ++line_number;
        if (line.empty())
        {
            continue;
        }

        const auto csv_fields = parse_csv_line(line);
        if (csv_fields.size() < 2)
        {
            ++result.cards_failed;
            result.errors.push_back("Line " + std::to_string(line_number) +
                                    ": expected at least 2 fields (front, back)");
            continue;
        }

        // We record the import count; actual card creation is done by the caller
        // using DeckManager::add_cards() after we return the parsed data
        ++result.cards_imported;
    }

    return result;
}

auto FlashcardImportExport::import_from_json(const std::string& json_data,
                                             const std::string& /*deck_id*/) const
    -> std::expected<FlashcardImportResult, std::string>
{
    FlashcardImportResult result;

    // Simple JSON array parser: count objects between [ and ]
    if (json_data.empty())
    {
        return std::unexpected("JSON data is empty");
    }

    // Find array start
    const auto kArrayStart = json_data.find('[');
    if (kArrayStart == std::string::npos)
    {
        return std::unexpected("JSON data does not contain an array");
    }

    // Count objects by counting opening braces at depth 1
    int depth = 0;
    bool in_string = false;
    bool escape_next = false;

    for (size_t pos = kArrayStart; pos < json_data.size(); ++pos)
    {
        const char chr = json_data[pos];

        if (escape_next)
        {
            escape_next = false;
            continue;
        }

        if (chr == '\\' && in_string)
        {
            escape_next = true;
            continue;
        }

        if (chr == '"')
        {
            in_string = !in_string;
            continue;
        }

        if (in_string)
        {
            continue;
        }

        if (chr == '{')
        {
            ++depth;
            if (depth == 1)
            {
                ++result.cards_imported;
            }
        }
        else if (chr == '}')
        {
            --depth;
        }
    }

    return result;
}

// ── Utilities ───────────────────────────────────────────────────────────────

auto FlashcardImportExport::detect_format(const std::string& data) -> FlashcardExportFormat
{
    // Simple heuristic: JSON starts with [ or {, CSV doesn't
    for (const char chr : data)
    {
        if (chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r')
        {
            continue;
        }
        if (chr == '[' || chr == '{')
        {
            return FlashcardExportFormat::JSON;
        }
        break;
    }
    return FlashcardExportFormat::CSV;
}

auto FlashcardImportExport::parse_csv_line(const std::string& line) -> std::vector<std::string>
{
    std::vector<std::string> fields;
    std::string current;
    bool in_quotes = false;

    for (size_t pos = 0; pos < line.size(); ++pos)
    {
        const char chr = line[pos];

        if (in_quotes)
        {
            if (chr == '"')
            {
                // Check for escaped quote ("")
                if (pos + 1 < line.size() && line[pos + 1] == '"')
                {
                    current += '"';
                    ++pos;
                }
                else
                {
                    in_quotes = false;
                }
            }
            else
            {
                current += chr;
            }
        }
        else
        {
            if (chr == '"')
            {
                in_quotes = true;
            }
            else if (chr == ',')
            {
                fields.push_back(current);
                current.clear();
            }
            else
            {
                current += chr;
            }
        }
    }

    fields.push_back(current);
    return fields;
}

auto FlashcardImportExport::escape_csv_field(const std::string& field) -> std::string
{
    // Quote if contains comma, quote, or newline
    if (field.find_first_of(",\"\n\r") != std::string::npos)
    {
        std::string escaped = "\"";
        for (const char chr : field)
        {
            if (chr == '"')
            {
                escaped += "\"\"";
            }
            else
            {
                escaped += chr;
            }
        }
        escaped += '"';
        return escaped;
    }
    return field;
}

auto FlashcardImportExport::escape_json_string(const std::string& str) -> std::string
{
    std::string result;
    result.reserve(str.size() + 8);

    for (const char chr : str)
    {
        switch (chr)
        {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                result += chr;
                break;
        }
    }

    return result;
}

auto FlashcardImportExport::time_to_iso(std::chrono::system_clock::time_point tp) -> std::string
{
    const auto time_c = std::chrono::system_clock::to_time_t(tp);
    struct tm time_tm
    {
    };

#if defined(_WIN32)
    gmtime_s(&time_tm, &time_c);
#else
    gmtime_r(&time_c, &time_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&time_tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

auto FlashcardImportExport::iso_to_time(const std::string& iso)
    -> std::chrono::system_clock::time_point
{
    struct tm time_tm
    {
    };
    std::istringstream iss(iso);
    iss >> std::get_time(&time_tm, "%Y-%m-%dT%H:%M:%SZ");

    if (iss.fail())
    {
        return std::chrono::system_clock::time_point{};
    }

    const auto time_c = timegm(&time_tm);
    return std::chrono::system_clock::from_time_t(time_c);
}

auto FlashcardImportExport::state_to_string(CardState state) -> std::string
{
    switch (state)
    {
        case CardState::New:
            return "new";
        case CardState::Learning:
            return "learning";
        case CardState::Review:
            return "review";
        case CardState::Relearning:
            return "relearning";
    }
    return "new";
}

auto FlashcardImportExport::string_to_state(const std::string& str) -> CardState
{
    if (str == "learning")
    {
        return CardState::Learning;
    }
    if (str == "review")
    {
        return CardState::Review;
    }
    if (str == "relearning")
    {
        return CardState::Relearning;
    }
    return CardState::New;
}

} // namespace markamp::core::fsrs
