#include "ShellChromeCoordinator.h"

namespace markamp::core
{

void ShellChromeCoordinator::rebalance_proportions(RebalanceMode mode)
{
    switch (mode)
    {
        case RebalanceMode::kDefault:
            proportions_ = {260, 260, 200, 48, 22};
            break;
        case RebalanceMode::kCompact:
            proportions_ = {200, 200, 160, 40, 22};
            break;
        case RebalanceMode::kCustom:
            // Keep existing proportions (user-customized)
            break;
    }
}

void ShellChromeCoordinator::update_responsiveness(int window_width)
{
    if (window_width >= kWideThreshold)
    {
        responsiveness_mode_ = ResponsivenessMode::kWide;
    }
    else if (window_width >= kCompactThreshold)
    {
        responsiveness_mode_ = ResponsivenessMode::kDefault;
    }
    else
    {
        responsiveness_mode_ = ResponsivenessMode::kCompact;
    }
}

} // namespace markamp::core
