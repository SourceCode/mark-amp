#include "SpringPhysics.h"

#include <cmath>

namespace markamp::ui::animation
{

auto SpringPhysics::evaluate(float time_seconds, const SpringConfig& config) -> float
{
    if (time_seconds <= 0.0f)
        return 0.0f;

    float m = config.mass > 0.0f ? config.mass : 0.001f;
    float k = config.stiffness;
    float c = config.damping;

    float omega_n = std::sqrt(k / m);
    float zeta = c / (2.0f * std::sqrt(m * k));

    float displacement = 0.0f;

    if (zeta < 1.0f) // Underdamped
    {
        float omega_d = omega_n * std::sqrt(1.0f - zeta * zeta);
        displacement = -1.0f * std::exp(-zeta * omega_n * time_seconds) *
                       (std::cos(omega_d * time_seconds) +
                        (zeta / std::sqrt(1.0f - zeta * zeta)) * std::sin(omega_d * time_seconds));
    }
    else if (zeta == 1.0f) // Critically damped
    {
        displacement = -1.0f * (1.0f + omega_n * time_seconds) * std::exp(-omega_n * time_seconds);
    }
    else // Overdamped
    {
        float root1 = -omega_n * (zeta - std::sqrt(zeta * zeta - 1.0f));
        float root2 = -omega_n * (zeta + std::sqrt(zeta * zeta - 1.0f));
        float c1 = root2 / (root2 - root1);
        float c2 = -root1 / (root2 - root1);
        displacement =
            -1.0f * (c1 * std::exp(root1 * time_seconds) + c2 * std::exp(root2 * time_seconds));
    }

    return 1.0f + displacement;
}

} // namespace markamp::ui::animation
