#include "ui/FileTypeIconResolver.h"

#include <algorithm>
#include <filesystem>

namespace markamp::ui
{

auto FileTypeIconResolver::GetFileIcon(const std::string& filename) -> std::string
{
    if (filename.empty())
    {
        return "filetype-default";
    }

    std::filesystem::path path(filename);
    std::string ext = path.extension().string();

    // Convert extension to lowercase for matching
    std::transform(
        ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });

    if (ext == ".md" || ext == ".markdown")
    {
        return "filetype-markdown";
    }
    else if (ext == ".cpp" || ext == ".cxx" || ext == ".cc" || ext == ".c")
    {
        return "filetype-cpp";
    }
    else if (ext == ".h" || ext == ".hpp" || ext == ".hxx")
    {
        return "filetype-header";
    }
    else if (ext == ".py" || ext == ".pyw")
    {
        return "filetype-python";
    }
    else if (ext == ".json")
    {
        return "filetype-default"; // We could add filetype-json later
    }
    else if (ext == ".txt")
    {
        return "filetype-default"; // General text
    }

    return "filetype-default";
}

auto FileTypeIconResolver::GetFolderIcon(bool is_open) -> std::string
{
    if (is_open)
    {
        return "filetype-folder-open"; // If available, else default to folder
    }
    return "filetype-folder";
}

} // namespace markamp::ui
