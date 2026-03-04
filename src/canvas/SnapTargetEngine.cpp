#include "SnapTargetEngine.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace markamp::canvas
{

auto SnapCandidate::type_name() const -> std::string
{
    switch (type)
    {
        case SnapTarget::kGrid:
            return "grid";
        case SnapTarget::kObjectEdge:
            return "object_edge";
        case SnapTarget::kObjectCenter:
            return "object_center";
        case SnapTarget::kKeyPoint:
            return "key_point";
    }
    return "unknown";
}

void SnapTargetEngine::set_tolerance(double tolerance)
{
    tolerance_ = tolerance;
}

auto SnapTargetEngine::tolerance() const -> double
{
    return tolerance_;
}

void SnapTargetEngine::set_target_enabled(SnapTarget target, bool enabled)
{
    switch (target)
    {
        case SnapTarget::kGrid:
            grid_enabled_ = enabled;
            break;
        case SnapTarget::kObjectEdge:
            edge_enabled_ = enabled;
            break;
        case SnapTarget::kObjectCenter:
            center_enabled_ = enabled;
            break;
        case SnapTarget::kKeyPoint:
            keypoint_enabled_ = enabled;
            break;
    }
}

auto SnapTargetEngine::is_target_enabled(SnapTarget target) const -> bool
{
    switch (target)
    {
        case SnapTarget::kGrid:
            return grid_enabled_;
        case SnapTarget::kObjectEdge:
            return edge_enabled_;
        case SnapTarget::kObjectCenter:
            return center_enabled_;
        case SnapTarget::kKeyPoint:
            return keypoint_enabled_;
    }
    return false;
}

void SnapTargetEngine::add_candidate(const SnapCandidate& candidate)
{
    candidates_.push_back(candidate);
}

void SnapTargetEngine::clear_candidates()
{
    candidates_.clear();
}

auto SnapTargetEngine::candidate_count() const -> int
{
    return static_cast<int>(candidates_.size());
}

auto SnapTargetEngine::resolve(double pos_x, double pos_y) const -> SnapCandidate
{
    SnapCandidate best;
    best.distance = tolerance_ + 1.0; // Start beyond tolerance

    for (const auto& candidate : candidates_)
    {
        if (!is_target_enabled(candidate.type))
        {
            continue;
        }

        double dist_x = candidate.snap_x - pos_x;
        double dist_y = candidate.snap_y - pos_y;
        double dist = std::sqrt(dist_x * dist_x + dist_y * dist_y);

        if (dist <= tolerance_ &&
            (dist < best.distance || (dist == best.distance && candidate.priority < best.priority)))
        {
            best = candidate;
            best.distance = dist;
        }
    }

    return best;
}

auto SnapTargetEngine::has_snap(double pos_x, double pos_y) const -> bool
{
    auto candidate = resolve(pos_x, pos_y);
    return candidate.distance <= tolerance_;
}

auto SnapTargetEngine::detect_equal_spacing(const std::vector<double>& positions,
                                            double spacing_tolerance) -> SpacingHint
{
    SpacingHint hint;

    if (positions.size() < 3)
    {
        return hint;
    }

    // Sort positions
    std::vector<double> sorted = positions;
    std::sort(sorted.begin(), sorted.end());

    // Compute gaps
    std::vector<double> gaps;
    for (size_t idx = 1; idx < sorted.size(); ++idx)
    {
        gaps.push_back(sorted[idx] - sorted[idx - 1]);
    }

    // Check if gaps are equal
    if (gaps.empty())
    {
        return hint;
    }

    double reference_gap = gaps[0];
    int equal_count = 0;
    for (const auto& gap : gaps)
    {
        if (std::abs(gap - reference_gap) <= spacing_tolerance)
        {
            ++equal_count;
        }
    }

    hint.spacing = reference_gap;
    hint.match_count = equal_count;
    hint.is_equal = (equal_count == static_cast<int>(gaps.size()));

    return hint;
}

} // namespace markamp::canvas
