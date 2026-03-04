/// @file RunConfigService.h
/// @brief Run configuration management — named launch configs for run/debug.
///
/// Features:
///   - Named run configurations (command, working directory, environment)
///   - Build-before-run flag per configuration
///   - Active configuration tracking and recent list
///   - Serialization to/from .markamp/launch.json
///
/// @see RunConfigDropdown, Toolbar, BuildService

#pragma once

#include "EventBus.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// A named run/debug configuration.
struct RunConfiguration
{
    std::string name;
    std::string command;
    std::string working_directory;
    std::vector<std::pair<std::string, std::string>> environment;
    bool build_before_run{true};
    bool use_terminal{true};  ///< Run in integrated terminal
    std::string type;         ///< "shell", "cmake", "ctest"
    std::string cmake_target; ///< For cmake type
};

/// Service for managing run/debug configurations.
class RunConfigService
{
public:
    explicit RunConfigService(EventBus& event_bus);

    /// Load configurations from a JSON file.
    [[nodiscard]] auto load_from_file(const std::string& path) -> bool;

    /// Save configurations to a JSON file.
    [[nodiscard]] auto save_to_file(const std::string& path) const -> bool;

    /// Add a new configuration. Rejects empty name/command.
    void add_configuration(RunConfiguration config);

    /// Remove a configuration by name.
    void remove_configuration(const std::string& name);

    /// Update an existing configuration (matched by name).
    void update_configuration(const RunConfiguration& config);

    /// Get all configurations.
    [[nodiscard]] auto configurations() const -> const std::vector<RunConfiguration>&;

    /// Find a configuration by name.
    [[nodiscard]] auto find(const std::string& name) const -> const RunConfiguration*;

    /// Get the currently active configuration.
    [[nodiscard]] auto active_configuration() const -> const RunConfiguration*;

    /// Set the active configuration by name.
    void set_active(const std::string& name);

    /// Get the most recently used configuration names.
    [[nodiscard]] auto recent_configurations(int max_count = 5) const -> std::vector<std::string>;

private:
    EventBus& event_bus_;
    std::vector<RunConfiguration> configurations_;
    std::string active_name_;
    std::vector<std::string> recent_;
};

} // namespace markamp::core
