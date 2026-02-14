#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace markamp::core
{

/// Value type stored in context keys.
/// Supports bool, int, double, and string values.
using ContextKeyValue = std::variant<bool, int, double, std::string>;

/// Hierarchical context-key service.
/// Keys are strings; values are ContextKeyValue variants.
/// Supports scoped layers (global → window → editor) via push/pop.
class ContextKeyService
{
public:
    ContextKeyService() = default;

    /// Set a context key at the current scope.
    void set_context(const std::string& key, ContextKeyValue value);

    /// Get a context key value, searching from current scope up to global.
    /// Returns nullptr if not found.
    [[nodiscard]] auto get_context(const std::string& key) const -> const ContextKeyValue*;

    /// Check if a key exists in any scope.
    [[nodiscard]] auto has_context(const std::string& key) const -> bool;

    /// Remove a key from the current scope.
    void remove_context(const std::string& key);

    /// Push a new scope (e.g. when focusing a specific editor).
    void push_scope();

    /// Pop the current scope, restoring the parent scope.
    void pop_scope();

    /// Get the current scope depth (0 = global only).
    [[nodiscard]] auto scope_depth() const -> std::size_t;

    /// Get a context key value as bool (false if not found or not bool).
    [[nodiscard]] auto get_bool(const std::string& key) const -> bool;

    /// Get a context key value as string (empty if not found or not string).
    [[nodiscard]] auto get_string(const std::string& key) const -> std::string;

    /// Get a context key value as int (0 if not found or not int).
    [[nodiscard]] auto get_int(const std::string& key) const -> int;

    /// Evaluate whether a context key is "truthy":
    /// - bool: value itself
    /// - int: non-zero
    /// - double: non-zero
    /// - string: non-empty
    [[nodiscard]] auto is_truthy(const std::string& key) const -> bool;

    /// Listener type for context-key changes.
    using ChangeListener = std::function<void(const std::string& key)>;

    /// Subscribe to changes. Returns a subscription ID.
    auto on_did_change(ChangeListener listener) -> std::size_t;

    /// Unsubscribe from changes.
    void remove_change_listener(std::size_t listener_id);

private:
    /// Each scope is a flat key→value map.
    using Scope = std::unordered_map<std::string, ContextKeyValue>;

    /// Stack of scopes; index 0 is the global scope.
    std::vector<Scope> scopes_{Scope{}};

    /// Change listeners.
    std::vector<std::pair<std::size_t, ChangeListener>> listeners_;
    std::size_t next_listener_id_{0};

    void fire_change(const std::string& key);
};

} // namespace markamp::core
