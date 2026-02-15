#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace markamp::core
{

/// 2D position with velocity for physics simulation.
struct LayoutNode
{
    std::string id;
    double pos_x{0.0};
    double pos_y{0.0};
    double vel_x{0.0};  // Velocity X
    double vel_y{0.0};  // Velocity Y
    double mass{1.0};   // Node mass (derived from ref_count)
    bool pinned{false}; // If true, position is fixed

    [[nodiscard]] auto kinetic_energy() const -> double
    {
        return 0.5 * mass * (vel_x * vel_x + vel_y * vel_y);
    }

    [[nodiscard]] auto distance_to(const LayoutNode& other) const -> double
    {
        const double delta_x = pos_x - other.pos_x;
        const double delta_y = pos_y - other.pos_y;
        return std::sqrt(delta_x * delta_x + delta_y * delta_y);
    }
};

/// A link in the physics simulation.
struct LayoutLink
{
    std::string source_id;
    std::string target_id;
    double strength{1.0};       // Spring constant
    double ideal_length{100.0}; // Rest length of the spring
};

/// Parameters for the force-directed simulation.
struct LayoutParams
{
    double repulsion_strength{-300.0}; // Coulomb constant (negative = repulsive)
    double spring_constant{0.01};      // Hooke's spring constant
    double damping{0.9};               // Velocity damping per step (0-1)
    double max_velocity{50.0};         // Velocity cap per axis
    double convergence_threshold{0.5}; // Total KE below this = converged
    double center_gravity{0.05};       // Pull toward center (prevents drift)
    double time_step{1.0};             // Simulation time step
    int max_iterations{500};           // Stop after this many steps
};

/// Force-directed layout engine using Fruchterman-Reingold style forces.
/// Computes 2D positions for nodes connected by links.
class ForceDirectedLayout
{
public:
    ForceDirectedLayout();
    explicit ForceDirectedLayout(const LayoutParams& params);

    /// Add a node to the simulation.
    void add_node(const std::string& node_id, double pos_x, double pos_y, double mass = 1.0);

    /// Add a link (spring) between two nodes.
    void add_link(const std::string& source_id,
                  const std::string& target_id,
                  double strength = 1.0,
                  double ideal_length = 100.0);

    /// Remove all nodes and links.
    void clear();

    /// Perform one simulation step. Returns false when converged.
    [[nodiscard]] auto step() -> bool;

    /// Run the simulation until convergence or max_iterations.
    /// Returns the number of iterations performed.
    [[nodiscard]] auto run_to_convergence() -> int;

    /// Get the current position of a node.
    [[nodiscard]] auto get_position(const std::string& node_id) const -> std::pair<double, double>;

    /// Get all node positions.
    [[nodiscard]] auto get_all_positions() const
        -> std::unordered_map<std::string, std::pair<double, double>>;

    /// Pin a node at its current position (prevents movement).
    void pin_node(const std::string& node_id);

    /// Unpin a node (allows movement again).
    void unpin_node(const std::string& node_id);

    /// Move a node to a specific position (e.g., during drag).
    void set_position(const std::string& node_id, double pos_x, double pos_y);

    /// Get the total kinetic energy (used for convergence check).
    [[nodiscard]] auto total_kinetic_energy() const -> double;

    /// Get the bounding box of all nodes: {min_x, min_y, max_x, max_y}.
    [[nodiscard]] auto bounding_box() const -> std::tuple<double, double, double, double>;

    /// Update simulation parameters.
    void set_params(const LayoutParams& params);

    /// Get current simulation parameters.
    [[nodiscard]] auto params() const -> const LayoutParams&
    {
        return params_;
    }

    /// Whether the simulation has converged.
    [[nodiscard]] auto is_converged() const -> bool
    {
        return converged_;
    }

    /// Current iteration count.
    [[nodiscard]] auto iteration_count() const -> int
    {
        return iteration_;
    }

    /// Number of nodes in the layout.
    [[nodiscard]] auto node_count() const -> int
    {
        return static_cast<int>(nodes_.size());
    }

private:
    LayoutParams params_;
    std::vector<LayoutNode> nodes_;
    std::vector<LayoutLink> links_;
    std::unordered_map<std::string, size_t> node_index_; // id -> index in nodes_
    bool converged_{false};
    int iteration_{0};

    /// Temporary force accumulators (indexed same as nodes_).
    std::vector<double> force_x_;
    std::vector<double> force_y_;

    /// Apply repulsive forces between all node pairs (O(n^2) Coulomb).
    void apply_repulsion();

    /// Apply attractive forces along links (Hooke's spring).
    void apply_attraction();

    /// Apply gravity toward center to prevent drift.
    void apply_center_gravity();

    /// Update velocities and positions with damping.
    void integrate();

    /// Clamp velocity to max_velocity.
    void clamp_velocities();

    /// Check convergence based on total kinetic energy.
    void check_convergence();
};

} // namespace markamp::core
