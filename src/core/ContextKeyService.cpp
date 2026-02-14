#include "ContextKeyService.h"

namespace markamp::core
{

void ContextKeyService::set_context(const std::string& key, ContextKeyValue value)
{
    scopes_.back()[key] = std::move(value);
    fire_change(key);
}

auto ContextKeyService::get_context(const std::string& key) const -> const ContextKeyValue*
{
    // Search from current (deepest) scope up to global
    for (auto idx = scopes_.size(); idx > 0; --idx)
    {
        const auto& scope = scopes_[idx - 1];
        auto found = scope.find(key);
        if (found != scope.end())
        {
            return &found->second;
        }
    }
    return nullptr;
}

auto ContextKeyService::has_context(const std::string& key) const -> bool
{
    return get_context(key) != nullptr;
}

void ContextKeyService::remove_context(const std::string& key)
{
    if (!scopes_.empty())
    {
        scopes_.back().erase(key);
        fire_change(key);
    }
}

void ContextKeyService::push_scope()
{
    scopes_.emplace_back();
}

void ContextKeyService::pop_scope()
{
    if (scopes_.size() > 1)
    {
        scopes_.pop_back();
    }
}

auto ContextKeyService::scope_depth() const -> std::size_t
{
    return scopes_.size() - 1; // 0 = global only
}

auto ContextKeyService::get_bool(const std::string& key) const -> bool
{
    const auto* val = get_context(key);
    if (val == nullptr)
    {
        return false;
    }
    if (const auto* bool_val = std::get_if<bool>(val))
    {
        return *bool_val;
    }
    return false;
}

auto ContextKeyService::get_string(const std::string& key) const -> std::string
{
    const auto* val = get_context(key);
    if (val == nullptr)
    {
        return {};
    }
    if (const auto* str_val = std::get_if<std::string>(val))
    {
        return *str_val;
    }
    return {};
}

auto ContextKeyService::get_int(const std::string& key) const -> int
{
    const auto* val = get_context(key);
    if (val == nullptr)
    {
        return 0;
    }
    if (const auto* int_val = std::get_if<int>(val))
    {
        return *int_val;
    }
    return 0;
}

auto ContextKeyService::is_truthy(const std::string& key) const -> bool
{
    const auto* val = get_context(key);
    if (val == nullptr)
    {
        return false;
    }

    return std::visit(
        [](const auto& visited_value) -> bool
        {
            using ValueType = std::decay_t<decltype(visited_value)>;
            if constexpr (std::is_same_v<ValueType, bool>)
            {
                return visited_value;
            }
            else if constexpr (std::is_same_v<ValueType, int>)
            {
                return visited_value != 0;
            }
            else if constexpr (std::is_same_v<ValueType, double>)
            {
                return visited_value != 0.0;
            }
            else if constexpr (std::is_same_v<ValueType, std::string>)
            {
                return !visited_value.empty();
            }
            else
            {
                return false;
            }
        },
        *val);
}

auto ContextKeyService::on_did_change(ChangeListener listener) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.emplace_back(listener_id, std::move(listener));
    return listener_id;
}

void ContextKeyService::remove_change_listener(std::size_t listener_id)
{
    listeners_.erase(std::remove_if(listeners_.begin(),
                                    listeners_.end(),
                                    [listener_id](const auto& pair)
                                    { return pair.first == listener_id; }),
                     listeners_.end());
}

void ContextKeyService::fire_change(const std::string& key)
{
    for (const auto& [id, listener] : listeners_)
    {
        listener(key);
    }
}

} // namespace markamp::core
