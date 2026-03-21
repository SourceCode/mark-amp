/// @file LifecycleTracer.cpp
/// @brief V20 P10-T01: Lifecycle tracer implementation.

#include "LifecycleTracer.h"

#include "Events.h"
#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

auto LifecycleTraceEntry::event_name() const -> std::string
{
    switch (event_type)
    {
        case TraceEventType::kCreate:    return "create";
        case TraceEventType::kMount:     return "mount";
        case TraceEventType::kFocus:     return "focus";
        case TraceEventType::kDirty:     return "dirty";
        case TraceEventType::kSave:      return "save";
        case TraceEventType::kSaveAs:    return "save-as";
        case TraceEventType::kAutosave:  return "autosave";
        case TraceEventType::kRename:    return "rename";
        case TraceEventType::kDuplicate: return "duplicate";
        case TraceEventType::kRestore:   return "restore";
        case TraceEventType::kClose:     return "close";
        case TraceEventType::kDelete:    return "delete";
        case TraceEventType::kError:     return "error";
    }
    return "unknown";
}

LifecycleTracer::LifecycleTracer(EventBus& bus)
    : event_bus_(bus)
{
}

void LifecycleTracer::trace(const std::string& artifact_id, TraceEventType event,
                             const std::string& source, const std::string& detail,
                             bool success)
{
    LifecycleTraceEntry entry;
    entry.artifact_id = artifact_id;
    entry.event_type = event;
    entry.source = source;
    entry.detail = detail;
    entry.timestamp = std::chrono::steady_clock::now();
    entry.success = success;

    if (!success)
    {
        ++error_count_;
    }

    // Cap entries to prevent unbounded growth
    if (static_cast<int>(entries_.size()) >= kMaxEntries)
    {
        entries_.erase(entries_.begin());
    }
    entries_.push_back(entry);

    events::LifecycleTraceEmittedEvent evt;
    evt.artifact_id = artifact_id;
    evt.event_name = entry.event_name();
    evt.source = source;
    evt.success = success;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Trace: {} {} [{}] success={} {}", artifact_id, entry.event_name(),
                     source, success, detail);
}

auto LifecycleTracer::traces_for(const std::string& artifact_id) const
    -> std::vector<LifecycleTraceEntry>
{
    std::vector<LifecycleTraceEntry> result;
    for (const auto& e : entries_)
    {
        if (e.artifact_id == artifact_id)
        {
            result.push_back(e);
        }
    }
    return result;
}

auto LifecycleTracer::recent_traces(int count) const -> std::vector<LifecycleTraceEntry>
{
    if (count <= 0)
    {
        return {};
    }

    const auto total = static_cast<int>(entries_.size());
    const auto start = std::max(0, total - count);
    return {entries_.begin() + start, entries_.end()};
}

auto LifecycleTracer::traces_by_type(TraceEventType type) const -> std::vector<LifecycleTraceEntry>
{
    std::vector<LifecycleTraceEntry> result;
    for (const auto& e : entries_)
    {
        if (e.event_type == type)
        {
            result.push_back(e);
        }
    }
    return result;
}

void LifecycleTracer::clear()
{
    entries_.clear();
    error_count_ = 0;
    MARKAMP_LOG_DEBUG("Lifecycle traces cleared");
}

} // namespace markamp::core
