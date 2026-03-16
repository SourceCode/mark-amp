/// @file ChangelogEngine.h
/// @brief V9 Phase 50 — Changelog entry management and markdown generation.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// A single changelog entry.
struct ChangelogEntry
{
    std::string date;
    std::string version;
    std::string category; ///< "Added", "Fixed", "Changed", "Removed", etc.
    std::string message;
};

/// Manages changelog entries and generates Keep-a-Changelog formatted output.
class ChangelogEngine
{
public:
    ChangelogEngine() = default;

    // ── Entry management ──────────────────────────────────────────────
    void add_entry(ChangelogEntry entry);
    [[nodiscard]] auto get_entries(const std::string& version) const -> std::vector<ChangelogEntry>;
    [[nodiscard]] auto get_all_entries() const -> const std::vector<ChangelogEntry>&;

    // ── Generation ────────────────────────────────────────────────────
    [[nodiscard]] auto generate_markdown() const -> std::string;
    void load_defaults();

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto entry_count() const -> int;
    void clear();

    // (#59) Version removal and entry search.
    void remove_entries_for_version(const std::string& version);
    [[nodiscard]] auto search_entries(const std::string& keyword) const
        -> std::vector<ChangelogEntry>;

private:
    std::vector<ChangelogEntry> entries_;
};

} // namespace markamp::core
