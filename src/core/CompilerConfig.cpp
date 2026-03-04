/// @file CompilerConfig.cpp
/// @brief Phase 37 — CompilerConfig serialization, command generation, and defaults.

#include "core/CompilerConfig.h"

#include <nlohmann/json.hpp>

#include <sstream>

namespace markamp::core
{

// ============================================================================
// JSON Serialization
// ============================================================================

auto CompilerConfig::to_json() const -> std::string
{
    nlohmann::json j;

    // Compiler selection
    j["compiler_path"] = compiler_path;
    j["compiler_name"] = compiler_name;
    j["compiler_version"] = compiler_version;
    j["cpp_standard"] = cpp_standard;

    // Optimization & warnings
    j["optimization_level"] = optimization_level;
    j["warning_level"] = warning_level;

    // Paths
    j["include_paths"] = include_paths;
    j["library_paths"] = library_paths;
    j["defines"] = defines;
    j["custom_flags"] = custom_flags;
    j["linker_flags"] = linker_flags;

    // Sanitizers
    j["enable_asan"] = enable_asan;
    j["enable_ubsan"] = enable_ubsan;
    j["enable_tsan"] = enable_tsan;
    j["enable_msan"] = enable_msan;
    j["enable_coverage"] = enable_coverage;
    j["position_independent"] = position_independent;

    // Output
    j["output_type"] = output_type;
    j["target_arch"] = target_arch;
    j["toolchain_file"] = toolchain_file;

    // Diagnostics
    j["diagnostic_color_mode"] = static_cast<int>(diagnostic_color_mode);
    j["diagnostic_format"] = static_cast<int>(diagnostic_format);
    j["template_backtrace_limit"] = template_backtrace_limit;
    j["diagnostic_column_limit"] = diagnostic_column_limit;
    j["show_source_ranges"] = show_source_ranges;
    j["show_fix_its"] = show_fix_its;

    // PCH
    j["pch_header_path"] = pch_header_path;
    j["pch_output_path"] = pch_output_path;
    j["enable_pch"] = enable_pch;
    j["force_include_pch"] = force_include_pch;

    // LTO
    j["lto_mode"] = static_cast<int>(lto_mode);
    j["lto_jobs"] = lto_jobs;
    j["lto_cache_dir"] = lto_cache_dir;

    // Cache
    j["cache_tool"] = static_cast<int>(cache_tool);
    j["cache_tool_path"] = cache_tool_path;
    j["cache_max_size"] = cache_max_size;
    j["cache_dir"] = cache_dir;

    // Sysroot
    j["sysroot_path"] = sysroot_path;
    j["sysroot_target_triple"] = sysroot_target_triple;

    return j.dump(2);
}

auto CompilerConfig::from_json(const std::string& json_str) -> CompilerConfig
{
    CompilerConfig config;
    if (json_str.empty())
    {
        return config;
    }

    auto j = nlohmann::json::parse(json_str, nullptr, false);
    if (j.is_discarded())
    {
        return config;
    }

    // Compiler selection
    if (j.contains("compiler_path"))
        config.compiler_path = j["compiler_path"].get<std::string>();
    if (j.contains("compiler_name"))
        config.compiler_name = j["compiler_name"].get<std::string>();
    if (j.contains("compiler_version"))
        config.compiler_version = j["compiler_version"].get<std::string>();
    if (j.contains("cpp_standard"))
        config.cpp_standard = j["cpp_standard"].get<std::string>();

    // Optimization & warnings
    if (j.contains("optimization_level"))
        config.optimization_level = j["optimization_level"].get<std::string>();
    if (j.contains("warning_level"))
        config.warning_level = j["warning_level"].get<std::string>();

    // Paths
    if (j.contains("include_paths"))
        config.include_paths = j["include_paths"].get<std::vector<std::string>>();
    if (j.contains("library_paths"))
        config.library_paths = j["library_paths"].get<std::vector<std::string>>();
    if (j.contains("defines"))
        config.defines = j["defines"].get<std::vector<std::string>>();
    if (j.contains("custom_flags"))
        config.custom_flags = j["custom_flags"].get<std::vector<std::string>>();
    if (j.contains("linker_flags"))
        config.linker_flags = j["linker_flags"].get<std::vector<std::string>>();

    // Sanitizers
    if (j.contains("enable_asan"))
        config.enable_asan = j["enable_asan"].get<bool>();
    if (j.contains("enable_ubsan"))
        config.enable_ubsan = j["enable_ubsan"].get<bool>();
    if (j.contains("enable_tsan"))
        config.enable_tsan = j["enable_tsan"].get<bool>();
    if (j.contains("enable_msan"))
        config.enable_msan = j["enable_msan"].get<bool>();
    if (j.contains("enable_coverage"))
        config.enable_coverage = j["enable_coverage"].get<bool>();
    if (j.contains("position_independent"))
        config.position_independent = j["position_independent"].get<bool>();

    // Output
    if (j.contains("output_type"))
        config.output_type = j["output_type"].get<std::string>();
    if (j.contains("target_arch"))
        config.target_arch = j["target_arch"].get<std::string>();
    if (j.contains("toolchain_file"))
        config.toolchain_file = j["toolchain_file"].get<std::string>();

    // Diagnostics
    if (j.contains("diagnostic_color_mode"))
        config.diagnostic_color_mode =
            static_cast<DiagnosticColorMode>(j["diagnostic_color_mode"].get<int>());
    if (j.contains("diagnostic_format"))
        config.diagnostic_format = static_cast<DiagnosticFormat>(j["diagnostic_format"].get<int>());
    if (j.contains("template_backtrace_limit"))
        config.template_backtrace_limit = j["template_backtrace_limit"].get<int>();
    if (j.contains("diagnostic_column_limit"))
        config.diagnostic_column_limit = j["diagnostic_column_limit"].get<int>();
    if (j.contains("show_source_ranges"))
        config.show_source_ranges = j["show_source_ranges"].get<bool>();
    if (j.contains("show_fix_its"))
        config.show_fix_its = j["show_fix_its"].get<bool>();

    // PCH
    if (j.contains("pch_header_path"))
        config.pch_header_path = j["pch_header_path"].get<std::string>();
    if (j.contains("pch_output_path"))
        config.pch_output_path = j["pch_output_path"].get<std::string>();
    if (j.contains("enable_pch"))
        config.enable_pch = j["enable_pch"].get<bool>();
    if (j.contains("force_include_pch"))
        config.force_include_pch = j["force_include_pch"].get<bool>();

    // LTO
    if (j.contains("lto_mode"))
        config.lto_mode = static_cast<LtoMode>(j["lto_mode"].get<int>());
    if (j.contains("lto_jobs"))
        config.lto_jobs = j["lto_jobs"].get<int>();
    if (j.contains("lto_cache_dir"))
        config.lto_cache_dir = j["lto_cache_dir"].get<std::string>();

    // Cache
    if (j.contains("cache_tool"))
        config.cache_tool = static_cast<CacheTool>(j["cache_tool"].get<int>());
    if (j.contains("cache_tool_path"))
        config.cache_tool_path = j["cache_tool_path"].get<std::string>();
    if (j.contains("cache_max_size"))
        config.cache_max_size = j["cache_max_size"].get<std::string>();
    if (j.contains("cache_dir"))
        config.cache_dir = j["cache_dir"].get<std::string>();

    // Sysroot
    if (j.contains("sysroot_path"))
        config.sysroot_path = j["sysroot_path"].get<std::string>();
    if (j.contains("sysroot_target_triple"))
        config.sysroot_target_triple = j["sysroot_target_triple"].get<std::string>();

    return config;
}

// ============================================================================
// Command generation
// ============================================================================

auto CompilerConfig::build_flags() const -> std::vector<std::string>
{
    std::vector<std::string> flags;

    // Standard
    if (!cpp_standard.empty())
    {
        flags.push_back("-std=" + cpp_standard);
    }

    // Optimization
    if (!optimization_level.empty() && optimization_level != "O0")
    {
        flags.push_back("-" + optimization_level);
    }

    // Warnings
    if (warning_level == "all")
    {
        flags.emplace_back("-Wall");
    }
    else if (warning_level == "extra")
    {
        flags.emplace_back("-Wall");
        flags.emplace_back("-Wextra");
    }
    else if (warning_level == "pedantic")
    {
        flags.emplace_back("-Wall");
        flags.emplace_back("-Wextra");
        flags.emplace_back("-Wpedantic");
    }

    // Sanitizers
    if (enable_asan)
        flags.emplace_back("-fsanitize=address");
    if (enable_ubsan)
        flags.emplace_back("-fsanitize=undefined");
    if (enable_tsan)
        flags.emplace_back("-fsanitize=thread");
    if (enable_msan)
        flags.emplace_back("-fsanitize=memory");
    if (enable_coverage)
    {
        flags.emplace_back("-fprofile-instr-generate");
        flags.emplace_back("-fcoverage-mapping");
    }
    if (position_independent)
        flags.emplace_back("-fPIC");

    // Include paths
    for (const auto& path : include_paths)
    {
        flags.push_back("-I" + path);
    }

    // Defines
    for (const auto& def : defines)
    {
        flags.push_back("-D" + def);
    }

    // Diagnostics
    if (diagnostic_color_mode == DiagnosticColorMode::kAlways)
        flags.emplace_back("-fcolor-diagnostics");
    else if (diagnostic_color_mode == DiagnosticColorMode::kNever)
        flags.emplace_back("-fno-color-diagnostics");

    if (diagnostic_format == DiagnosticFormat::kSarif)
        flags.emplace_back("-fdiagnostics-format=sarif");
    else if (diagnostic_format == DiagnosticFormat::kJson)
        flags.emplace_back("-fdiagnostics-format=json");

    if (template_backtrace_limit != 10)
        flags.push_back("-ftemplate-backtrace-limit=" + std::to_string(template_backtrace_limit));

    if (show_source_ranges)
        flags.emplace_back("-fdiagnostics-show-ranges");
    if (show_fix_its)
        flags.emplace_back("-fdiagnostics-fixit-info");

    // PCH
    if (enable_pch && force_include_pch && !pch_header_path.empty())
    {
        flags.emplace_back("-include");
        flags.push_back(pch_header_path);
    }

    // LTO
    if (lto_mode == LtoMode::kFull)
    {
        flags.emplace_back("-flto=full");
    }
    else if (lto_mode == LtoMode::kThin)
    {
        flags.emplace_back("-flto=thin");
        if (lto_jobs > 0)
            flags.push_back("-flto-jobs=" + std::to_string(lto_jobs));
        if (!lto_cache_dir.empty())
            flags.push_back("-flto-cache-dir=" + lto_cache_dir);
    }

    // Sysroot
    if (!sysroot_path.empty())
        flags.push_back("--sysroot=" + sysroot_path);
    if (!sysroot_target_triple.empty())
        flags.push_back("--target=" + sysroot_target_triple);

    // Custom flags
    for (const auto& flag : custom_flags)
    {
        flags.push_back(flag);
    }

    return flags;
}

auto CompilerConfig::build_command_line() const -> std::string
{
    std::ostringstream cmd;

    // Cache wrapper
    if (cache_tool == CacheTool::kCcache)
    {
        cmd << (cache_tool_path.empty() ? "ccache" : cache_tool_path) << " ";
    }
    else if (cache_tool == CacheTool::kSccache)
    {
        cmd << (cache_tool_path.empty() ? "sccache" : cache_tool_path) << " ";
    }

    // Compiler
    cmd << (compiler_path.empty() ? "c++" : compiler_path);

    // Flags
    for (const auto& flag : build_flags())
    {
        cmd << " " << flag;
    }

    // Library paths (linker)
    for (const auto& path : library_paths)
    {
        cmd << " -L" << path;
    }

    // Linker flags
    for (const auto& flag : linker_flags)
    {
        cmd << " " << flag;
    }

    // Output type
    if (output_type == "shared_lib")
        cmd << " -shared";

    return cmd.str();
}

// ============================================================================
// Helpers
// ============================================================================

auto CompilerConfig::is_clang() const -> bool
{
    return compiler_name.find("clang") != std::string::npos;
}

auto CompilerConfig::is_gcc() const -> bool
{
    return compiler_name.find("g++") != std::string::npos ||
           compiler_name.find("gcc") != std::string::npos;
}

auto CompilerConfig::is_cross_compiling() const -> bool
{
    return target_arch != "native" && !target_arch.empty();
}

auto CompilerConfig::default_debug() -> CompilerConfig
{
    CompilerConfig config;
    config.optimization_level = "O0";
    config.warning_level = "extra";
    config.cpp_standard = "c++17";
    config.defines.push_back("DEBUG=1");
    return config;
}

auto CompilerConfig::default_release() -> CompilerConfig
{
    CompilerConfig config;
    config.optimization_level = "O2";
    config.warning_level = "all";
    config.cpp_standard = "c++17";
    config.defines.push_back("NDEBUG");
    config.lto_mode = LtoMode::kThin;
    return config;
}

} // namespace markamp::core
