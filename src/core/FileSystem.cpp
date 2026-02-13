#include "FileSystem.h"

#include "Logger.h"

#include <wx/dirdlg.h>
#include <wx/filedlg.h>

#include <algorithm>
#include <fstream>

namespace markamp::core
{

// ── Static data ──

const std::set<std::string> FileSystem::kMarkdownExtensions = {
    ".md", ".markdown", ".mdown", ".mkd", ".txt"};

const std::set<std::string> FileSystem::kExcludedDirectories = {"node_modules",
                                                                ".git",
                                                                "build",
                                                                "__pycache__",
                                                                ".vscode",
                                                                ".idea",
                                                                "dist",
                                                                "target",
                                                                ".build",
                                                                "DerivedData"};

// ── Constructor / Destructor ──

FileSystem::FileSystem(EventBus& event_bus)
    : event_bus_(event_bus)
{
    (void)event_bus_; // Will be used for file events in later phases
}

FileSystem::~FileSystem() = default;

// ── IFileSystem interface ──

auto FileSystem::read_file(const std::filesystem::path& path)
    -> std::expected<std::string, std::string>
{
    try
    {
        if (!std::filesystem::exists(path))
        {
            return std::unexpected("File does not exist: " + path.string());
        }

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            return std::unexpected("Cannot open file: " + path.string());
        }

        auto size = file.tellg();
        if (size < 0)
        {
            return std::unexpected("Cannot determine file size: " + path.string());
        }
        file.seekg(0, std::ios::beg);
        std::string content(static_cast<size_t>(size), '\0');
        if (!file.read(content.data(), size))
        {
            return std::unexpected("Read failed: " + path.string());
        }
        return content;
    }
    catch (const std::exception& e)
    {
        return std::unexpected(std::string("Read error: ") + e.what());
    }
}

auto FileSystem::write_file(const std::filesystem::path& path, std::string_view content)
    -> std::expected<void, std::string>
{
    try
    {
        // Ensure parent directory exists
        auto parent = path.parent_path();
        if (!parent.empty() && !std::filesystem::exists(parent))
        {
            std::filesystem::create_directories(parent);
        }

        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if (!file.is_open())
        {
            return std::unexpected("Cannot open file for writing: " + path.string());
        }

        file.write(content.data(), static_cast<std::streamsize>(content.size()));
        if (!file.good())
        {
            return std::unexpected("Write error: " + path.string());
        }

        return {};
    }
    catch (const std::exception& e)
    {
        return std::unexpected(std::string("Write error: ") + e.what());
    }
}

auto FileSystem::list_directory(const std::filesystem::path& path)
    -> std::expected<std::vector<FileNode>, std::string>
{
    try
    {
        if (!std::filesystem::exists(path))
        {
            return std::unexpected("Directory does not exist: " + path.string());
        }
        if (!std::filesystem::is_directory(path))
        {
            return std::unexpected("Not a directory: " + path.string());
        }

        std::vector<FileNode> entries;
        entries.reserve(64);
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            FileNode node;
            node.name = entry.path().filename().string();
            node.id = entry.path().string();

            if (entry.is_directory())
            {
                node.type = FileNodeType::Folder;
            }
            else
            {
                node.type = FileNodeType::File;
            }

            entries.push_back(std::move(node));
        }

        // Sort: folders first, then alphabetical
        std::sort(entries.begin(),
                  entries.end(),
                  [](const FileNode& a, const FileNode& b)
                  {
                      if (a.is_folder() != b.is_folder())
                      {
                          return a.is_folder(); // folders first
                      }
                      return a.name < b.name;
                  });

        return entries;
    }
    catch (const std::exception& e)
    {
        return std::unexpected(std::string("List directory error: ") + e.what());
    }
}

auto FileSystem::watch_file(const std::filesystem::path& path, std::function<void()> callback)
    -> Subscription
{
    std::filesystem::file_time_type mtime{};
    try
    {
        if (std::filesystem::exists(path))
        {
            mtime = std::filesystem::last_write_time(path);
        }
    }
    catch (const std::exception&)
    {
        // File may not exist yet, that's fine
    }

    auto id = next_watch_id_++;
    watch_entries_.push_back({path, mtime, std::move(callback), id});

    // Return a subscription that removes the watch when destroyed
    return Subscription(
        [this, id]() {
            std::erase_if(watch_entries_, [id](const WatchEntry& entry) { return entry.id == id; });
        });
}

// ── Extended operations ──

auto FileSystem::read_file_with_encoding(const std::filesystem::path& path)
    -> std::expected<std::pair<std::string, DetectedEncoding>, std::string>
{
    auto result = read_file(path);
    if (!result.has_value())
    {
        return std::unexpected(result.error());
    }

    // R20 Fix 18: Use *result (already guarded above) instead of .value()
    auto encoding = detect_encoding(*result);
    auto content = strip_bom(*result, encoding.encoding);

    return std::pair{std::move(content), std::move(encoding)};
}

