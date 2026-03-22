#include "UtilityPanelCoordinator.h"

namespace markamp::core
{

auto UtilityPanelCoordinator::severity_icon_size(SeverityLevel level) const -> int
{
    switch (level)
    {
        case SeverityLevel::kError:
            return 16;
        case SeverityLevel::kWarning:
            return 16;
        case SeverityLevel::kInfo:
            return 14;
        case SeverityLevel::kHint:
            return 14;
    }
    return 14;
}

} // namespace markamp::core
