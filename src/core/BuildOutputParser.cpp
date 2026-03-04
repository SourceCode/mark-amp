/// @file BuildOutputParser.cpp
/// @brief Phase 38 Task 8 — Build output parser implementation.

#include "core/BuildOutputParser.h"

#include <regex>
#include <sstream>

namespace markamp::core
{

auto BuildOutputParser::parse_line(const std::string& line) const -> std::vector<BuildDiagnostic>
{
    // Try each format in order of specificity
    auto result = try_gcc_clang(line);
    if (!result.empty())
        return result;

    result = try_msvc(line);
    if (!result.empty())
        return result;

    result = try_cmake(line);
    return result;
}

auto BuildOutputParser::parse(const std::string& output) const -> std::vector<BuildDiagnostic>
{
    std::vector<BuildDiagnostic> all;
    std::istringstream stream(output);
    std::string line;
    while (std::getline(stream, line))
    {
        auto diagnostics = parse_line(line);
        all.insert(all.end(), diagnostics.begin(), diagnostics.end());
    }
    return all;
}

auto BuildOutputParser::try_gcc_clang(const std::string& line) const -> std::vector<BuildDiagnostic>
{
    // Pattern: "file:line:col: severity: message"
    // Also: "file:line: severity: message" (no column)
    static const std::regex re(
        R"(^(.+?):(\d+):(?:(\d+):)?\s*(error|warning|note|fatal error):\s*(.+))");

    std::smatch match;
    if (!std::regex_match(line, match, re))
        return {};

    BuildDiagnostic diag;
    diag.file = match[1].str();
    diag.line = std::stoi(match[2].str());
    if (match[3].matched)
        diag.column = std::stoi(match[3].str());
    diag.severity = parse_severity(match[4].str());
    diag.message = match[5].str();
    diag.raw_line = line;

    // Extract error code from message like "[-Wunused-variable]"
    static const std::regex code_re(R"(\[(-W[a-z-]+)\]\s*$)");
    std::smatch code_match;
    if (std::regex_search(diag.message, code_match, code_re))
    {
        diag.error_code = code_match[1].str();
    }

    return {diag};
}

auto BuildOutputParser::try_msvc(const std::string& line) const -> std::vector<BuildDiagnostic>
{
    // Pattern: "file(line,col): severity C####: message"
    // Also: "file(line): severity C####: message"
    static const std::regex re(
        R"(^(.+?)\((\d+)(?:,(\d+))?\)\s*:\s*(error|warning)\s+(C\d+):\s*(.+))");

    std::smatch match;
    if (!std::regex_match(line, match, re))
        return {};

    BuildDiagnostic diag;
    diag.file = match[1].str();
    diag.line = std::stoi(match[2].str());
    if (match[3].matched)
        diag.column = std::stoi(match[3].str());
    diag.severity = parse_severity(match[4].str());
    diag.error_code = match[5].str();
    diag.message = match[6].str();
    diag.raw_line = line;

    return {diag};
}

auto BuildOutputParser::try_cmake(const std::string& line) const -> std::vector<BuildDiagnostic>
{
    // Pattern: "CMake Error at file:line (command):" or "CMake Warning at file:line:"
    static const std::regex re(
        R"(^CMake\s+(Error|Warning)\s+at\s+(.+?):(\d+)\s*(?:\(.*\))?\s*:\s*(.*))");

    std::smatch match;
    if (!std::regex_match(line, match, re))
        return {};

    BuildDiagnostic diag;
    diag.severity = (match[1].str() == "Error") ? BuildDiagnosticSeverity::kError
                                                : BuildDiagnosticSeverity::kWarning;
    diag.file = match[2].str();
    diag.line = std::stoi(match[3].str());
    diag.message = match[4].str();
    diag.raw_line = line;

    return {diag};
}

auto BuildOutputParser::parse_severity(const std::string& text) -> BuildDiagnosticSeverity
{
    if (text == "error" || text == "fatal error")
        return BuildDiagnosticSeverity::kError;
    if (text == "warning")
        return BuildDiagnosticSeverity::kWarning;
    if (text == "note")
        return BuildDiagnosticSeverity::kNote;
    return BuildDiagnosticSeverity::kInfo;
}

auto BuildOutputParser::error_count(const std::vector<BuildDiagnostic>& diagnostics) -> int
{
    int count = 0;
    for (const auto& d : diagnostics)
    {
        if (d.severity == BuildDiagnosticSeverity::kError)
            ++count;
    }
    return count;
}

auto BuildOutputParser::warning_count(const std::vector<BuildDiagnostic>& diagnostics) -> int
{
    int count = 0;
    for (const auto& d : diagnostics)
    {
        if (d.severity == BuildDiagnosticSeverity::kWarning)
            ++count;
    }
    return count;
}

auto BuildOutputParser::format_diagnostic(const BuildDiagnostic& diag) -> std::string
{
    std::ostringstream ss;
    ss << diag.file << ":" << diag.line;
    if (diag.column > 0)
        ss << ":" << diag.column;
    ss << ": ";
    switch (diag.severity)
    {
        case BuildDiagnosticSeverity::kError:
            ss << "error";
            break;
        case BuildDiagnosticSeverity::kWarning:
            ss << "warning";
            break;
        case BuildDiagnosticSeverity::kNote:
            ss << "note";
            break;
        case BuildDiagnosticSeverity::kInfo:
            ss << "info";
            break;
    }
    ss << ": " << diag.message;
    if (!diag.error_code.empty())
        ss << " [" << diag.error_code << "]";
    return ss.str();
}

} // namespace markamp::core
