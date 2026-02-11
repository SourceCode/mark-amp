#pragma once

#include "FileNode.h"

namespace markamp::core
{

/// Returns the default sample file tree matching the MarkAmp reference project.
[[nodiscard]] auto get_sample_file_tree() -> FileNode;

} // namespace markamp::core
