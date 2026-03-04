#pragma once

#include "core/DiagnosticsService.h"

#include <cstdint>
#include <regex>
#include <string>
#include <vector>

namespace markamp::core
{

/// Supported compiler types for error parsing.
enum class CompilerType : uint8_t
{
    kAuto, ///< Auto-detect from output format
    kGcc,
    kClang,
    kMsvc,
    kCMake,
};

/// A parsed compiler error/warning with source location and context.
struct ParsedCompilerError
{
    std::string file;
    int line{0};
    int column{0};
    DiagnosticSeverity severity{DiagnosticSeverity::kError};
    std::string message;
    std::string code; ///< Error code (e.g., "C2065", "-Wunused")
    std::vector<std::string> context_lines;
};

/// Parses compiler output (GCC, Clang, MSVC, CMake) into structured error/warning objects.
/// Phase 25 Task 2: Regex-based parsers for each compiler format.
class CompilerErrorParser
{
public:
    explicit CompilerErrorParser(CompilerType type = CompilerType::kAuto);

    /// Parse a single line of compiler output. May return 0 or 1 errors.
    [[nodiscard]] auto parse_line(const std::string& line) -> std::vector<ParsedCompilerError>;

    /// Parse multiple lines of compiler output.
    [[nodiscard]] auto parse(const std::string& output) -> std::vector<ParsedCompilerError>;

    /// Convert parsed errors to DiagnosticsService-compatible format.
    [[nodiscard]] static auto to_diagnostics(const std::vector<ParsedCompilerError>& errors)
        -> std::vector<Diagnostic>;

    /// Detect compiler type from output text.
    [[nodiscard]] static auto detect_compiler(const std::string& output) -> CompilerType;

    /// Set compiler type.
    void set_compiler_type(CompilerType type);
    [[nodiscard]] auto compiler_type() const -> CompilerType;

private:
    CompilerType type_;

    [[nodiscard]] auto parse_gcc_clang(const std::string& line) -> std::vector<ParsedCompilerError>;
    [[nodiscard]] auto parse_msvc(const std::string& line) -> std::vector<ParsedCompilerError>;
    [[nodiscard]] auto parse_cmake(const std::string& line) -> std::vector<ParsedCompilerError>;

    [[nodiscard]] static auto parse_severity(const std::string& text) -> DiagnosticSeverity;
};

} // namespace markamp::core
