#pragma once

#include <expected>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace markamp::core
{

/// Phase 19 Task 2: Launch configuration for running/debugging programs.
/// Corresponds to a single entry in `.markamp/launch.json`.
struct LaunchConfig
{
    std::string name;                       // "Debug MarkAmp"
    std::string type;                       // "cppdbg", "python", "node"
    std::string request;                    // "launch" or "attach"
    std::string program;                    // path to executable
    std::vector<std::string> args;          // command line arguments
    std::string cwd;                        // working directory
    std::map<std::string, std::string> env; // environment variables
    std::string pre_launch_task;            // build task to run first
    bool stop_on_entry{false};
    int port{0}; // for attach mode

    // C++ specific
    std::string debugger; // "lldb" or "gdb"
    std::string mi_mode;  // "lldb" or "gdb"

    // Compound launch (Task 23)
    std::vector<std::string> compound_configs; // sub-configurations
};

/// A collection of launch configurations loaded from launch.json.
struct LaunchConfigFile
{
    std::string version{"0.2.0"};
    std::vector<LaunchConfig> configurations;
};

/// Service for loading, saving, and managing launch configurations.
class LaunchConfigService
{
public:
    /// Load configurations from a workspace directory's .markamp/launch.json.
    [[nodiscard]] auto load(const std::filesystem::path& workspace_dir)
        -> std::expected<LaunchConfigFile, std::string>;

    /// Save configurations to .markamp/launch.json.
    [[nodiscard]] auto save(const std::filesystem::path& workspace_dir,
                            const LaunchConfigFile& config_file)
        -> std::expected<void, std::string>;

    /// Create a default launch.json with a C++ debug template.
    [[nodiscard]] auto create_default(const std::filesystem::path& workspace_dir)
        -> std::expected<LaunchConfigFile, std::string>;

    /// Perform variable substitution on a config's paths.
    /// Replaces ${workspaceFolder}, ${file}, ${fileBasename}.
    static void substitute_variables(LaunchConfig& config,
                                     const std::filesystem::path& workspace_dir,
                                     const std::filesystem::path& active_file);

private:
    /// Parse a JSON string into a LaunchConfigFile.
    [[nodiscard]] static auto parse_json(const std::string& json_content)
        -> std::expected<LaunchConfigFile, std::string>;

    /// Serialize a LaunchConfigFile to JSON string.
    [[nodiscard]] static auto to_json(const LaunchConfigFile& config_file) -> std::string;
};

} // namespace markamp::core
