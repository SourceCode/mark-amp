// ============================================================================
// File: src/ui/PeekEditorHost.cpp
// Phase 47: Peek View System — Editor host model
// ============================================================================
#include "PeekEditorHost.h"

#include <algorithm>

namespace markamp::ui
{

void PeekEditorHostModel::load_content(const std::string& file_path,
                                       const std::string& content,
                                       int target_line)
{
    file_path_ = file_path;
    content_ = content;
    target_line_ = target_line;
    is_loaded_ = true;

    // Count lines.
    total_lines_ = 1;
    for (char ch : content)
    {
        if (ch == '\n')
        {
            ++total_lines_;
        }
    }

    // Center target in viewport (assume ~20 visible lines).
    scroll_to_line_ = std::max(0, target_line - 10);
}

void PeekEditorHostModel::set_visible_range(int first_line, int last_line)
{
    visible_first_ = first_line;
    visible_last_ = last_line;
}

} // namespace markamp::ui