auto FileSystem::scan_directory_to_tree(const std::filesystem::path& dir_path, int max_depth)
    -> std::expected<FileNode, std::string>
{
    try
    {
        if (!std::filesystem::exists(dir_path))
        {
            return std::unexpected("Directory does not exist: " + dir_path.string());
        }
        if (!std::filesystem::is_directory(dir_path))
        {
            return std::unexpected("Not a directory: " + dir_path.string());
        }

        FileNode root;
        root.id = dir_path.string();
        root.name = dir_path.filename().string();
        root.type = FileNodeType::Folder;
        root.is_open = true;

        scan_recursive(dir_path, root, 0, max_depth);

        return root;
    }
    catch (const std::exception& e)
    {
        return std::unexpected(std::string("Scan error: ") + e.what());
    }
}

auto FileSystem::open_file_dialog(wxWindow* parent, const std::string& wildcard)
    -> std::optional<std::filesystem::path>
{
    wxFileDialog dialog(parent,
                        "Open File",
                        wxEmptyString,
                        wxEmptyString,
                        wxString(wildcard),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return std::nullopt;
    }

    return std::filesystem::path(dialog.GetPath().ToStdString());
}

auto FileSystem::open_folder_dialog(wxWindow* parent) -> std::optional<std::filesystem::path>
{
    wxDirDialog dialog(
        parent, "Open Folder", wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return std::nullopt;
    }

    return std::filesystem::path(dialog.GetPath().ToStdString());
}

auto FileSystem::save_file_dialog(wxWindow* parent, const std::string& default_name)
    -> std::optional<std::filesystem::path>
{
    wxFileDialog dialog(parent,
                        "Save File",
                        wxEmptyString,
                        wxString(default_name),
                        "Markdown files (*.md)|*.md|All files (*.*)|*.*",
                        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return std::nullopt;
    }

    return std::filesystem::path(dialog.GetPath().ToStdString());
}

// ── Private helpers ──

void FileSystem::scan_recursive(const std::filesystem::path& dir_path,
                                FileNode& parent_node,
                                int current_depth,
                                int max_depth)
{
    if (current_depth >= max_depth)
    {
        return;
    }

    std::vector<FileNode> folders;
    std::vector<FileNode> files;

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(
                 dir_path, std::filesystem::directory_options::skip_permission_denied))
        {
            auto filename = entry.path().filename().string();

            // Skip hidden files/directories (starting with '.')
            if (!filename.empty() && filename[0] == '.')
            {
                continue;
            }

            // Improvement 22: cache status to reduce stat calls
            const auto status = entry.status();
            const auto file_type = status.type();

            if (file_type == std::filesystem::file_type::directory)
            {
                // Skip excluded directories
                if (kExcludedDirectories.contains(filename))
                {
                    continue;
                }

                // Skip symlinks to prevent infinite loops
                if (entry.symlink_status().type() == std::filesystem::file_type::symlink)
                {
                    continue;
                }

                FileNode folder_node;
                folder_node.id = entry.path().string();
                folder_node.name = filename;
                folder_node.type = FileNodeType::Folder;
                folder_node.is_open = false;

                scan_recursive(entry.path(), folder_node, current_depth + 1, max_depth);

                // Only include folders that contain matching files (recursively)
                if (folder_node.file_count() > 0)
                {
                    folders.push_back(std::move(folder_node));
                }
            }
            else if (file_type == std::filesystem::file_type::regular)
            {
                auto ext = entry.path().extension().string();
                // Convert to lowercase for comparison
                std::transform(ext.begin(),
                               ext.end(),
                               ext.begin(),
                               [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

                if (kMarkdownExtensions.contains(ext))
                {
                    FileNode file_node;
                    file_node.id = entry.path().string();
                    file_node.name = filename;
                    file_node.type = FileNodeType::File;
                    // Content is NOT loaded during scan (lazy loading)

                    files.push_back(std::move(file_node));
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        MARKAMP_LOG_WARN("Filesystem error scanning {}: {}", dir_path.string(), e.what());
    }

    // Sort folders and files alphabetically
    std::sort(folders.begin(),
              folders.end(),
              [](const FileNode& a, const FileNode& b) { return a.name < b.name; });

    std::sort(files.begin(),
              files.end(),
              [](const FileNode& a, const FileNode& b) { return a.name < b.name; });

    // Folders first, then files
    for (auto& f : folders)
    {
        parent_node.children.push_back(std::move(f));
    }
    for (auto& f : files)
    {
        parent_node.children.push_back(std::move(f));
    }
}

} // namespace markamp::core
