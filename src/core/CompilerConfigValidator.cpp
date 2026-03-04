/// @file CompilerConfigValidator.cpp
/// @brief Phase 37 Task 18 — Compiler configuration validation implementation.

#include "core/CompilerConfigValidator.h"

#include <filesystem>

namespace markamp::core
{

auto CompilerConfigValidator::validate(const CompilerConfig& config) const
    -> std::vector<ValidationResult>
{
    std::vector<ValidationResult> results;

    auto path_results = validate_compiler_path(config);
    results.insert(results.end(), path_results.begin(), path_results.end());

    auto dir_results = validate_paths(config);
    results.insert(results.end(), dir_results.begin(), dir_results.end());

    auto san_results = validate_sanitizers(config);
    results.insert(results.end(), san_results.begin(), san_results.end());

    auto pch_results = validate_pch(config);
    results.insert(results.end(), pch_results.begin(), pch_results.end());

    auto lto_results = validate_lto(config);
    results.insert(results.end(), lto_results.begin(), lto_results.end());

    auto sysroot_results = validate_sysroot(config);
    results.insert(results.end(), sysroot_results.begin(), sysroot_results.end());

    return results;
}

auto CompilerConfigValidator::validate_compiler_path(const CompilerConfig& config) const
    -> std::vector<ValidationResult>
{
    std::vector<ValidationResult> results;

    if (config.compiler_path.empty())
    {
        results.push_back({ValidationSeverity::kWarning,
                           "Compiler",
                           "No compiler path set. Auto-detection will be used.",
                           "Select a compiler from the detected compilers list."});
        return results;
    }

    if (!std::filesystem::exists(config.compiler_path))
    {
        results.push_back({ValidationSeverity::kError,
                           "Compiler",
                           "Compiler not found at: " + config.compiler_path,
                           "Verify the compiler is installed and the path is correct."});
    }

    return results;
}

auto CompilerConfigValidator::validate_paths(const CompilerConfig& config) const
    -> std::vector<ValidationResult>
{
    std::vector<ValidationResult> results;

    for (const auto& path : config.include_paths)
    {
        if (!std::filesystem::exists(path))
        {
            results.push_back({ValidationSeverity::kWarning,
                               "Include Paths",
                               "Include path does not exist: " + path,
                               "Verify the path or remove it from the include list."});
        }
        else if (!std::filesystem::is_directory(path))
        {
            results.push_back({ValidationSeverity::kWarning,
                               "Include Paths",
                               "Include path is not a directory: " + path,
                               "Include paths should be directories."});
        }
    }

    for (const auto& path : config.library_paths)
    {
        if (!std::filesystem::exists(path))
        {
            results.push_back({ValidationSeverity::kWarning,
                               "Library Paths",
                               "Library path does not exist: " + path,
                               "Verify the path or remove it from the library list."});
        }
    }

    return results;
}

auto CompilerConfigValidator::validate_sanitizers(const CompilerConfig& config) const
    -> std::vector<ValidationResult>
{
    std::vector<ValidationResult> results;

    // ASan and TSan are mutually exclusive
    if (config.enable_asan && config.enable_tsan)
    {
        results.push_back({ValidationSeverity::kError,
                           "Sanitizers",
                           "Address Sanitizer and Thread Sanitizer cannot be used together.",
                           "Disable one of the sanitizers."});
    }

    // MSan is only supported by Clang
    if (config.enable_msan && !config.is_clang())
    {
        results.push_back({ValidationSeverity::kWarning,
                           "Sanitizers",
                           "Memory Sanitizer is only supported by Clang.",
                           "Switch to a Clang compiler to use MSan."});
    }

    // ASan and MSan are mutually exclusive
    if (config.enable_asan && config.enable_msan)
    {
        results.push_back({ValidationSeverity::kError,
                           "Sanitizers",
                           "Address Sanitizer and Memory Sanitizer cannot be used together.",
                           "Disable one of the sanitizers."});
    }

    return results;
}

auto CompilerConfigValidator::validate_pch(const CompilerConfig& config) const
    -> std::vector<ValidationResult>
{
    std::vector<ValidationResult> results;

    if (config.enable_pch)
    {
        if (config.pch_header_path.empty())
        {
            results.push_back({ValidationSeverity::kError,
                               "Precompiled Headers",
                               "PCH is enabled but no header path is specified.",
                               "Set the PCH header path to the precompiled header file."});
        }
        else if (!std::filesystem::exists(config.pch_header_path))
        {
            results.push_back({ValidationSeverity::kWarning,
                               "Precompiled Headers",
                               "PCH header file does not exist: " + config.pch_header_path,
                               "Verify the PCH header path."});
        }
    }

    return results;
}

auto CompilerConfigValidator::validate_lto(const CompilerConfig& config) const
    -> std::vector<ValidationResult>
{
    std::vector<ValidationResult> results;

    if (config.lto_mode == LtoMode::kThin && !config.is_clang())
    {
        results.push_back({ValidationSeverity::kWarning,
                           "LTO",
                           "ThinLTO is a Clang-specific feature. GCC supports full LTO only.",
                           "Switch to Full LTO or use a Clang compiler."});
    }

    if (config.lto_mode != LtoMode::kOff && config.optimization_level == "O0")
    {
        results.push_back({ValidationSeverity::kInfo,
                           "LTO",
                           "LTO is enabled with -O0 optimization. This has limited benefit.",
                           "Consider using -O2 or higher with LTO for best results."});
    }

    return results;
}

auto CompilerConfigValidator::validate_sysroot(const CompilerConfig& config) const
    -> std::vector<ValidationResult>
{
    std::vector<ValidationResult> results;

    if (!config.sysroot_path.empty())
    {
        if (!std::filesystem::exists(config.sysroot_path))
        {
            results.push_back({ValidationSeverity::kError,
                               "Sysroot",
                               "Sysroot path does not exist: " + config.sysroot_path,
                               "Verify the sysroot path."});
        }
        else
        {
            // Check expected directories
            auto usr_include = std::filesystem::path(config.sysroot_path) / "usr" / "include";
            auto usr_lib = std::filesystem::path(config.sysroot_path) / "usr" / "lib";
            if (!std::filesystem::exists(usr_include))
            {
                results.push_back({ValidationSeverity::kWarning,
                                   "Sysroot",
                                   "Sysroot missing expected directory: usr/include",
                                   "Verify the sysroot contains the expected SDK structure."});
            }
            if (!std::filesystem::exists(usr_lib))
            {
                results.push_back({ValidationSeverity::kWarning,
                                   "Sysroot",
                                   "Sysroot missing expected directory: usr/lib",
                                   "Verify the sysroot contains the expected SDK structure."});
            }
        }

        if (config.sysroot_target_triple.empty())
        {
            results.push_back({ValidationSeverity::kWarning,
                               "Sysroot",
                               "Sysroot is set but target triple is empty.",
                               "Specify a target triple (e.g., aarch64-linux-gnu)."});
        }
    }

    return results;
}

auto CompilerConfigValidator::validate_standard(const CompilerConfig& config,
                                                const std::vector<std::string>& supported) const
    -> std::vector<ValidationResult>
{
    std::vector<ValidationResult> results;

    if (supported.empty())
        return results;

    bool found = false;
    for (const auto& std_val : supported)
    {
        if (std_val == config.cpp_standard)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        std::string supported_list;
        for (size_t i = 0; i < supported.size(); ++i)
        {
            if (i > 0)
                supported_list += ", ";
            supported_list += supported[i];
        }
        results.push_back(
            {ValidationSeverity::kError,
             "Standard",
             "C++ standard '" + config.cpp_standard + "' is not supported by this compiler.",
             "Supported standards: " + supported_list});
    }

    return results;
}

auto CompilerConfigValidator::count_errors(const std::vector<ValidationResult>& results) -> int
{
    int count = 0;
    for (const auto& r : results)
    {
        if (r.severity == ValidationSeverity::kError)
            ++count;
    }
    return count;
}

auto CompilerConfigValidator::count_warnings(const std::vector<ValidationResult>& results) -> int
{
    int count = 0;
    for (const auto& r : results)
    {
        if (r.severity == ValidationSeverity::kWarning)
            ++count;
    }
    return count;
}

auto CompilerConfigValidator::has_errors(const std::vector<ValidationResult>& results) -> bool
{
    return count_errors(results) > 0;
}

} // namespace markamp::core
