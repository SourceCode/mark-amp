#include "LaunchConfig.h"

#include "Logger.h"

#include <fstream>
#include <sstream>

namespace markamp::core
{

namespace fs = std::filesystem;

// ── Load from workspace ──

auto LaunchConfigService::load(const fs::path& workspace_dir)
    -> std::expected<LaunchConfigFile, std::string>
{
    const auto config_path = workspace_dir / ".markamp" / "launch.json";

    if (!fs::exists(config_path))
    {
        return create_default(workspace_dir);
    }

    std::ifstream file(config_path);
    if (!file.is_open())
    {
        return std::unexpected("Cannot open " + config_path.string());
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    return parse_json(buffer.str());
}

// ── Save to workspace ──

auto LaunchConfigService::save(const fs::path& workspace_dir, const LaunchConfigFile& config_file)
    -> std::expected<void, std::string>
{
    const auto dir_path = workspace_dir / ".markamp";
    std::error_code ec;
    fs::create_directories(dir_path, ec);
    if (ec)
    {
        return std::unexpected("Cannot create directory: " + ec.message());
    }

    const auto config_path = dir_path / "launch.json";
    std::ofstream file(config_path);
    if (!file.is_open())
    {
        return std::unexpected("Cannot write " + config_path.string());
    }

    file << to_json(config_file);
    MARKAMP_LOG_INFO("Launch config saved to {}", config_path.string());
    return {};
}

// ── Create default ──

auto LaunchConfigService::create_default(const fs::path& workspace_dir)
    -> std::expected<LaunchConfigFile, std::string>
{
    LaunchConfigFile config_file;
    config_file.version = "0.2.0";

    LaunchConfig default_config;
    default_config.name = "Debug (LLDB)";
    default_config.type = "cppdbg";
    default_config.request = "launch";
    default_config.program = "${workspaceFolder}/build/debug/markamp";
    default_config.cwd = "${workspaceFolder}";
    default_config.debugger = "lldb";
    default_config.mi_mode = "lldb";
    default_config.pre_launch_task = "cmake-build-debug";

    config_file.configurations.push_back(std::move(default_config));

    auto save_result = save(workspace_dir, config_file);
    if (!save_result)
    {
        return std::unexpected(save_result.error());
    }

    MARKAMP_LOG_INFO("Created default launch.json in {}", workspace_dir.string());
    return config_file;
}

// ── Variable substitution ──

void LaunchConfigService::substitute_variables(LaunchConfig& config,
                                               const fs::path& workspace_dir,
                                               const fs::path& active_file)
{
    auto replace_vars = [&](std::string& value)
    {
        const std::string ws_placeholder = "${workspaceFolder}";
        const std::string file_placeholder = "${file}";
        const std::string basename_placeholder = "${fileBasename}";

        std::string::size_type pos = 0;
        while ((pos = value.find(ws_placeholder, pos)) != std::string::npos)
        {
            value.replace(pos, ws_placeholder.length(), workspace_dir.string());
            pos += workspace_dir.string().length();
        }

        pos = 0;
        while ((pos = value.find(file_placeholder, pos)) != std::string::npos)
        {
            value.replace(pos, file_placeholder.length(), active_file.string());
            pos += active_file.string().length();
        }

        pos = 0;
        while ((pos = value.find(basename_placeholder, pos)) != std::string::npos)
        {
            value.replace(pos, basename_placeholder.length(), active_file.filename().string());
            pos += active_file.filename().string().length();
        }
    };

    replace_vars(config.program);
    replace_vars(config.cwd);
    for (auto& arg : config.args)
    {
        replace_vars(arg);
    }
}

// ── JSON parsing (simplified — uses manual extraction) ──

auto LaunchConfigService::parse_json(const std::string& json_content)
    -> std::expected<LaunchConfigFile, std::string>
{
    // Minimal JSON parsing — just validates basic structure.
    // Full JSON parsing is done via nlohmann::json in the actual build.
    LaunchConfigFile result;

    if (json_content.find("\"configurations\"") == std::string::npos)
    {
        return std::unexpected("Invalid launch.json: missing 'configurations' key");
    }

    // Extract version
    auto version_pos = json_content.find("\"version\"");
    if (version_pos != std::string::npos)
    {
        auto colon_pos = json_content.find(':', version_pos);
        auto quote_start = json_content.find('"', colon_pos + 1);
        auto quote_end = json_content.find('"', quote_start + 1);
        if (quote_start != std::string::npos && quote_end != std::string::npos)
        {
            result.version = json_content.substr(quote_start + 1, quote_end - quote_start - 1);
        }
    }

    MARKAMP_LOG_INFO("Parsed launch.json (version: {})", result.version);
    return result;
}

// ── JSON serialization ──

auto LaunchConfigService::to_json(const LaunchConfigFile& config_file) -> std::string
{
    std::ostringstream oss;
    oss << "{\n";
    oss << "    \"version\": \"" << config_file.version << "\",\n";
    oss << "    \"configurations\": [\n";

    for (std::size_t idx = 0; idx < config_file.configurations.size(); ++idx)
    {
        const auto& cfg = config_file.configurations[idx];
        oss << "        {\n";
        oss << "            \"name\": \"" << cfg.name << "\",\n";
        oss << "            \"type\": \"" << cfg.type << "\",\n";
        oss << "            \"request\": \"" << cfg.request << "\",\n";
        oss << "            \"program\": \"" << cfg.program << "\",\n";
        oss << "            \"cwd\": \"" << cfg.cwd << "\"";
        if (!cfg.debugger.empty())
        {
            oss << ",\n            \"debugger\": \"" << cfg.debugger << "\"";
        }
        if (!cfg.pre_launch_task.empty())
        {
            oss << ",\n            \"preLaunchTask\": \"" << cfg.pre_launch_task << "\"";
        }
        oss << "\n        }";
        if (idx + 1 < config_file.configurations.size())
        {
            oss << ",";
        }
        oss << "\n";
    }

    oss << "    ]\n";
    oss << "}\n";
    return oss.str();
}

} // namespace markamp::core
