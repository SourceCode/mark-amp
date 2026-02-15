#include "SqlQuery.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════
// SqlValidator
// ════════════════════════════════════════════════════════════

auto SqlValidator::first_keyword(std::string_view sql) const -> std::string
{
    auto sanitized = sanitize(sql);
    std::istringstream stream(sanitized);
    std::string keyword;
    stream >> keyword;

    std::transform(keyword.begin(),
                   keyword.end(),
                   keyword.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::toupper(chr)); });
    return keyword;
}

auto SqlValidator::classify(std::string_view sql) const -> SqlStatementType
{
    const auto keyword = first_keyword(sql);

    if (keyword == "SELECT")
    {
        return SqlStatementType::Select;
    }
    if (keyword == "INSERT")
    {
        return SqlStatementType::Insert;
    }
    if (keyword == "UPDATE")
    {
        return SqlStatementType::Update;
    }
    if (keyword == "DELETE")
    {
        return SqlStatementType::Delete;
    }
    if (keyword == "CREATE")
    {
        return SqlStatementType::Create;
    }
    if (keyword == "DROP")
    {
        return SqlStatementType::Drop;
    }
    if (keyword == "ALTER")
    {
        return SqlStatementType::Alter;
    }
    if (keyword == "PRAGMA")
    {
        return SqlStatementType::Pragma;
    }
    if (keyword == "ATTACH")
    {
        return SqlStatementType::Attach;
    }
    if (keyword == "DETACH")
    {
        return SqlStatementType::Detach;
    }

    return SqlStatementType::Unknown;
}

auto SqlValidator::validate(std::string_view sql) const -> std::string
{
    if (sql.empty())
    {
        return "SQL query cannot be empty";
    }

    const auto stmt_type = classify(sql);

    switch (stmt_type)
    {
        case SqlStatementType::Select:
            break; // Allowed
        case SqlStatementType::Insert:
            return "INSERT statements are not allowed (read-only access)";
        case SqlStatementType::Update:
            return "UPDATE statements are not allowed (read-only access)";
        case SqlStatementType::Delete:
            return "DELETE statements are not allowed (read-only access)";
        case SqlStatementType::Create:
            return "CREATE statements are not allowed (read-only access)";
        case SqlStatementType::Drop:
            return "DROP statements are not allowed (read-only access)";
        case SqlStatementType::Alter:
            return "ALTER statements are not allowed (read-only access)";
        case SqlStatementType::Pragma:
            return "PRAGMA statements are not allowed for security";
        case SqlStatementType::Attach:
            return "ATTACH statements are not allowed (prevents accessing other databases)";
        case SqlStatementType::Detach:
            return "DETACH statements are not allowed";
        case SqlStatementType::Unknown:
            return "Unrecognized SQL statement type";
    }

    if (has_injection_pattern(sql))
    {
        return "SQL query contains potentially dangerous patterns";
    }

    return {}; // Valid
}

auto SqlValidator::is_read_only(std::string_view sql) const -> bool
{
    const auto stmt_type = classify(sql);
    return stmt_type == SqlStatementType::Select;
}

namespace
{

/// Process a single character in the sanitizer state machine.
/// Returns true if the character should be appended, false if consumed by comment state.
auto process_sanitize_char(char current,
                           char next,
                           bool has_next,
                           bool& in_line_comment,
                           bool& in_block_comment,
                           bool& skip_next) -> bool
{
    if (in_block_comment)
    {
        if (current == '*' && has_next && next == '/')
        {
            in_block_comment = false;
            skip_next = true;
        }
        return false;
    }

    if (in_line_comment)
    {
        if (current == '\n')
        {
            in_line_comment = false;
        }
        return false;
    }

    if (current == '-' && has_next && next == '-')
    {
        in_line_comment = true;
        return false;
    }
    if (current == '/' && has_next && next == '*')
    {
        in_block_comment = true;
        skip_next = true;
        return false;
    }

    return true;
}

} // anonymous namespace

auto SqlValidator::sanitize(std::string_view sql) const -> std::string
{
    std::string result;
    result.reserve(sql.size());

    bool in_line_comment = false;
    bool in_block_comment = false;
    bool prev_was_space = false;

    for (std::size_t idx = 0; idx < sql.size(); ++idx)
    {
        const char current = sql[idx];
        const bool has_next = (idx + 1 < sql.size());
        const char next = has_next ? sql[idx + 1] : '\0';
        bool skip_next = false;

        if (!process_sanitize_char(
                current, next, has_next, in_line_comment, in_block_comment, skip_next))
        {
            if (skip_next)
            {
                ++idx;
            }
            continue;
        }

        if (skip_next)
        {
            ++idx;
        }

        if (std::isspace(static_cast<unsigned char>(current)) != 0)
        {
            if (!prev_was_space && !result.empty())
            {
                result += ' ';
                prev_was_space = true;
            }
        }
        else
        {
            result += current;
            prev_was_space = false;
        }
    }

    while (!result.empty() && result.back() == ' ')
    {
        result.pop_back();
    }

    return result;
}

auto SqlValidator::has_injection_pattern(std::string_view sql) const -> bool
{
    const auto sanitized = sanitize(sql);
    std::string upper = sanitized;
    std::transform(upper.begin(),
                   upper.end(),
                   upper.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::toupper(chr)); });

    const auto semicolon_pos = upper.find(';');
    if (semicolon_pos != std::string::npos && semicolon_pos != upper.size() - 1)
    {
        const auto after = upper.substr(semicolon_pos + 1);
        if (!after.empty() && after.find_first_not_of(' ') != std::string::npos)
        {
            return true;
        }
    }

    return false;
}

} // namespace markamp::core
