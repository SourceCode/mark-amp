#pragma once

#include "../core/EventBus.h"
#include "../core/ThemeEngine.h"
#include "ComponentSizeResolver.h"
#include "ElevationSystem.h"
#include "SpacingGrid.h"
#include "TypographyScale.h"

namespace markamp::ui
{

class DesignTokenRegistry
{
public:
    DesignTokenRegistry(core::ThemeEngine& theme, core::EventBus& bus);

    auto color(core::ThemeColorToken token) const -> const wxColour&;
    auto type(TypeSlot slot) const -> TypeSpec;
    auto spacing(SpacingToken token) const -> int;
    auto component(ComponentKind kind) const -> ComponentMetrics;
    auto elevation() const -> const ElevationSystem&;

    void set_density(DensityProfile profile);

    auto typography() -> TypographyScale&
    {
        return typography_;
    }
    auto spacing() -> SpacingGrid&
    {
        return spacing_;
    }
    auto elevation() -> ElevationSystem&
    {
        return elevation_;
    }
    auto components() -> ComponentSizeResolver&
    {
        return *component_resolver_;
    }

private:
    core::ThemeEngine& theme_;
    core::EventBus& bus_;

    TypographyScale typography_;
    SpacingGrid spacing_;
    ElevationSystem elevation_;
    ComponentSizeResolver* component_resolver_;
};

} // namespace markamp::ui
