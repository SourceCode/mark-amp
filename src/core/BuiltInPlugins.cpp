#include "BuiltInPlugins.h"

#include "FeatureRegistry.h"
#include "IPanelService.h"
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
    explicit BuiltInPlugin(PluginManifest manifest, std::string feature_id)
        : manifest_(std::move(manifest))
        , feature_id_(std::move(feature_id))
    {
    }

    [[nodiscard]] auto manifest() const -> const PluginManifest& override
    {
        return manifest_;
    }

    void activate(PluginContext& ctx) override
    {
        active_ = true;
        ctx_ = ctx;

        // Register the feature.toggle.<id> command
        if (ctx.register_command_handler && ctx.feature_registry != nullptr)
        {
            const std::string toggle_cmd = "feature.toggle." + feature_id_;
            ctx.register_command_handler(toggle_cmd,
                                         [this, &ctx]()
                                         {
                                             if (ctx.feature_registry != nullptr)
                                             {
                                                 ctx.feature_registry->toggle(feature_id_);
                                                 MARKAMP_LOG_INFO(
                                                     "Feature '{}' toggled to: {}",
                                                     feature_id_,
                                                     ctx.feature_registry->is_enabled(feature_id_));
                                             }
                                         });
        }

        MARKAMP_LOG_INFO("Built-in plugin activated: {}", manifest_.id);
    }

    void deactivate() override
    {
        active_ = false;
        ctx_.reset();
        MARKAMP_LOG_INFO("Built-in plugin deactivated: {}", manifest_.id);
    }

protected:
    PluginManifest manifest_;
    std::string feature_id_;
    std::optional<PluginContext> ctx_;
};

// ── 9.1: Mermaid Diagrams Plugin ──

class MermaidPlugin final : public BuiltInPlugin
{
public:
    MermaidPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.mermaid",
                  .name = "Mermaid Diagrams",
                  .version = "1.0.0",
                  .description = "Render Mermaid diagrams in the preview pane",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "mermaid.enabled",
                                                .label = "Enable Mermaid Diagrams",
                                                .description =
                                                    "Render Mermaid fenced code blocks as diagrams",
                                                .category = "Features",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.mermaid",
                                                .title = "Toggle Mermaid Diagrams"}}}},
              builtin_features::kMermaid)
    {
    }
};

// ── 9.2: Table Editor Overlay Plugin ──

class TableEditorPlugin final : public BuiltInPlugin
{
public:
    TableEditorPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.table-editor",
                  .name = "Table Editor",
                  .version = "1.0.0",
                  .description = "Visual table editing overlay for Markdown tables",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "table-editor.enabled",
                                                .label = "Enable Table Editor",
                                                .description =
                                                    "Show interactive table editing overlay",
                                                .category = "Features",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.table-editor",
                                                .title = "Toggle Table Editor"}}}},
              builtin_features::kTableEditor)
    {
    }
};

// ── 9.3: Floating Format Bar Plugin ──

class FormatBarPlugin final : public BuiltInPlugin
{
public:
    FormatBarPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.format-bar",
                  .name = "Floating Format Bar",
                  .version = "1.0.0",
                  .description = "Context-sensitive formatting toolbar on text selection",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "format-bar.enabled",
                                                .label = "Enable Floating Format Bar",
                                                .description =
                                                    "Show formatting toolbar on text selection",
                                                .category = "Features",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.format-bar",
                                                .title = "Toggle Format Bar"}}}},
              builtin_features::kFormatBar)
    {
    }
};

// ── 9.4: Theme Gallery Plugin ──

class ThemeGalleryPlugin final : public BuiltInPlugin
{
public:
    ThemeGalleryPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.theme-gallery",
                  .name = "Theme Gallery",
                  .version = "1.0.0",
                  .description = "Browse and apply editor themes from the gallery",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "theme-gallery.enabled",
                                                .label = "Enable Theme Gallery",
                                                .description =
                                                    "Allow browsing themes in the settings panel",
                                                .category = "Features",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.theme-gallery",
                                                .title = "Toggle Theme Gallery"}}}},
              builtin_features::kThemeGallery)
    {
    }
};

// ── 9.5: Link Preview Plugin ──

class LinkPreviewPlugin final : public BuiltInPlugin
{
public:
    LinkPreviewPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.link-preview",
                  .name = "Link Preview",
                  .version = "1.0.0",
                  .description = "Show popover previews when hovering over links",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "link-preview.enabled",
                                                .label = "Enable Link Preview",
                                                .description =
                                                    "Show hovering popover previews for links",
                                                .category = "Features",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.link-preview",
                                                .title = "Toggle Link Preview"}}}},
              builtin_features::kLinkPreview)
    {
    }
};

