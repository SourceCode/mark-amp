#pragma once

/// @file BuildLogManager.h
/// @brief Phase 38 Task 29 — Build log persistence and comparison.

#include <string>
#include <vector>

namespace markamp::core
{

/// A stored build log entry.
struct BuildLogEntry
{
    std::string timestamp;
    std::string build_config;
    std::string target;
    bool success{false};
    int error_count{0};
    int warning_count{0};
    float duration_seconds{0.0F};
    std::string output;
};

/// Manages build log storage, export, and comparison.
class BuildLogManager
{
public:
    BuildLogManager() = default;

    void add_entry(BuildLogEntry entry);
    [[nodiscard]] auto entries() const -> const std::vector<BuildLogEntry>&;
    [[nodiscard]] auto latest() const -> const BuildLogEntry*;
    [[nodiscard]] auto entry_count() const -> std::size_t;
    void clear();

    [[nodiscard]] static auto export_text(const BuildLogEntry& entry) -> std::string;
    [[nodiscard]] static auto export_html(const BuildLogEntry& entry) -> std::string;
    [[nodiscard]] static auto compare(const BuildLogEntry& lhs, const BuildLogEntry& rhs)
        -> std::string;

    void save_logs(const std::string& dir_path) const;
    void load_logs(const std::string& dir_path);

    static constexpr std::size_t kMaxLogs = 50;

private:
    std::vector<BuildLogEntry> entries_;
};

} // namespace markamp::core
