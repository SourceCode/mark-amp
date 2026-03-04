#pragma once

/// @file BuildOutputParser.h
/// @brief Phase 38 Task 8 — Parse build output for errors/warnings from multiple compilers.

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Severity of a parsed build diagnostic.
enum class BuildDiagnosticSeverity : uint8_t
{
    kError,
    kWarning,
    kNote,
    kInfo,
};

/// A parsed diagnostic from build output.
struct BuildDiagnostic
{
    std::string file;
    int line{0};
    int column{0};
    BuildDiagnosticSeverity severity{BuildDiagnosticSeverity::kError};
    std::string message;
    std::string error_code; ///< e.g., "-Wunused-variable", "C2065"
    std::string raw_line;   ///< The original output line
};

/// Parse build output from various compilers and build systems.
class BuildOutputParser
{
public:
    BuildOutputParser() = default;

    /// Parse a single line of build output.
    [[nodiscard]] auto parse_line(const std::string& line) const -> std::vector<BuildDiagnostic>;

    /// Parse multiple lines.
    [[nodiscard]] auto parse(const std::string& output) const -> std::vector<BuildDiagnostic>;

    /// Count errors in a set of diagnostics.
    [[nodiscard]] static auto error_count(const std::vector<BuildDiagnostic>& diagnostics) -> int;

    /// Count warnings in a set of diagnostics.
    [[nodiscard]] static auto warning_count(const std::vector<BuildDiagnostic>& diagnostics) -> int;

    /// Format a diagnostic as a human-readable string.
    [[nodiscard]] static auto format_diagnostic(const BuildDiagnostic& diag) -> std::string;

private:
    /// Try GCC/Clang format: "file:line:col: severity: message"
    [[nodiscard]] auto try_gcc_clang(const std::string& line) const -> std::vector<BuildDiagnostic>;

    /// Try MSVC format: "file(line,col): severity C####: message"
    [[nodiscard]] auto try_msvc(const std::string& line) const -> std::vector<BuildDiagnostic>;

    /// Try CMake format: "CMake Error at file:line..."
    [[nodiscard]] auto try_cmake(const std::string& line) const -> std::vector<BuildDiagnostic>;

    /// Parse severity string.
    [[nodiscard]] static auto parse_severity(const std::string& text) -> BuildDiagnosticSeverity;
};

} // namespace markamp::core
