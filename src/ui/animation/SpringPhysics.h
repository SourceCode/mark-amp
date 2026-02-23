#pragma once

namespace markamp::ui::animation
{

struct SpringConfig
{
    float stiffness{100.0f};
    float damping{10.0f};
    float mass{1.0f};
};

/// A utility for simulating critically damped or underdamped spring motion.
class SpringPhysics
{
public:
    /// Evaluates the physical position of a spring system at `time_seconds`.
    /// Initial position is 0, target is 1.0. Initial velocity is 0.
    [[nodiscard]] static auto evaluate(float time_seconds, const SpringConfig& config) -> float;
};

} // namespace markamp::ui::animation
