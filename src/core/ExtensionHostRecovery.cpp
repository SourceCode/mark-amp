#include "ExtensionHostRecovery.h"

#include <exception>

namespace markamp::core
{

const std::vector<ExtensionError> ExtensionHostRecovery::kEmptyErrors;

ExtensionHostRecovery::ExtensionHostRecovery(int max_errors_before_disable)
    : max_errors_(max_errors_before_disable)
{
}

auto ExtensionHostRecovery::execute_safely(const std::string& extension_id,
                                           const std::function<void()>& action) -> bool
{
    if (is_disabled(extension_id))
    {
        return false;
    }

    try
    {
        action();
        return true;
    }
    catch (const std::exception& ex)
    {
        ExtensionError error{
            .extension_id = extension_id,
            .error_message = ex.what(),
            .timestamp = std::chrono::steady_clock::now(),
        };
        error_history_[extension_id].push_back(std::move(error));

        // Check if we should auto-disable
        if (static_cast<int>(error_history_[extension_id].size()) >= max_errors_)
        {
            disabled_[extension_id] = true;
            const std::string reason = "Extension disabled after " + std::to_string(max_errors_) +
                                       " errors. Last: " + ex.what();
            for (const auto& [id, listener] : disable_listeners_)
            {
                listener(extension_id, reason);
            }
        }
        return false;
    }
}

auto ExtensionHostRecovery::get_errors(const std::string& extension_id) const
    -> const std::vector<ExtensionError>&
{
    auto found = error_history_.find(extension_id);
    return found != error_history_.end() ? found->second : kEmptyErrors;
}

auto ExtensionHostRecovery::is_disabled(const std::string& extension_id) const -> bool
{
    auto found = disabled_.find(extension_id);
    return found != disabled_.end() && found->second;
}

auto ExtensionHostRecovery::disabled_extensions() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& [ext_id, is_disabled] : disabled_)
    {
        if (is_disabled)
        {
            result.push_back(ext_id);
        }
    }
    return result;
}

void ExtensionHostRecovery::reset_extension(const std::string& extension_id)
{
    error_history_.erase(extension_id);
    disabled_.erase(extension_id);
}

void ExtensionHostRecovery::clear_all()
{
    error_history_.clear();
    disabled_.clear();
}

auto ExtensionHostRecovery::on_extension_disabled(DisableListener listener) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    disable_listeners_.emplace_back(listener_id, std::move(listener));
    return listener_id;
}

} // namespace markamp::core
