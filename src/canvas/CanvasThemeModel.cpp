#include "CanvasThemeModel.h"

#include <algorithm>

namespace markamp::canvas
{

void CanvasThemeModel::set_tokens(std::vector<CanvasToken> tokens)
{
    tokens_ = std::move(tokens);
}
auto CanvasThemeModel::tokens() const -> const std::vector<CanvasToken>&
{
    return tokens_;
}

auto CanvasThemeModel::resolve(const std::string& token_name) const -> std::string
{
    for (const auto& token : tokens_)
    {
        if (token.token_name == token_name)
        {
            return token.value.empty() ? token.fallback : token.value;
        }
    }
    return ""; // missing token
}

auto CanvasThemeModel::missing_tokens() const -> std::vector<std::string>
{
    std::vector<std::string> missing;
    for (const auto& token : tokens_)
    {
        if (token.value.empty() && token.fallback.empty())
        {
            missing.push_back(token.token_name);
        }
    }
    return missing;
}

void CanvasThemeModel::set_background(BoardBackground preset)
{
    background_ = preset;
}
auto CanvasThemeModel::background() const -> BoardBackground
{
    return background_;
}

void CanvasThemeModel::set_min_contrast(double ratio)
{
    min_contrast_ = std::max(1.0, ratio);
}
auto CanvasThemeModel::min_contrast() const -> double
{
    return min_contrast_;
}
auto CanvasThemeModel::passes_contrast(double ratio) const -> bool
{
    return ratio >= min_contrast_;
}

} // namespace markamp::canvas
