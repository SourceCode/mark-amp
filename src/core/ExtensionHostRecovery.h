#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Per-extension error info used for crash isolation (#39).
struct ExtensionError
{
    std::string extension_id;
    std::string error_message;
    std::chrono::steady_clock::time_point timestamp;
    int error_count{1};
};

/// Extension host recovery service — crash isolation per extension (#39).
/// Wraps extension activate() and command handler calls in try-catch.
/// Records errors and auto-disables faulty extensions after a threshold.
class ExtensionHostRecovery
{
public:
    explicit ExtensionHostRecovery(int max_errors_before_disable = 3);

    /// Execute a function in a crash-isolated context.
    /// Returns true if the function succeeded, false if it threw.
    auto execute_safely(const std::string& extension_id, const std::function<void()>& action)
        -> bool;

    /// Get the error history for an extension.
    [[nodiscard]] auto get_errors(const std::string& extension_id) const
        -> const std::vector<ExtensionError>&;

    /// Check if an extension has been auto-disabled.
    [[nodiscard]] auto is_disabled(const std::string& extension_id) const -> bool;

    /// Get all disabled extension IDs.
    [[nodiscard]] auto disabled_extensions() const -> std::vector<std::string>;

    /// Reset error count and re-enable an extension.
    void reset_extension(const std::string& extension_id);

    /// Clear all error history.
    void clear_all();

    /// Listener for when an extension is auto-disabled.
    using DisableListener =
        std::function<void(const std::string& extension_id, const std::string& reason)>;
    auto on_extension_disabled(DisableListener listener) -> std::size_t;

private:
    int max_errors_;
    std::unordered_map<std::string, std::vector<ExtensionError>> error_history_;
    std::unordered_map<std::string, bool> disabled_;
    static const std::vector<ExtensionError> kEmptyErrors;

    std::vector<std::pair<std::size_t, DisableListener>> disable_listeners_;
    std::size_t next_listener_id_{0};
};

} // namespace markamp::core
