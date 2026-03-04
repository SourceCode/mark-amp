#pragma once

/// @file CompilerConfigValidator.h
/// @brief Phase 37 Task 18 — Validate compiler configuration for correctness.

#include "CompilerConfig.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Severity of a validation result.
enum class ValidationSeverity : uint8_t
{
    kInfo,
    kWarning,
    kError,
};

/// A single validation result with severity, message, and optional fix.
struct ValidationResult
{
    ValidationSeverity severity{ValidationSeverity::kInfo};
    std::string category; ///< e.g., "Compiler", "Paths", "Sanitizers"
    std::string message;
    std::string fix_suggestion; ///< e.g., "Install clang via: brew install llvm"
};

/// Validates a CompilerConfig for correctness and consistency.
class CompilerConfigValidator
{
public:
    CompilerConfigValidator() = default;

    /// Run all validations and return results.
    [[nodiscard]] auto validate(const CompilerConfig& config) const
        -> std::vector<ValidationResult>;

    /// Validate only the compiler path.
    [[nodiscard]] auto validate_compiler_path(const CompilerConfig& config) const
        -> std::vector<ValidationResult>;

    /// Validate include and library paths.
    [[nodiscard]] auto validate_paths(const CompilerConfig& config) const
        -> std::vector<ValidationResult>;

    /// Validate sanitizer selection (mutual exclusion checks).
    [[nodiscard]] auto validate_sanitizers(const CompilerConfig& config) const
        -> std::vector<ValidationResult>;

    /// Validate PCH configuration.
    [[nodiscard]] auto validate_pch(const CompilerConfig& config) const
        -> std::vector<ValidationResult>;

    /// Validate LTO configuration.
    [[nodiscard]] auto validate_lto(const CompilerConfig& config) const
        -> std::vector<ValidationResult>;

    /// Validate sysroot configuration.
    [[nodiscard]] auto validate_sysroot(const CompilerConfig& config) const
        -> std::vector<ValidationResult>;

    /// Check if the selected C++ standard is supported by the compiler.
    [[nodiscard]] auto validate_standard(const CompilerConfig& config,
                                         const std::vector<std::string>& supported) const
        -> std::vector<ValidationResult>;

    /// Count results by severity.
    [[nodiscard]] static auto count_errors(const std::vector<ValidationResult>& results) -> int;
    [[nodiscard]] static auto count_warnings(const std::vector<ValidationResult>& results) -> int;

    /// Check if there are any errors.
    [[nodiscard]] static auto has_errors(const std::vector<ValidationResult>& results) -> bool;
};

} // namespace markamp::core
