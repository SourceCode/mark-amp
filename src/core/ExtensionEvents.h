#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Reason why a document is being saved.
enum class TextDocumentSaveReason : std::uint8_t
{
    kManual = 1,     ///< Explicitly triggered (Ctrl+S)
    kAfterDelay = 2, ///< Auto-save after a delay
    kFocusOut = 3    ///< Auto-save when focus leaves the editor
};

/// Event fired before a document is saved.
/// Mirrors VS Code's `TextDocumentWillSaveEvent`.
struct TextDocumentWillSaveEvent
{
    std::string uri;
    TextDocumentSaveReason reason{TextDocumentSaveReason::kManual};
};

/// Event fired when configuration changes.
/// Mirrors VS Code's `ConfigurationChangeEvent`.
struct ConfigurationChangeEvent
{
    std::vector<std::string> affected_sections; ///< Which config sections changed
};

/// Typed event bus for extension-specific lifecycle events.
///
/// Provides strongly-typed event subscriptions beyond the generic `EventBus`.
/// Extensions use this for document lifecycle hooks and configuration monitoring.
///
/// Mirrors VS Code's typed event pattern:
/// - `workspace.onWillSaveTextDocument`
/// - `workspace.onDidChangeConfiguration`
///
/// Usage:
/// ```cpp
/// ctx.extension_event_bus->on_will_save([](const TextDocumentWillSaveEvent& e) {
///     // Format on save, etc.
/// });
/// ```
class ExtensionEventBus
{
public:
    using WillSaveCallback = std::function<void(const TextDocumentWillSaveEvent&)>;
    using ConfigChangeCallback = std::function<void(const ConfigurationChangeEvent&)>;

    ExtensionEventBus() = default;

    // ── Subscriptions ──

    auto on_will_save(WillSaveCallback callback) -> std::size_t;
    auto on_did_change_configuration(ConfigChangeCallback callback) -> std::size_t;
    void remove_listener(std::size_t listener_id);

    // ── Event Firing (called by the app, not extensions) ──

    void fire_will_save(const TextDocumentWillSaveEvent& event);
    void fire_configuration_change(const ConfigurationChangeEvent& event);

private:
    struct ListenerEntry
    {
        std::size_t listener_id{0};
        bool is_will_save{true};
        WillSaveCallback will_save_callback;
        ConfigChangeCallback config_callback;
    };
    std::vector<ListenerEntry> listeners_;
    std::size_t next_listener_id_{0};
};

} // namespace markamp::core
