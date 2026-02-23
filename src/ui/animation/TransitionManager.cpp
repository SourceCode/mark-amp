#include "TransitionManager.h"

namespace markamp::ui::animation
{

TransitionManager::TransitionManager(wxWindow* target)
    : target_(target)
{
}

void TransitionManager::register_transition(const std::string& name, const AnimationConfig& config)
{
    configs_[name] = config;
}

} // namespace markamp::ui::animation
