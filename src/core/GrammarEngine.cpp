#include "GrammarEngine.h"

namespace markamp::core
{

auto GrammarEngine::load_grammar(const std::string& /*path*/) -> bool
{
    // Stub: TextMate grammar loading not yet implemented.
    // A full implementation would parse .tmLanguage.json and build tokenization rules.
    return false;
}

auto GrammarEngine::get_grammar(const std::string& /*scope_name*/) const -> const Grammar*
{
    return nullptr;
}

auto GrammarEngine::tokenize_line(const std::string& /*scope_name*/,
                                  const std::string& /*line*/) const -> std::vector<GrammarToken>
{
    return {};
}

auto GrammarEngine::grammars() const -> const std::vector<Grammar>&
{
    return grammars_;
}

} // namespace markamp::core
