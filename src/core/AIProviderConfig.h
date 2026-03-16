// ============================================================================
// File: src/core/AIProviderConfig.h
// Phase 26: AI Integration — AI provider configuration management
// ============================================================================
#pragma once

#include "AITypes.h"

#include <expected>
#include <string>
#include <unordered_map>

namespace markamp::core
{

class EventBus;
class Config;
class AIService;

/// Manages AI provider configuration: API keys, models, endpoints.
/// Supports OpenAI, Anthropic, and local (Ollama) providers.
class AIProviderConfig
{
public:
    AIProviderConfig(EventBus& event_bus, Config& config);

    /// Configure a specific provider with model, API key, and endpoint.
    auto configure(AIProvider provider,
                   const std::string& model,
                   const std::string& api_key,
                   const std::string& endpoint = "") -> void;

    /// Get the configuration for a specific provider.
    [[nodiscard]] auto get_config(AIProvider provider) const
        -> std::expected<AIModelConfig, std::string>;

    /// Set the active provider.
    auto set_active_provider(AIProvider provider) -> void;

    /// Get the active provider.
    [[nodiscard]] auto active_provider() const -> AIProvider;

    /// Get the active provider's full model config.
    [[nodiscard]] auto active_config() const -> AIModelConfig;

    /// Check if a provider is configured (has API key or is local).
    [[nodiscard]] auto is_configured(AIProvider provider) const -> bool;

    /// Get current provider status.
    [[nodiscard]] auto status() const -> AIProviderStatus;

    /// Test connection to a specific provider.
    [[nodiscard]] auto test_connection(AIService& ai_service) const
        -> std::expected<void, std::string>;

    /// Check for local Ollama instance at localhost:11434.
    [[nodiscard]] auto detect_ollama() const -> bool;

    /// Get available models for a provider.
    [[nodiscard]] auto available_models(AIProvider provider) const -> std::vector<std::string>;

    /// Get all configured providers.
    [[nodiscard]] auto configured_providers() const -> std::vector<AIProvider>;

    /// (#199) Return the number of configured providers.
    [[nodiscard]] auto provider_count() const -> std::size_t;

    /// (#200) Check if the given provider has an API key set.
    [[nodiscard]] auto has_api_key(AIProvider provider) const -> bool;

private:
    EventBus& event_bus_;
    Config& config_;
    AIProvider active_provider_{AIProvider::OpenAI};
    std::unordered_map<int, AIModelConfig> provider_configs_;

    /// Apply provider-specific defaults (model name, base URL).
    static auto default_config(AIProvider provider) -> AIModelConfig;
};

} // namespace markamp::core
