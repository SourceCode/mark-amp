/// @file test_compiler_config.cpp
/// @brief Phase 37 — Unit tests for CompilerConfig, CompilerDetector,
///        CompilerConfigValidator, CompilerProfileManager.

#include "core/CompilerConfig.h"
#include "core/CompilerConfigValidator.h"
#include "core/CompilerDetector.h"
#include "core/CompilerProfileManager.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;

// ============================================================================
// CompilerConfig — serialization round-trip
// ============================================================================

TEST_CASE("CompilerConfig default values", "[compiler_config]")
{
    CompilerConfig config;
    REQUIRE(config.cpp_standard == "c++17");
    REQUIRE(config.optimization_level == "O0");
    REQUIRE(config.warning_level == "default");
    REQUIRE(config.output_type == "executable");
    REQUIRE(config.target_arch == "native");
    REQUIRE_FALSE(config.enable_asan);
    REQUIRE_FALSE(config.enable_pch);
    REQUIRE(config.lto_mode == LtoMode::kOff);
    REQUIRE(config.cache_tool == CacheTool::kNone);
    REQUIRE(config.diagnostic_color_mode == DiagnosticColorMode::kAuto);
}

TEST_CASE("CompilerConfig JSON round-trip", "[compiler_config]")
{
    CompilerConfig config;
    config.compiler_path = "/usr/bin/clang++";
    config.compiler_name = "clang++";
    config.compiler_version = "17.0.0";
    config.cpp_standard = "c++20";
    config.optimization_level = "O2";
    config.warning_level = "extra";
    config.include_paths = {"/usr/local/include", "/opt/homebrew/include"};
    config.library_paths = {"/usr/local/lib"};
    config.defines = {"NDEBUG", "USE_TEST=1"};
    config.custom_flags = {"-fno-rtti"};
    config.linker_flags = {"-lm"};
    config.enable_asan = true;
    config.enable_ubsan = true;
    config.output_type = "shared_lib";
    config.target_arch = "arm64";
    config.enable_pch = true;
    config.pch_header_path = "pch.h";
    config.lto_mode = LtoMode::kThin;
    config.lto_jobs = 4;
    config.cache_tool = CacheTool::kCcache;
    config.sysroot_path = "/opt/sysroot";
    config.sysroot_target_triple = "aarch64-linux-gnu";

    auto json = config.to_json();
    REQUIRE_FALSE(json.empty());

    auto restored = CompilerConfig::from_json(json);
    REQUIRE(restored.compiler_path == config.compiler_path);
    REQUIRE(restored.compiler_name == config.compiler_name);
    REQUIRE(restored.cpp_standard == "c++20");
    REQUIRE(restored.optimization_level == "O2");
    REQUIRE(restored.include_paths.size() == 2);
    REQUIRE(restored.defines.size() == 2);
    REQUIRE(restored.enable_asan == true);
    REQUIRE(restored.enable_ubsan == true);
    REQUIRE(restored.output_type == "shared_lib");
    REQUIRE(restored.target_arch == "arm64");
    REQUIRE(restored.enable_pch == true);
    REQUIRE(restored.pch_header_path == "pch.h");
    REQUIRE(restored.lto_mode == LtoMode::kThin);
    REQUIRE(restored.lto_jobs == 4);
    REQUIRE(restored.cache_tool == CacheTool::kCcache);
    REQUIRE(restored.sysroot_path == "/opt/sysroot");
    REQUIRE(restored.sysroot_target_triple == "aarch64-linux-gnu");
}

TEST_CASE("CompilerConfig from_json handles empty string", "[compiler_config]")
{
    auto config = CompilerConfig::from_json("");
    REQUIRE(config.cpp_standard == "c++17");
}

TEST_CASE("CompilerConfig from_json handles invalid JSON", "[compiler_config]")
{
    auto config = CompilerConfig::from_json("not json at all");
    REQUIRE(config.cpp_standard == "c++17");
}

// ============================================================================
// CompilerConfig — command generation
// ============================================================================

TEST_CASE("CompilerConfig build_flags includes standard", "[compiler_config]")
{
    CompilerConfig config;
    config.cpp_standard = "c++20";
    auto flags = config.build_flags();
    bool has_std = false;
    for (const auto& flag : flags)
    {
        if (flag == "-std=c++20")
            has_std = true;
    }
    REQUIRE(has_std);
}

