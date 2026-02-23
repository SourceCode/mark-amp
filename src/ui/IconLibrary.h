#pragma once

namespace markamp::ui
{
class IconRegistry;

/// Registers all core built-in icons (activity bar, toolbar, file types, etc.)
/// into the provided IconRegistry.
auto RegisterCoreIcons(IconRegistry& registry) -> void;

} // namespace markamp::ui
