// ============================================================================
// File: src/core/AIProviderConfig.cpp
// Phase 26: AI Integration — AI provider configuration implementation
// ============================================================================

#include "AIProviderConfig.h"

#include "AIService.h"
#include "Config.h"
#include "Events.h"

namespace markamp::core
{

AIProviderConfig::AIProviderConfig(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
    // Initialize default configs for all providers.
    provider_configs_[static_cast<int>(AIProvider::OpenAI)] = default_config(AIProvider::OpenAI);
    provider_configs_[static_cast<int>(AIProvider::Anthropic)] =
        default_config(AIProvider::Anthropic);
    provider_configs_[static_cast<int>(AIProvider::Local)] = default_config(AIProvider::Local);
    provider_configs_[static_cast<int>(AIProvider::Custom)] = default_config(AIProvider::Custom);

    // Load saved provider from config if available.
    const auto saved_provider = config_.get_string("ai.active_provider", "openai");
    if (saved_provider == "anthropic")
    {
        active_provider_ = AIProvider::Anthropic;
    }
    else if (saved_provider == "local")
    {
        active_provider_ = AIProvider::Local;
    }
    else if (saved_provider == "custom")
    {
        active_provider_ = AIProvider::Custom;
    }

    // Load saved API keys.
    const auto openai_key = config_.get_string("ai.openai.api_key", "");
    if (!openai_key.empty())
    {
        provider_configs_[static_cast<int>(AIProvider::OpenAI)].api_key = openai_key;
    }
    const auto anthropic_key = config_.get_string("ai.anthropic.api_key", "");
    if (!anthropic_key.empty())
    {
        provider_configs_[static_cast<int>(AIProvider::Anthropic)].api_key = anthropic_key;
    }
}

auto AIProviderConfig::configure(AIProvider provider,
                                 const std::string& model,
                                 const std::string& api_key,
                                 const std::string& endpoint) -> void
{
    auto& provider_config = provider_configs_[static_cast<int>(provider)];
    provider_config.provider = provider;
    provider_config.model_name = model;
    provider_config.api_key = api_key;
    if (!endpoint.empty())
    {
        provider_config.api_base_url = endpoint;
    }

    // (#8) Persist API key to config storage.
    std::string provider_key;
    switch (provider)
    {
        case AIProvider::OpenAI:
            provider_key = "openai";
            break;
        case AIProvider::Anthropic:
            provider_key = "anthropic";
            break;
        case AIProvider::Local:
            provider_key = "local";
            break;
        case AIProvider::Custom:
            provider_key = "custom";
            break;
    }
    if (!api_key.empty())
    {
        config_.set("ai." + provider_key + ".api_key", api_key);
    }
    if (!model.empty())
    {
        config_.set("ai." + provider_key + ".model", model);
    }
    if (!endpoint.empty())
    {
        config_.set("ai." + provider_key + ".endpoint", endpoint);
    }

    // Publish configuration event.
    events::AIProviderConfiguredEvent evt;
    evt.provider = static_cast<int>(provider);
    evt.model = model;
    event_bus_.publish(evt);
}

auto AIProviderConfig::get_config(AIProvider provider) const
    -> std::expected<AIModelConfig, std::string>
{
    const auto iter = provider_configs_.find(static_cast<int>(provider));
    if (iter == provider_configs_.end())
    {
        return std::unexpected("Provider not configured");
    }
    return iter->second;
}

auto AIProviderConfig::set_active_provider(AIProvider provider) -> void
{
    active_provider_ = provider;

    // (#10) Persist the active provider choice.
    std::string provider_name;
    switch (provider)
    {
        case AIProvider::OpenAI:
            provider_name = "openai";
            break;
        case AIProvider::Anthropic:
            provider_name = "anthropic";
            break;
        case AIProvider::Local:
            provider_name = "local";
            break;
        case AIProvider::Custom:
            provider_name = "custom";
            break;
    }
    config_.set("ai.active_provider", provider_name);
}

auto AIProviderConfig::active_provider() const -> AIProvider
{
    return active_provider_;
}

auto AIProviderConfig::active_config() const -> AIModelConfig
{
    const auto iter = provider_configs_.find(static_cast<int>(active_provider_));
    if (iter != provider_configs_.end())
    {
        return iter->second;
    }
    return default_config(active_provider_);
}

auto AIProviderConfig::is_configured(AIProvider provider) const -> bool
{
    const auto iter = provider_configs_.find(static_cast<int>(provider));
    if (iter == provider_configs_.end())
    {
        return false;
    }
    // Local models don't need an API key.
    if (provider == AIProvider::Local)
    {
        return true;
    }
    return !iter->second.api_key.empty();
}

auto AIProviderConfig::status() const -> AIProviderStatus
{
    if (!is_configured(active_provider_))
    {
        return AIProviderStatus::kNoApiKey;
    }
    return AIProviderStatus::kConnected;
}

auto AIProviderConfig::test_connection(AIService& ai_service) const
    -> std::expected<void, std::string>
{
    return ai_service.test_connection(active_config());
}

auto AIProviderConfig::detect_ollama() const -> bool
{
    // Check if local Ollama is configured or default endpoint is set.
    const auto iter = provider_configs_.find(static_cast<int>(AIProvider::Local));
    if (iter != provider_configs_.end())
    {
        return !iter->second.api_base_url.empty();
    }
    return false;
}

auto AIProviderConfig::available_models(AIProvider provider) const -> std::vector<std::string>
{
    switch (provider)
    {
        case AIProvider::OpenAI:
            return {"gpt-4", "gpt-4-turbo", "gpt-4o", "gpt-3.5-turbo"};
        case AIProvider::Anthropic:
            return {"claude-3-opus", "claude-3-sonnet", "claude-3-haiku", "claude-3.5-sonnet"};
        case AIProvider::Local:
            return {"llama3", "mistral", "codellama", "phi-3"};
        case AIProvider::Custom:
            return {"custom-model"};
    }
    return {};
}

auto AIProviderConfig::configured_providers() const -> std::vector<AIProvider>
{
    std::vector<AIProvider> result;
    for (const auto& [key, provider_config] : provider_configs_)
    {
        const auto provider = static_cast<AIProvider>(key);
        if (is_configured(provider))
        {
            result.push_back(provider);
        }
    }
    return result;
}

auto AIProviderConfig::default_config(AIProvider provider) -> AIModelConfig
{
    AIModelConfig model_config;
    model_config.provider = provider;
    switch (provider)
    {
        case AIProvider::OpenAI:
            model_config.model_name = "gpt-4";
            model_config.api_base_url = "https://api.openai.com/v1";
            model_config.request_timeout_ms = 60000;   // (#9) 60s timeout
            model_config.max_retries = 3;               // (#9) 3 retries
            model_config.retry_base_delay_ms = 1000;    // (#9) 1s base delay
            break;
        case AIProvider::Anthropic:
            model_config.model_name = "claude-3-sonnet";
            model_config.api_base_url = "https://api.anthropic.com/v1";
            model_config.request_timeout_ms = 90000;   // (#9) 90s for longer context
            model_config.max_retries = 2;
            model_config.retry_base_delay_ms = 1500;
            break;
        case AIProvider::Local:
            model_config.model_name = "llama3";
            model_config.api_base_url = "http://localhost:11434";
            model_config.request_timeout_ms = 120000;  // (#9) 120s for local models
            model_config.max_retries = 1;              // Local: retry rarely helps
            model_config.retry_base_delay_ms = 500;
            break;
        case AIProvider::Custom:
            model_config.model_name = "custom-model";
            model_config.request_timeout_ms = 60000;
            model_config.max_retries = 2;
            model_config.retry_base_delay_ms = 1000;
            break;
    }
    return model_config;
}

// ── Batch 34 (#199-200) ─────────────────────────────────────────────────────

/// (#199) Return the number of configured providers (those with API keys or Local).
auto AIProviderConfig::provider_count() const -> std::size_t
{
    return configured_providers().size();
}

/// (#200) Check if the given provider has an API key set.
auto AIProviderConfig::has_api_key(AIProvider provider) const -> bool
{
    const auto iter = provider_configs_.find(static_cast<int>(provider));
    if (iter == provider_configs_.end())
    {
        return false;
    }
    return !iter->second.api_key.empty();
}

} // namespace markamp::core
