/// @file ActionManifestBootstrap.h
/// @brief V25 P04: Populates ActionManifest with release-path actions.
#pragma once

#include "ActionManifest.h"

namespace markamp::core
{

/// Populates ActionManifest with all release-path actions.
void bootstrap_release_actions(ActionManifest& manifest);

/// Total number of release-path actions registered.
[[nodiscard]] auto release_action_count() noexcept -> int;

} // namespace markamp::core
