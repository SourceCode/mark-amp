#pragma once

#include <regex>
#include <string>
#include <vector>

namespace markamp::core
{

/// Severity of a compiler diagnostic.
enum class DiagnosticSeverity
{
    kError,
    kWarning,
    kNote,
    kInfo
};

/// A parsed compiler diagnostic message.
struct CompilerDiagnostic
{
    std::string file_path;
    int line{0};
    int column{0};
    DiagnosticSeverity severity{DiagnosticSeverity::kError};
    std::string message;
    std::string raw_line; // Original compiler output line
};

/// Phase 19 Task 16: Parses compiler output (gcc/clang format) into
/// structured diagnostics for the Problems panel.
class CompilerOutputParser
{
public:
    /// Parse a single line of compiler output.
    /// Returns a diagnostic if the line matches a known error format,
    /// or none if it's not a diagnostic line.
    [[nodiscard]] static auto ParseLine(const std::string& line) -> std::vector<CompilerDiagnostic>;

    /// Parse multiple lines of output at once.
    [[nodiscard]] static auto ParseOutput(const std::string& full_output)
        -> std::vector<CompilerDiagnostic>;

    /// Check if a line looks like a diagnostic (heuristic).
    [[nodiscard]] static auto IsDiagnosticLine(const std::string& line) -> bool;

private:
    /// Regex for gcc/clang format: file:line:col: severity: message
    static const std::regex kGccClangPattern;

    /// Regex for MSVC format: file(line,col): severity Cxxxx: message
    static const std::regex kMsvcPattern;
};

} // namespace markamp::core
