/// @file MagicCommandEngine.cpp
/// @brief V4 Phase 38 – Magic Commands & Environment Manager implementation.

#include "core/MagicCommandEngine.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <chrono>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// MagicCommandEngine
// ============================================================================

MagicCommandEngine::MagicCommandEngine(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto MagicCommandEngine::is_magic(const std::string& source) const -> bool
{
    if (source.empty())
    {
        return false;
    }

    // Find first non-whitespace character.
    size_t idx = 0;
    while (idx < source.size() && (source[idx] == ' ' || source[idx] == '\t'))
    {
        ++idx;
    }

    return idx < source.size() && source[idx] == '%';
}

auto MagicCommandEngine::parse(const std::string& source) const -> std::optional<MagicCommand>
{
    if (!is_magic(source))
    {
        return std::nullopt;
    }

    MagicCommand cmd;
    cmd.raw_source = source;

    // Find the first line.
    const size_t first_newline = source.find('\n');
    const std::string first_line =
        (first_newline != std::string::npos) ? source.substr(0, first_newline) : source;

    // Skip leading whitespace.
    size_t idx = 0;
    while (idx < first_line.size() && (first_line[idx] == ' ' || first_line[idx] == '\t'))
    {
        ++idx;
    }

    // Determine line vs cell magic.
    if (idx + 1 < first_line.size() && first_line[idx] == '%' && first_line[idx + 1] == '%')
    {
        cmd.type = MagicType::kCell;
        idx += 2;
    }
    else if (idx < first_line.size() && first_line[idx] == '%')
    {
        cmd.type = MagicType::kLine;
        idx += 1;
    }
    else
    {
        return std::nullopt;
    }

    // Extract command name.
    const size_t name_start = idx;
    while (idx < first_line.size() && first_line[idx] != ' ' && first_line[idx] != '\t')
    {
        ++idx;
    }
    cmd.name = first_line.substr(name_start, idx - name_start);

    // Skip whitespace after name.
    while (idx < first_line.size() && (first_line[idx] == ' ' || first_line[idx] == '\t'))
    {
        ++idx;
    }

    // Rest of first line is args.
    if (idx < first_line.size())
    {
        cmd.args = first_line.substr(idx);
    }

    // For cell magics, body is everything after first line.
    if (cmd.type == MagicType::kCell && first_newline != std::string::npos)
    {
        cmd.body = source.substr(first_newline + 1);
    }

    return cmd;
}

auto MagicCommandEngine::execute_magic(const MagicCommand& cmd) -> MagicResult
{
    MagicResult result;

    if (cmd.name == "time" || cmd.name == "timeit")
    {
        result = execute_time(cmd.type == MagicType::kCell ? cmd.body : cmd.args);
    }
    else if (cmd.name == "env")
    {
        result = execute_env(cmd.args);
    }
    else if (cmd.name == "who" || cmd.name == "whos")
    {
        result = execute_who(cmd.name);
    }
    else if (cmd.name == "run")
    {
        result = execute_run(cmd.args);
    }
    else if (cmd.name == "help" || cmd.name == "lsmagic")
    {
        result = execute_help(cmd.args);
    }
    else
    {
        // Check custom magics.
        auto custom_it = custom_magics_.find(cmd.name);
        if (custom_it != custom_magics_.end())
        {
            result = custom_it->second(cmd);
        }
        else
        {
            result.success = false;
            result.error = "Unknown magic command: %" + cmd.name;
            return result;
        }
    }

    events::MagicExecutedEvent event;
    event.command = cmd.name;
    event.success = result.success;
    event_bus_.publish(event);

    return result;
}

auto MagicCommandEngine::execute_time(const std::string& code) -> MagicResult
{
    MagicResult result;

    const auto start = std::chrono::steady_clock::now();

    // Stub: we don't actually execute code, just simulate timing.
    // In real implementation, this would delegate to the kernel.
    result.output = "Code: " + code;

    const auto end = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    result.elapsed_ms = static_cast<double>(duration.count()) / 1000.0;
    result.output += "\nWall time: " + std::to_string(result.elapsed_ms) + " ms";
    result.success = true;

    return result;
}

auto MagicCommandEngine::execute_env(const std::string& args) -> MagicResult
{
    MagicResult result;

    if (args.empty())
    {
        // List all env vars.
        std::ostringstream oss;
        for (const auto& [key, value] : env_vars_)
        {
            oss << key << "=" << value << "\n";
        }
        result.output = oss.str();
        result.success = true;
    }
    else
    {
        // Check for KEY=VALUE format.
        const size_t eq_pos = args.find('=');
        if (eq_pos != std::string::npos)
        {
            const std::string key = args.substr(0, eq_pos);
            const std::string value = args.substr(eq_pos + 1);
            env_vars_[key] = value;
            result.output = "env: " + key + "=" + value;
            result.success = true;
        }
        else
        {
            // Get single variable.
            auto env_it = env_vars_.find(args);
            if (env_it != env_vars_.end())
            {
                result.output = env_it->second;
                result.success = true;
            }
            else
            {
                result.output = "";
                result.success = true;
            }
        }
    }

    return result;
}

auto MagicCommandEngine::execute_who(const std::string& args) -> MagicResult
{
    MagicResult result;
    std::ostringstream oss;

    if (args == "whos")
    {
        // Detailed list.
        oss << "Variable   Type\n";
        oss << "--------------------\n";
        for (const auto& [name, type] : variables_)
        {
            oss << name << "   " << type << "\n";
        }
    }
    else
    {
        // Simple name list.
        for (const auto& [name, type] : variables_)
        {
            oss << name << "   ";
        }
        if (!variables_.empty())
        {
            oss << "\n";
        }
    }

    result.output = oss.str();
    result.success = true;
    return result;
}

auto MagicCommandEngine::execute_run(const std::string& path) -> MagicResult
{
    MagicResult result;

    if (path.empty())
    {
        result.success = false;
        result.error = "Usage: %run <filename>";
        return result;
    }

    // Stub: would execute the file via kernel.
    result.output = "Running file: " + path;
    result.success = true;
    return result;
}

auto MagicCommandEngine::execute_help(const std::string& /*args*/) -> MagicResult
{
    MagicResult result;
    auto magics = available_magics();
    std::ostringstream oss;
    oss << "Available magic commands:\n";
    for (const auto& magic_name : magics)
    {
        oss << "  %" << magic_name << "\n";
    }
    result.output = oss.str();
    result.success = true;
    return result;
}

auto MagicCommandEngine::register_custom(const std::string& name, MagicHandler handler) -> void
{
    custom_magics_[name] = std::move(handler);
}

auto MagicCommandEngine::available_magics() const -> std::vector<std::string>
{
    std::vector<std::string> magics = {
        "time", "timeit", "env", "who", "whos", "run", "help", "lsmagic"};

    for (const auto& [name, _handler] : custom_magics_)
    {
        magics.push_back(name);
    }

    std::sort(magics.begin(), magics.end());
    return magics;
}

auto MagicCommandEngine::set_variables(const std::vector<std::pair<std::string, std::string>>& vars)
    -> void
{
    variables_ = vars;
}

auto MagicCommandEngine::set_env_vars(const std::unordered_map<std::string, std::string>& env_vars)
    -> void
{
    env_vars_ = env_vars;
}

// ============================================================================
// EnvironmentManager
// ============================================================================

EnvironmentManager::EnvironmentManager(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto EnvironmentManager::detect_environments() -> std::vector<EnvironmentInfo>
{
    // Stub: In production, this would scan the filesystem.
    events::EnvironmentDetectedEvent event;
    event.count = static_cast<int>(environments_.size());
    event_bus_.publish(event);

    return environments_;
}

auto EnvironmentManager::active_environment() const -> std::optional<EnvironmentInfo>
{
    for (const auto& env_info : environments_)
    {
        if (env_info.name == active_name_)
        {
            return env_info;
        }
    }
    return std::nullopt;
}

auto EnvironmentManager::switch_environment(const std::string& name) -> bool
{
    for (const auto& env_info : environments_)
    {
        if (env_info.name == name)
        {
            active_name_ = name;

            events::EnvironmentChangedEvent event;
            event.environment = name;
            event_bus_.publish(event);

            return true;
        }
    }
    return false;
}

auto EnvironmentManager::environment_info(const std::string& name) const
    -> std::optional<EnvironmentInfo>
{
    for (const auto& env_info : environments_)
    {
        if (env_info.name == name)
        {
            return env_info;
        }
    }
    return std::nullopt;
}

auto EnvironmentManager::add_environment(const EnvironmentInfo& env_info) -> void
{
    environments_.push_back(env_info);
    if (active_name_.empty() && env_info.is_active)
    {
        active_name_ = env_info.name;
    }
}

} // namespace markamp::core
