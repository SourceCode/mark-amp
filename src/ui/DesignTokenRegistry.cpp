#include "DesignTokenRegistry.h"

#include "../core/Events.h"

namespace markamp::ui
{

DesignTokenRegistry::DesignTokenRegistry(core::ThemeEngine& theme, core::EventBus& bus)
    : theme_(theme)
    , bus_(bus)
    , component_resolver_(&ComponentSizeResolver::get())
{
}

auto DesignTokenRegistry::color(core::ThemeColorToken token) const -> const wxColour&
{
    return theme_.color(token);
}

auto DesignTokenRegistry::type(TypeSlot slot) const -> TypeSpec
{
    return typography_.resolve(slot);
}

auto DesignTokenRegistry::spacing(SpacingToken token) const -> int
{
    return spacing_.scaled(token);
}

auto DesignTokenRegistry::component(ComponentKind kind) const -> ComponentMetrics
{
    return component_resolver_->resolve(kind);
}

auto DesignTokenRegistry::elevation() const -> const ElevationSystem&
{
    return elevation_;
}

void DesignTokenRegistry::set_density(DensityProfile profile)
{
    typography_.set_density(profile);
    spacing_.set_density(profile);
    component_resolver_->set_density(profile);

    core::events::DensityProfileChangedEvent evt;
    evt.new_profile = static_cast<int>(profile);
    bus_.publish(evt);
}

} // namespace markamp::ui
