#include "AsyncEvaluator.h"

#include <algorithm>

namespace markamp::node_editor
{

AsyncEvaluator::AsyncEvaluator(std::size_t max_concurrent)
    : max_concurrent_(max_concurrent)
{
}

// ---------------------------------------------------------------------------
// Job ID generation
// ---------------------------------------------------------------------------

auto AsyncEvaluator::next_job_id() -> JobId
{
    return JobId{next_id_++};
}

// ---------------------------------------------------------------------------
// Job submission
// ---------------------------------------------------------------------------

auto AsyncEvaluator::submit_job(const std::string& description,
                                const std::vector<NodeId>& dirty_nodes) -> JobId
{
    const auto job_id = next_job_id();

    JobInfo info;
    info.id = job_id;
    info.status = JobStatus::kPending;
    info.progress = 0.0F;
    info.description = description;
    info.dirty_node_count = dirty_nodes.size();
    info.submitted_at = std::chrono::steady_clock::now();

    jobs_[job_id] = info;
    return job_id;
}

void AsyncEvaluator::cancel_job(JobId job_id)
{
    auto found = jobs_.find(job_id);
    if (found != jobs_.end() && !found->second.is_terminal())
    {
        found->second.status = JobStatus::kCancelled;
        found->second.completed_at = std::chrono::steady_clock::now();
    }
}

void AsyncEvaluator::cancel_all()
{
    for (auto& [id, info] : jobs_)
    {
        if (!info.is_terminal())
        {
            info.status = JobStatus::kCancelled;
            info.completed_at = std::chrono::steady_clock::now();
        }
    }
}

// ---------------------------------------------------------------------------
// Job queries
// ---------------------------------------------------------------------------

auto AsyncEvaluator::job_status(JobId job_id) const -> JobStatus
{
    const auto found = jobs_.find(job_id);
    if (found == jobs_.end())
    {
        return JobStatus::kFailed;
    }
    return found->second.status;
}

auto AsyncEvaluator::job_progress(JobId job_id) const -> float
{
    const auto found = jobs_.find(job_id);
    if (found == jobs_.end())
    {
        return 0.0F;
    }
    return found->second.progress;
}

auto AsyncEvaluator::job_info(JobId job_id) const -> const JobInfo*
{
    const auto found = jobs_.find(job_id);
    if (found == jobs_.end())
    {
        return nullptr;
    }
    return &found->second;
}

auto AsyncEvaluator::active_jobs() const -> std::vector<JobId>
{
    std::vector<JobId> result;
    for (const auto& [id, info] : jobs_)
    {
        if (!info.is_terminal())
        {
            result.push_back(id);
        }
    }
    return result;
}

auto AsyncEvaluator::all_jobs() const -> std::vector<JobId>
{
    std::vector<JobId> result;
    result.reserve(jobs_.size());
    for (const auto& [id, info] : jobs_)
    {
        result.push_back(id);
    }
    return result;
}

auto AsyncEvaluator::job_count() const -> std::size_t
{
    return jobs_.size();
}

auto AsyncEvaluator::active_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, info] : jobs_)
    {
        if (!info.is_terminal())
        {
            ++count;
        }
    }
    return count;
}

// ---------------------------------------------------------------------------
// Job updates
// ---------------------------------------------------------------------------

void AsyncEvaluator::start_job(JobId job_id)
{
    auto found = jobs_.find(job_id);
    if (found != jobs_.end() && found->second.status == JobStatus::kPending)
    {
        found->second.status = JobStatus::kRunning;
    }
}

void AsyncEvaluator::update_progress(JobId job_id, float progress)
{
    auto found = jobs_.find(job_id);
    if (found != jobs_.end() && found->second.status == JobStatus::kRunning)
    {
        found->second.progress = std::clamp(progress, 0.0F, 1.0F);
    }
}

void AsyncEvaluator::complete_job(JobId job_id)
{
    auto found = jobs_.find(job_id);
    if (found != jobs_.end() && !found->second.is_terminal())
    {
        found->second.status = JobStatus::kCompleted;
        found->second.progress = 1.0F;
        found->second.completed_at = std::chrono::steady_clock::now();
    }
}

void AsyncEvaluator::fail_job(JobId job_id, const std::string& error)
{
    auto found = jobs_.find(job_id);
    if (found != jobs_.end() && !found->second.is_terminal())
    {
        found->second.status = JobStatus::kFailed;
        found->second.error_message = error;
        found->second.completed_at = std::chrono::steady_clock::now();
    }
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void AsyncEvaluator::clear_completed()
{
    auto iter = jobs_.begin();
    while (iter != jobs_.end())
    {
        if (iter->second.is_terminal())
        {
            iter = jobs_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

} // namespace markamp::node_editor
