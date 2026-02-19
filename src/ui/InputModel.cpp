#include "InputModel.h"

namespace markamp::ui
{

auto InputModel::get() -> InputModel&
{
    static InputModel instance;
    return instance;
}

} // namespace markamp::ui
