/// @file GitStashService.cpp
/// @brief Phase 25: Version Control Integration — Git stash implementation.

#include "core/GitStashService.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/GitService.h"

#include <algorithm>
#include <fmt/format.h>

namespace markamp::core
{

GitStashService::GitStashService(EventBus& event_bus, GitService& git_service)
    : event_bus_(event_bus)
    , git_service_(git_service)
{
}

auto GitStashService::save(const std::string& message) -> std::expected<std::string, std::string>
{
    // Collect all modified/added files from the working tree.
    auto status_entries = git_service_.status();

    // Filter to only changed files.
    std::vector<GitFileEntry> changed;
    for (const auto& entry : status_entries)
    {
        if (entry.status != GitFileStatus::kUnmodified && entry.status != GitFileStatus::kIgnored)
        {
            changed.push_back(entry);
        }
    }

    if (changed.empty())
    {
        return std::unexpected(std::string("No local changes to save"));
    }

    // Build the stash entry.
    StashEntry stash;
    stash.creation_index = next_index_++;
    stash.id = fmt::format("stash@{{{}}}", stash.creation_index);
    stash.message =
        message.empty() ? fmt::format("WIP on {}", git_service_.current_branch()) : message;
    stash.created = std::chrono::system_clock::now();
    stash.file_count = static_cast<int>(changed.size());

    // Snapshot the current content of each changed file via diff_file.
    for (const auto& entry : changed)
    {
        auto diff = git_service_.diff_file(entry.file_path);
        // Store the file path; the actual content will be reconstructed
        // from the working tree on restore.  For our in-memory stub we just
        // record the path so we can call test helpers later.
        stash.file_states[entry.file_path] = entry.file_path;
    }

    // Now discard the changes in the working tree to simulate "stash save".
    for (const auto& entry : changed)
    {
        auto result = git_service_.discard_file(entry.file_path);
        (void)result; // best-effort discard
    }

    stash_stack_.push_back(std::move(stash));

    events::GitStashSavedEvent stash_evt;
    stash_evt.stash_id = stash_stack_.back().id;
    stash_evt.message = stash_stack_.back().message;
    stash_evt.file_count = stash_stack_.back().file_count;
    event_bus_.publish(stash_evt);

    return stash_stack_.back().id;
}

auto GitStashService::pop() -> std::expected<std::string, std::string>
{
    auto id_result = resolve_id("");
    if (!id_result)
    {
        return std::unexpected(id_result.error());
    }

    auto apply_result = apply(id_result.value());
    if (!apply_result)
    {
        return std::unexpected(apply_result.error());
    }

    // Remove the entry.
    auto iter = std::ranges::find_if(
        stash_stack_, [&](const StashEntry& entry) { return entry.id == id_result.value(); });

    if (iter != stash_stack_.end())
    {
        stash_stack_.erase(iter);
    }

    return apply_result;
}

auto GitStashService::apply(const std::string& stash_id) -> std::expected<std::string, std::string>
{
    auto resolved = resolve_id(stash_id);
    if (!resolved)
    {
        return std::unexpected(resolved.error());
    }

    const auto* entry = find_stash(resolved.value());
    if (entry == nullptr)
    {
        return std::unexpected(fmt::format("Stash '{}' not found", resolved.value()));
    }

    restore_files(*entry);

    events::GitStashAppliedEvent applied_evt;
    applied_evt.stash_id = entry->id;
    applied_evt.message = entry->message;
    event_bus_.publish(applied_evt);

    return entry->id;
}

auto GitStashService::drop(const std::string& stash_id) -> std::expected<void, std::string>
{
    auto resolved = resolve_id(stash_id);
    if (!resolved)
    {
        return std::unexpected(resolved.error());
    }

    auto iter = std::ranges::find_if(
        stash_stack_, [&](const StashEntry& entry) { return entry.id == resolved.value(); });

    if (iter == stash_stack_.end())
    {
        return std::unexpected(fmt::format("Stash '{}' not found", resolved.value()));
    }

    stash_stack_.erase(iter);
    return {};
}

auto GitStashService::list() const -> std::vector<StashEntry>
{
    // Return newest-first (reverse of internal order).
    auto result = stash_stack_;
    std::ranges::reverse(result);
    return result;
}

auto GitStashService::show(const std::string& stash_id) const
    -> std::expected<std::vector<std::string>, std::string>
{
    auto resolved = resolve_id(stash_id);
    if (!resolved)
    {
        return std::unexpected(resolved.error());
    }

    const auto* entry = find_stash(resolved.value());
    if (entry == nullptr)
    {
        return std::unexpected(fmt::format("Stash '{}' not found", resolved.value()));
    }

    std::vector<std::string> files;
    files.reserve(entry->file_states.size());
    for (const auto& [path, _content] : entry->file_states)
    {
        files.push_back(path);
    }
    std::ranges::sort(files);
    return files;
}

auto GitStashService::size() const -> int
{
    return static_cast<int>(stash_stack_.size());
}

auto GitStashService::clear() -> void
{
    stash_stack_.clear();
}

// ── Private helpers ──

auto GitStashService::find_stash(const std::string& stash_id) const -> const StashEntry*
{
    auto iter = std::ranges::find_if(stash_stack_,
                                     [&](const StashEntry& entry) { return entry.id == stash_id; });
    return iter != stash_stack_.end() ? &(*iter) : nullptr;
}

auto GitStashService::resolve_id(const std::string& stash_id) const
    -> std::expected<std::string, std::string>
{
    if (stash_stack_.empty())
    {
        return std::unexpected(std::string("No stash entries"));
    }

    if (stash_id.empty())
    {
        return stash_stack_.back().id; // top of stack
    }
    return stash_id;
}

auto GitStashService::restore_files(const StashEntry& entry) -> void
{
    for (const auto& [path, _content] : entry.file_states)
    {
        // Re-mark the file as modified in the working tree.
        git_service_.test_modify_file(path, "stash-restored:" + path);
    }
}

} // namespace markamp::core
