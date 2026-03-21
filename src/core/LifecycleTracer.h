/// @file LifecycleTracer.h
/// @brief V20 P10-T01: Full artifact lifecycle tracing.
///
/// Instruments create/open/save/restore/delete transitions with
/// structured trace events keyed by artifact ID and command source.
#pragma once

#include "ArtifactRegistry.h"
#include "EventBus.h"

#include <chrono>
#include <string>
#include <vector>

namespace markamp::core
{

/// Lifecycle trace event type.
enum class TraceEventType
{
    kCreate,
    kMount,
    kFocus,
    kDirty,
    kSave,
    kSaveAs,
    kAutosave,
    kRename,
    kDuplicate,
    kRestore,
    kClose,
    kDelete,
    kError
};

/// A single lifecycle trace entry.
struct LifecycleTraceEntry
{
    std::string artifact_id;
    TraceEventType event_type{TraceEventType::kCreate};
    std::string source;              ///< What triggered this (command/event/system)
    std::string detail;              ///< Additional info
    std::chrono::steady_clock::time_point timestamp;
    bool success{true};

    [[nodiscard]] auto event_name() const -> std::string;
};

/// Manages structured lifecycle traces for diagnostics.
class LifecycleTracer
{
public:
    explicit LifecycleTracer(EventBus& bus);

    /// Record a lifecycle trace entry.
    void trace(const std::string& artifact_id, TraceEventType event,
               const std::string& source, const std::string& detail = "",
               bool success = true);

    /// Get all traces for an artifact.
    [[nodiscard]] auto traces_for(const std::string& artifact_id) const
        -> std::vector<LifecycleTraceEntry>;

    /// Get recent traces (last N).
    [[nodiscard]] auto recent_traces(int count = 50) const -> std::vector<LifecycleTraceEntry>;

    /// Get traces by event type.
    [[nodiscard]] auto traces_by_type(TraceEventType type) const -> std::vector<LifecycleTraceEntry>;

    /// Total trace count.
    [[nodiscard]] auto trace_count() const noexcept -> int
    {
        return static_cast<int>(entries_.size());
    }

    /// Error trace count.
    [[nodiscard]] auto error_count() const noexcept -> int { return error_count_; }

    /// Clear all traces.
    void clear();

private:
    EventBus& event_bus_;
    std::vector<LifecycleTraceEntry> entries_;
    int error_count_{0};
    static constexpr int kMaxEntries = 10000;  ///< Cap to prevent unbounded growth
};

} // namespace markamp::core
