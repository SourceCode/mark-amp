/// @file MagicCommandEngine.h
/// @brief V4 Phase 38 – Magic Commands & Environment Manager.

#pragma once

#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Data structures
// ============================================================================

/// Magic command type.
enum class MagicType
{
    kLine, ///< Single-line magic (%cmd).
    kCell  ///< Cell-level magic (%%cmd).
};

/// Parsed magic command.
struct MagicCommand
{
    MagicType type{MagicType::kLine};
    std::string name;       ///< Command name (e.g. "time", "env").
    std::string args;       ///< Arguments after command name.
    std::string body;       ///< For cell magics, the cell body after first line.
    std::string raw_source; ///< Original source text.
};

/// Result of executing a magic command.
struct MagicResult
{
    bool success{false};
    std::string output;
    std::string error;
    double elapsed_ms{0.0};
};

/// Information about a detected environment.
struct EnvironmentInfo
{
    std::string name;
    std::string python_version;
    std::string path;
    bool is_active{false};
    std::vector<std::string> packages;
};

// ============================================================================
// MagicCommandEngine
// ============================================================================

/// Handler function type for custom magic commands.
using MagicHandler = std::function<MagicResult(const MagicCommand&)>;

class MagicCommandEngine
{
public:
    explicit MagicCommandEngine(EventBus& event_bus);

    /// Check if source contains a magic command.
    [[nodiscard]] auto is_magic(const std::string& source) const -> bool;

    /// Parse a magic command from source text.
    [[nodiscard]] auto parse(const std::string& source) const -> std::optional<MagicCommand>;

    /// Execute a magic command.
    [[nodiscard]] auto execute_magic(const MagicCommand& cmd) -> MagicResult;

    /// Built-in magics.
    [[nodiscard]] auto execute_time(const std::string& code) -> MagicResult;
    [[nodiscard]] auto execute_env(const std::string& args) -> MagicResult;
    [[nodiscard]] auto execute_who(const std::string& args) -> MagicResult;
    [[nodiscard]] auto execute_run(const std::string& path) -> MagicResult;
    [[nodiscard]] auto execute_help(const std::string& args) -> MagicResult;

    /// Register a custom magic command.
    auto register_custom(const std::string& name, MagicHandler handler) -> void;

    /// List all available magic commands.
    [[nodiscard]] auto available_magics() const -> std::vector<std::string>;

    /// Set variables for %who/%whos introspection.
    auto set_variables(const std::vector<std::pair<std::string, std::string>>& vars) -> void;

    /// Set environment variables for %env.
    auto set_env_vars(const std::unordered_map<std::string, std::string>& env_vars) -> void;

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, MagicHandler> custom_magics_;
    std::vector<std::pair<std::string, std::string>> variables_; ///< (name, type) pairs.
    std::unordered_map<std::string, std::string> env_vars_;
};

// ============================================================================
// EnvironmentManager
// ============================================================================

class EnvironmentManager
{
public:
    explicit EnvironmentManager(EventBus& event_bus);

    /// Detect available environments on the system (stubbed).
    [[nodiscard]] auto detect_environments() -> std::vector<EnvironmentInfo>;

    /// Get the currently active environment.
    [[nodiscard]] auto active_environment() const -> std::optional<EnvironmentInfo>;

    /// Switch to a named environment.
    [[nodiscard]] auto switch_environment(const std::string& name) -> bool;

    /// Get info about a specific environment.
    [[nodiscard]] auto environment_info(const std::string& name) const
        -> std::optional<EnvironmentInfo>;

    /// Add environment for testing.
    auto add_environment(const EnvironmentInfo& env_info) -> void;

private:
    EventBus& event_bus_;
    std::vector<EnvironmentInfo> environments_;
    std::string active_name_;
};

} // namespace markamp::core