// ── 9.6: Image Preview Plugin ──

class ImagePreviewPlugin final : public BuiltInPlugin
{
public:
    ImagePreviewPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.image-preview",
                  .name = "Image Preview",
                  .version = "1.0.0",
                  .description = "Show popover previews when hovering over image references",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "image-preview.enabled",
                                                .label = "Enable Image Preview",
                                                .description =
                                                    "Show hovering popover previews for images",
                                                .category = "Features",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.image-preview",
                                                .title = "Toggle Image Preview"}}}},
              builtin_features::kImagePreview)
    {
    }
};

// ── 9.7: Breadcrumb Bar Plugin ──

class BreadcrumbPlugin final : public BuiltInPlugin
{
public:
    BreadcrumbPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.breadcrumb",
                  .name = "Breadcrumb Bar",
                  .version = "1.0.0",
                  .description = "File path breadcrumb navigation above the editor",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "breadcrumb.enabled",
                                                .label = "Enable Breadcrumb Bar",
                                                .description =
                                                    "Show file path breadcrumbs above the editor",
                                                .category = "Features",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.breadcrumb",
                                                .title = "Toggle Breadcrumb Bar"}}}},
              builtin_features::kBreadcrumb)
    {
    }
};

// ── V8 Extension Extraction: FX Engine Plugin ──

class FxEnginePlugin final : public BuiltInPlugin
{
public:
    FxEnginePlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.fx-engine",
                  .name = "FX Engine",
                  .version = "1.0.0",
                  .description =
                      "Compiz/Beryl-style compositor effects: blur, glow, shadow, wobbly windows, "
                      "transitions, and visual presets",
                  .author = "MarkAmp",
                  .contributes =
                      {.settings = {{.id = "fx-engine.enabled",
                                     .label = "Enable FX Engine",
                                     .description =
                                         "Enable real-time compositor and visual effects",
                                     .category = "Visual Effects",
                                     .type = SettingType::Boolean,
                                     .default_value = "true"}},
                       .commands = {{.id = "feature.toggle.fx-engine",
                                     .title = "Toggle FX Engine"}}}},
              builtin_features::kFxEngine)
    {
    }
};

// ── V8 Extension Extraction: Text FX Plugin ──

class TextFxPlugin final : public BuiltInPlugin
{
public:
    TextFxPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.text-fx",
                  .name = "Text FX Renderer",
                  .version = "1.0.0",
                  .description =
                      "Text visual effects: glow, shadow, neon, gradient, bloom, CRT mode, and "
                      "per-token FX channels",
                  .author = "MarkAmp",
                  .contributes =
                      {.settings = {{.id = "text-fx.enabled",
                                     .label = "Enable Text FX",
                                     .description = "Enable text rendering effects in editor and "
                                                    "preview",
                                     .category = "Visual Effects",
                                     .type = SettingType::Boolean,
                                     .default_value = "true"}},
                       .commands = {{.id = "feature.toggle.text-fx", .title = "Toggle Text FX"}}}},
              builtin_features::kTextFx)
    {
    }
};

// ── V8 Extension Extraction: VSCode Theme Adapter Plugin ──

class VscodeThemesPlugin final : public BuiltInPlugin
{
public:
    VscodeThemesPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.vscode-themes",
                  .name = "VSCode Theme Adapter",
                  .version = "1.0.0",
                  .description =
                      "Import and apply VSCode-compatible themes including TextMate scope mapping "
                      "and semantic token support",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "vscode-themes.enabled",
                                                .label = "Enable VSCode Theme Import",
                                                .description =
                                                    "Allow importing and applying VSCode themes",
                                                .category = "Appearance",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.vscode-themes",
                                                .title = "Toggle VSCode Theme Import"}}}},
              builtin_features::kVscodeThemes)
    {
    }
};

// ── V8 Extension Extraction: Canvas Collaboration Plugin ──

class CanvasCollabPlugin final : public BuiltInPlugin
{
public:
    CanvasCollabPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.canvas-collab",
                  .name = "Canvas Collaboration",
                  .version = "1.0.0",
                  .description =
                      "Real-time collaboration for canvas: live cursors, presence, comments, "
                      "voting, timer, and facilitation tools",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "canvas-collab.enabled",
                                                .label = "Enable Canvas Collaboration",
                                                .description =
                                                    "Enable real-time collaboration features on "
                                                    "canvas boards",
                                                .category = "Canvas",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.canvas-collab",
                                                .title = "Toggle Canvas Collaboration"}}}},
              builtin_features::kCanvasCollab)
    {
    }
};

