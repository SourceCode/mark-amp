#include "BuiltInPlugins.h"

#include "FeatureRegistry.h"
#include "IPlugin.h"
#include "Logger.h"
#include "PluginManager.h"

#include <memory>

namespace markamp::core
{

// ── Built-in Plugin Template ──
// Each built-in plugin follows VS Code's IPlugin pattern with a PluginManifest,
// activate()/deactivate() lifecycle, and contribution points via
// PluginManifest::ContributionPoints.

namespace
{

/// Base class for built-in plugins to reduce boilerplate.
/// Provides a pre-filled manifest and simple activate/deactivate lifecycle.
class BuiltInPlugin : public IPlugin
{
public:
    explicit BuiltInPlugin(PluginManifest manifest)
        : manifest_(std::move(manifest))
    {
    }

    [[nodiscard]] auto manifest() const -> const PluginManifest& override
    {
        return manifest_;
    }

    void activate(PluginContext& ctx) override
    {
        active_ = true;
        ctx_ = &ctx;
        MARKAMP_LOG_INFO("Built-in plugin activated: {}", manifest_.id);
    }

    void deactivate() override
    {
        active_ = false;
        ctx_ = nullptr;
        MARKAMP_LOG_INFO("Built-in plugin deactivated: {}", manifest_.id);
    }

protected:
    PluginManifest manifest_;
    PluginContext* ctx_{nullptr};
};

// ── 9.1: Mermaid Diagrams Plugin ──

class MermaidPlugin final : public BuiltInPlugin
{
public:
    MermaidPlugin()
        : BuiltInPlugin(PluginManifest{
              .id = "markamp.mermaid",
              .name = "Mermaid Diagrams",
              .version = "1.0.0",
              .description = "Render Mermaid diagrams in the preview pane",
              .author = "MarkAmp",
              .contributes = {
                  .settings = {{.id = "mermaid.enabled",
                                .label = "Enable Mermaid Diagrams",
                                .description = "Render Mermaid fenced code blocks as diagrams",
                                .category = "Features",
                                .type = SettingType::Boolean,
                                .default_value = "true"}}}})
    {
    }
};

// ── 9.2: Table Editor Overlay Plugin ──

class TableEditorPlugin final : public BuiltInPlugin
{
public:
    TableEditorPlugin()
        : BuiltInPlugin(PluginManifest{
              .id = "markamp.table-editor",
              .name = "Table Editor",
              .version = "1.0.0",
              .description = "Visual table editing overlay for Markdown tables",
              .author = "MarkAmp",
              .contributes = {.settings = {{.id = "table-editor.enabled",
                                            .label = "Enable Table Editor",
                                            .description = "Show interactive table editing overlay",
                                            .category = "Features",
                                            .type = SettingType::Boolean,
                                            .default_value = "true"}}}})
    {
    }
};

// ── 9.3: Floating Format Bar Plugin ──

class FormatBarPlugin final : public BuiltInPlugin
{
public:
    FormatBarPlugin()
        : BuiltInPlugin(PluginManifest{
              .id = "markamp.format-bar",
              .name = "Floating Format Bar",
              .version = "1.0.0",
              .description = "Context-sensitive formatting toolbar on text selection",
              .author = "MarkAmp",
              .contributes = {
                  .settings = {{.id = "format-bar.enabled",
                                .label = "Enable Floating Format Bar",
                                .description = "Show formatting toolbar on text selection",
                                .category = "Features",
                                .type = SettingType::Boolean,
                                .default_value = "true"}}}})
    {
    }
};

// ── 9.4: Theme Gallery Plugin ──

class ThemeGalleryPlugin final : public BuiltInPlugin
{
public:
    ThemeGalleryPlugin()
        : BuiltInPlugin(PluginManifest{
              .id = "markamp.theme-gallery",
              .name = "Theme Gallery",
              .version = "1.0.0",
              .description = "Browse and apply editor themes from the gallery",
              .author = "MarkAmp",
              .contributes = {
                  .settings = {{.id = "theme-gallery.enabled",
                                .label = "Enable Theme Gallery",
                                .description = "Allow browsing themes in the settings panel",
                                .category = "Features",
                                .type = SettingType::Boolean,
                                .default_value = "true"}}}})
    {
    }
};

// ── 9.5: Link Preview Plugin ──

class LinkPreviewPlugin final : public BuiltInPlugin
{
public:
    LinkPreviewPlugin()
        : BuiltInPlugin(PluginManifest{
              .id = "markamp.link-preview",
              .name = "Link Preview",
              .version = "1.0.0",
              .description = "Show popover previews when hovering over links",
              .author = "MarkAmp",
              .contributes = {
                  .settings = {{.id = "link-preview.enabled",
                                .label = "Enable Link Preview",
                                .description = "Show hovering popover previews for links",
                                .category = "Features",
                                .type = SettingType::Boolean,
                                .default_value = "true"}}}})
    {
    }
};

// ── 9.6: Image Preview Plugin ──

class ImagePreviewPlugin final : public BuiltInPlugin
{
public:
    ImagePreviewPlugin()
        : BuiltInPlugin(PluginManifest{
              .id = "markamp.image-preview",
              .name = "Image Preview",
              .version = "1.0.0",
              .description = "Show popover previews when hovering over image references",
              .author = "MarkAmp",
              .contributes = {
                  .settings = {{.id = "image-preview.enabled",
                                .label = "Enable Image Preview",
                                .description = "Show hovering popover previews for images",
                                .category = "Features",
                                .type = SettingType::Boolean,
                                .default_value = "true"}}}})
    {
    }
};

// ── 9.7: Breadcrumb Bar Plugin ──

class BreadcrumbPlugin final : public BuiltInPlugin
{
public:
    BreadcrumbPlugin()
        : BuiltInPlugin(PluginManifest{
              .id = "markamp.breadcrumb",
              .name = "Breadcrumb Bar",
              .version = "1.0.0",
              .description = "File path breadcrumb navigation above the editor",
              .author = "MarkAmp",
              .contributes = {
                  .settings = {{.id = "breadcrumb.enabled",
                                .label = "Enable Breadcrumb Bar",
                                .description = "Show file path breadcrumbs above the editor",
                                .category = "Features",
                                .type = SettingType::Boolean,
                                .default_value = "true"}}}})
    {
    }
};

} // anonymous namespace

// ── Registration ──

void register_builtin_plugins(PluginManager& plugin_manager, FeatureRegistry& feature_registry)
{
    MARKAMP_LOG_INFO("Registering built-in plugins...");

    // Define feature info for each plugin
    struct BuiltInFeature
    {
        std::string id;
        std::string display_name;
        std::string description;
    };

    const std::vector<BuiltInFeature> features = {
        {builtin_features::kMermaid,
         "Mermaid Diagrams",
         "Render Mermaid fenced code blocks as diagrams in the preview pane"},
        {builtin_features::kTableEditor,
         "Table Editor",
         "Visual table editing overlay for Markdown tables"},
        {builtin_features::kFormatBar,
         "Floating Format Bar",
         "Context-sensitive formatting toolbar on text selection"},
        {builtin_features::kThemeGallery,
         "Theme Gallery",
         "Browse and apply editor themes from the gallery"},
        {builtin_features::kLinkPreview,
         "Link Preview",
         "Show popover previews when hovering over links"},
        {builtin_features::kImagePreview,
         "Image Preview",
         "Show popover previews when hovering over image references"},
        {builtin_features::kBreadcrumb,
         "Breadcrumb Bar",
         "File path breadcrumb navigation above the editor"},
    };

    // Register features in FeatureRegistry
    for (const auto& feat : features)
    {
        feature_registry.register_feature(FeatureInfo{.id = feat.id,
                                                      .display_name = feat.display_name,
                                                      .description = feat.description,
                                                      .default_enabled = true});
    }

    // Register plugins with PluginManager
    plugin_manager.register_plugin(std::make_unique<MermaidPlugin>());
    plugin_manager.register_plugin(std::make_unique<TableEditorPlugin>());
    plugin_manager.register_plugin(std::make_unique<FormatBarPlugin>());
    plugin_manager.register_plugin(std::make_unique<ThemeGalleryPlugin>());
    plugin_manager.register_plugin(std::make_unique<LinkPreviewPlugin>());
    plugin_manager.register_plugin(std::make_unique<ImagePreviewPlugin>());
    plugin_manager.register_plugin(std::make_unique<BreadcrumbPlugin>());

    MARKAMP_LOG_INFO("Registered {} built-in plugins and {} features", 7, features.size());
}

} // namespace markamp::core