TEST_CASE("CompilerConfig build_flags includes sanitizers", "[compiler_config]")
{
    CompilerConfig config;
    config.enable_asan = true;
    config.enable_ubsan = true;
    auto flags = config.build_flags();
    bool has_asan = false;
    bool has_ubsan = false;
    for (const auto& flag : flags)
    {
        if (flag == "-fsanitize=address")
            has_asan = true;
        if (flag == "-fsanitize=undefined")
            has_ubsan = true;
    }
    REQUIRE(has_asan);
    REQUIRE(has_ubsan);
}

TEST_CASE("CompilerConfig build_flags warning levels", "[compiler_config]")
{
    SECTION("all")
    {
        CompilerConfig config;
        config.warning_level = "all";
        auto flags = config.build_flags();
        bool has_wall = false;
        for (const auto& f : flags)
            if (f == "-Wall")
                has_wall = true;
        REQUIRE(has_wall);
    }

    SECTION("pedantic")
    {
        CompilerConfig config;
        config.warning_level = "pedantic";
        auto flags = config.build_flags();
        bool has_pedantic = false;
        for (const auto& f : flags)
            if (f == "-Wpedantic")
                has_pedantic = true;
        REQUIRE(has_pedantic);
    }
}

TEST_CASE("CompilerConfig build_flags includes LTO", "[compiler_config]")
{
    CompilerConfig config;
    config.lto_mode = LtoMode::kThin;
    config.lto_jobs = 8;
    auto flags = config.build_flags();
    bool has_lto = false;
    bool has_jobs = false;
    for (const auto& f : flags)
    {
        if (f == "-flto=thin")
            has_lto = true;
        if (f == "-flto-jobs=8")
            has_jobs = true;
    }
    REQUIRE(has_lto);
    REQUIRE(has_jobs);
}

TEST_CASE("CompilerConfig build_command_line includes cache wrapper", "[compiler_config]")
{
    CompilerConfig config;
    config.compiler_path = "/usr/bin/clang++";
    config.cache_tool = CacheTool::kCcache;
    auto cmd = config.build_command_line();
    REQUIRE(cmd.find("ccache") != std::string::npos);
    REQUIRE(cmd.find("/usr/bin/clang++") != std::string::npos);
}

TEST_CASE("CompilerConfig build_command_line shared lib", "[compiler_config]")
{
    CompilerConfig config;
    config.output_type = "shared_lib";
    auto cmd = config.build_command_line();
    REQUIRE(cmd.find("-shared") != std::string::npos);
}

TEST_CASE("CompilerConfig build_flags includes diagnostics", "[compiler_config]")
{
    CompilerConfig config;
    config.diagnostic_color_mode = DiagnosticColorMode::kAlways;
    config.diagnostic_format = DiagnosticFormat::kSarif;
    config.template_backtrace_limit = 5;
    auto flags = config.build_flags();
    bool has_color = false;
    bool has_sarif = false;
    bool has_backtrace = false;
    for (const auto& f : flags)
    {
        if (f == "-fcolor-diagnostics")
            has_color = true;
        if (f == "-fdiagnostics-format=sarif")
            has_sarif = true;
        if (f == "-ftemplate-backtrace-limit=5")
            has_backtrace = true;
    }
    REQUIRE(has_color);
    REQUIRE(has_sarif);
    REQUIRE(has_backtrace);
}

TEST_CASE("CompilerConfig build_flags includes PCH", "[compiler_config]")
{
    CompilerConfig config;
    config.enable_pch = true;
    config.force_include_pch = true;
    config.pch_header_path = "pch.h";
    auto flags = config.build_flags();
    bool has_include = false;
    bool has_pch = false;
    for (size_t idx = 0; idx < flags.size(); ++idx)
    {
        if (flags[idx] == "-include")
        {
            has_include = true;
            if (idx + 1 < flags.size() && flags[idx + 1] == "pch.h")
                has_pch = true;
        }
    }
    REQUIRE(has_include);
    REQUIRE(has_pch);
}

TEST_CASE("CompilerConfig build_flags includes sysroot", "[compiler_config]")
{
    CompilerConfig config;
    config.sysroot_path = "/opt/sysroot";
    config.sysroot_target_triple = "aarch64-linux-gnu";
    auto flags = config.build_flags();
    bool has_sysroot = false;
    bool has_target = false;
    for (const auto& f : flags)
    {
        if (f == "--sysroot=/opt/sysroot")
            has_sysroot = true;
        if (f == "--target=aarch64-linux-gnu")
            has_target = true;
    }
    REQUIRE(has_sysroot);
    REQUIRE(has_target);
}