// ── V8 Extension Extraction: Canvas Apps & Integration Plugin ──

class CanvasAppsPlugin final : public BuiltInPlugin
{
public:
    CanvasAppsPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.canvas-apps",
                  .name = "Canvas Apps & Widgets",
                  .version = "1.0.0",
                  .description =
                      "Canvas app marketplace: embeddable data widgets, integration service, and "
                      "third-party app hosting",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "canvas-apps.enabled",
                                                .label = "Enable Canvas Apps",
                                                .description =
                                                    "Enable canvas app widgets and integrations",
                                                .category = "Canvas",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.canvas-apps",
                                                .title = "Toggle Canvas Apps"}}}},
              builtin_features::kCanvasApps)
    {
    }
};

// ── V8 Extension Extraction: Kanban Board Plugin ──

class KanbanPlugin final : public BuiltInPlugin
{
public:
    KanbanPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.kanban",
                  .name = "Kanban Board",
                  .version = "1.0.0",
                  .description =
                      "Kanban board workflow for canvas: columns, swimlanes, WIP limits, and "
                      "card management",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "kanban.enabled",
                                                .label = "Enable Kanban Board",
                                                .description =
                                                    "Enable kanban board creation on canvas",
                                                .category = "Canvas",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.kanban",
                                                .title = "Toggle Kanban Board"}}}},
              builtin_features::kKanban)
    {
    }
};

// ── V8 Extension Extraction: Mind Map Plugin ──

class MindMapPlugin final : public BuiltInPlugin
{
public:
    MindMapPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.mind-map",
                  .name = "Mind Map",
                  .version = "1.0.0",
                  .description = "Mind map tool for canvas: radial/tree layouts, auto-balance, "
                                 "expand/collapse, "
                                 "and node styling",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "mind-map.enabled",
                                                .label = "Enable Mind Map",
                                                .description = "Enable mind map creation on canvas",
                                                .category = "Canvas",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.mind-map",
                                                .title = "Toggle Mind Map"}}}},
              builtin_features::kMindMap)
    {
    }
};

// ── V8 Extension Extraction: Diagram Library Plugin ──

class DiagramLibraryPlugin final : public BuiltInPlugin
{
public:
    DiagramLibraryPlugin()
        : BuiltInPlugin(
              PluginManifest{
                  .id = "markamp.diagram-library",
                  .name = "Diagram Library",
                  .version = "1.0.0",
                  .description =
                      "UML/BPMN diagram stencils: class diagrams, sequence diagrams, flowcharts, "
                      "and process model shapes",
                  .author = "MarkAmp",
                  .contributes = {.settings = {{.id = "diagram-library.enabled",
                                                .label = "Enable Diagram Library",
                                                .description =
                                                    "Enable UML/BPMN diagram stencils on canvas",
                                                .category = "Canvas",
                                                .type = SettingType::Boolean,
                                                .default_value = "true"}},
                                  .commands = {{.id = "feature.toggle.diagram-library",
                                                .title = "Toggle Diagram Library"}}}},
              builtin_features::kDiagramLibrary)
    {
    }
};

// ── Test Panel API Plugin ──

class SamplePanelPlugin final : public BuiltInPlugin
{
public:
    SamplePanelPlugin()
        : BuiltInPlugin(
              PluginManifest{.id = "markamp.sample-panel",
                             .name = "Sample Panel",
                             .version = "1.0.0",
                             .description = "Test plugin to demonstrate Panel Contribution API",
                             .author = "MarkAmp",
                             .contributes = {}},
              "sample-panel")
    {
    }

    void activate(PluginContext& ctx) override
    {
        BuiltInPlugin::activate(ctx);

        if (ctx.panel_service != nullptr)
        {
            // Register panel via the Panel Contribution API
            ctx.panel_service->register_panel(manifest_.id + ".main", manifest_.name, "icon-plug");
            MARKAMP_LOG_INFO(
                "SamplePanelPlugin successfully contributed a panel to the bottom area.");
        }
        else
        {
            MARKAMP_LOG_ERROR("SamplePanelPlugin failed: PanelService is null in PluginContext");
        }
    }

    void deactivate() override
    {
        if (active_ && ctx_.has_value() && ctx_->panel_service != nullptr)
        {
            ctx_->panel_service->unregister_panel(manifest_.id + ".main");
        }
        BuiltInPlugin::deactivate();
    }
};

