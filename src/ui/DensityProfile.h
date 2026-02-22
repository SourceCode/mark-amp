#pragma once

#include <cstdint>

namespace markamp::ui
{

/// Density profile controlling spacing, row heights, icon sizes, and paddings
/// across all chrome surfaces.
enum class DensityProfile : uint8_t
{
    kComfortable, ///< Generous spacing — touch-friendly
    kDefault,     ///< Standard spacing — balanced
    kCompact,     ///< Tight spacing — information-dense
};

} // namespace markamp::ui
