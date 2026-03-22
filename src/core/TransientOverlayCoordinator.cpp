#include "TransientOverlayCoordinator.h"

namespace markamp::core
{

auto TransientOverlayCoordinator::dismiss_delay_for_level(NotificationLevel level) const -> int
{
    switch (level)
    {
        case NotificationLevel::kInfo:
            return 5000;
        case NotificationLevel::kSuccess:
            return 4000;
        case NotificationLevel::kWarning:
            return 8000;
        case NotificationLevel::kError:
            return 0; // Errors don't auto-dismiss
    }
    return 5000;
}

} // namespace markamp::core
