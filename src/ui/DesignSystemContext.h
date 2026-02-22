#pragma once

namespace markamp::core
{
class ThemeEngine;
}

namespace markamp::ui
{

class DesignTokenRegistry;
class TypographyScale;
class SpacingGrid;
class ComponentSizeResolver;
class ElevationSystem;
class LayoutMetrics;

/// A lightweight context object that bundles references to all design system
/// subsystems. Passed by reference to UI controls that need design token access,
/// avoiding the need for 4-5 separate constructor parameters.
struct DesignSystemContext
{
    core::ThemeEngine& theme;
    DesignTokenRegistry& registry;
    TypographyScale& typography;
    SpacingGrid& spacing;
    ComponentSizeResolver& component_sizes;
    ElevationSystem& elevation;
    LayoutMetrics& metrics;
};

} // namespace markamp::ui
