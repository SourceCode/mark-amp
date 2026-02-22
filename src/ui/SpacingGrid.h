#pragma once

#include "DensityProfile.h"

#include <cmath>

namespace markamp::ui
{

enum class SpacingToken
{
    kNone,
    kXxs,
    kXs,
    kSm,
    kMd,
    kLg,
    kXl,
    kXxl,
    kXxxl
};

class SpacingGrid
{
public:
    SpacingGrid() = default;

    static constexpr auto resolve(SpacingToken token) -> int
    {
        switch (token)
        {
            case SpacingToken::kNone:
                return 0;
            case SpacingToken::kXxs:
                return 2;
            case SpacingToken::kXs:
                return 4;
            case SpacingToken::kSm:
                return 8;
            case SpacingToken::kMd:
                return 12;
            case SpacingToken::kLg:
                return 16;
            case SpacingToken::kXl:
                return 24;
            case SpacingToken::kXxl:
                return 32;
            case SpacingToken::kXxxl:
                return 48;
        }
        return 0;
    }

    auto scaled(SpacingToken token) const -> int;

    auto gap() const -> int
    {
        return scaled(SpacingToken::kSm);
    }
    auto section_gap() const -> int
    {
        return scaled(SpacingToken::kXl);
    }

    void set_density(DensityProfile profile);

private:
    DensityProfile density_{DensityProfile::kDefault};
};

} // namespace markamp::ui
