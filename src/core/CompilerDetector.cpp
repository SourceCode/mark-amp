/// @file CompilerDetector.cpp
/// @brief Phase 37 Task 2 — Compiler auto-detection implementation.

#include "core/CompilerDetector.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <regex>
#include <sstream>

namespace markamp::core
{

namespace
{

/// Run a command and capture stdout.
auto run_command(const std::string& cmd) -> std::string
{
    std::string result;
    std::array<char, 256> buffer{};
    auto* pipe = popen((cmd + " 2>&1").c_str(), "r");
    if (!pipe)
        return {};
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
    {
        result += buffer.data();
    }
    pclose(pipe);
    return result;
}

/// Check if a file exists and is executable.
auto is_executable(const std::string& path) -> bool
{
    return std::filesystem::exists(path) && !std::filesystem::is_directory(path);
}

} // namespace

auto CompilerDetector::detect_compilers() const -> std::vector<DetectedCompiler>
{
    std::vector<DetectedCompiler> compilers;

    // Standard paths to check
    std::vector<std::string> candidates = {
        "/usr/bin/clang++",
        "/usr/bin/g++",
        "/usr/local/bin/g++",
        "/usr/local/bin/clang++",
        "/opt/homebrew/bin/g++-14",
        "/opt/homebrew/bin/g++-13",
        "/opt/homebrew/bin/clang++",
    };

    // Also check xcrun on macOS
    auto xcrun_path = run_command("xcrun --find clang++ 2>/dev/null");
    if (!xcrun_path.empty())
    {
        // Trim newline
        while (!xcrun_path.empty() && (xcrun_path.back() == '\n' || xcrun_path.back() == '\r'))
            xcrun_path.pop_back();
        if (!xcrun_path.empty())
            candidates.push_back(xcrun_path);
    }

    // Check PATH for g++/clang++
    auto path_clang = find_in_path("clang++");
    if (!path_clang.empty())
        candidates.push_back(path_clang);
    auto path_gcc = find_in_path("g++");
    if (!path_gcc.empty())
        candidates.push_back(path_gcc);

    // Deduplicate and check
    std::vector<std::string> seen;
    for (const auto& path : candidates)
    {
        if (!is_executable(path))
            continue;

        // Resolve symlinks to deduplicate
        std::string resolved;
        try
        {
            resolved = std::filesystem::canonical(path).string();
        }
        catch (...)
        {
            resolved = path;
        }

        bool already_seen = false;
        for (const auto& s : seen)
        {
            if (s == resolved)
            {
                already_seen = true;
                break;
            }
        }
        if (already_seen)
            continue;
        seen.push_back(resolved);

        auto compiler = detect_compiler_at(path);
        if (!compiler.name.empty())
        {
            compilers.push_back(std::move(compiler));
        }
    }

    return compilers;
}

auto CompilerDetector::detect_compiler_at(const std::string& path) const -> DetectedCompiler
{
    DetectedCompiler compiler;
    compiler.path = path;

    auto output = run_command(path + " --version");
    if (output.empty())
        return compiler;

    // Determine compiler name
    if (output.find("clang") != std::string::npos || output.find("Apple") != std::string::npos)
    {
        compiler.name = std::filesystem::path(path).filename().string();
        if (compiler.name.empty())
            compiler.name = "clang++";
    }
    else if (output.find("g++") != std::string::npos || output.find("gcc") != std::string::npos ||
             output.find("GCC") != std::string::npos)
    {
        compiler.name = std::filesystem::path(path).filename().string();
        if (compiler.name.empty())
            compiler.name = "g++";
    }
    else
    {
        compiler.name = std::filesystem::path(path).filename().string();
    }

    compiler.version = extract_version(output);
    compiler.supported_standards = derive_standards(compiler.name, compiler.version);

    return compiler;
}

auto CompilerDetector::extract_version(const std::string& version_output) -> std::string
{
    // Match patterns like "17.0.0", "14.2.1", "Apple clang version 15.0.0"
    std::regex version_re(R"((\d+\.\d+(?:\.\d+)?))");
    std::smatch match;
    if (std::regex_search(version_output, match, version_re))
    {
        return match[1].str();
    }
    return {};
}

auto CompilerDetector::derive_standards(const std::string& compiler_name,
                                        const std::string& version) -> std::vector<std::string>
{
    std::vector<std::string> standards = {"c++14", "c++17"};

    // Extract major version
    int major = 0;
    try
    {
        major = std::stoi(version);
    }
    catch (...)
    {
        return standards;
    }

    bool is_clang = compiler_name.find("clang") != std::string::npos;

    if (is_clang)
    {
        // Clang 10+ supports C++20, 17+ supports C++23, 19+ partial C++26
        if (major >= 10)
            standards.emplace_back("c++20");
        if (major >= 17)
            standards.emplace_back("c++23");
        if (major >= 19)
            standards.emplace_back("c++26");
    }
    else
    {
        // GCC 10+ supports C++20, 13+ supports C++23, 15+ partial C++26
        if (major >= 10)
            standards.emplace_back("c++20");
        if (major >= 13)
            standards.emplace_back("c++23");
        if (major >= 15)
            standards.emplace_back("c++26");
    }

    return standards;
}

auto CompilerDetector::detect_cache_tools() const -> std::vector<CacheStats>
{
    std::vector<CacheStats> tools;

    auto ccache_path = find_in_path("ccache");
    if (!ccache_path.empty())
    {
        auto stats = query_cache_stats(CacheTool::kCcache);
        stats.available = true;
        tools.push_back(std::move(stats));
    }

    auto sccache_path = find_in_path("sccache");
    if (!sccache_path.empty())
    {
        auto stats = query_cache_stats(CacheTool::kSccache);
        stats.available = true;
        tools.push_back(std::move(stats));
    }

    return tools;
}

auto CompilerDetector::query_cache_stats(CacheTool tool) const -> CacheStats
{
    CacheStats stats;

    if (tool == CacheTool::kCcache)
    {
        stats.tool_name = "ccache";
        auto output = run_command("ccache -s");
        if (!output.empty())
        {
            stats.available = true;
            // Parse cache size and hit rate from ccache -s output
            std::regex size_re(R"(cache size\s+(\S+))");
            std::smatch size_match;
            if (std::regex_search(output, size_match, size_re))
                stats.cache_size = size_match[1].str();

            std::regex max_re(R"(max cache size\s+(\S+))");
            std::smatch max_match;
            if (std::regex_search(output, max_match, max_re))
                stats.max_size = max_match[1].str();

            std::regex hit_re(R"(cache hit rate\s+(\d+(?:\.\d+)?)\s*%)");
            std::smatch hit_match;
            if (std::regex_search(output, hit_match, hit_re))
            {
                try
                {
                    stats.hit_rate = std::stod(hit_match[1].str());
                }
                catch (...)
                {
                }
            }
        }
    }
    else if (tool == CacheTool::kSccache)
    {
        stats.tool_name = "sccache";
        auto output = run_command("sccache --show-stats");
        stats.available = !output.empty();
    }

    return stats;
}

auto CompilerDetector::find_in_path(const std::string& binary_name) -> std::string
{
    auto output = run_command("which " + binary_name);
    if (!output.empty())
    {
        while (!output.empty() && (output.back() == '\n' || output.back() == '\r'))
            output.pop_back();
        if (is_executable(output))
            return output;
    }
    return {};
}

auto CompilerDetector::default_search_paths() -> std::vector<std::string>
{
    return {
        "/usr/bin",
        "/usr/local/bin",
        "/opt/homebrew/bin",
        "/opt/local/bin",
    };
}

} // namespace markamp::core
