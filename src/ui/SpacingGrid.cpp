#include "SpacingGrid.h"

namespace markamp::ui
{

auto SpacingGrid::scaled(SpacingToken token) const -> int
{
    int base = resolve(token);
    float scale = 1.0f;

    if (density_ == DensityProfile::kCompact)
    {
        scale = 0.75f;
    }
    else if (density_ == DensityProfile::kComfortable)
    {
        scale = 1.25f;
    }

    float float_val = std::round(base * scale);
    int int_val = static_cast<int>(float_val);

    // Round to nearest even integer for cleaner rendering layout (except for very small values like
    // 1)
    if (int_val > 1 && int_val % 2 != 0)
    {
        int_val++;
    }

    return int_val;
}

void SpacingGrid::set_density(DensityProfile profile)
{
    density_ = profile;
}

} // namespace markamp::ui
