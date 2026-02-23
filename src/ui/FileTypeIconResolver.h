#pragma once

#include <string>

namespace markamp::ui
{

/// Resolves a file name or path to the corresponding SVG icon name in the IconRegistry.
class FileTypeIconResolver
{
public:
    /// Returns the name of the icon for the given filename (e.g., "filetype-cpp").
    static auto GetFileIcon(const std::string& filename) -> std::string;

    /// Returns the folder icon name, optionally for an open folder state.
    static auto GetFolderIcon(bool is_open = false) -> std::string;
};

} // namespace markamp::ui
