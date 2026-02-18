/// @file WorkspaceHookManager.h
/// @brief V9 Phase 40 — Workspace lifecycle hooks with callback registration.
#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Types of workspace lifecycle events that can trigger hooks.
enum class WorkspaceHookType : uint8_t
{
    kOnOpen = 0,      ///< Workspace/vault opened
    kOnClose = 1,     ///< Workspace/vault closed
    kOnSave = 2,      ///< File saved
    kOnCreate = 3,    ///< File created
    kOnDelete = 4,    ///< File deleted
    kOnRename = 5,    ///< File renamed
    kOnFocusGain = 6, ///< Window gains focus
    kOnFocusLost = 7, ///< Window loses focus
    kOnStartup = 8,   ///< Application startup complete
    kOnShutdown = 9,  ///< Application shutting down
};

/// Convert hook type to a display name.
[[nodiscard]] auto hook_type_name(WorkspaceHookType hook_type) -> std::string;

/// Context passed to hook callbacks.
struct HookContext
{
    WorkspaceHookType hook_type{WorkspaceHookType::kOnOpen};
    std::string workspace_path; ///< Current workspace root
    std::string file_path;      ///< Affected file (if applicable)
    std::string old_path;       ///< Previous path (for rename)
    std::string metadata;       ///< Extra context
};

/// Callback type for hooks.
using HookCallback = std::function<void(const HookContext&)>;

/// A registered hook entry.
struct HookRegistration
{
    std::string hook_id;     ///< Unique identifier
    std::string description; ///< Human-readable label
    WorkspaceHookType hook_type{WorkspaceHookType::kOnOpen};
    HookCallback callback;
    int priority{0}; ///< Higher = runs first
    bool enabled{true};
};

/// Manages workspace lifecycle hooks.
///
/// Extensions and plugins can register callbacks for workspace events
/// (open, close, save, create, delete, etc). Hooks run in priority order
/// and can be enabled/disabled individually.
class WorkspaceHookManager
{
public:
    WorkspaceHookManager() = default;

    // ── Registration ──────────────────────────────────────────────────
    void register_hook(HookRegistration registration);
    auto unregister_hook(const std::string& hook_id) -> bool;
    [[nodiscard]] auto find_hook(const std::string& hook_id) const -> const HookRegistration*;
    [[nodiscard]] auto hook_count() const -> int;

    // ── State ─────────────────────────────────────────────────────────
    auto enable_hook(const std::string& hook_id) -> bool;
    auto disable_hook(const std::string& hook_id) -> bool;

    // ── Execution ─────────────────────────────────────────────────────
    /// Fire all enabled hooks of the given type, in priority order.
    void fire_hooks(WorkspaceHookType hook_type, const HookContext& context) const;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto hooks_for_type(WorkspaceHookType hook_type) const
        -> std::vector<const HookRegistration*>;
    [[nodiscard]] auto all_hooks() const -> std::vector<const HookRegistration*>;
    [[nodiscard]] auto registered_types() const -> std::vector<WorkspaceHookType>;

    void clear_all();

private:
    std::vector<HookRegistration> hooks_;
};

} // namespace markamp::core
