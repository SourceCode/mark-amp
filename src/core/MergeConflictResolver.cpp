/// @file MergeConflictResolver.cpp
/// @brief Phase 25: Version Control Integration — Merge conflict resolution.

#include "core/MergeConflictResolver.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <sstream>

namespace markamp::core
{

MergeConflictResolver::MergeConflictResolver(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto MergeConflictResolver::parse_conflicts(const std::string& file_path,
                                            const std::string& content) -> ConflictFile
{
    ConflictFile result;
    result.file_path = file_path;
    result.original_content = content;

    std::istringstream stream(content);
    std::string line;
    int line_num = 0;

    // State machine for parsing conflict markers.
    enum class State
    {
        kNormal,
        kInLocal,
        kInRemote
    };

    State state = State::kNormal;
    ConflictRegion current_region;

    while (std::getline(stream, line))
    {
        ++line_num;

        if (line.starts_with("<<<<<<<"))
        {
            state = State::kInLocal;
            current_region = ConflictRegion{};
            current_region.start_line = line_num;
        }
        else if (line.starts_with("=======") && state == State::kInLocal)
        {
            state = State::kInRemote;
        }
        else if (line.starts_with(">>>>>>>") && state == State::kInRemote)
        {
            current_region.end_line = line_num;
            result.regions.push_back(std::move(current_region));
            current_region = ConflictRegion{};
            state = State::kNormal;
        }
        else if (state == State::kInLocal)
        {
            if (!current_region.local_content.empty())
            {
                current_region.local_content += "\n";
            }
            current_region.local_content += line;
        }
        else if (state == State::kInRemote)
        {
            if (!current_region.remote_content.empty())
            {
                current_region.remote_content += "\n";
            }
            current_region.remote_content += line;
        }
    }

    if (!result.regions.empty())
    {
        events::MergeConflictsDetectedEvent detected_evt;
        detected_evt.file_path = file_path;
        detected_evt.conflict_count = static_cast<int>(result.regions.size());
        event_bus_.publish(detected_evt);
    }

    return result;
}

auto MergeConflictResolver::resolve_region(ConflictFile& file,
                                           int region_index,
                                           ResolutionStrategy strategy) -> bool
{
    if (region_index < 0 || region_index >= static_cast<int>(file.regions.size()))
    {
        return false;
    }

    auto& region = file.regions[static_cast<size_t>(region_index)];

    switch (strategy)
    {
        case ResolutionStrategy::kUseLocal:
            region.resolved_content = region.local_content;
            break;
        case ResolutionStrategy::kUseRemote:
            region.resolved_content = region.remote_content;
            break;
        case ResolutionStrategy::kUseBase:
            region.resolved_content = region.base_content;
            break;
        case ResolutionStrategy::kManual:
            return false; // Manual requires explicit content.
    }

    region.is_resolved = true;

    // If all regions are now resolved, publish an event.
    if (file.all_resolved())
    {
        std::string strategy_name;
        switch (strategy)
        {
            case ResolutionStrategy::kUseLocal:
                strategy_name = "local";
                break;
            case ResolutionStrategy::kUseRemote:
                strategy_name = "remote";
                break;
            case ResolutionStrategy::kUseBase:
                strategy_name = "base";
                break;
            case ResolutionStrategy::kManual:
                strategy_name = "manual";
                break;
        }

        events::MergeConflictsResolvedEvent resolved_evt;
        resolved_evt.file_path = file.file_path;
        resolved_evt.strategy = strategy_name;
        event_bus_.publish(resolved_evt);
    }

    return true;
}

auto MergeConflictResolver::resolve_region_manual(ConflictFile& file,
                                                  int region_index,
                                                  const std::string& content) -> bool
{
    if (region_index < 0 || region_index >= static_cast<int>(file.regions.size()))
    {
        return false;
    }

    auto& region = file.regions[static_cast<size_t>(region_index)];
    region.resolved_content = content;
    region.is_resolved = true;

    if (file.all_resolved())
    {
        events::MergeConflictsResolvedEvent resolved_evt;
        resolved_evt.file_path = file.file_path;
        resolved_evt.strategy = "manual";
        event_bus_.publish(resolved_evt);
    }

    return true;
}

auto MergeConflictResolver::resolve_all(ConflictFile& file, ResolutionStrategy strategy) -> int
{
    int resolved = 0;
    for (int i = 0; i < static_cast<int>(file.regions.size()); ++i)
    {
        if (!file.regions[static_cast<size_t>(i)].is_resolved)
        {
            if (resolve_region(file, i, strategy))
            {
                ++resolved;
            }
        }
    }
    return resolved;
}

auto MergeConflictResolver::generate_merged_content(const ConflictFile& file) -> std::string
{
    if (!file.all_resolved())
    {
        return file.original_content; // Return original if not all resolved.
    }

    std::istringstream stream(file.original_content);
    std::string line;
    std::ostringstream output;
    size_t region_idx = 0;

    enum class State
    {
        kNormal,
        kSkipping
    };

    State state = State::kNormal;

    while (std::getline(stream, line))
    {
        // Check if we're at a conflict start.
        if (line.starts_with("<<<<<<<") && region_idx < file.regions.size())
        {
            // Emit the resolved content for this region.
            const auto& resolved = file.regions[region_idx].resolved_content;
            if (!resolved.empty())
            {
                output << resolved << "\n";
            }
            state = State::kSkipping;
            continue;
        }

        if (state == State::kSkipping)
        {
            if (line.starts_with(">>>>>>>"))
            {
                state = State::kNormal;
                ++region_idx;
            }
            continue; // Skip all lines within conflict markers.
        }

        // Normal line — emit as-is.
        output << line << "\n";
    }

    return output.str();
}

auto MergeConflictResolver::has_conflicts(const std::string& content) -> bool
{
    return content.find("<<<<<<<") != std::string::npos;
}

auto MergeConflictResolver::count_conflicts(const std::string& content) -> int
{
    int count = 0;
    std::string::size_type pos = 0;
    const std::string kMarker = "<<<<<<<";

    while ((pos = content.find(kMarker, pos)) != std::string::npos)
    {
        ++count;
        pos += kMarker.size();
    }

    return count;
}

} // namespace markamp::core
