/// @file MakeIntegration.cpp
/// @brief Phase 38 Task 5 — Makefile integration implementation.

#include "core/MakeIntegration.h"

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <regex>
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

auto MakeIntegration::detect_makefile(const std::string& workspace_path) const -> bool
{
    return std::filesystem::exists(std::filesystem::path(workspace_path) / "Makefile") ||
           std::filesystem::exists(std::filesystem::path(workspace_path) / "makefile") ||
           std::filesystem::exists(std::filesystem::path(workspace_path) / "GNUmakefile");
}

auto MakeIntegration::parse_targets(const std::string& makefile_path) const
    -> std::vector<MakeTarget>
{
    std::vector<MakeTarget> targets;

    std::ifstream file(makefile_path);
    if (!file.is_open())
        return targets;

    std::string line;
    std::vector<std::string> phony_targets;
    bool first_target = true;

    // Regex for target lines: "target: [deps]"
    std::regex target_re(R"(^([a-zA-Z_][a-zA-Z0-9_.-]*)\s*:.*)");
    // Regex for .PHONY declarations
    std::regex phony_re(R"(^\.PHONY\s*:\s*(.*))");

    while (std::getline(file, line))
    {
        // Check for .PHONY
        std::smatch phony_match;
        if (std::regex_match(line, phony_match, phony_re))
        {
            std::istringstream iss(phony_match[1].str());
            std::string target_name;
            while (iss >> target_name)
            {
                phony_targets.push_back(target_name);
            }
            continue;
        }

        // Check for target definitions
        std::smatch target_match;
        if (std::regex_match(line, target_match, target_re))
        {
            MakeTarget target;
            target.name = target_match[1].str();

            // Skip internal targets
            if (target.name[0] == '.' || target.name[0] == '_')
                continue;

            if (first_target)
            {
                target.is_default = true;
                first_target = false;
            }

            targets.push_back(std::move(target));
        }
    }

    // Mark phony targets
    for (auto& target : targets)
    {
        for (const auto& phony : phony_targets)
        {
            if (target.name == phony)
            {
                target.is_phony = true;
                break;
            }
        }
    }

    return targets;
}

auto MakeIntegration::build_command(const std::string& target,
                                    const std::string& makefile_dir,
                                    int parallel_jobs) const -> std::string
{
    auto make = find_make();
    if (make.empty())
        make = "make";

    std::ostringstream cmd;
    cmd << make << " -C " << makefile_dir;
    if (!target.empty())
        cmd << " " << target;
    if (parallel_jobs > 0)
        cmd << " -j" << parallel_jobs;
    return cmd.str();
}

auto MakeIntegration::clean_command(const std::string& makefile_dir) const -> std::string
{
    return build_command("clean", makefile_dir);
}

auto MakeIntegration::find_make() -> std::string
{
    auto output = run_cmd("which make");
    if (!output.empty())
    {
        while (!output.empty() && (output.back() == '\n' || output.back() == '\r'))
            output.pop_back();
        return output;
    }
    return {};
}

} // namespace markamp::core
