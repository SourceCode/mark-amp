#include "CompilerErrorParser.h"

#include <sstream>

namespace markamp::core
{

CompilerErrorParser::CompilerErrorParser(CompilerType type)
    : type_(type)
{
}

auto CompilerErrorParser::parse_line(const std::string& line) -> std::vector<ParsedCompilerError>
{
    CompilerType effective_type = type_;
    if (effective_type == CompilerType::kAuto)
    {
        effective_type = detect_compiler(line);
    }

    switch (effective_type)
    {
        case CompilerType::kGcc:
        case CompilerType::kClang:
            return parse_gcc_clang(line);
        case CompilerType::kMsvc:
            return parse_msvc(line);
        case CompilerType::kCMake:
            return parse_cmake(line);
        case CompilerType::kAuto:
            // Try GCC/Clang first, then MSVC, then CMake
            {
                auto results = parse_gcc_clang(line);
                if (!results.empty())
                {
                    return results;
                }
                results = parse_msvc(line);
                if (!results.empty())
                {
                    return results;
                }
                return parse_cmake(line);
            }
    }
    return {};
}

auto CompilerErrorParser::parse(const std::string& output) -> std::vector<ParsedCompilerError>
{
    std::vector<ParsedCompilerError> all_errors;
    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line))
    {
        auto errors = parse_line(line);
        all_errors.insert(all_errors.end(), errors.begin(), errors.end());
    }

    return all_errors;
}

auto CompilerErrorParser::to_diagnostics(const std::vector<ParsedCompilerError>& errors)
    -> std::vector<Diagnostic>
{
    std::vector<Diagnostic> diags;
    diags.reserve(errors.size());

    for (const auto& error : errors)
    {
        Diagnostic diag;
        diag.range.start.line = error.line > 0 ? error.line - 1 : 0; // Convert to 0-indexed
        diag.range.start.character = error.column > 0 ? error.column - 1 : 0;
        diag.range.end = diag.range.start;
        diag.message = error.message;
        diag.severity = error.severity;
        diag.source = "compiler";
        diag.code = error.code;
        diags.push_back(std::move(diag));
    }

    return diags;
}

auto CompilerErrorParser::detect_compiler(const std::string& output) -> CompilerType
{
    // MSVC format: file(line): error C1234:
    if (output.find("): error C") != std::string::npos ||
        output.find("): warning C") != std::string::npos ||
        output.find("): fatal error") != std::string::npos)
    {
        return CompilerType::kMsvc;
    }

    // CMake format: CMake Error at or CMake Warning at
    if (output.find("CMake Error") != std::string::npos ||
        output.find("CMake Warning") != std::string::npos)
    {
        return CompilerType::kCMake;
    }

    // GCC/Clang format: file:line:col: error: or file:line: error:
    if (output.find(": error:") != std::string::npos ||
        output.find(": warning:") != std::string::npos ||
        output.find(": note:") != std::string::npos ||
        output.find(": fatal error:") != std::string::npos)
    {
        return CompilerType::kGcc;
    }

    return CompilerType::kAuto;
}

void CompilerErrorParser::set_compiler_type(CompilerType type)
{
    type_ = type;
}

auto CompilerErrorParser::compiler_type() const -> CompilerType
{
    return type_;
}

auto CompilerErrorParser::parse_gcc_clang(const std::string& line)
    -> std::vector<ParsedCompilerError>
{
    // Format: file:line:column: severity: message
    // or:     file:line: severity: message
    static const std::regex kGccPattern(
        R"(^(.+?):(\d+):(\d+):\s*(error|warning|note|fatal error):\s*(.+)$)");
    static const std::regex kGccNoColPattern(
        R"(^(.+?):(\d+):\s*(error|warning|note|fatal error):\s*(.+)$)");

    std::smatch match;
    if (std::regex_match(line, match, kGccPattern))
    {
        ParsedCompilerError error;
        error.file = match[1].str();
        error.line = std::stoi(match[2].str());
        error.column = std::stoi(match[3].str());
        error.severity = parse_severity(match[4].str());
        error.message = match[5].str();
        return {error};
    }

    if (std::regex_match(line, match, kGccNoColPattern))
    {
        ParsedCompilerError error;
        error.file = match[1].str();
        error.line = std::stoi(match[2].str());
        error.severity = parse_severity(match[3].str());
        error.message = match[4].str();
        return {error};
    }

    return {};
}

auto CompilerErrorParser::parse_msvc(const std::string& line) -> std::vector<ParsedCompilerError>
{
    // Format: file(line): error C1234: message
    // or:     file(line,column): error C1234: message
    static const std::regex kMsvcPattern(
        R"(^(.+?)\((\d+)(?:,(\d+))?\)\s*:\s*(error|warning|fatal error)\s+(C\d+):\s*(.+)$)");

    std::smatch match;
    if (std::regex_match(line, match, kMsvcPattern))
    {
        ParsedCompilerError error;
        error.file = match[1].str();
        error.line = std::stoi(match[2].str());
        if (match[3].matched)
        {
            error.column = std::stoi(match[3].str());
        }
        error.severity = parse_severity(match[4].str());
        error.code = match[5].str();
        error.message = match[6].str();
        return {error};
    }

    return {};
}

auto CompilerErrorParser::parse_cmake(const std::string& line) -> std::vector<ParsedCompilerError>
{
    // Format: CMake Error at file:line (command):
    // or:     CMake Warning at file:line:
    static const std::regex kCMakePattern(
        R"(^CMake\s+(Error|Warning)(?:\s+at\s+(.+?):(\d+))?\s*(?:\((.+?)\))?:\s*(.*)$)");

    std::smatch match;
    if (std::regex_match(line, match, kCMakePattern))
    {
        ParsedCompilerError error;
        error.severity = parse_severity(match[1].str());
        if (match[2].matched)
        {
            error.file = match[2].str();
        }
        if (match[3].matched)
        {
            error.line = std::stoi(match[3].str());
        }
        if (match[4].matched)
        {
            error.code = match[4].str();
        }
        error.message = match[5].str();
        return {error};
    }

    return {};
}

auto CompilerErrorParser::parse_severity(const std::string& text) -> DiagnosticSeverity
{
    if (text == "error" || text == "Error" || text == "fatal error")
    {
        return DiagnosticSeverity::kError;
    }
    if (text == "warning" || text == "Warning")
    {
        return DiagnosticSeverity::kWarning;
    }
    if (text == "note")
    {
        return DiagnosticSeverity::kInformation;
    }
    return DiagnosticSeverity::kError;
}

} // namespace markamp::core
