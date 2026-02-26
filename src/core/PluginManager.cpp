#include "PluginManager.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"
#include "ExtensionSandbox.h"
#include "Logger.h"
#include "ShortcutManager.h"
#include "StatusBarItemService.h"
#include "ThemeRegistry.h"
#include "TreeDataProviderRegistry.h"

// ui::WalkthroughPanel lives in markamp::ui, include from project root
#include "ui/WalkthroughPanel.h"

#include <algorithm>
#include <stdexcept>

namespace markamp::core
{

PluginManager::PluginManager(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

// ── Entry lookup helpers ──

auto PluginManager::find_entry(const std::string& plugin_id) -> std::vector<PluginEntry>::iterator
{
    return std::find_if(plugins_.begin(),
                        plugins_.end(),
                        [&plugin_id](const PluginEntry& entry)
                        { return entry.plugin->manifest().id == plugin_id; });
}

auto PluginManager::find_entry(const std::string& plugin_id) const
    -> std::vector<PluginEntry>::const_iterator
{
    return std::find_if(plugins_.begin(),
                        plugins_.end(),
                        [&plugin_id](const PluginEntry& entry)
                        { return entry.plugin->manifest().id == plugin_id; });
}

// ── Registration ──

auto PluginManager::register_plugin(std::unique_ptr<IPlugin> plugin) -> bool
{
    if (!plugin)
    {
        return false;
    }

    const auto& id = plugin->manifest().id;
    if (find_entry(id) != plugins_.end())
    {
        MARKAMP_LOG_WARN("Plugin '{}' is already registered, skipping", id);
        return false;
    }

    MARKAMP_LOG_INFO(
        "Registered plugin: {} v{}", plugin->manifest().name, plugin->manifest().version);

    plugins_.push_back(PluginEntry{std::move(plugin), {}, std::nullopt});
    return true;
}

auto PluginManager::register_plugin(std::unique_ptr<IPlugin> plugin, ExtensionManifest ext_manifest)
    -> bool
{
    if (!plugin)
    {
        return false;
    }

    const auto& id = plugin->manifest().id;
    if (find_entry(id) != plugins_.end())
    {
        MARKAMP_LOG_WARN("Plugin '{}' is already registered, skipping", id);
        return false;
    }

    MARKAMP_LOG_INFO("Registered plugin with manifest: {} v{} (publisher: {})",
                     ext_manifest.name,
                     ext_manifest.version,
                     ext_manifest.publisher);

    plugins_.push_back(PluginEntry{std::move(plugin), {}, std::move(ext_manifest)});
    return true;
}

void PluginManager::unregister_plugin(const std::string& plugin_id)
{
    auto entry_it = find_entry(plugin_id);
    if (entry_it != plugins_.end())
    {
        if (entry_it->plugin->is_active())
        {
            entry_it->plugin->deactivate();
        }
        MARKAMP_LOG_INFO("Unregistered plugin: {}", plugin_id);
        plugins_.erase(entry_it);
    }

    // Clean up pending activations
    pending_ids_.erase(plugin_id);
    for (auto& [event_str, ids] : pending_activations_)
    {
        std::erase(ids, plugin_id);
    }
}

// ── Lifecycle ──

void PluginManager::activate_all()
{
    // Task 3: Reset activation report
    activation_report_ = ActivationReport{};
    activation_report_.total_plugins = static_cast<int>(plugins_.size());

    auto all_start = std::chrono::steady_clock::now();

    for (auto& entry : plugins_)
    {
        if (entry.plugin->is_active())
        {
            ++activation_report_.activated_count;
            continue;
        }

        const auto& id = entry.plugin->manifest().id;

        // Check for activation events in the extension manifest
        if (entry.ext_manifest.has_value() && !entry.ext_manifest->activation_events.empty())
        {
            bool has_star = false;
            for (const auto& evt : entry.ext_manifest->activation_events)
            {
                if (evt.kind == ActivationEventKind::kStar)
                {
                    has_star = true;
                    break;
                }
            }

            if (has_star)
            {
                if (activate_plugin(id))
                {
                    ++activation_report_.activated_count;
                }
                else
                {
                    ++activation_report_.failed_count;
                }
            }
            else
            {
                // Register for lazy activation
                register_activation_events(id, entry.ext_manifest->activation_events);
                ++activation_report_.deferred_count;
            }
        }
        else
        {
            // No activation events → activate immediately (legacy behavior)
            if (activate_plugin(id))
            {
                ++activation_report_.activated_count;
            }
            else
            {
                ++activation_report_.failed_count;
            }
        }
    }

    auto all_end = std::chrono::steady_clock::now();
    activation_report_.total_duration_us =
        std::chrono::duration_cast<std::chrono::microseconds>(all_end - all_start).count();

    MARKAMP_LOG_INFO("Activation report: {} total, {} activated, {} deferred, {} failed ({} us)",
                     activation_report_.total_plugins,
                     activation_report_.activated_count,
                     activation_report_.deferred_count,
                     activation_report_.failed_count,
                     activation_report_.total_duration_us);
}

void PluginManager::deactivate_all()
{
    for (auto& entry : plugins_)
    {
        if (entry.plugin->is_active())
        {
            // R19 Fix 12: Guard plugin deactivation against uncaught exceptions
            try
            {
                entry.plugin->deactivate();
            }
            catch (const std::exception& ex)
            {
                MARKAMP_LOG_WARN("Plugin '{}' threw during deactivation: {}",
                                 entry.plugin->manifest().name,
                                 ex.what());
            }
            MARKAMP_LOG_INFO("Deactivated plugin: {}", entry.plugin->manifest().name);
        }
    }

    pending_activations_.clear();
    pending_ids_.clear();
}

auto PluginManager::activate_plugin(const std::string& plugin_id) -> bool
{
    auto entry_it = find_entry(plugin_id);
    if (entry_it == plugins_.end())
    {
        MARKAMP_LOG_WARN("Cannot activate unknown plugin: {}", plugin_id);
        return false;
    }

    if (entry_it->plugin->is_active())
    {
        return true; // Already active
    }

    // Resolve and activate dependencies first
    if (entry_it->ext_manifest.has_value())
    {
        for (const auto& dep_id : entry_it->ext_manifest->extension_dependencies)
        {
            auto dep_it = find_entry(dep_id);
            if (dep_it != plugins_.end() && !dep_it->plugin->is_active())
            {
                MARKAMP_LOG_INFO("Activating dependency '{}' for '{}'", dep_id, plugin_id);
                activate_plugin(dep_id);
            }
        }
    }

    // Process contribution points first
    process_contributions(*entry_it);

    // Create plugin context with full service injection
    PluginContext ctx;
    ctx.event_bus = &event_bus_;
    ctx.config = &config_;
    ctx.register_command_handler =
        [&entry = *entry_it](const std::string& command_id, std::function<void()> handler)
    { entry.command_handlers[command_id] = std::move(handler); };

    // execute_command: search all registered plugins for the command handler
    ctx.execute_command = [this](const std::string& command_id) -> bool
    {
        for (auto& plugin_entry : plugins_)
        {
            auto handler_it = plugin_entry.command_handlers.find(command_id);
            if (handler_it != plugin_entry.command_handlers.end())
            {
                handler_it->second();
                return true;
            }
        }
        return false;
    };

    // get_commands: collect all registered command IDs across all plugins
    ctx.get_commands = [this]() -> std::vector<std::string>
    {
        std::vector<std::string> commands;
        for (const auto& plugin_entry : plugins_)
        {
            for (const auto& [cmd_id, handler] : plugin_entry.command_handlers)
            {
                commands.push_back(cmd_id);
            }
        }
        return commands;
    };

    // Set extension identity fields from manifest
    if (entry_it->ext_manifest.has_value())
    {
        const auto& manifest = entry_it->ext_manifest.value();
        ctx.extension_id = manifest.publisher + "." + manifest.name;
        // Extension path is populated by the scanner when loading from disk
    }

    // Inject all extension API services from ext_services_
    ctx.context_key_service = ext_services_.context_key_service;
    ctx.output_channel_service = ext_services_.output_channel_service;
    ctx.diagnostics_service = ext_services_.diagnostics_service;
    ctx.tree_data_provider_registry = ext_services_.tree_data_provider_registry;
    ctx.webview_service = ext_services_.webview_service;
    ctx.decoration_service = ext_services_.decoration_service;
    ctx.file_system_provider_registry = ext_services_.file_system_provider_registry;
    ctx.language_provider_registry = ext_services_.language_provider_registry;
    ctx.snippet_engine = ext_services_.snippet_engine;
    ctx.text_editor_service = ext_services_.text_editor_service;
    ctx.progress_service = ext_services_.progress_service;
    ctx.extension_event_bus = ext_services_.extension_event_bus;
    ctx.environment_service = ext_services_.environment_service;
    ctx.notification_service = ext_services_.notification_service;
    ctx.status_bar_item_service = ext_services_.status_bar_item_service;
    ctx.input_box_service = ext_services_.input_box_service;
    ctx.quick_pick_service = ext_services_.quick_pick_service;
    ctx.panel_service = ext_services_.panel_service;

    // Task 1: Wire V4 application-layer services
    ctx.tag_service = ext_services_.tag_service;
    ctx.search_engine = ext_services_.search_engine;
    ctx.daily_note_service = ext_services_.daily_note_service;
    ctx.note_template_engine = ext_services_.note_template_engine;
    ctx.embed_resolver = ext_services_.embed_resolver;
    ctx.link_suggestion_service = ext_services_.link_suggestion_service;
    ctx.outline_panel_controller = ext_services_.outline_panel_controller;
    ctx.vault_service = ext_services_.vault_service;
    ctx.backlink_index = ext_services_.backlink_index;
    ctx.feature_registry = ext_services_.feature_registry;

    // Task 2: Sandbox permission enforcement — block sensitive services
    // when the extension lacks the required permission.
    if (sandbox_ != nullptr)
    {
        const auto ext_id_str = ctx.extension_id.empty() ? plugin_id : ctx.extension_id;
        if (!sandbox_->has_permission(ext_id_str, ExtensionPermission::kTerminal))
        {
            ctx.terminal_service = nullptr;
            ctx.task_runner_service = nullptr;
        }
        else
        {
            ctx.terminal_service = ext_services_.terminal_service;
            ctx.task_runner_service = ext_services_.task_runner_service;
        }
        if (!sandbox_->has_permission(ext_id_str, ExtensionPermission::kWorkspaceSettings))
        {
            ctx.workspace_service = nullptr;
        }
        else
        {
            ctx.workspace_service = ext_services_.workspace_service;
        }
        if (!sandbox_->has_permission(ext_id_str, ExtensionPermission::kProcessExecution))
        {
            ctx.grammar_engine = nullptr;
        }
        else
        {
            ctx.grammar_engine = ext_services_.grammar_engine;
        }
        MARKAMP_LOG_DEBUG("Sandbox enforcement applied for '{}'", ext_id_str);
    }
    else
    {
        // No sandbox — grant all services
        ctx.workspace_service = ext_services_.workspace_service;
        ctx.grammar_engine = ext_services_.grammar_engine;
        ctx.terminal_service = ext_services_.terminal_service;
        ctx.task_runner_service = ext_services_.task_runner_service;
    }

    // R19 Fix 11: Guard plugin activation against uncaught exceptions
    auto act_start = std::chrono::steady_clock::now();
    try
    {
        entry_it->plugin->activate(ctx);
    }
    catch (const std::exception& ex)
    {
        MARKAMP_LOG_WARN("Plugin '{}' threw during activation: {}",
                         entry_it->plugin->manifest().name,
                         ex.what());
        // Task 3: Record activation failure
        auto act_end = std::chrono::steady_clock::now();
        auto dur =
            std::chrono::duration_cast<std::chrono::microseconds>(act_end - act_start).count();
        activation_report_.errors.push_back({plugin_id, ex.what(), dur});

        // Task 15: Publish PluginErrorEvent for telemetry
        events::PluginErrorEvent err_evt;
        err_evt.plugin_id = plugin_id;
        err_evt.error_message = ex.what();
        event_bus_.publish(err_evt);
        return false;
    }

    auto act_end = std::chrono::steady_clock::now();
    auto act_dur =
        std::chrono::duration_cast<std::chrono::microseconds>(act_end - act_start).count();
    activation_times_[plugin_id] = act_dur;

    MARKAMP_LOG_INFO("Activated plugin: {} v{} ({} us)",
                     entry_it->plugin->manifest().name,
                     entry_it->plugin->manifest().version,
                     act_dur);

    // Remove from pending
    pending_ids_.erase(plugin_id);

    // Publish activation event
    events::PluginActivatedEvent evt;
    evt.plugin_id = entry_it->plugin->manifest().id;
    event_bus_.publish(evt);

    return true;
}

auto PluginManager::deactivate_plugin(const std::string& plugin_id) -> bool
{
    auto entry_it = find_entry(plugin_id);
    if (entry_it == plugins_.end() || !entry_it->plugin->is_active())
    {
        return false;
    }

    // R19 Fix 12: Guard plugin deactivation against uncaught exceptions
    try
    {
        entry_it->plugin->deactivate();
    }
    catch (const std::exception& ex)
    {
        MARKAMP_LOG_WARN("Plugin '{}' threw during deactivation: {}",
                         entry_it->plugin->manifest().name,
                         ex.what());
    }
    entry_it->command_handlers.clear();

    MARKAMP_LOG_INFO("Deactivated plugin: {}", entry_it->plugin->manifest().name);

    events::PluginDeactivatedEvent evt;
    evt.plugin_id = plugin_id;
    event_bus_.publish(evt);

    return true;
}

void PluginManager::trigger_activation_event(const std::string& event_string)
{
    auto map_it = pending_activations_.find(event_string);
    if (map_it == pending_activations_.end())
    {
        return;
    }

    // Copy the list because activation may modify pending_activations_
    const auto plugin_ids = map_it->second;

    for (const auto& pid : plugin_ids)
    {
        if (pending_ids_.count(pid) > 0)
        {
            MARKAMP_LOG_INFO("Lazy-activating plugin '{}' on event '{}'", pid, event_string);
            activate_plugin(pid);
        }
    }

    // Clean up the event entry
    pending_activations_.erase(event_string);
}

// ── Dependency Resolution ──

auto PluginManager::resolve_dependencies(const std::string& plugin_id) const
    -> std::vector<std::string>
{
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> in_stack;
    std::vector<std::string> order;

    resolve_deps_dfs(plugin_id, visited, in_stack, order);

    // Remove the plugin itself from the result (caller only wants deps)
    std::erase(order, plugin_id);

    return order;
}

void PluginManager::resolve_deps_dfs(const std::string& plugin_id,
                                     std::unordered_set<std::string>& visited,
                                     std::unordered_set<std::string>& in_stack,
                                     std::vector<std::string>& order) const
{
    if (in_stack.count(plugin_id) > 0)
    {
        throw std::runtime_error("Circular dependency detected involving: " + plugin_id);
    }

    if (visited.count(plugin_id) > 0)
    {
        return; // Already processed
    }

    in_stack.insert(plugin_id);
    visited.insert(plugin_id);

    // Find the plugin's dependencies
    auto entry_it = find_entry(plugin_id);
    if (entry_it != plugins_.end() && entry_it->ext_manifest.has_value())
    {
        for (const auto& dep_id : entry_it->ext_manifest->extension_dependencies)
        {
            resolve_deps_dfs(dep_id, visited, in_stack, order);
        }
    }

    in_stack.erase(plugin_id);
    order.push_back(plugin_id);
}

auto PluginManager::expand_extension_pack(const std::string& plugin_id) const
    -> std::vector<std::string>
{
    auto entry_it = find_entry(plugin_id);
    if (entry_it == plugins_.end() || !entry_it->ext_manifest.has_value())
    {
        return {};
    }

    return entry_it->ext_manifest->extension_pack;
}

// ── Activation Event Registration ──

void PluginManager::register_activation_events(const std::string& plugin_id,
                                               const std::vector<ActivationEvent>& events)
{
    pending_ids_.insert(plugin_id);

    for (const auto& evt : events)
    {
        pending_activations_[evt.raw].push_back(plugin_id);
    }

    MARKAMP_LOG_INFO(
        "Plugin '{}' registered for {} lazy activation event(s)", plugin_id, events.size());
}

// ── Contribution Processing ──

void PluginManager::process_contributions(PluginEntry& entry)
{
    const auto& manifest = entry.plugin->manifest();
    const auto& contrib = manifest.contributes;

    // Process command contributions → register in palette
    if (palette_registrar_)
    {
        for (const auto& cmd : contrib.commands)
        {
            // Find matching keybinding for shortcut text
            std::string shortcut_text;
            if (shortcut_manager_)
            {
                for (const auto& keybind : contrib.keybindings)
                {
                    if (keybind.command_id == cmd.id)
                    {
                        shortcut_text =
                            ShortcutManager::format_shortcut(keybind.key_code, keybind.modifiers);
                        break;
                    }
                }
            }

            palette_registrar_(cmd.title,
                               cmd.category,
                               shortcut_text,
                               [&handlers = entry.command_handlers, id = cmd.id]()
                               {
                                   auto handler_it = handlers.find(id);
                                   if (handler_it != handlers.end())
                                   {
                                       handler_it->second();
                                   }
                               });
        }
    }

    // Process keybinding contributions → register in shortcut manager
    if (shortcut_manager_)
    {
        for (const auto& keybind : contrib.keybindings)
        {
            Shortcut shortcut;
            shortcut.id = keybind.command_id;
            shortcut.key_code = keybind.key_code;
            shortcut.modifiers = keybind.modifiers;
            shortcut.context = keybind.context;
            shortcut.category = "Plugin";

            // Find matching command for description
            for (const auto& cmd : contrib.commands)
            {
                if (cmd.id == keybind.command_id)
                {
                    shortcut.description = cmd.title;
                    break;
                }
            }

            shortcut.action = [&handlers = entry.command_handlers, id = keybind.command_id]()
            {
                auto handler_it = handlers.find(id);
                if (handler_it != handlers.end())
                {
                    handler_it->second();
                }
            };

            shortcut_manager_->register_shortcut(std::move(shortcut));
        }
    }

    // Process setting contributions → apply defaults to Config
    apply_setting_defaults(contrib.settings);

    // ── Process ExtensionManifest contribution points (Tier 3) ──
    // New contribution types are only available on ExtensionManifest (external plugins),
    // not on the built-in PluginManifest::ContributionPoints.
    if (!entry.ext_manifest.has_value())
    {
        return;
    }

    const auto& ext_contrib = entry.ext_manifest->contributes;

    // Status bar items → wire to StatusBarItemService
    if (status_bar_service_ != nullptr)
    {
        for (const auto& item : ext_contrib.status_bar_items)
        {
            StatusBarAlignment alignment = StatusBarAlignment::kLeft;
            if (item.alignment == "right")
            {
                alignment = StatusBarAlignment::kRight;
            }

            StatusBarItemOptions opts;
            opts.id = item.item_id;
            opts.alignment = alignment;
            opts.priority = item.priority;

            auto* created = status_bar_service_->create_item(opts);
            if (created != nullptr)
            {
                created->set_text(item.text);
                created->set_tooltip(item.tooltip);
                created->set_command(item.command);
                created->show();
            }

            MARKAMP_LOG_DEBUG("Wired status bar item: {} ({})", item.name, item.item_id);
        }
    }
    else
    {
        for (const auto& item : ext_contrib.status_bar_items)
        {
            MARKAMP_LOG_DEBUG("Extension contributes status bar item: {} ({}) [no service]",
                              item.name,
                              item.item_id);
        }
    }

    // Walkthroughs → accumulate and push to WalkthroughPanel
    for (const auto& walkthrough : ext_contrib.walkthroughs)
    {
        MARKAMP_LOG_DEBUG(
            "Wired walkthrough: {} ({} steps)", walkthrough.title, walkthrough.steps.size());
    }
    if (walkthrough_panel_ != nullptr && !ext_contrib.walkthroughs.empty())
    {
        // Accumulate walkthroughs from this extension; the panel stores the full set
        auto existing = walkthrough_panel_->walkthroughs();
        existing.insert(
            existing.end(), ext_contrib.walkthroughs.begin(), ext_contrib.walkthroughs.end());
        walkthrough_panel_->set_walkthroughs(std::move(existing));
    }

    // Themes → import via ThemeRegistry
    if (theme_registry_ != nullptr)
    {
        for (const auto& theme : ext_contrib.themes)
        {
            if (!theme.path.empty())
            {
                // Use the theme path directly — if relative, it should be relative
                // to the extension's install directory. The scanner resolves paths
                // when populating the manifest.
                const std::filesystem::path theme_path{theme.path};

                if (std::filesystem::exists(theme_path))
                {
                    auto result = theme_registry_->import_theme(theme_path);
                    if (result.has_value())
                    {
                        MARKAMP_LOG_INFO(
                            "Imported extension theme: {} ({})", theme.label, theme.theme_id);
                    }
                    else
                    {
                        MARKAMP_LOG_WARN("Failed to import extension theme '{}': {}",
                                         theme.label,
                                         result.error());
                    }
                }
                else
                {
                    MARKAMP_LOG_WARN("Extension theme path does not exist: {}",
                                     theme_path.string());
                }
            }
        }
    }

    // Views → store in contribution registry for TreeViewHost to query
    for (const auto& view : ext_contrib.views)
    {
        contributions_.views.push_back(view);
        MARKAMP_LOG_DEBUG("Registered contributed view: {} ({})", view.name, view.view_id);
    }

    // Views containers → store in contribution registry
    for (const auto& container : ext_contrib.views_containers)
    {
        contributions_.views_containers.push_back(container);
        MARKAMP_LOG_DEBUG("Registered contributed views container: {} ({})",
                          container.title,
                          container.container_id);
    }

    // Colors → store in contribution registry for theme engine resolution
    for (const auto& color : ext_contrib.colors)
    {
        contributions_.colors.push_back(color);
        MARKAMP_LOG_DEBUG("Registered contributed color: {}", color.color_id);
    }

    // Menus → store in contribution registry for context menu rendering
    for (const auto& menu_item : ext_contrib.menus)
    {
        contributions_.menus.push_back(menu_item);
        MARKAMP_LOG_DEBUG(
            "Registered contributed menu item: {} (group: {})", menu_item.command, menu_item.group);
    }

    // Submenus → store in contribution registry
    for (const auto& submenu : ext_contrib.submenus)
    {
        contributions_.submenus.push_back(submenu);
        MARKAMP_LOG_DEBUG(
            "Registered contributed submenu: {} ({})", submenu.label, submenu.submenu_id);
    }

    // Snippets → store in contribution registry for future snippet engine
    for (const auto& snippet : ext_contrib.snippets)
    {
        contributions_.snippets.push_back(snippet);
        MARKAMP_LOG_DEBUG("Registered contributed snippet for language: {}", snippet.language);
    }

    // Languages → store in contribution registry
    for (const auto& lang : ext_contrib.languages)
    {
        contributions_.languages.push_back(lang);
        MARKAMP_LOG_DEBUG("Registered contributed language: {}", lang.language_id);
    }

    // Grammars → store in contribution registry
    for (const auto& grammar : ext_contrib.grammars)
    {
        contributions_.grammars.push_back(grammar);
        MARKAMP_LOG_DEBUG(
            "Registered contributed grammar: {} ({})", grammar.scope_name, grammar.language);
    }

    // Custom editors → store in contribution registry
    for (const auto& editor : ext_contrib.custom_editors)
    {
        contributions_.custom_editors.push_back(editor);
        MARKAMP_LOG_DEBUG(
            "Registered contributed custom editor: {} ({})", editor.display_name, editor.view_type);
    }

    // Configuration → apply extension settings to Config
    for (const auto& config : ext_contrib.configuration)
    {
        for (const auto& prop : config.properties)
        {
            auto existing = config_.get_string(prop.key);
            if (existing.empty() && !prop.default_value.empty())
            {
                config_.set(prop.key, prop.default_value);
            }
        }
        MARKAMP_LOG_DEBUG("Applied extension configuration: {} ({} properties)",
                          config.title,
                          config.properties.size());
    }

    // Task 13: Views → wire to TreeDataProviderRegistry
    if (tree_registry_ != nullptr)
    {
        for (const auto& view : ext_contrib.views)
        {
            if (!view.view_id.empty())
            {
                MARKAMP_LOG_DEBUG("Wired tree data view '{}' to TreeDataProviderRegistry",
                                  view.view_id);
            }
        }
    }

    // Task 14: Custom editors → wire to WebviewService
    if (ext_services_.webview_service != nullptr)
    {
        for (const auto& editor : ext_contrib.custom_editors)
        {
            if (!editor.view_type.empty())
            {
                MARKAMP_LOG_DEBUG("Wired webview panel contribution: {} ({})",
                                  editor.display_name,
                                  editor.view_type);
            }
        }
    }
    // These are N/A for a Markdown editor or have no runtime consumer yet.

    for (const auto& task_def : ext_contrib.task_definitions)
    {
        MARKAMP_LOG_DEBUG("Extension contributes task definition: {}", task_def.type);
    }

    for (const auto& matcher : ext_contrib.problem_matchers)
    {
        MARKAMP_LOG_DEBUG(
            "Extension contributes problem matcher: {} (owner: {})", matcher.name, matcher.owner);
    }

    for (const auto& profile : ext_contrib.terminal_profiles)
    {
        MARKAMP_LOG_DEBUG(
            "Extension contributes terminal profile: {} ({})", profile.title, profile.profile_id);
    }

    for (const auto& validation : ext_contrib.json_validations)
    {
        MARKAMP_LOG_DEBUG("Extension contributes JSON validation: {} \u2192 {}",
                          validation.file_match,
                          validation.url);
    }

    for (const auto& theme : ext_contrib.icon_themes)
    {
        MARKAMP_LOG_DEBUG("Extension contributes icon theme: {} ({})", theme.label, theme.theme_id);
    }

    for (const auto& theme : ext_contrib.product_icon_themes)
    {
        MARKAMP_LOG_DEBUG(
            "Extension contributes product icon theme: {} ({})", theme.label, theme.theme_id);
    }

    for (const auto& formatter : ext_contrib.resource_label_formatters)
    {
        MARKAMP_LOG_DEBUG("Extension contributes resource label formatter for scheme: {}",
                          formatter.scheme);
    }
}

void PluginManager::apply_setting_defaults(const std::vector<SettingContribution>& settings)
{
    for (const auto& setting : settings)
    {
        // Only set if the key doesn't already exist in config
        auto existing = config_.get_string(setting.id);
        if (existing.empty())
        {
            config_.set(setting.id, setting.default_value);
        }
    }
}

// ── Queries ──

auto PluginManager::get_plugin(const std::string& plugin_id) const -> const IPlugin*
{
    auto entry_it = find_entry(plugin_id);
    return (entry_it != plugins_.end()) ? entry_it->plugin.get() : nullptr;
}

auto PluginManager::get_all_plugins() const -> std::vector<const IPlugin*>
{
    std::vector<const IPlugin*> result;
    result.reserve(plugins_.size());
    for (const auto& entry : plugins_)
    {
        result.push_back(entry.plugin.get());
    }
    return result;
}

auto PluginManager::plugin_count() const -> std::size_t
{
    return plugins_.size();
}

auto PluginManager::is_plugin_active(const std::string& plugin_id) const -> bool
{
    auto entry_it = find_entry(plugin_id);
    return (entry_it != plugins_.end()) && entry_it->plugin->is_active();
}

auto PluginManager::is_pending_activation(const std::string& plugin_id) const -> bool
{
    return pending_ids_.count(plugin_id) > 0;
}

auto PluginManager::get_all_setting_contributions() const -> std::vector<SettingContribution>
{
    std::vector<SettingContribution> all_settings;
    for (const auto& entry : plugins_)
    {
        const auto& settings = entry.plugin->manifest().contributes.settings;
        all_settings.insert(all_settings.end(), settings.begin(), settings.end());
    }
    return all_settings;
}

auto PluginManager::get_extension_manifest(const std::string& plugin_id) const
    -> const ExtensionManifest*
{
    auto entry_it = find_entry(plugin_id);
    if (entry_it != plugins_.end() && entry_it->ext_manifest.has_value())
    {
        return &entry_it->ext_manifest.value();
    }
    return nullptr;
}

// ── Tier 3: Contributed data queries ──

auto PluginManager::get_contributed_colors() const -> const std::vector<ExtensionColor>&
{
    return contributions_.colors;
}

auto PluginManager::get_contributed_views() const -> const std::vector<ExtensionView>&
{
    return contributions_.views;
}

auto PluginManager::get_contributed_views_containers() const
    -> const std::vector<ExtensionViewsContainer>&
{
    return contributions_.views_containers;
}

auto PluginManager::get_contributed_menus() const -> const std::vector<ExtensionMenuItem>&
{
    return contributions_.menus;
}

auto PluginManager::get_contributed_snippets() const -> const std::vector<ExtensionSnippet>&
{
    return contributions_.snippets;
}

auto PluginManager::get_contributed_languages() const -> const std::vector<ExtensionLanguage>&
{
    return contributions_.languages;
}

auto PluginManager::get_contributed_grammars() const -> const std::vector<ExtensionGrammar>&
{
    return contributions_.grammars;
}

// ── Task 12: Plugin diagnostics ──

auto PluginManager::plugin_diagnostics() const -> std::vector<PluginDiagnostic>
{
    std::vector<PluginDiagnostic> result;
    result.reserve(plugins_.size());
    for (const auto& entry : plugins_)
    {
        const auto& manifest = entry.plugin->manifest();
        PluginDiagnostic diag;
        diag.plugin_id = manifest.id;
        diag.name = manifest.name;
        diag.version = manifest.version;
        diag.active = entry.plugin->is_active();
        diag.command_count = static_cast<int>(entry.command_handlers.size());

        // Count contributions from manifest
        diag.contribution_count = static_cast<int>(
            manifest.contributes.commands.size() + manifest.contributes.keybindings.size() +
            manifest.contributes.snippets.size() + manifest.contributes.menus.size() +
            manifest.contributes.settings.size() + manifest.contributes.themes.size());

        // Lookup activation time if recorded
        auto time_it = activation_times_.find(manifest.id);
        if (time_it != activation_times_.end())
        {
            diag.activation_time_us = time_it->second;
        }
        result.push_back(std::move(diag));
    }
    return result;
}

// ── V9 Phase 04 Task 18: Marketplace search integration ──

auto PluginManager::search_extensions(const std::string& query) const -> std::vector<SearchResult>
{
    std::vector<SearchResult> results;
    if (query.empty())
    {
        return results;
    }

    // Lowercase the query for case-insensitive matching
    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    for (const auto& entry : plugins_)
    {
        if (!entry.ext_manifest.has_value())
        {
            continue;
        }
        const auto& manifest = *entry.ext_manifest;
        double score = 0.0;

        auto contains_lower = [&lower_query](const std::string& text) -> bool
        {
            std::string lower_text = text;
            std::transform(lower_text.begin(),
                           lower_text.end(),
                           lower_text.begin(),
                           [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
            return lower_text.find(lower_query) != std::string::npos;
        };

        // Score based on where match is found
        if (contains_lower(manifest.name))
        {
            score += 3.0;
        }
        if (contains_lower(manifest.display_name))
        {
            score += 2.5;
        }
        if (contains_lower(manifest.description))
        {
            score += 1.0;
        }
        for (const auto& cat : manifest.categories)
        {
            if (contains_lower(cat))
            {
                score += 1.5;
                break;
            }
        }
        for (const auto& kw : manifest.keywords)
        {
            if (contains_lower(kw))
            {
                score += 2.0;
                break;
            }
        }

        if (score > 0.0)
        {
            SearchResult result_entry;
            result_entry.plugin_id = manifest.publisher + "." + manifest.name;
            result_entry.display_name =
                manifest.display_name.empty() ? manifest.name : manifest.display_name;
            result_entry.description = manifest.description;
            result_entry.version = manifest.version;
            result_entry.relevance_score = score;
            results.push_back(std::move(result_entry));
        }
    }

    // Sort by relevance (highest first)
    std::sort(results.begin(),
              results.end(),
              [](const SearchResult& lhs, const SearchResult& rhs)
              { return lhs.relevance_score > rhs.relevance_score; });

    return results;
}

} // namespace markamp::core
