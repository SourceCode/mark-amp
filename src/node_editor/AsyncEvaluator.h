#pragma once

#include "NodeEditorTypes.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// Job types
// ---------------------------------------------------------------------------

/// Unique identifier for an evaluation job.
struct JobId
{
    uint64_t value{0};

    auto operator==(const JobId& other) const -> bool
    {
        return value == other.value;
    }
    auto operator!=(const JobId& other) const -> bool
    {
        return value != other.value;
    }
    [[nodiscard]] auto is_valid() const -> bool
    {
        return value != 0;
    }
};

} // namespace markamp::node_editor

// Hash specialization for JobId — MUST come before any unordered_map<JobId, ...>
template <>
struct std::hash<markamp::node_editor::JobId>
{
    auto operator()(const markamp::node_editor::JobId& job_id) const noexcept -> std::size_t
    {
        return std::hash<uint64_t>{}(job_id.value);
    }
};

namespace markamp::node_editor
{

enum class JobStatus
{
    kPending,
    kRunning,
    kCompleted,
    kFailed,
    kCancelled
};

struct JobInfo
{
    JobId id;
    JobStatus status{JobStatus::kPending};
    float progress{0.0F}; ///< 0.0 – 1.0
    std::string description;
    std::string error_message; ///< Populated on failure
    std::size_t dirty_node_count{0};
    std::chrono::steady_clock::time_point submitted_at;
    std::chrono::steady_clock::time_point completed_at;

    [[nodiscard]] auto is_terminal() const -> bool
    {
        return status == JobStatus::kCompleted || status == JobStatus::kFailed ||
               status == JobStatus::kCancelled;
    }

    [[nodiscard]] auto elapsed_ms() const -> float
    {
        const auto end_time = is_terminal() ? completed_at : std::chrono::steady_clock::now();
        return static_cast<float>(
            std::chrono::duration_cast<std::chrono::milliseconds>(end_time - submitted_at).count());
    }
};

// ---------------------------------------------------------------------------
// AsyncEvaluator — manages evaluation jobs with progress and cancellation
// ---------------------------------------------------------------------------

class AsyncEvaluator
{
public:
    explicit AsyncEvaluator(std::size_t max_concurrent = 4);

    // --- Job submission ---------------------------------------------------

    /// Submit a new evaluation job.
    auto submit_job(const std::string& description, const std::vector<NodeId>& dirty_nodes)
        -> JobId;

    /// Cancel a specific job.
    void cancel_job(JobId job_id);

    /// Cancel all jobs.
    void cancel_all();

    // --- Job queries ------------------------------------------------------

    /// Get job status.
    [[nodiscard]] auto job_status(JobId job_id) const -> JobStatus;

    /// Get job progress (0.0 – 1.0).
    [[nodiscard]] auto job_progress(JobId job_id) const -> float;

    /// Get full job info.
    [[nodiscard]] auto job_info(JobId job_id) const -> const JobInfo*;

    /// Get all active (non-terminal) jobs.
    [[nodiscard]] auto active_jobs() const -> std::vector<JobId>;

    /// Get all jobs.
    [[nodiscard]] auto all_jobs() const -> std::vector<JobId>;

    [[nodiscard]] auto job_count() const -> std::size_t;
    [[nodiscard]] auto active_count() const -> std::size_t;

    // --- Job updates (called by evaluation engine) ------------------------

    /// Mark job as running.
    void start_job(JobId job_id);

    /// Update job progress.
    void update_progress(JobId job_id, float progress);

    /// Mark job as completed.
    void complete_job(JobId job_id);

    /// Mark job as failed.
    void fail_job(JobId job_id, const std::string& error);

    // --- Configuration ----------------------------------------------------

    void set_max_concurrent(std::size_t max)
    {
        max_concurrent_ = max;
    }
    [[nodiscard]] auto max_concurrent() const -> std::size_t
    {
        return max_concurrent_;
    }

    /// Remove all terminal jobs from the queue.
    void clear_completed();

private:
    std::size_t max_concurrent_;
    std::unordered_map<JobId, JobInfo> jobs_;
    uint64_t next_id_{1};

    auto next_job_id() -> JobId;
};

} // namespace markamp::node_editor
