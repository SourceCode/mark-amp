#pragma once

#include "Theme.h"

#include <vector>

namespace markamp::core
{

/// Returns all 8 built-in themes (loaded once, cached thereafter).
auto get_builtin_themes() -> const std::vector<Theme>&;

/// Returns the default theme (Midnight Neon).
auto get_default_theme() -> const Theme&;

} // namespace markamp::core
