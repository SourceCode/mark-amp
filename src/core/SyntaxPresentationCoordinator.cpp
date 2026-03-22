#include "SyntaxPresentationCoordinator.h"

namespace markamp::core
{

auto SyntaxPresentationCoordinator::token_weight(TokenPriority priority) const -> TokenWeightConfig
{
    TokenWeightConfig config;
    config.priority = priority;

    switch (priority)
    {
        case TokenPriority::kKeyword:
            config.is_bold = true;
            config.opacity = 1.0F;
            break;
        case TokenPriority::kType:
            config.is_bold = false;
            config.opacity = 1.0F;
            break;
        case TokenPriority::kFunction:
            config.is_bold = false;
            config.opacity = 1.0F;
            break;
        case TokenPriority::kVariable:
            config.is_bold = false;
            config.opacity = 0.95F;
            break;
        case TokenPriority::kOperator:
            config.is_bold = false;
            config.opacity = 0.85F;
            break;
        case TokenPriority::kLiteral:
            config.is_bold = false;
            config.opacity = 1.0F;
            break;
        case TokenPriority::kString:
            config.is_bold = false;
            config.opacity = 1.0F;
            break;
        case TokenPriority::kComment:
            config.is_italic = true;
            config.opacity = 0.65F;
            break;
    }

    return config;
}

} // namespace markamp::core
