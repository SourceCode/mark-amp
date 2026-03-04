/// @file CMakeIntegration.cpp
/// @brief Phase 38 Tasks 2-4 — CMake integration implementation.

#include "core/CMakeIntegration.h"

#include <nlohmann/json.hpp>

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace markamp::core
{

namespace
{

auto run_cmd(const std::string& cmd) -> std::string
{
    std::string result;
    std::array<char, 256> buffer{};
    auto* pipe = popen((cmd + " 2>&1").c_str(), "r");
    if (!pipe)
        return {};
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
        result += buffer.data();
    pclose(pipe);
    return result;
}

} // namespace

auto CMakeIntegration::detect_project(const std::string& workspace_path) const -> bool
{
    return std::filesystem::exists(std::filesystem::path(workspace_path) / "CMakeLists.txt");
}

auto CMakeIntegration::has_presets(const std::string& workspace_path) const -> bool
{
    return std::filesystem::exists(std::filesystem::path(workspace_path) / "CMakePresets.json") ||
           std::filesystem::exists(std::filesystem::path(workspace_path) / "CMakeUserPresets.json");
}

auto CMakeIntegration::parse_presets(const std::string& workspace_path) const
    -> std::vector<CMakePreset>
{
    std::vector<CMakePreset> presets;

    auto presets_path = std::filesystem::path(workspace_path) / "CMakePresets.json";
    if (!std::filesystem::exists(presets_path))
        return presets;

    std::ifstream file(presets_path);
    if (!file.is_open())
        return presets;

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto j = nlohmann::json::parse(content, nullptr, false);
    if (j.is_discarded())
        return presets;

    // Parse configurePresets
    if (j.contains("configurePresets"))
    {
        for (const auto& p : j["configurePresets"])
        {
            CMakePreset preset;
            preset.name = p.value("name", "");
            preset.display_name = p.value("displayName", preset.name);
            preset.description = p.value("description", "");
            preset.hidden = p.value("hidden", false);

            if (p.contains("generator"))
                preset.generator = p["generator"].get<std::string>();
            if (p.contains("binaryDir"))
                preset.build_dir = p["binaryDir"].get<std::string>();
            if (p.contains("cacheVariables") && p["cacheVariables"].contains("CMAKE_BUILD_TYPE"))
                preset.cmake_build_type =
                    p["cacheVariables"]["CMAKE_BUILD_TYPE"].get<std::string>();

            if (!preset.hidden)
                presets.push_back(std::move(preset));
        }
    }

    return presets;
}

auto CMakeIntegration::configure_command(const std::string& workspace_path,
                                         const std::string& build_dir,
                                         const std::string& build_type) const -> std::string
{
    auto cmake = find_cmake();
    if (cmake.empty())
        cmake = "cmake";

    std::ostringstream cmd;
    cmd << cmake << " -S " << workspace_path << " -B " << build_dir
        << " -DCMAKE_BUILD_TYPE=" << build_type;
    return cmd.str();
}

auto CMakeIntegration::configure_with_preset(const std::string& workspace_path,
                                             const std::string& preset_name) const -> std::string
{
    auto cmake = find_cmake();
    if (cmake.empty())
        cmake = "cmake";

    std::ostringstream cmd;
    cmd << cmake << " --preset " << preset_name << " -S " << workspace_path;
    return cmd.str();
}

auto CMakeIntegration::build_command(const std::string& build_dir,
                                     const std::string& target,
                                     int parallel_jobs) const -> std::string
{
    auto cmake = find_cmake();
    if (cmake.empty())
        cmake = "cmake";

    std::ostringstream cmd;
    cmd << cmake << " --build " << build_dir;
    if (!target.empty())
        cmd << " --target " << target;
    if (parallel_jobs > 0)
        cmd << " -j" << parallel_jobs;
    return cmd.str();
}

auto CMakeIntegration::clean_command(const std::string& build_dir) const -> std::string
{
    auto cmake = find_cmake();
    if (cmake.empty())
        cmake = "cmake";
    return cmake + " --build " + build_dir + " --target clean";
}

auto CMakeIntegration::discover_targets(const std::string& build_dir) const
    -> std::vector<CMakeTarget>
{
    std::vector<CMakeTarget> targets;

    // Try to read targets from cmake file API
    auto reply_dir = std::filesystem::path(build_dir) / ".cmake" / "api" / "v1" / "reply";
    if (!std::filesystem::exists(reply_dir))
        return targets;

    for (const auto& entry : std::filesystem::directory_iterator(reply_dir))
    {
        if (entry.path().filename().string().find("target-") != 0)
            continue;

        std::ifstream file(entry.path());
        if (!file.is_open())
            continue;

        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        auto j = nlohmann::json::parse(content, nullptr, false);
        if (j.is_discarded())
            continue;

        CMakeTarget target;
        target.name = j.value("name", "");
        target.type = j.value("type", "");
        if (j.contains("sourceDirectory"))
            target.source_dir = j["sourceDirectory"].get<std::string>();

        if (!target.name.empty())
            targets.push_back(std::move(target));
    }

    return targets;
}

auto CMakeIntegration::find_cmake() -> std::string
{
    auto output = run_cmd("which cmake");
    if (!output.empty())
    {
        while (!output.empty() && (output.back() == '\n' || output.back() == '\r'))
            output.pop_back();
        return output;
    }
    return {};
}

auto CMakeIntegration::cmake_version() -> std::string
{
    auto output = run_cmd("cmake --version");
    if (output.empty())
        return {};

    // Extract "cmake version X.Y.Z"
    auto pos = output.find("cmake version ");
    if (pos == std::string::npos)
        return {};
    auto ver_start = pos + 14;
    auto ver_end = output.find('\n', ver_start);
    if (ver_end == std::string::npos)
        ver_end = output.size();
    return output.substr(ver_start, ver_end - ver_start);
}

} // namespace markamp::core
