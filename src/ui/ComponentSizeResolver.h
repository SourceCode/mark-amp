#pragma once

#include "DensityProfile.h"

#include <algorithm>
#include <array>

namespace markamp::ui
{

enum class ComponentKind
{
    kButton,
    kInput,
    kTab,
    kTreeRow,
    kListRow,
    kToolbarButton,
    kActivityBarSlot,
    kStatusBarSegment,
    kBreadcrumbSegment,
    kPanelHeader,

    kCount
};

struct ComponentMetrics
{
    int height;
    int min_width;
    int icon_size;
    int padding_h;
    int padding_v;
};

class ComponentSizeResolver
{
public:
    static auto get() -> ComponentSizeResolver&;

    auto resolve(ComponentKind kind) const -> ComponentMetrics;

    void set_density(DensityProfile profile);

private:
    ComponentSizeResolver();

    DensityProfile density_{DensityProfile::kDefault};
    std::array<std::array<ComponentMetrics, static_cast<size_t>(ComponentKind::kCount)>, 3>
        metrics_table_;

    void InitializeTable();
};

} // namespace markamp::ui
