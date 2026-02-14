#pragma once

#include <string>

namespace markamp::core
{

class PluginManager;
class FeatureRegistry;

/// Feature IDs for built-in plugins — use these constants in feature guards.
namespace builtin_features
{
inline constexpr auto kMermaid = "mermaid";
inline constexpr auto kTableEditor = "table-editor";
inline constexpr auto kFormatBar = "format-bar";
inline constexpr auto kThemeGallery = "theme-gallery";
inline constexpr auto kLinkPreview = "link-preview";
inline constexpr auto kImagePreview = "image-preview";
inline constexpr auto kBreadcrumb = "breadcrumb";
} // namespace builtin_features

/// Register all built-in features as plugins + feature-registry entries.
/// Call this once during application startup before `PluginManager::activate_all()`.
void register_builtin_plugins(PluginManager& plugin_manager, FeatureRegistry& feature_registry);

} // namespace markamp::core
