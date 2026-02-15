#include "ForceDirectedLayout.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace markamp::core
{

ForceDirectedLayout::ForceDirectedLayout() = default;

ForceDirectedLayout::ForceDirectedLayout(const LayoutParams& params)
    : params_(params)
{
}

void ForceDirectedLayout::add_node(const std::string& node_id,
                                   double pos_x,
                                   double pos_y,
                                   double mass)
{
    if (node_index_.contains(node_id))
    {
        return; // Already exists
    }

    LayoutNode node;
    node.id = node_id;
    node.pos_x = pos_x;
    node.pos_y = pos_y;
    node.mass = mass;

    node_index_[node_id] = nodes_.size();
    nodes_.push_back(std::move(node));
    force_x_.push_back(0.0);
    force_y_.push_back(0.0);
}

void ForceDirectedLayout::add_link(const std::string& source_id,
                                   const std::string& target_id,
                                   double strength,
                                   double ideal_length)
{
    LayoutLink link;
    link.source_id = source_id;
    link.target_id = target_id;
    link.strength = strength;
    link.ideal_length = ideal_length;
    links_.push_back(std::move(link));
}

void ForceDirectedLayout::clear()
{
    nodes_.clear();
    links_.clear();
    node_index_.clear();
    force_x_.clear();
    force_y_.clear();
    converged_ = false;
    iteration_ = 0;
}

auto ForceDirectedLayout::step() -> bool
{
    if (nodes_.empty())
    {
        converged_ = true;
        return false;
    }

    // Clear forces
    std::fill(force_x_.begin(), force_x_.end(), 0.0);
    std::fill(force_y_.begin(), force_y_.end(), 0.0);

    // Apply forces
    apply_repulsion();
    apply_attraction();
    apply_center_gravity();

    // Integrate (update velocities and positions)
    integrate();
    clamp_velocities();
    check_convergence();

    ++iteration_;

    if (iteration_ >= params_.max_iterations)
    {
        converged_ = true;
    }

    return !converged_;
}

auto ForceDirectedLayout::run_to_convergence() -> int
{
    while (step())
    {
        // Continue until converged or max_iterations
    }
    return iteration_;
}

auto ForceDirectedLayout::get_position(const std::string& node_id) const
    -> std::pair<double, double>
{
    auto iter = node_index_.find(node_id);
    if (iter == node_index_.end())
    {
        return {0.0, 0.0};
    }
    const auto& node = nodes_[iter->second];
    return {node.pos_x, node.pos_y};
}

auto ForceDirectedLayout::get_all_positions() const
    -> std::unordered_map<std::string, std::pair<double, double>>
{
    std::unordered_map<std::string, std::pair<double, double>> positions;
    for (const auto& node : nodes_)
    {
        positions[node.id] = {node.pos_x, node.pos_y};
    }
    return positions;
}

void ForceDirectedLayout::pin_node(const std::string& node_id)
{
    auto iter = node_index_.find(node_id);
    if (iter != node_index_.end())
    {
        nodes_[iter->second].pinned = true;
    }
}

void ForceDirectedLayout::unpin_node(const std::string& node_id)
{
    auto iter = node_index_.find(node_id);
    if (iter != node_index_.end())
    {
        nodes_[iter->second].pinned = false;
    }
}

void ForceDirectedLayout::set_position(const std::string& node_id, double pos_x, double pos_y)
{
    auto iter = node_index_.find(node_id);
    if (iter != node_index_.end())
    {
        auto& node = nodes_[iter->second];
        node.pos_x = pos_x;
        node.pos_y = pos_y;
        node.vel_x = 0.0;
        node.vel_y = 0.0;
    }
}

auto ForceDirectedLayout::total_kinetic_energy() const -> double
{
    double total_ke = 0.0;
    for (const auto& node : nodes_)
    {
        total_ke += node.kinetic_energy();
    }
    return total_ke;
}

auto ForceDirectedLayout::bounding_box() const -> std::tuple<double, double, double, double>
{
    if (nodes_.empty())
    {
        return {0.0, 0.0, 0.0, 0.0};
    }

    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double max_y = std::numeric_limits<double>::lowest();

    for (const auto& node : nodes_)
    {
        min_x = std::min(min_x, node.pos_x);
        min_y = std::min(min_y, node.pos_y);
        max_x = std::max(max_x, node.pos_x);
        max_y = std::max(max_y, node.pos_y);
    }

    return {min_x, min_y, max_x, max_y};
}

void ForceDirectedLayout::set_params(const LayoutParams& params)
{
    params_ = params;
    converged_ = false;
}

// ── Force Computations ──

void ForceDirectedLayout::apply_repulsion()
{
    const auto count = nodes_.size();
    constexpr double kMinDistance = 0.1;

    for (size_t idx_a = 0; idx_a < count; ++idx_a)
    {
        for (size_t idx_b = idx_a + 1; idx_b < count; ++idx_b)
        {
            double delta_x = nodes_[idx_a].pos_x - nodes_[idx_b].pos_x;
            double delta_y = nodes_[idx_a].pos_y - nodes_[idx_b].pos_y;
            double dist_sq = delta_x * delta_x + delta_y * delta_y;

            // Guard against zero distance
            if (dist_sq < kMinDistance * kMinDistance)
            {
                dist_sq = kMinDistance * kMinDistance;
                delta_x = kMinDistance;
                delta_y = 0.0;
            }

            // Coulomb's law: F = k / r^2
            // repulsion_strength is negative (repulsive)
            const double force = params_.repulsion_strength / dist_sq;
            const double dist = std::sqrt(dist_sq);
            const double force_x_component = force * (delta_x / dist);
            const double force_y_component = force * (delta_y / dist);

            // Apply force to both nodes (Newton's third law)
            force_x_[idx_a] += force_x_component;
            force_y_[idx_a] += force_y_component;
            force_x_[idx_b] -= force_x_component;
            force_y_[idx_b] -= force_y_component;
        }
    }
}

void ForceDirectedLayout::apply_attraction()
{
    for (const auto& link : links_)
    {
        auto src_iter = node_index_.find(link.source_id);
        auto tgt_iter = node_index_.find(link.target_id);

        if (src_iter == node_index_.end() || tgt_iter == node_index_.end())
        {
            continue;
        }

        const auto src_idx = src_iter->second;
        const auto tgt_idx = tgt_iter->second;

        const double delta_x = nodes_[tgt_idx].pos_x - nodes_[src_idx].pos_x;
        const double delta_y = nodes_[tgt_idx].pos_y - nodes_[src_idx].pos_y;
        const double dist = std::sqrt(delta_x * delta_x + delta_y * delta_y);

        if (dist < 0.001)
        {
            continue;
        }

        // Hooke's law: F = k * (distance - ideal_length)
        const double displacement = dist - link.ideal_length;
        const double force = params_.spring_constant * link.strength * displacement;

        const double force_x_component = force * (delta_x / dist);
        const double force_y_component = force * (delta_y / dist);

        force_x_[src_idx] += force_x_component;
        force_y_[src_idx] += force_y_component;
        force_x_[tgt_idx] -= force_x_component;
        force_y_[tgt_idx] -= force_y_component;
    }
}

void ForceDirectedLayout::apply_center_gravity()
{
    for (size_t idx = 0; idx < nodes_.size(); ++idx)
    {
        force_x_[idx] -= params_.center_gravity * nodes_[idx].pos_x;
        force_y_[idx] -= params_.center_gravity * nodes_[idx].pos_y;
    }
}

void ForceDirectedLayout::integrate()
{
    for (size_t idx = 0; idx < nodes_.size(); ++idx)
    {
        auto& node = nodes_[idx];

        if (node.pinned)
        {
            continue;
        }

        // Acceleration = Force / Mass
        const double accel_x = force_x_[idx] / node.mass;
        const double accel_y = force_y_[idx] / node.mass;

        // Update velocity with damping
        node.vel_x = (node.vel_x + accel_x * params_.time_step) * params_.damping;
        node.vel_y = (node.vel_y + accel_y * params_.time_step) * params_.damping;

        // Update position
        node.pos_x += node.vel_x * params_.time_step;
        node.pos_y += node.vel_y * params_.time_step;
    }
}

void ForceDirectedLayout::clamp_velocities()
{
    for (auto& node : nodes_)
    {
        node.vel_x = std::clamp(node.vel_x, -params_.max_velocity, params_.max_velocity);
        node.vel_y = std::clamp(node.vel_y, -params_.max_velocity, params_.max_velocity);
    }
}

void ForceDirectedLayout::check_convergence()
{
    converged_ = total_kinetic_energy() < params_.convergence_threshold;
}

} // namespace markamp::core
