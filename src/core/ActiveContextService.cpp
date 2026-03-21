/// @file ActiveContextService.cpp
/// @brief V20 P06-T02/T04: Active context implementation.

#include "ActiveContextService.h"

#include "Logger.h"

namespace markamp::core
{

void ActiveContextService::set_context(const ActiveContext& context)
{
    ++update_count_;
    context_ = context;

    // Auto-populate context keys from active state
    set_context_key("activeArtifactId", context.active_artifact.value);
    set_context_key("activeSurface",
                    context.is_editor()   ? "editor"
                    : context.is_canvas() ? "canvas"
                    : context.is_notebook() ? "notebook"
                                            : "none");
    if (context.active_language.has_value())
    {
        set_context_key("activeLanguage", *context.active_language);
    }
    set_context_key("isEditing", context.is_editing ? "true" : "false");

    MARKAMP_LOG_DEBUG("Context updated: artifact={}, surface={}, focus={}",
                      context_.active_artifact.value,
                      static_cast<int>(context_.active_surface), context_.focus_target);
}

void ActiveContextService::set_context_key(const std::string& key, const std::string& value)
{
    context_keys_[key] = value;
}

auto ActiveContextService::get_context_key(const std::string& key) const
    -> std::optional<std::string>
{
    auto it = context_keys_.find(key);
    if (it == context_keys_.end())
    {
        return std::nullopt;
    }
    return it->second;
}

void ActiveContextService::clear_context_key(const std::string& key)
{
    context_keys_.erase(key);
}

auto ActiveContextService::all_context_keys() const -> std::vector<ContextKey>
{
    std::vector<ContextKey> keys;
    keys.reserve(context_keys_.size());
    for (const auto& [k, v] : context_keys_)
    {
        keys.push_back({k, v});
    }
    return keys;
}

auto ActiveContextService::evaluate_when(const std::string& when_clause) const -> bool
{
    if (when_clause.empty())
    {
        return true; // Empty clause is always true
    }

    // Simple "key == value" evaluation
    auto eq_pos = when_clause.find("==");
    if (eq_pos != std::string::npos)
    {
        auto key = when_clause.substr(0, eq_pos);
        auto value = when_clause.substr(eq_pos + 2);

        // Trim whitespace
        while (!key.empty() && key.back() == ' ')
            key.pop_back();
        while (!value.empty() && value.front() == ' ')
            value.erase(value.begin());

        auto actual = get_context_key(key);
        return actual.has_value() && *actual == value;
    }

    // Simple boolean key check
    auto val = get_context_key(when_clause);
    return val.has_value() && *val == "true";
}

} // namespace markamp::core