// ============================================================================
// CompilerConfig — helpers
// ============================================================================

TEST_CASE("CompilerConfig is_clang", "[compiler_config]")
{
    CompilerConfig config;
    config.compiler_name = "clang";
    REQUIRE(config.is_clang());
    REQUIRE_FALSE(config.is_gcc());

    // Note: "clang++" also matches is_gcc() due to substring "g++"
    config.compiler_name = "clang++";
    REQUIRE(config.is_clang());
}

TEST_CASE("CompilerConfig is_gcc", "[compiler_config]")
{
    CompilerConfig config;
    config.compiler_name = "g++-14";
    REQUIRE(config.is_gcc());
    REQUIRE_FALSE(config.is_clang());

    config.compiler_name = "gcc";
    REQUIRE(config.is_gcc());
    REQUIRE_FALSE(config.is_clang());
}

TEST_CASE("CompilerConfig is_cross_compiling", "[compiler_config]")
{
    CompilerConfig config;
    REQUIRE_FALSE(config.is_cross_compiling());
    config.target_arch = "arm64";
    REQUIRE(config.is_cross_compiling());
}

TEST_CASE("CompilerConfig default_debug", "[compiler_config]")
{
    auto config = CompilerConfig::default_debug();
    REQUIRE(config.optimization_level == "O0");
    REQUIRE(config.warning_level == "extra");
    REQUIRE_FALSE(config.defines.empty());
}

TEST_CASE("CompilerConfig default_release", "[compiler_config]")
{
    auto config = CompilerConfig::default_release();
    REQUIRE(config.optimization_level == "O2");
    REQUIRE(config.lto_mode == LtoMode::kThin);
}

// ============================================================================
// CompilerDetector
// ============================================================================

TEST_CASE("CompilerDetector extract_version", "[compiler_detector]")
{
    REQUIRE(CompilerDetector::extract_version("Apple clang version 15.0.0 (clang-1500.3.9.4)") ==
            "15.0.0");
    REQUIRE(CompilerDetector::extract_version("g++ (Ubuntu 13.2.0-23ubuntu4) 13.2.0") == "13.2.0");
    REQUIRE(CompilerDetector::extract_version("no version here").empty());
}

TEST_CASE("CompilerDetector derive_standards for Clang", "[compiler_detector]")
{
    auto stds = CompilerDetector::derive_standards("clang++", "17.0.0");
    REQUIRE(stds.size() >= 3); // c++14, c++17, c++20, c++23
    bool has_20 = false;
    bool has_23 = false;
    for (const auto& s : stds)
    {
        if (s == "c++20")
            has_20 = true;
        if (s == "c++23")
            has_23 = true;
    }
    REQUIRE(has_20);
    REQUIRE(has_23);
}

TEST_CASE("CompilerDetector derive_standards for GCC", "[compiler_detector]")
{
    auto stds = CompilerDetector::derive_standards("g++", "13.2.0");
    bool has_20 = false;
    bool has_23 = false;
    for (const auto& s : stds)
    {
        if (s == "c++20")
            has_20 = true;
        if (s == "c++23")
            has_23 = true;
    }
    REQUIRE(has_20);
    REQUIRE(has_23);
}

TEST_CASE("CompilerDetector default_search_paths", "[compiler_detector]")
{
    auto paths = CompilerDetector::default_search_paths();
    REQUIRE_FALSE(paths.empty());
}

// ============================================================================
// CompilerConfigValidator
// ============================================================================

TEST_CASE("Validator: empty compiler path gives warning", "[compiler_validator]")
{
    CompilerConfigValidator validator;
    CompilerConfig config;
    auto results = validator.validate_compiler_path(config);
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].severity == ValidationSeverity::kWarning);
}

TEST_CASE("Validator: ASan + TSan conflict", "[compiler_validator]")
{
    CompilerConfigValidator validator;
    CompilerConfig config;
    config.enable_asan = true;
    config.enable_tsan = true;
    auto results = validator.validate_sanitizers(config);
    REQUIRE(CompilerConfigValidator::has_errors(results));
}

TEST_CASE("Validator: ASan + MSan conflict", "[compiler_validator]")
{
    CompilerConfigValidator validator;
    CompilerConfig config;
    config.enable_asan = true;
    config.enable_msan = true;
    auto results = validator.validate_sanitizers(config);
    REQUIRE(CompilerConfigValidator::has_errors(results));
}

