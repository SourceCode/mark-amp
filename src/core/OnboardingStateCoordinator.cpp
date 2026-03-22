#include "OnboardingStateCoordinator.h"

namespace markamp::core
{

auto OnboardingStateCoordinator::empty_message_for_category(EmptyStateCategory cat) const
    -> std::string
{
    switch (cat)
    {
        case EmptyStateCategory::kExplorer:
            return "No folder is open. Open a folder to get started.";
        case EmptyStateCategory::kSearch:
            return "Type to search across your workspace.";
        case EmptyStateCategory::kCanvas:
            return "This board is empty. Use the tools to start creating.";
        case EmptyStateCategory::kNotebook:
            return "Add a cell to begin writing.";
        case EmptyStateCategory::kTerminal:
            return "No terminal is open. Create a new terminal.";
        case EmptyStateCategory::kGeneric:
            return "Nothing to show.";
    }
    return "Nothing to show.";
}

} // namespace markamp::core
