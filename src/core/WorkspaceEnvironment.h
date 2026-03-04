#pragma once

/// @file WorkspaceEnvironment.h
/// @brief Phase 40 Task 10 — Per-workspace environment variables.

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Manages workspace-specific environment variables.
class WorkspaceEnvironment
{
public:
    WorkspaceEnvironment() = default;

    /// Set an environment variable.
    void set(const std::string& key, const std::string& value);

    /// Get an environment variable.
    [[nodiscard]] auto get(const std::string& key) const -> std::string;

    /// Check if a variable exists.
    [[nodiscard]] auto has(const std::string& key) const -> bool;

    /// Remove a variable.
    auto remove(const std::string& key) -> bool;

    /// Get all variables.
    [[nodiscard]] auto all() const -> const std::unordered_map<std::string, std::string>&;

    /// Expand variable references in a string (${VAR} syntax).
    [[nodiscard]] auto expand(const std::string& input) const -> std::string;

    /// Merge system env with workspace env (workspace overrides).
    [[nodiscard]] auto merged_environment() const -> std::unordered_map<std::string, std::string>;

    /// Clear all variables.
    void clear();

    /// Count.
    [[nodiscard]] auto count() const -> std::size_t;

private:
    std::unordered_map<std::string, std::string> vars_;
};

} // namespace markamp::core
