#include "LanguageProviderRegistry.h"

namespace markamp::core
{

// ── Completion ──

void LanguageProviderRegistry::register_completion_provider(
    const std::string& language_id, std::shared_ptr<ICompletionProvider> provider)
{
    completion_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_completion_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<ICompletionProvider>>
{
    auto found = completion_providers_.find(language_id);
    return found != completion_providers_.end()
               ? found->second
               : std::vector<std::shared_ptr<ICompletionProvider>>{};
}

// ── Hover ──

void LanguageProviderRegistry::register_hover_provider(const std::string& language_id,
                                                       std::shared_ptr<IHoverProvider> provider)
{
    hover_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_hover_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<IHoverProvider>>
{
    auto found = hover_providers_.find(language_id);
    return found != hover_providers_.end() ? found->second
                                           : std::vector<std::shared_ptr<IHoverProvider>>{};
}

// ── Code Action ──

void LanguageProviderRegistry::register_code_action_provider(
    const std::string& language_id, std::shared_ptr<ICodeActionProvider> provider)
{
    code_action_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_code_action_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<ICodeActionProvider>>
{
    auto found = code_action_providers_.find(language_id);
    return found != code_action_providers_.end()
               ? found->second
               : std::vector<std::shared_ptr<ICodeActionProvider>>{};
}

// ── Document Formatting ──

void LanguageProviderRegistry::register_formatting_provider(
    const std::string& language_id, std::shared_ptr<IDocumentFormattingProvider> provider)
{
    formatting_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_formatting_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<IDocumentFormattingProvider>>
{
    auto found = formatting_providers_.find(language_id);
    return found != formatting_providers_.end()
               ? found->second
               : std::vector<std::shared_ptr<IDocumentFormattingProvider>>{};
}

// ── Utility ──

auto LanguageProviderRegistry::registered_languages() const -> std::vector<std::string>
{
    std::unordered_map<std::string, bool> seen;
    auto add_langs = [&seen](const auto& map)
    {
        for (const auto& [lang, providers] : map)
        {
            seen[lang] = true;
        }
    };

    add_langs(completion_providers_);
    add_langs(hover_providers_);
    add_langs(code_action_providers_);
    add_langs(formatting_providers_);

    std::vector<std::string> langs;
    langs.reserve(seen.size());
    for (const auto& [lang, unused] : seen)
    {
        langs.push_back(lang);
    }
    return langs;
}

} // namespace markamp::core