class TerminalPanelPlugin final : public BuiltInPlugin
{
public:
    TerminalPanelPlugin()
        : BuiltInPlugin(PluginManifest{.id = "markamp.terminal-panel",
                                       .name = "Terminal Panel",
                                       .version = "1.0.0",
                                       .description = "Built-in Terminal Panel for MarkAmp",
                                       .author = "MarkAmp",
                                       .contributes = {}},
                        "terminal-panel")
    {
    }

    void activate(PluginContext& ctx) override
    {
        BuiltInPlugin::activate(ctx);
        if (ctx.panel_service != nullptr)
        {
            ctx.panel_service->register_panel("terminal", manifest_.name, "\xEF\x84\xA0");
            MARKAMP_LOG_INFO("TerminalPanelPlugin contributed the terminal panel.");
        }
    }

    void deactivate() override
    {
        if (active_ && ctx_.has_value() && ctx_->panel_service != nullptr)
        {
            ctx_->panel_service->unregister_panel("terminal");
        }
        BuiltInPlugin::deactivate();
    }
};

class DebugConsolePanelPlugin final : public BuiltInPlugin
{
public:
    DebugConsolePanelPlugin()
        : BuiltInPlugin(PluginManifest{.id = "markamp.debug-console-panel",
                                       .name = "Debug Console",
                                       .version = "1.0.0",
                                       .description = "Built-in Debug Console Panel for MarkAmp",
                                       .author = "MarkAmp",
                                       .contributes = {}},
                        "debug-console-panel")
    {
    }

    void activate(PluginContext& ctx) override
    {
        BuiltInPlugin::activate(ctx);
        if (ctx.panel_service != nullptr)
        {
            ctx.panel_service->register_panel("debug", manifest_.name, "\xEF\x86\x88");
            MARKAMP_LOG_INFO("DebugConsolePanelPlugin contributed the debug console panel.");
        }
    }

    void deactivate() override
    {
        if (active_ && ctx_.has_value() && ctx_->panel_service != nullptr)
        {
            ctx_->panel_service->unregister_panel("debug");
        }
        BuiltInPlugin::deactivate();
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
        // V8 extension extraction
        {builtin_features::kFxEngine,
         "FX Engine",
         "Compiz/Beryl-style compositor effects pipeline"},
        {builtin_features::kTextFx,
         "Text FX Renderer",
         "Text visual effects: glow, shadow, neon, gradient, bloom, CRT mode"},
        {builtin_features::kVscodeThemes,
         "VSCode Theme Adapter",
         "Import and apply VSCode-compatible themes with TextMate scope mapping"},
        {builtin_features::kCanvasCollab,
         "Canvas Collaboration",
         "Real-time collaboration: live cursors, comments, voting, and facilitation"},
        {builtin_features::kCanvasApps,
         "Canvas Apps & Widgets",
         "Canvas app marketplace and embeddable data widgets"},
        {builtin_features::kKanban, "Kanban Board", "Kanban board workflow for canvas"},
        {builtin_features::kMindMap,
         "Mind Map",
         "Mind map tool for canvas with radial/tree layouts"},
        {builtin_features::kDiagramLibrary,
         "Diagram Library",
         "UML/BPMN diagram stencils for canvas"},
        // V11 Node Editor
        {builtin_features::kNodeEditor,
         "Node Editor",
         "Multi-domain node-based visual programming editor"},
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
    // V8 extension extraction
    plugin_manager.register_plugin(std::make_unique<FxEnginePlugin>());
    plugin_manager.register_plugin(std::make_unique<TextFxPlugin>());
    plugin_manager.register_plugin(std::make_unique<VscodeThemesPlugin>());
    plugin_manager.register_plugin(std::make_unique<CanvasCollabPlugin>());
    plugin_manager.register_plugin(std::make_unique<CanvasAppsPlugin>());
    plugin_manager.register_plugin(std::make_unique<KanbanPlugin>());
    plugin_manager.register_plugin(std::make_unique<MindMapPlugin>());
    plugin_manager.register_plugin(std::make_unique<DiagramLibraryPlugin>());
    plugin_manager.register_plugin(std::make_unique<SamplePanelPlugin>());
    plugin_manager.register_plugin(std::make_unique<TerminalPanelPlugin>());
    plugin_manager.register_plugin(std::make_unique<DebugConsolePanelPlugin>());

    MARKAMP_LOG_INFO("Registered {} built-in plugins and {} features", 15, features.size());
}

} // namespace markamp::core