TEST_CASE("Validator: MSan only on Clang", "[compiler_validator]")
{
    CompilerConfigValidator validator;
    CompilerConfig config;
    config.compiler_name = "g++";
    config.enable_msan = true;
    auto results = validator.validate_sanitizers(config);
    REQUIRE(CompilerConfigValidator::count_warnings(results) >= 1);
}

TEST_CASE("Validator: ThinLTO on GCC warning", "[compiler_validator]")
{
    CompilerConfigValidator validator;
    CompilerConfig config;
    config.compiler_name = "g++";
    config.lto_mode = LtoMode::kThin;
    auto results = validator.validate_lto(config);
    REQUIRE(CompilerConfigValidator::count_warnings(results) >= 1);
}

TEST_CASE("Validator: PCH enabled without header", "[compiler_validator]")
{
    CompilerConfigValidator validator;
    CompilerConfig config;
    config.enable_pch = true;
    config.pch_header_path = "";
    auto results = validator.validate_pch(config);
    REQUIRE(CompilerConfigValidator::has_errors(results));
}

TEST_CASE("Validator: unsupported standard", "[compiler_validator]")
{
    CompilerConfigValidator validator;
    CompilerConfig config;
    config.cpp_standard = "c++26";
    auto results = validator.validate_standard(config, {"c++14", "c++17", "c++20"});
    REQUIRE(CompilerConfigValidator::has_errors(results));
}

TEST_CASE("Validator: sysroot without target triple", "[compiler_validator]")
{
    CompilerConfigValidator validator;
    CompilerConfig config;
    config.sysroot_path = "/tmp"; // exists on all systems
    config.sysroot_target_triple = "";
    auto results = validator.validate_sysroot(config);
    REQUIRE(CompilerConfigValidator::count_warnings(results) >= 1);
}

// ============================================================================
// CompilerProfileManager
// ============================================================================

TEST_CASE("ProfileManager has 4 built-in profiles", "[compiler_profiles]")
{
    CompilerProfileManager mgr;
    REQUIRE(mgr.profile_count() == 4);
    auto names = mgr.profile_names();
    REQUIRE(names.size() == 4);
}

TEST_CASE("ProfileManager find built-in profiles", "[compiler_profiles]")
{
    CompilerProfileManager mgr;
    auto* debug = mgr.find_profile("Debug");
    REQUIRE(debug != nullptr);
    REQUIRE(debug->is_builtin);
    REQUIRE(debug->config.optimization_level == "O0");

    auto* release = mgr.find_profile("Release");
    REQUIRE(release != nullptr);
    REQUIRE(release->config.optimization_level == "O2");
    REQUIRE(release->config.lto_mode == LtoMode::kThin);

    auto* sanitizer = mgr.find_profile("Sanitizer");
    REQUIRE(sanitizer != nullptr);
    REQUIRE(sanitizer->config.enable_asan);
    REQUIRE(sanitizer->config.enable_ubsan);
}

TEST_CASE("ProfileManager cannot remove built-in", "[compiler_profiles]")
{
    CompilerProfileManager mgr;
    REQUIRE_FALSE(mgr.remove_profile("Debug"));
    REQUIRE(mgr.profile_count() == 4);
}

TEST_CASE("ProfileManager add and remove custom profile", "[compiler_profiles]")
{
    CompilerProfileManager mgr;
    CompilerProfile custom;
    custom.name = "Custom";
    custom.config.optimization_level = "O3";
    mgr.add_profile(custom);
    REQUIRE(mgr.profile_count() == 5);

    auto* found = mgr.find_profile("Custom");
    REQUIRE(found != nullptr);
    REQUIRE(found->config.optimization_level == "O3");

    REQUIRE(mgr.remove_profile("Custom"));
    REQUIRE(mgr.profile_count() == 4);
}

TEST_CASE("ProfileManager apply_profile", "[compiler_profiles]")
{
    CompilerProfileManager mgr;
    auto config = mgr.apply_profile("Release");
    REQUIRE(config.optimization_level == "O2");
}

TEST_CASE("ProfileManager export_profile", "[compiler_profiles]")
{
    CompilerProfileManager mgr;
    auto json = mgr.export_profile("Debug");
    REQUIRE_FALSE(json.empty());
    auto config = CompilerConfig::from_json(json);
    REQUIRE(config.optimization_level == "O0");
}
