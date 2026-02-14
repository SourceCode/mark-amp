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

// ── Definition ──

void LanguageProviderRegistry::register_definition_provider(
    const std::string& language_id, std::shared_ptr<IDefinitionProvider> provider)
{
    definition_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_definition_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<IDefinitionProvider>>
{
    auto found = definition_providers_.find(language_id);
    return found != definition_providers_.end()
               ? found->second
               : std::vector<std::shared_ptr<IDefinitionProvider>>{};
}

// ── Reference ──

void LanguageProviderRegistry::register_reference_provider(
    const std::string& language_id, std::shared_ptr<IReferenceProvider> provider)
{
    reference_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_reference_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<IReferenceProvider>>
{
    auto found = reference_providers_.find(language_id);
    return found != reference_providers_.end() ? found->second
                                               : std::vector<std::shared_ptr<IReferenceProvider>>{};
}

// ── Document Symbol ──

void LanguageProviderRegistry::register_document_symbol_provider(
    const std::string& language_id, std::shared_ptr<IDocumentSymbolProvider> provider)
{
    document_symbol_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_document_symbol_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<IDocumentSymbolProvider>>
{
    auto found = document_symbol_providers_.find(language_id);
    return found != document_symbol_providers_.end()
               ? found->second
               : std::vector<std::shared_ptr<IDocumentSymbolProvider>>{};
}

// ── Rename ──

void LanguageProviderRegistry::register_rename_provider(const std::string& language_id,
                                                        std::shared_ptr<IRenameProvider> provider)
{
    rename_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_rename_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<IRenameProvider>>
{
    auto found = rename_providers_.find(language_id);
    return found != rename_providers_.end() ? found->second
                                            : std::vector<std::shared_ptr<IRenameProvider>>{};
}

// ── Folding Range ──

void LanguageProviderRegistry::register_folding_range_provider(
    const std::string& language_id, std::shared_ptr<IFoldingRangeProvider> provider)
{
    folding_range_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_folding_range_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<IFoldingRangeProvider>>
{
    auto found = folding_range_providers_.find(language_id);
    return found != folding_range_providers_.end()
               ? found->second
               : std::vector<std::shared_ptr<IFoldingRangeProvider>>{};
}

// ── Signature Help ──

void LanguageProviderRegistry::register_signature_help_provider(
    const std::string& language_id, std::shared_ptr<ISignatureHelpProvider> provider)
{
    signature_help_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_signature_help_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<ISignatureHelpProvider>>
{
    auto found = signature_help_providers_.find(language_id);
    return found != signature_help_providers_.end()
               ? found->second
               : std::vector<std::shared_ptr<ISignatureHelpProvider>>{};
}

// ── Inlay Hints ──

void LanguageProviderRegistry::register_inlay_hints_provider(
    const std::string& language_id, std::shared_ptr<IInlayHintsProvider> provider)
{
    inlay_hints_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_inlay_hints_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<IInlayHintsProvider>>
{
    auto found = inlay_hints_providers_.find(language_id);
    return found != inlay_hints_providers_.end()
               ? found->second
               : std::vector<std::shared_ptr<IInlayHintsProvider>>{};
}

// ── Document Link ──

void LanguageProviderRegistry::register_document_link_provider(
    const std::string& language_id, std::shared_ptr<IDocumentLinkProvider> provider)
{
    document_link_providers_[language_id].push_back(std::move(provider));
}

auto LanguageProviderRegistry::get_document_link_providers(const std::string& language_id) const
    -> std::vector<std::shared_ptr<IDocumentLinkProvider>>
{
    auto found = document_link_providers_.find(language_id);
    return found != document_link_providers_.end()
               ? found->second
               : std::vector<std::shared_ptr<IDocumentLinkProvider>>{};
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
    add_langs(definition_providers_);
    add_langs(reference_providers_);
    add_langs(document_symbol_providers_);
    add_langs(rename_providers_);
    add_langs(folding_range_providers_);
    add_langs(signature_help_providers_);
    add_langs(inlay_hints_providers_);
    add_langs(document_link_providers_);

    std::vector<std::string> langs;
    langs.reserve(seen.size());
    for (const auto& [lang, unused] : seen)
    {
        langs.push_back(lang);
    }
    return langs;
}

} // namespace markamp::core
