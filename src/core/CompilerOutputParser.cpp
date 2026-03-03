#include "CompilerOutputParser.h"

#include <sstream>

namespace markamp::core
{

// gcc/clang: /path/to/file.cpp:42:10: error: undeclared identifier 'foo'
const std::regex CompilerOutputParser::kGccClangPattern(
    R"((.+):(\d+):(\d+):\s+(error|warning|note|info):\s+(.+))");

// MSVC: file.cpp(42,10): error C2065: 'foo': undeclared identifier
const std::regex
    CompilerOutputParser::kMsvcPattern(R"((.+)\((\d+),(\d+)\):\s+(error|warning)\s+\w+:\s+(.+))");

auto CompilerOutputParser::ParseLine(const std::string& line) -> std::vector<CompilerDiagnostic>
{
    std::vector<CompilerDiagnostic> results;
    std::smatch match;

    if (std::regex_search(line, match, kGccClangPattern))
    {
        CompilerDiagnostic diag;
        diag.file_path = match[1].str();
        diag.line = std::stoi(match[2].str());
        diag.column = std::stoi(match[3].str());
        diag.message = match[5].str();
        diag.raw_line = line;

        const std::string severity_str = match[4].str();
        if (severity_str == "error")
        {
            diag.severity = DiagnosticSeverity::kError;
        }
        else if (severity_str == "warning")
        {
            diag.severity = DiagnosticSeverity::kWarning;
        }
        else if (severity_str == "note")
        {
            diag.severity = DiagnosticSeverity::kNote;
        }
        else
        {
            diag.severity = DiagnosticSeverity::kInfo;
        }

        results.push_back(std::move(diag));
    }
    else if (std::regex_search(line, match, kMsvcPattern))
    {
        CompilerDiagnostic diag;
        diag.file_path = match[1].str();
        diag.line = std::stoi(match[2].str());
        diag.column = std::stoi(match[3].str());
        diag.message = match[5].str();
        diag.raw_line = line;

        const std::string severity_str = match[4].str();
        diag.severity =
            (severity_str == "error") ? DiagnosticSeverity::kError : DiagnosticSeverity::kWarning;

        results.push_back(std::move(diag));
    }

    return results;
}

auto CompilerOutputParser::ParseOutput(const std::string& full_output)
    -> std::vector<CompilerDiagnostic>
{
    std::vector<CompilerDiagnostic> all_diagnostics;
    std::istringstream stream(full_output);
    std::string current_line;

    while (std::getline(stream, current_line))
    {
        auto diagnostics = ParseLine(current_line);
        for (auto& diag : diagnostics)
        {
            all_diagnostics.push_back(std::move(diag));
        }
    }

    return all_diagnostics;
}

auto CompilerOutputParser::IsDiagnosticLine(const std::string& line) -> bool
{
    // Quick heuristic: contains "error:" or "warning:" or "note:"
    return line.find("error:") != std::string::npos || line.find("warning:") != std::string::npos ||
           line.find("note:") != std::string::npos;
}

} // namespace markamp::core
