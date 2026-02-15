#include "AVRollupConfigDialog.h"

namespace markamp::ui::av
{

void AVRollupConfigDialog::set_state(AVRollupConfigState new_state)
{
    state_ = std::move(new_state);
}

} // namespace markamp::ui::av
