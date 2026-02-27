#pragma once

#include "core/FileNode.h"

#include <wx/colour.h>

#include <optional>
#include <string>

namespace markamp::ui
{

struct FileTreeDecoration
{
    std::optional<std::string> badge_text;
    std::optional<wxColour> badge_color;
    std::optional<wxColour> text_color;
    std::optional<std::string> tooltip;
    int priority{0}; // Higher priority overrides lower priority decorations
};

class IFileTreeDecorationProvider
{
public:
    virtual ~IFileTreeDecorationProvider() = default;

    /// Called to get decorations for a specific file/folder node
    [[nodiscard]] virtual auto ProvideDecoration(const core::FileNode& node) const
        -> std::optional<FileTreeDecoration> = 0;
};

} // namespace markamp::ui
