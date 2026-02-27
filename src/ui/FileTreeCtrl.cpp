#include "ui/FileTreeCtrl.h"

#include "FileTreeCtrl.h"
#include "SidebarSkeletonPlaceholder.h"
#include "ThemeAwareWindow.h"
#include "ThemedScrollbar.h"
#include "core/CommandRegistry.h"
#include "core/Events.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "FileTypeIconResolver.h"
#include "IconManager.h"
#include "SidebarSkeletonPlaceholder.h"
#include "core/Logger.h"
#include "ui/FileTypeIconRegistry.h"
#include "ui/FocusManager.h"
#include "ui/FocusRingRenderer.h"
#include "ui/IFileTreeDecorationProvider.h"
#include "ui/accessibility/AccessibilityController.h"

#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/datetime.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/dnd.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/graphics.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/textctrl.h>
#include <wx/textdlg.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace markamp::ui
{

// Helper drop target for the file tree
class FileTreeDropTarget : public wxFileDropTarget
{
public:
    explicit FileTreeDropTarget(FileTreeCtrl* owner)
        : owner_(owner)
    {
    }

    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override
    {
        return owner_->HandleDropData(x, y, filenames[0]);
    }

    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult) override
    {
        return owner_->HandleDragOver(x, y, defResult);
    }

    virtual void OnLeave() override
    {
        owner_->HandleDragLeave();
    }

private:
    FileTreeCtrl* owner_;
};

FileTreeCtrl::FileTreeCtrl(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           core::EventBus& event_bus,
                           FileTypeIconRegistry& icon_registry)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER)
    , event_bus_(event_bus)
    , icon_registry_(icon_registry)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &FileTreeCtrl::OnPaint, this);
    Bind(wxEVT_MOTION, &FileTreeCtrl::OnMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &FileTreeCtrl::OnMouseDown, this);
    Bind(wxEVT_SET_FOCUS, &FileTreeCtrl::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &FileTreeCtrl::OnKillFocus, this);
    Bind(wxEVT_LEFT_DCLICK, &FileTreeCtrl::OnDoubleClick, this);
    Bind(wxEVT_RIGHT_DOWN, &FileTreeCtrl::OnRightClick, this);
    Bind(wxEVT_LEAVE_WINDOW, &FileTreeCtrl::OnMouseLeave, this);
    Bind(wxEVT_MOUSEWHEEL, &FileTreeCtrl::OnScroll, this);
    Bind(wxEVT_KEY_DOWN, &FileTreeCtrl::OnKeyDown, this);
    Bind(wxEVT_SIZE, &FileTreeCtrl::OnSize, this);

    Bind(wxEVT_SCROLLWIN_THUMBTRACK, &FileTreeCtrl::OnScrollbarDrag, this);
    Bind(wxEVT_SCROLLWIN_THUMBRELEASE, &FileTreeCtrl::OnScrollbarDrag, this);

    // Allow focus for keyboard events
    SetCanFocus(true);

    // R3 Fix 4: Type-ahead timer — clears buffer after 500ms
    type_ahead_timer_.SetOwner(this);
    Bind(wxEVT_TIMER, &FileTreeCtrl::OnTypeAheadTimerExpired, this);

    keyboard_mode_sub_ = event_bus_.subscribe<core::events::KeyboardModeChangedEvent>(
        [this](const core::events::KeyboardModeChangedEvent& /*evt*/) { Refresh(); });

    loading_skeleton_ = new SidebarSkeletonPlaceholder(
        this, theme_engine, SidebarSkeletonPlaceholder::Style::kList);
    loading_skeleton_->Hide();

    scrollbar_ = new ThemedScrollbar(this, theme_engine, this);
    scrollbar_->Hide();

    git_status_.SetRefreshCallback([this]() { this->Refresh(); });

    LoadIcons();

    // Setup Drag and Drop
    SetDropTarget(new FileTreeDropTarget(this));

    // Default decorations
    decoration_providers_.push_back(&git_status_);
}

void FileTreeCtrl::AddDecorationProvider(IFileTreeDecorationProvider* provider)
{
    if (provider &&
        std::find(decoration_providers_.begin(), decoration_providers_.end(), provider) ==
            decoration_providers_.end())
    {
        decoration_providers_.push_back(provider);
        Refresh();
    }
}

void FileTreeCtrl::RemoveDecorationProvider(IFileTreeDecorationProvider* provider)
{
    auto it = std::find(decoration_providers_.begin(), decoration_providers_.end(), provider);
    if (it != decoration_providers_.end())
    {
        decoration_providers_.erase(it);
        Refresh();
    }
}

void FileTreeCtrl::SetFileTree(const std::vector<core::FileNode>& roots)
{
    roots_ = roots;

    std::function<void(std::vector<core::FileNode>&)> process_nodes;
    process_nodes = [this, &process_nodes](std::vector<core::FileNode>& nodes)
    {
        if (!exclusions_.empty())
        {
            nodes.erase(std::remove_if(nodes.begin(),
                                       nodes.end(),
                                       [this](const core::FileNode& n)
                                       {
                                           for (const auto& excl : exclusions_)
                                           {
                                               if (n.name == excl)
                                                   return true;
                                           }
                                           return false;
                                       }),
                        nodes.end());
        }

        if (file_nesting_)
        {
            // Nest files like foo.test.cpp under foo.cpp, or foo.h under foo.cpp
            std::vector<core::FileNode> nested_nodes;
            std::vector<core::FileNode*> non_nested_files;

            // Separate folders from files
            for (auto& n : nodes)
            {
                if (n.is_folder())
                {
                    nested_nodes.push_back(std::move(n));
                }
                else
                {
                    non_nested_files.push_back(&n);
                }
            }

            // Try to nest files
            std::vector<bool> consumed(non_nested_files.size(), false);

            // First pass: find parents
            for (size_t i = 0; i < non_nested_files.size(); ++i)
            {
                if (consumed[i])
                    continue;
                auto* parent_candidate = non_nested_files[i];
                std::string parent_name = parent_candidate->name;

                // Typical parent extentions: .cpp, .c, .ts, .js
                std::string parent_stem = std::filesystem::path(parent_name).stem().string();

                bool is_parent = false;

                // Second pass: find children for this parent
                for (size_t j = 0; j < non_nested_files.size(); ++j)
                {
                    if (i == j || consumed[j])
                        continue;
                    auto* child_candidate = non_nested_files[j];
                    std::string child_name = child_candidate->name;

                    // Nesting conditions:
                    // 1. Same stem, different extension (e.g. foo.cpp -> foo.h)
                    // 2. Contains stem + special suffix (e.g. foo.test.cpp, foo.spec.ts, foo.d.ts)

                    bool is_child = false;

                    std::string child_stem = std::filesystem::path(child_name).stem().string();

                    if (parent_stem == child_stem && parent_name != child_name)
                    {
                        // E.g., foo.cpp and foo.h. Prefer .cpp/.ts as parent.
                        std::string p_ext = std::filesystem::path(parent_name).extension().string();
                        std::string c_ext = std::filesystem::path(child_name).extension().string();
                        if ((p_ext == ".cpp" || p_ext == ".c" || p_ext == ".ts" || p_ext == ".js" ||
                             p_ext == ".tsx" || p_ext == ".jsx") &&
                            (c_ext == ".h" || c_ext == ".hpp" || c_ext == ".hh"))
                        {
                            is_child = true;
                        }
                    }
                    else if (child_name.starts_with(parent_stem + ".test.") ||
                             child_name.starts_with(parent_stem + ".spec.") ||
                             child_name.starts_with(parent_stem + ".d.") ||
                             child_name.starts_with(parent_stem + ".min."))
                    {
                        is_child = true;
                    }

                    if (is_child)
                    {
                        parent_candidate->children.push_back(std::move(*child_candidate));
                        parent_candidate->is_open = false; // Closed by default
                        consumed[j] = true;
                        is_parent = true;
                    }
                }

                if (is_parent || !consumed[i])
                {
                    nested_nodes.push_back(std::move(*parent_candidate));
                    consumed[i] = true;
                }
            }

            nodes = std::move(nested_nodes);
        }

        for (auto& n : nodes)
        {
            if (n.is_folder())
            {
                process_nodes(n.children);

                if (compact_folders_ && n.children.size() == 1 && n.children[0].is_folder())
                {
                    auto child = n.children[0];
                    n.name = n.name + "/" + child.name;
                    n.id = child.id; // user interaction targets the child
                    n.children = std::move(child.children);
                }
            }
        }
    };
    process_nodes(roots_);

    SortFileTree();

    UpdateVirtualHeight();
    Refresh();
}

void FileTreeCtrl::SetAutoReveal(bool enabled)
{
    auto_reveal_ = enabled;
}

void FileTreeCtrl::SetCompactFolders(bool enabled)
{
    if (compact_folders_ != enabled)
    {
        compact_folders_ = enabled;
        core::events::WorkspaceRefreshRequestEvent evt;
        event_bus_.publish(evt);
    }
}

void FileTreeCtrl::SetFileNesting(bool enabled)
{
    if (file_nesting_ != enabled)
    {
        file_nesting_ = enabled;
        core::events::WorkspaceRefreshRequestEvent evt;
        event_bus_.publish(evt);
    }
}

void FileTreeCtrl::SetShowFileMetadata(bool enabled)
{
    if (show_file_metadata_ != enabled)
    {
        show_file_metadata_ = enabled;
        Refresh();
    }
}

void FileTreeCtrl::SetExclusions(const std::vector<std::string>& exclusions)
{
    exclusions_ = exclusions;
    core::events::WorkspaceRefreshRequestEvent evt;
    event_bus_.publish(evt);
}

void FileTreeCtrl::SetSortOrder(SortOrder order)
{
    if (sort_order_ != order)
    {
        sort_order_ = order;
        SortFileTree();
        UpdateVirtualHeight();
        Refresh();
    }
}

void FileTreeCtrl::SortFileTree()
{
    std::function<void(std::vector<core::FileNode>&)> sort_recursive;
    sort_recursive = [this, &sort_recursive](std::vector<core::FileNode>& nodes)
    {
        std::sort(nodes.begin(),
                  nodes.end(),
                  [this](const core::FileNode& left, const core::FileNode& right)
                  {
                      if (left.is_folder() != right.is_folder())
                      {
                          return left.is_folder(); // folders first
                      }

                      // Base case: default to alphabetical Name sort
                      std::string l_name = left.name;
                      std::string r_name = right.name;
                      std::transform(l_name.begin(), l_name.end(), l_name.begin(), ::tolower);
                      std::transform(r_name.begin(), r_name.end(), r_name.begin(), ::tolower);

                      if (sort_order_ == SortOrder::TypeFirst && left.is_file() && right.is_file())
                      {
                          std::string l_ext = std::filesystem::path(left.name).extension().string();
                          std::string r_ext =
                              std::filesystem::path(right.name).extension().string();
                          std::transform(l_ext.begin(), l_ext.end(), l_ext.begin(), ::tolower);
                          std::transform(r_ext.begin(), r_ext.end(), r_ext.begin(), ::tolower);
                          if (l_ext != r_ext)
                              return l_ext < r_ext;
                      }
                      else if (sort_order_ == SortOrder::ModifiedDate && left.is_file() &&
                               right.is_file())
                      {
                          try
                          {
                              auto l_time = std::filesystem::last_write_time(left.id);
                              auto r_time = std::filesystem::last_write_time(right.id);
                              if (l_time != r_time)
                                  return l_time > r_time; // Descending (newest first)
                          }
                          catch (...)
                          {
                              // Fall back to name on stat error
                          }
                      }

                      return l_name < r_name;
                  });
        for (auto& node : nodes)
        {
            if (node.is_folder())
            {
                sort_recursive(node.children);
            }
        }
    };
    sort_recursive(roots_);
}

void FileTreeCtrl::SetActiveFileId(const std::string& file_id)
{
    if (active_file_id_ != file_id)
    {
        active_file_id_ = file_id;

        if (auto_reveal_)
        {
            // R3 Fix 2: Auto-expand parent folders so the node is visible
            ExpandAncestors(file_id);
            UpdateVirtualHeight();
            EnsureNodeVisible(file_id);
        }
        Refresh();
    }
}

// Fix 1: Auto-scroll so node_id is visible
void FileTreeCtrl::EnsureNodeVisible(const std::string& node_id)
{
    auto visible = GetVisibleNodes();
    int row_index = 0;
    for (const auto* node : visible)
    {
        if (node->id == node_id)
        {
            const int node_top = row_index * kRowHeight;
            const int node_bottom = node_top + kRowHeight;
            const int client_height = GetClientSize().GetHeight();

            if (node_top < scroll_offset_)
            {
                scroll_offset_ = node_top;
            }
            else if (node_bottom > scroll_offset_ + client_height)
            {
                scroll_offset_ = node_bottom - client_height;
            }
            return;
        }
        ++row_index;
    }
}

// R4 Fix 15: Collapse all folders in the tree
void FileTreeCtrl::CollapseAllNodes()
{
    std::function<void(core::FileNode&)> collapse_recursive;
    collapse_recursive = [&collapse_recursive](core::FileNode& target)
    {
        if (target.is_folder())
        {
            target.is_open = false;
            for (auto& child : target.children)
            {
                collapse_recursive(child);
            }
        }
    };
    for (auto& root : roots_)
    {
        collapse_recursive(root);
    }
    UpdateVirtualHeight();
    Refresh();
}

void FileTreeCtrl::SetOnFileSelect(FileSelectCallback callback)
{
    on_file_select_ = std::move(callback);
}

void FileTreeCtrl::SetOnFileOpen(FileOpenCallback callback)
{
    on_file_open_ = std::move(callback);
}

void FileTreeCtrl::SetWorkspaceRoot(const std::string& root_path)
{
    workspace_root_ = root_path;
    git_status_.SetWorkspaceRoot(root_path);
}

// --- Inline Editing ---

core::FileNode* FindNodeInRoots(std::vector<core::FileNode>& roots, const std::string& target_id)
{
    for (auto& root : roots)
    {
        if (auto* found = root.find_by_id(target_id))
            return found;
    }
    return nullptr;
}

void FileTreeCtrl::CreateNewFile()
{
    std::string target_dir = workspace_root_;
    if (!active_file_id_.empty())
    {
        if (std::filesystem::is_directory(active_file_id_))
        {
            target_dir = active_file_id_;
        }
        else
        {
            target_dir = std::filesystem::path(active_file_id_).parent_path().string();
        }
    }
    ExpandAncestors(target_dir);
    if (auto target_node = FindNodeInRoots(roots_, target_dir))
        target_node->is_open = true;

    StartInlineEdit(InlineEditMode::NewFile, target_dir, "");
}

void FileTreeCtrl::CreateNewFolder()
{
    std::string target_dir = workspace_root_;
    if (!active_file_id_.empty())
    {
        if (std::filesystem::is_directory(active_file_id_))
        {
            target_dir = active_file_id_;
        }
        else
        {
            target_dir = std::filesystem::path(active_file_id_).parent_path().string();
        }
    }
    ExpandAncestors(target_dir);
    if (auto target_node = FindNodeInRoots(roots_, target_dir))
        target_node->is_open = true;

    StartInlineEdit(InlineEditMode::NewFolder, target_dir, "");
}

void FileTreeCtrl::RenameActiveNode()
{
    if (active_file_id_.empty() || active_file_id_ == workspace_root_)
        return;
    auto node = FindNodeInRoots(roots_, active_file_id_);
    if (!node)
        return;

    StartInlineEdit(InlineEditMode::Rename, node->id, node->name);
}

void FileTreeCtrl::InsertDummyNodeForEdit()
{
    std::string fake_id = "<inline-edit>";
    core::FileNode dummy;
    dummy.id = fake_id;
    dummy.name = "";
    dummy.type = (inline_edit_mode_ == InlineEditMode::NewFolder) ? core::FileNodeType::Folder
                                                                  : core::FileNodeType::File;
    dummy.is_open = false;

    if (inline_edit_parent_id_ == workspace_root_)
    {
        roots_.insert(roots_.begin(), dummy);
    }
    else
    {
        auto parent = FindNodeInRoots(roots_, inline_edit_parent_id_);
        if (parent)
        {
            parent->children.insert(parent->children.begin(), dummy);
            parent->is_open = true;
        }
        else
        {
            roots_.insert(roots_.begin(), dummy);
        }
    }
}

void FileTreeCtrl::RemoveDummyNodeForEdit()
{
    std::function<bool(std::vector<core::FileNode>&)> remove_dummy;
    remove_dummy = [&](std::vector<core::FileNode>& nodes) -> bool
    {
        for (auto it = nodes.begin(); it != nodes.end(); ++it)
        {
            if (it->id == "<inline-edit>")
            {
                nodes.erase(it);
                return true;
            }
            if (it->is_folder() && remove_dummy(it->children))
                return true;
        }
        return false;
    };
    remove_dummy(roots_);
}

void FileTreeCtrl::StartInlineEdit(InlineEditMode mode,
                                   const std::string& parent_id,
                                   const std::string& initial_text)
{
    if (inline_editor_ != nullptr)
        CancelInlineEdit();
    if (!filter_text_.empty())
        ClearFilter();

    inline_edit_mode_ = mode;
    inline_edit_parent_id_ = parent_id;
    inline_edit_original_name_ = initial_text;

    if (mode == InlineEditMode::NewFile || mode == InlineEditMode::NewFolder)
    {
        InsertDummyNodeForEdit();
    }

    inline_editor_ = new wxTextCtrl(this,
                                    wxID_ANY,
                                    initial_text,
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
    inline_editor_->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));
    inline_editor_->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMain));
    inline_editor_->SetBackgroundColour(
        theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(120));

    inline_editor_->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent&) { CommitInlineEdit(); });
    inline_editor_->Bind(wxEVT_CHAR_HOOK,
                         [this](wxKeyEvent& event)
                         {
                             if (event.GetKeyCode() == WXK_ESCAPE)
                                 CancelInlineEdit();
                             else
                                 event.Skip();
                         });
    inline_editor_->Bind(wxEVT_KILL_FOCUS,
                         [this](wxFocusEvent& event)
                         {
                             // Delay commit to avoid interfering with current focus processing
                             this->CallAfter(
                                 [this]()
                                 {
                                     if (inline_edit_mode_ != InlineEditMode::None &&
                                         inline_editor_ != nullptr)
                                     {
                                         CommitInlineEdit();
                                     }
                                 });
                             event.Skip();
                         });

    Refresh();
    UpdateVirtualHeight();
}

void FileTreeCtrl::CommitInlineEdit()
{
    if (!inline_editor_ || inline_edit_mode_ == InlineEditMode::None)
        return;

    std::string new_name = inline_editor_->GetValue().ToStdString();
    auto mode = inline_edit_mode_;
    auto parent_id = inline_edit_parent_id_;
    auto original_name = inline_edit_original_name_;

    CancelInlineEdit(); // cleans up dummy node and destroys editor

    if (new_name.empty() || new_name == original_name)
        return;

    try
    {
        if (mode == InlineEditMode::Rename)
        {
            auto old_path = std::filesystem::path(parent_id);
            auto new_path = old_path.parent_path() / new_name;
            std::filesystem::rename(old_path, new_path);
            if (active_file_id_ == parent_id)
                active_file_id_ = new_path.string();
        }
        else
        {
            auto new_path = std::filesystem::path(parent_id) / new_name;
            if (mode == InlineEditMode::NewFolder)
            {
                std::filesystem::create_directory(new_path);
            }
            else
            {
                std::ofstream ofs(new_path);
                ofs.close();
                if (on_file_open_)
                {
                    core::FileNode dummy;
                    dummy.id = new_path.string();
                    dummy.name = new_name;
                    dummy.type = core::FileNodeType::File;
                    on_file_open_(dummy);
                }
            }
        }

        core::events::WorkspaceRefreshRequestEvent evt;
        event_bus_.publish(evt);
    }
    catch (const std::exception& e)
    {
        wxMessageBox(
            "Operation failed: " + std::string(e.what()), "Error", wxICON_ERROR | wxOK, this);
    }
}

void FileTreeCtrl::CancelInlineEdit()
{
    if (inline_edit_mode_ == InlineEditMode::None)
        return;

    if (inline_edit_mode_ == InlineEditMode::NewFile ||
        inline_edit_mode_ == InlineEditMode::NewFolder)
    {
        RemoveDummyNodeForEdit();
    }

    inline_edit_mode_ = InlineEditMode::None;

    if (inline_editor_)
    {
        inline_editor_->Destroy();
        inline_editor_ = nullptr;
    }

    UpdateVirtualHeight();
    Refresh();
    SetFocus();
}

// --- File Operations ---

void FileTreeCtrl::DeleteSelectedNodes()
{
    if (selected_node_ids_.empty())
        return;

    if (wxMessageBox("Are you sure you want to delete the selected item(s)?",
                     "Confirm Delete",
                     wxYES_NO | wxICON_QUESTION) != wxYES)
    {
        return;
    }

    try
    {
        for (const auto& id : selected_node_ids_)
        {
            if (id == workspace_root_)
                continue;
            std::filesystem::remove_all(id);
        }
        selected_node_ids_.clear();
        core::events::WorkspaceRefreshRequestEvent evt;
        event_bus_.publish(evt);
    }
    catch (const std::exception& e)
    {
        wxMessageBox("Failed to delete: " + std::string(e.what()), "Error", wxICON_ERROR);
    }
}

void FileTreeCtrl::CopySelectedNodes()
{
    if (selected_node_ids_.empty())
        return;
    clipboard_paths_ = selected_node_ids_;
    is_cut_operation_ = false;
}

void FileTreeCtrl::CutSelectedNodes()
{
    if (selected_node_ids_.empty())
        return;
    clipboard_paths_ = selected_node_ids_;
    is_cut_operation_ = true;
}

void FileTreeCtrl::PasteNodes()
{
    if (clipboard_paths_.empty())
        return;

    std::string target_dir = workspace_root_;
    if (!active_file_id_.empty())
    {
        if (std::filesystem::is_directory(active_file_id_))
        {
            target_dir = active_file_id_;
        }
        else
        {
            target_dir = std::filesystem::path(active_file_id_).parent_path().string();
        }
    }

    try
    {
        for (const auto& src_path : clipboard_paths_)
        {
            auto src = std::filesystem::path(src_path);
            auto dest = std::filesystem::path(target_dir) / src.filename();

            if (src == dest)
                continue; // Skip pasting onto itself

            if (is_cut_operation_)
            {
                std::filesystem::rename(src, dest);
            }
            else
            {
                std::filesystem::copy(src,
                                      dest,
                                      std::filesystem::copy_options::recursive |
                                          std::filesystem::copy_options::overwrite_existing);
            }
        }

        if (is_cut_operation_)
        {
            clipboard_paths_.clear();
            is_cut_operation_ = false;
        }

        core::events::WorkspaceRefreshRequestEvent evt;
        event_bus_.publish(evt);
    }
    catch (const std::exception& e)
    {
        wxMessageBox("Failed to paste: " + std::string(e.what()), "Error", wxICON_ERROR);
    }
}

// --- Drag and Drop (Tasks 7, 22) ---

void FileTreeCtrl::OnBeginDrag(wxMouseEvent& event)
{
    if (selected_node_ids_.empty())
        return;

    // Create file data object
    wxFileDataObject file_data;
    for (const auto& path : selected_node_ids_)
    {
        file_data.AddFile(path);
    }

    // Start drag operation
    wxDropSource drag_source(this);
    drag_source.SetData(file_data);

    // Let the system handle the drag loop (blocks until dropped or cancelled)
    (void)drag_source.DoDragDrop(wxDrag_AllowMove);

    // Clear hover state after drag finishes
    hover_drop_target_id_.clear();
    Refresh();

    event.Skip(false);
}

bool FileTreeCtrl::HandleDropData(wxCoord x, wxCoord y, const wxString& text)
{
    // The drop target has received the files.
    // 'text' happens to be the first file, but we should process all logic here.
    // However, since we're using wxFileDataObject, we typically get an array.
    // We adapted the DropTarget to pass the first string for simplicity or we can pass the array.
    // In our helper class above, we just passed `filenames[0]`.

    // For full support, let's just do a basic implementation that uses the drop coordinate
    // to find the target folder and moves the currently selected files there.
    // Since we triggered the drop, the paths are in `selected_node_ids_`.

    // BUT what if files were dragged from outside the app?
    // In that case, we need the actual file list.
    auto hit = HitTest(wxPoint(x, y));
    std::string target_dir = workspace_root_;

    if (hit.node)
    {
        if (hit.node->is_folder())
        {
            target_dir = hit.node->id;
        }
        else
        {
            target_dir = std::filesystem::path(hit.node->id).parent_path().string();
        }
    }

    // Clean up highlighted row state
    hover_drop_target_id_.clear();
    Refresh();

    if (text.empty())
        return false;

    // Actually move or copy the dropped item into the target dir.
    // If it's internal drag, we should move it.
    // If external, maybe copy. For now, just move.
    try
    {
        auto src = std::filesystem::path(text.ToStdString());
        auto dest = std::filesystem::path(target_dir) / src.filename();

        if (src != dest)
        {
            std::filesystem::rename(src, dest);

            core::events::WorkspaceRefreshRequestEvent evt;
            event_bus_.publish(evt);
            return true;
        }
    }
    catch (const std::exception& e)
    {
        wxMessageBox("Failed to move file: " + std::string(e.what()), "Error", wxICON_ERROR);
    }

    return false;
}

wxDragResult FileTreeCtrl::HandleDragOver(wxCoord x, wxCoord y, wxDragResult defResult)
{
    auto hit = HitTest(wxPoint(x, y));
    std::string new_hover = hit.node ? hit.node->id : "";

    bool new_insert_after = false;
    if (hit.node && !hit.node->is_folder())
    {
        // If dropping onto a file, we draw a line below it instead of highlighting it
        new_insert_after = true;
    }

    if (new_hover != hover_drop_target_id_ || new_insert_after != hover_drop_insert_after_)
    {
        hover_drop_target_id_ = new_hover;
        hover_drop_insert_after_ = new_insert_after;
        Refresh();
    }

    return defResult;
}

void FileTreeCtrl::HandleDragLeave()
{
    if (!hover_drop_target_id_.empty())
    {
        hover_drop_target_id_.clear();
        Refresh();
    }
}

// --- Filtering ---

void FileTreeCtrl::ApplyFilter(const std::string& filter)
{
    filter_text_ = filter;

    if (filter.empty())
    {
        ClearFilter();
        return;
    }

    // Lowercase the filter for case-insensitive match
    std::string lower_filter = filter;
    std::transform(lower_filter.begin(),
                   lower_filter.end(),
                   lower_filter.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    ApplyFilterRecursive(roots_, lower_filter);
    scroll_offset_ = 0;
    UpdateVirtualHeight();
    Refresh();
}

void FileTreeCtrl::ClearFilter()
{
    filter_text_.clear();

    // Mark all nodes visible
    std::function<void(std::vector<core::FileNode>&)> reset_visible;
    reset_visible = [&](std::vector<core::FileNode>& nodes)
    {
        for (auto& node : nodes)
        {
            node.filter_visible = true;
            if (node.is_folder())
            {
                reset_visible(node.children);
            }
        }
    };

    reset_visible(roots_);
    UpdateVirtualHeight();
    Refresh();
}

auto FileTreeCtrl::MatchesFilter(const core::FileNode& node, const std::string& lower_filter) const
    -> bool
{
    std::string lower_name = node.name;
    std::transform(lower_name.begin(),
                   lower_name.end(),
                   lower_name.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
    return lower_name.find(lower_filter) != std::string::npos;
}

void FileTreeCtrl::ApplyFilterRecursive(std::vector<core::FileNode>& nodes,
                                        const std::string& lower_filter)
{
    for (auto& node : nodes)
    {
        if (node.is_folder())
        {
            // Recurse into children first
            ApplyFilterRecursive(node.children, lower_filter);

            // Folder is visible if it matches OR any child is visible
            bool has_visible_child =
                std::any_of(node.children.begin(),
                            node.children.end(),
                            [](const core::FileNode& child) { return child.filter_visible; });

            node.filter_visible = MatchesFilter(node, lower_filter) || has_visible_child;

            // Auto-expand folders with visible children during filtering
            if (has_visible_child)
            {
                node.is_open = true;
            }
        }
        else
        {
            node.filter_visible = MatchesFilter(node, lower_filter);
        }
    }
}

// --- Rendering ---

void FileTreeCtrl::OnSize(wxSizeEvent& event)
{
    auto sz = GetClientSize();
    if (loading_skeleton_ != nullptr)
    {
        loading_skeleton_->SetSize(sz);
    }

    if (scrollbar_ != nullptr)
    {
        scrollbar_->SetSize(
            sz.GetWidth() - ThemedScrollbar::kWidth, 0, ThemedScrollbar::kWidth, sz.GetHeight());
    }

    UpdateScrollbar();
    event.Skip();
}

void FileTreeCtrl::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    auto sz = GetClientSize();

    // Background
    dc.SetBrush(theme_engine().brush(core::ThemeColorToken::BgPanel));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(sz);

    if (is_loading_)
    {
        return; // Let the child placeholder draw itself
    }

    // Set clip region
    dc.SetClippingRegion(0, 0, sz.GetWidth(), sz.GetHeight());

    // Font
    dc.SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));

    int current_index = 0;
    int y_offset = -scroll_offset_;

#if wxUSE_GRAPHICS_CONTEXT
    std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
#else
    std::unique_ptr<wxGraphicsContext> gc;
#endif

    for (const auto& node : roots_)
    {
        if (node.filter_visible)
        {
            DrawNode(dc, gc.get(), node, 0, y_offset, current_index);
        }
    }

    // Check if we need to hide the editor because it scrolled out of view
    if (inline_editor_ && inline_editor_->IsShown())
    {
        // Actually, if the tree painted and editor was positioned, it might still stick out
        // For simplicity we let it stick out or we can clip it.
        // It's a native window, it will show over the panel if not clipped.
    }

    // Draw animated focus ring
    FocusRingRenderer::get().draw(dc, this, theme_engine());
}

void FileTreeCtrl::ShowLoadingState()
{
    is_loading_ = true;
    if (loading_skeleton_ != nullptr)
    {
        loading_skeleton_->SetSize(GetClientSize());
        loading_skeleton_->ShowAndAnimate();
    }
    Refresh();
}

void FileTreeCtrl::HideLoadingState()
{
    is_loading_ = false;
    if (loading_skeleton_ != nullptr)
    {
        loading_skeleton_->HideAndStop();
    }
    Refresh();
}

void FileTreeCtrl::SetScrollOffset(int offset)
{
    scroll_offset_ = std::clamp(offset, 0, std::max(0, virtual_height_ - GetClientSize().y));
    UpdateScrollbar();
    Refresh();
}

void FileTreeCtrl::LoadIcons()
{
    // Icons are now loaded dynamically via IconManager
}

void FileTreeCtrl::DrawNode(wxDC& dc,
                            wxGraphicsContext* gc,
                            const core::FileNode& node,
                            int depth,
                            int& y_offset,
                            int& current_index)
{
    auto sz = GetClientSize();
    int row_top = y_offset;
    int row_w = sz.GetWidth();
    int row_height = kRowHeight; // Use kRowHeight for consistency

    // Only draw if the row is visible
    if (row_top + kRowHeight > 0 && row_top < sz.GetHeight())
    {
        // VS Code style layout:
        // [Indent] [Twistie] [Icon] [Text]
        // Twistie is always present in the slot, but only drawn for folders.
        // Icon is always present.

        int content_x = kLeftPadding + depth * kIndentWidth;
        int twistie_x = content_x;
        int icon_x = twistie_x + kTwistieSize;
        int text_x = icon_x + kIconSize + kIconTextGap;

        // Centering vertically
        int text_y = row_top + (kRowHeight - dc.GetCharHeight()) / 2;
        int icon_y = row_top + (kRowHeight - kIconSize) / 2;
        int twistie_y = row_top + (kRowHeight - kTwistieSize) / 2;

        // Fix 6: Draw indent guide lines (VS Code style vertical lines at each indent level)
        if (depth > 0)
        {
            dc.SetPen(wxPen(
                theme_engine().color(core::ThemeColorToken::BorderLight).ChangeLightness(90), 1));
            for (int guide_depth = 1; guide_depth <= depth; ++guide_depth)
            {
                const int guide_x = kLeftPadding + guide_depth * kIndentWidth - (kIndentWidth / 2);
                dc.DrawLine(guide_x, row_top, guide_x, row_top + kRowHeight);
            }
        }

        // Row background
        bool is_hovered = (node.id == hovered_node_id_);

        // Active/Select highlight takes precedence over drag highlight
        bool is_selected = false;
        if (std::find(selected_node_ids_.begin(), selected_node_ids_.end(), node.id) !=
            selected_node_ids_.end())
        {
            is_selected = true;
            gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentPrimary),
                                 wxBRUSHSTYLE_SOLID));
            gc->SetPen(*wxTRANSPARENT_PEN);
            gc->DrawRectangle(0, row_top, sz.GetWidth(), row_height);
        }
        else if (node.id == active_file_id_)
        {
            gc->SetBrush(wxBrush(
                theme_engine().color(core::ThemeColorToken::AccentPrimary).ChangeLightness(85),
                wxBRUSHSTYLE_SOLID));
            gc->SetPen(*wxTRANSPARENT_PEN);
            gc->DrawRectangle(0, row_top, sz.GetWidth(), row_height);
        }
        else if (is_hovered)
        {
            gc->SetBrush(
                wxBrush(theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(115),
                        wxBRUSHSTYLE_SOLID));
            gc->SetPen(*wxTRANSPARENT_PEN);
            gc->DrawRectangle(0, row_top, sz.GetWidth(), row_height);
        }

        // --- Drag & Drop Highlight & Insert Line ---
        if (node.id == hover_drop_target_id_)
        {
            if (hover_drop_insert_after_)
            {
                // Draw a horizontal line *below* the node
                gc->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::AccentPrimary), 2));
                gc->StrokeLine(
                    text_x, row_top + row_height - 1, sz.GetWidth(), row_top + row_height - 1);
            }
            else if (!is_selected)
            {
                // Highlight the folder background as a drop target (if not already highlighted)
                // R17 Fix 20: Translucent drop highlight explicitly for folders
                gc->SetBrush(wxBrush(
                    theme_engine().color(core::ThemeColorToken::AccentPrimary).ChangeLightness(50),
                    wxBRUSHSTYLE_SOLID));
                gc->SetPen(*wxTRANSPARENT_PEN);
                gc->DrawRectangle(0, row_top, sz.GetWidth(), row_height);
            }
        }

        // R16 Fix 26: Faint bottom border on each row
        dc.SetPen(
            wxPen(theme_engine().color(core::ThemeColorToken::BorderLight).ChangeLightness(95), 1));
        dc.DrawLine(content_x, row_top + kRowHeight - 1, row_w, row_top + kRowHeight - 1);

        // R3 Fix 3: Phase 06 Task 6: Register focus ring bounds
        wxRect rect(1, row_top + 1, row_w - 2, kRowHeight - 2);
        FocusRingRenderer::get().register_item_bounds(
            FocusZoneId::kSidebar, current_index, this, rect);

        // 1. Draw Twistie (Chevron) - LEFT ALIGNED now
        wxColour chevron_color = theme_engine().color(core::ThemeColorToken::TextMuted);
        if (node.is_folder())
        {
            std::string chevron_icon =
                node.is_open
                    ? "panel-minimize"
                    : "panel-menu"; // using panel menu as a placeholder for chevron-right/down if
                                    // they don't exist, wait, we don't have chevron icons in
                                    // IconLibrary yet, let's use the closest ones or add them.
                                    // Actually, wait. I will check IconLibrary.cpp. 'panel-menu' is
                                    // not a chevron. I'll just use drawing for now or rely on the
                                    // fact that I should add them to IconLibrary. For now, since I
                                    // don't see chevron-right/down in IconLibrary, I will add them
                                    // in a parallel edit. Let's assume they are "chevron-right" and
                                    // "chevron-down"
            // Let's use drawing code for chevrons if we don't have SVG, or I can just define them.
            // Actually, I can just use "activity-explorer" as a placeholder? No.
            // Let's look at IconLibrary.cpp... There are no chevrons. I will add them to
            // IconLibrary.cpp in a separate replacement chunk if needed, or just use
            // `panel-maximize` temporarily. Actually let's assume they will be added:
            // "chevron-right", "chevron-down"
            std::string chevron_name = node.is_open ? "chevron-down" : "chevron-right";

            // Just use IconManager.
            auto bmp = IconManager::get().get_icon_bitmap(
                chevron_name, wxSize(kTwistieSize, kTwistieSize), chevron_color);
            if (bmp.IsOk())
            {
                dc.DrawBitmap(bmp, twistie_x, twistie_y, true);
            }
        }

        // 2. Draw Icon
        if (gc)
        {
            if (node.is_folder())
            {
                auto type = FileTypeIconRegistry::GetFolderIconType(node.name);
                if (node.is_open)
                {
                    icon_registry_.DrawOpenFolderIcon(
                        *gc, type, icon_x, icon_y, kIconSize, theme_engine());
                }
                else
                {
                    icon_registry_.DrawClosedFolderIcon(
                        *gc, type, icon_x, icon_y, kIconSize, theme_engine());
                }
            }
            else
            {
                icon_registry_.DrawFileIcon(
                    *gc, node.name, icon_x, icon_y, kIconSize, theme_engine());
            }
        }

        // 3. Draw Text
        // Gather decorations
        FileTreeDecoration active_dec;
        active_dec.priority = -1; // lowest
        for (auto* provider : decoration_providers_)
        {
            if (auto dec = provider->ProvideDecoration(node))
            {
                if (dec->priority > active_dec.priority)
                {
                    active_dec = *dec;
                }
            }
        }

        bool is_open_folder = node.is_folder() && node.is_open;
        bool should_bold = is_open_folder || is_selected;
        if (should_bold)
        {
            wxFont bold_font = dc.GetFont();
            bold_font.SetWeight(wxFONTWEIGHT_BOLD);
            dc.SetFont(bold_font);
        }

        if (is_selected)
        {
            dc.SetTextForeground(wxColour(255, 255, 255));
        }
        else if (is_hovered)
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMain));
        }
        else if (active_dec.text_color)
        {
            dc.SetTextForeground(*active_dec.text_color);
        }
        else if (is_open_folder)
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMain));
        }
        else
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
        }

        bool is_inline_editing_this_node = false;
        if (inline_edit_mode_ != InlineEditMode::None &&
            ((inline_edit_mode_ == InlineEditMode::Rename && node.id == inline_edit_parent_id_) ||
             ((inline_edit_mode_ == InlineEditMode::NewFile ||
               inline_edit_mode_ == InlineEditMode::NewFolder) &&
              node.id == "<inline-edit>")))
        {
            is_inline_editing_this_node = true;
            if (inline_editor_)
            {
                int expected_y = text_y - 2;
                int expected_h = dc.GetCharHeight() + 4;
                inline_editor_->SetSize(
                    text_x, expected_y, row_w - text_x - kLeftPadding - 4, expected_h);
                if (!inline_editor_->IsShown())
                {
                    inline_editor_->Show();
                    inline_editor_->SetFocus();
                    if (inline_edit_mode_ == InlineEditMode::Rename)
                    {
                        size_t dot_pos = inline_edit_original_name_.find_last_of('.');
                        if (dot_pos != std::string::npos && dot_pos > 0)
                        {
                            inline_editor_->SetSelection(0, static_cast<long>(dot_pos));
                        }
                        else
                        {
                            inline_editor_->SelectAll();
                        }
                    }
                }
            }
        }

        if (!is_inline_editing_this_node)
        {
            // Truncate text with ellipsis if it overflows
            int max_text_width = row_w - text_x - kLeftPadding;
            wxString display_name = node.name;
            auto text_extent = dc.GetTextExtent(display_name);
            if (text_extent.GetWidth() > max_text_width && max_text_width > 0)
            {
                // Simple ellipsis truncation
                while (display_name.length() > 1)
                {
                    display_name = display_name.Left(display_name.length() - 1);
                    if (dc.GetTextExtent(display_name + "...").GetWidth() <= max_text_width)
                    {
                        display_name += "...";
                        break;
                    }
                }
            }
            // R20 Fix 24: Bold matched filter characters in file names
            auto font_normal = dc.GetFont();
            wxFont font_bold = font_normal;
            font_bold.SetWeight(wxFONTWEIGHT_BOLD);

            if (!filter_text_.empty() && !node.is_folder())
            {
                // Find the match position in display_name (case-insensitive)
                std::string lower_display;
                for (char ch : display_name.ToStdString())
                {
                    lower_display +=
                        static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
                }
                std::string lower_filter = filter_text_;
                std::transform(lower_filter.begin(),
                               lower_filter.end(),
                               lower_filter.begin(),
                               [](unsigned char chr)
                               { return static_cast<char>(std::tolower(chr)); });
                auto match_pos = lower_display.find(lower_filter);
                if (match_pos != std::string::npos)
                {
                    // Draw text in segments: before, match (bold), after

                    wxString before_text = display_name.Left(match_pos);
                    wxString match_text = display_name.Mid(match_pos, lower_filter.size());
                    wxString after_text = display_name.Mid(match_pos + lower_filter.size());

                    int draw_x = text_x;
                    if (!before_text.empty())
                    {
                        dc.DrawText(before_text, draw_x, text_y);
                        draw_x += dc.GetTextExtent(before_text).GetWidth();
                    }
                    dc.SetFont(font_bold);
                    dc.SetTextForeground(
                        theme_engine().color(core::ThemeColorToken::AccentPrimary));
                    dc.DrawText(match_text, draw_x, text_y);
                    draw_x += dc.GetTextExtent(match_text).GetWidth();
                    dc.SetFont(node.is_folder() ? font_bold : font_normal);
                    dc.SetTextForeground(
                        is_selected ? theme_engine()
                                          .color(core::ThemeColorToken::AccentPrimary)
                                          .ChangeLightness(80)
                        : is_hovered ? theme_engine().color(core::ThemeColorToken::TextMain)
                                     : theme_engine().color(core::ThemeColorToken::TextMuted));
                    if (!after_text.empty())
                    {
                        dc.DrawText(after_text, draw_x, text_y);
                    }
                }
                else
                {
                    dc.SetFont(node.is_folder() ? font_bold : font_normal);
                    dc.DrawText(display_name, text_x, text_y);
                }
            }
            else
            {
                dc.SetFont(node.is_folder() ? font_bold : font_normal);
                dc.DrawText(display_name, text_x, text_y);
            }
        } // end of !is_inline_editing_this_node

        // R5 Fix 14: Draw file metadata (size or child count) right-aligned in muted text
        {
            int meta_x_offset = kLeftPadding;

            // Draw Badge decorators first
            if (active_dec.badge_text)
            {
                std::string badge = *active_dec.badge_text;
                wxColour badge_color = active_dec.badge_color
                                           ? *active_dec.badge_color
                                           : theme_engine().color(core::ThemeColorToken::TextMuted);

                dc.SetTextForeground(badge_color);
                auto badge_extent = dc.GetTextExtent(badge);
                int badge_x = row_w - badge_extent.GetWidth() - meta_x_offset;
                if (badge_x > text_x + 40)
                {
                    dc.DrawText(badge, badge_x, text_y);
                    meta_x_offset += badge_extent.GetWidth() + 8;
                }
            }

            std::string meta_text;
            if (show_file_metadata_)
            {
                if (node.is_file())
                {
                    try
                    {
                        const auto fsize = std::filesystem::file_size(node.id);
                        if (fsize < 1024)
                        {
                            meta_text = std::to_string(fsize) + " B";
                        }
                        else if (fsize < 1024 * 1024)
                        {
                            meta_text = std::to_string(fsize / 1024) + " KB";
                        }
                        else
                        {
                            meta_text = std::to_string(fsize / (1024 * 1024)) + " MB";
                        }

                        wxFileName fn(node.id);
                        wxDateTime modDate;
                        if (fn.GetTimes(nullptr, &modDate, nullptr))
                        {
                            meta_text +=
                                " \xE2\x80\xA2 " + modDate.Format("%b %d, %H:%M").ToStdString();
                        }
                    }
                    catch (const std::filesystem::filesystem_error& /*err*/)
                    {
                        // Untitled file — no size
                    }
                }
                else
                {
                    const auto child_count = node.children.size();
                    meta_text =
                        std::to_string(child_count) + (child_count == 1 ? " item" : " items");
                }
            }

            if (!meta_text.empty())
            {
                dc.SetTextForeground(
                    theme_engine().color(core::ThemeColorToken::TextMuted).ChangeLightness(85));
                auto meta_extent = dc.GetTextExtent(meta_text);
                int meta_x = row_w - meta_extent.GetWidth() - meta_x_offset;
                if (meta_x > text_x + 40) // only draw if there's room
                {
                    dc.DrawText(meta_text, meta_x, text_y);
                }
            }
        }

        // V8 Phase 1 Task 3: Restore regular font after open folder text draw
        if (should_bold)
        {
            dc.SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));
        }

        // Chevron for folders (right-aligned)
        // OBSOLETE: Chevron is now left-aligned and drawn above.
    }

    y_offset += kRowHeight;
    current_index++;

    // Draw children if folder is open
    if (node.is_folder() && node.is_open)
    {
        bool has_visible_children = false;
        for (const auto& child : node.children)
        {
            if (child.filter_visible)
            {
                has_visible_children = true;
                DrawNode(dc, gc, child, depth + 1, y_offset, current_index);
            }
        }

        // Fix 8: Show placeholder for empty open folders
        if (!has_visible_children)
        {
            int empty_x = kLeftPadding + (depth + 1) * kIndentWidth + kTwistieSize;
            int empty_y = y_offset + (kRowHeight - dc.GetCharHeight()) / 2;
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
            dc.DrawText("No files in folder", empty_x, empty_y);
            y_offset += kRowHeight;
        }
    }
}

// --- Interaction ---

void FileTreeCtrl::OnMouseMove(wxMouseEvent& event)
{
    // Start drag and drop if left button is down and we are moving over a selected node
    if (event.Dragging() && event.LeftIsDown() && !selected_node_ids_.empty())
    {
        OnBeginDrag(event);
        return;
    }

    auto hit = HitTest(event.GetPosition());
    std::string new_hovered = hit.node != nullptr ? hit.node->id : "";

    if (new_hovered != hovered_node_id_)
    {
        hovered_node_id_ = new_hovered;

        // Fix 6: Show file size / child count tooltip on hover
        if (hit.node != nullptr)
        {
            // R4 Fix 18: Show relative path instead of absolute
            std::string tip;
            if (!workspace_root_.empty())
            {
                const auto rel = std::filesystem::relative(hit.node->id, workspace_root_);
                tip = rel.string();
            }
            else
            {
                tip = hit.node->id;
            }
            if (hit.node->is_file())
            {
                try
                {
                    const auto file_size = std::filesystem::file_size(hit.node->id);
                    if (file_size < 1024)
                    {
                        tip += "  (" + std::to_string(file_size) + " B)";
                    }
                    else if (file_size < 1024 * 1024)
                    {
                        tip += "  (" + std::to_string(file_size / 1024) + " KB)";
                    }
                    else
                    {
                        tip += "  (" + std::to_string(file_size / (1024 * 1024)) + " MB)";
                    }
                }
                catch (const std::filesystem::filesystem_error& /*err*/)
                {
                    // File may not exist on disk (e.g., untitled)
                }
            }
            else
            {
                const auto num_children = hit.node->children.size();
                tip += "  (" + std::to_string(num_children) + " items)";
            }
            SetToolTip(tip);
        }
        else
        {
            UnsetToolTip();
        }

        // R4 Fix 10: Cursor feedback — hand for all rows (like VS Code)
        if (hit.node != nullptr)
        {
            SetCursor(wxCURSOR_HAND);
        }
        else
        {
            SetCursor(wxCURSOR_DEFAULT);
        }

        Refresh();
    }
}

void FileTreeCtrl::OnMouseDown(wxMouseEvent& event)
{
    // Fix 1: Acquire focus so keyboard navigation works immediately
    SetFocus();

    auto hit = HitTest(event.GetPosition());

    // Fix 7: Click empty area below nodes deselects
    if (hit.node == nullptr)
    {
        active_file_id_.clear();
        focused_node_index_ = -1;
        Refresh();
        return;
    }

    // Sync focused_node_index_ with clicked node
    auto visible = GetVisibleNodes();
    for (size_t idx = 0; idx < visible.size(); ++idx)
    {
        if (visible[idx]->id == hit.node->id)
        {
            focused_node_index_ = static_cast<int>(idx);
            break;
        }
    }

    auto* clicked_node = hit.node;

    if (clicked_node)
    {
        if (event.CmdDown() || event.ControlDown())
        {
            // Toggle selection
            auto it =
                std::find(selected_node_ids_.begin(), selected_node_ids_.end(), clicked_node->id);
            if (it != selected_node_ids_.end())
            {
                selected_node_ids_.erase(it);
                if (active_file_id_ == clicked_node->id)
                    active_file_id_.clear();
            }
            else
            {
                selected_node_ids_.push_back(clicked_node->id);
                active_file_id_ = clicked_node->id;
            }
        }
        else if (event.ShiftDown() && !selected_node_ids_.empty())
        {
            // Range selection
            if (std::find(selected_node_ids_.begin(), selected_node_ids_.end(), clicked_node->id) ==
                selected_node_ids_.end())
            {
                selected_node_ids_.push_back(clicked_node->id);
            }
            active_file_id_ = clicked_node->id;
        }
        else
        {
            // Normal click
            selected_node_ids_.clear();
            selected_node_ids_.push_back(clicked_node->id);
            active_file_id_ = clicked_node->id;

            if (clicked_node->is_folder())
            {
                // R4 Fix 10: Toggle fold state on single click
                clicked_node->is_open = !clicked_node->is_open;
                UpdateVirtualHeight();
            }
            else
            {
                if (on_file_select_)
                {
                    on_file_select_(*clicked_node);
                }

                if (on_file_open_)
                {
                    // Open the file on single click
                    on_file_open_(*clicked_node);
                }
            }
        }
        Refresh();

        // Publish Breadcrumb Event
        core::events::SidebarBreadcrumbUpdateEvent breadcrumb_evt;
        breadcrumb_evt.panel_id = "explorer";

        std::vector<std::string> path;
        std::function<bool(const std::vector<core::FileNode>&, const std::string&)> find_path;
        find_path = [&](const std::vector<core::FileNode>& nodes,
                        const std::string& target_id) -> bool
        {
            for (const auto& n : nodes)
            {
                path.push_back(n.name);
                if (n.id == target_id)
                    return true;
                if (n.is_folder() && find_path(n.children, target_id))
                    return true;
                path.pop_back();
            }
            return false;
        };

        if (!find_path(roots_, clicked_node->id))
        {
            path = {clicked_node->name};
        }
        breadcrumb_evt.breadcrumb_path = path;
        event_bus_.publish(breadcrumb_evt);

        MARKAMP_LOG_DEBUG("File selected: {}", clicked_node->name);
    }
}

void FileTreeCtrl::OnMouseLeave(wxMouseEvent& /*event*/)
{
    if (!hovered_node_id_.empty())
    {
        hovered_node_id_.clear();
        // Fix 4: Reset cursor on leave
        SetCursor(wxCURSOR_DEFAULT);
        Refresh();
    }
}

// --- Hit testing ---

auto FileTreeCtrl::HitTest(const wxPoint& point) -> HitResult
{
    int y_offset = -scroll_offset_;
    return HitTestRecursive(point, roots_, 0, y_offset);
}

auto FileTreeCtrl::HitTestRecursive(const wxPoint& point,
                                    std::vector<core::FileNode>& nodes,
                                    int depth,
                                    int& y_offset) -> HitResult
{
    for (auto& node : nodes)
    {
        if (!node.filter_visible)
        {
            continue;
        }

        int row_top = y_offset;
        y_offset += kRowHeight;

        if (point.y >= row_top && point.y < row_top + kRowHeight)
        {
            bool on_chevron = false;
            if (node.is_folder())
            {
                // VS Code style: Click anywhere on the twistie area to toggle
                // Twistie is at: kLeftPadding + depth * kIndentWidth
                int twistie_x = kLeftPadding + depth * kIndentWidth;

                // Allow clicking slightly wider area for usability?
                // VS Code allows clicking the icon to select, twistie to toggle.
                // Twistie width is 16px.

                on_chevron = (point.x >= twistie_x && point.x < twistie_x + kTwistieSize);
            }
            return {&node, on_chevron};
        }

        if (node.is_folder() && node.is_open)
        {
            auto result = HitTestRecursive(point, node.children, depth + 1, y_offset);
            if (result.node != nullptr)
            {
                return result;
            }
        }
    }

    return {nullptr, false};
}

// --- Scrolling ---

void FileTreeCtrl::UpdateVirtualHeight()
{
    // Count visible nodes
    std::function<int(const std::vector<core::FileNode>&)> count_visible;
    count_visible = [&](const std::vector<core::FileNode>& nodes) -> int
    {
        int count = 0;
        for (const auto& node : nodes)
        {
            if (!node.filter_visible)
            {
                continue;
            }
            count += 1;
            if (node.is_folder() && node.is_open)
            {
                count += count_visible(node.children);
            }
        }
        return count;
    };

    virtual_height_ = count_visible(roots_) * kRowHeight;
    UpdateScrollbar();
}

void FileTreeCtrl::OnScroll(wxMouseEvent& event)
{
    int delta = event.GetWheelRotation();
    int scroll_amount = kRowHeight * 2; // Scroll 2 rows at a time

    if (delta > 0)
    {
        scroll_offset_ = std::max(0, scroll_offset_ - scroll_amount);
    }
    else if (delta < 0)
    {
        int max_scroll = std::max(0, virtual_height_ - GetClientSize().GetHeight());
        scroll_offset_ = std::min(max_scroll, scroll_offset_ + scroll_amount);
    }

    UpdateScrollbar();
    Refresh();
}

void FileTreeCtrl::OnScrollbarDrag(wxScrollWinEvent& event)
{
    scroll_offset_ = event.GetPosition();
    Refresh();
}

void FileTreeCtrl::UpdateScrollbar()
{
    if (scrollbar_ != nullptr)
    {
        auto sz = GetClientSize();
        scrollbar_->UpdateScrollPosition(scroll_offset_, sz.GetHeight(), virtual_height_);
        if (virtual_height_ > sz.GetHeight())
        {
            scrollbar_->Show();
        }
        else
        {
            scrollbar_->Hide();
        }
    }
}

// --- Theme ---

void FileTreeCtrl::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    LoadIcons();
    Refresh();
}

// --- Double-click to open (QoL feature 2) ---

void FileTreeCtrl::OnDoubleClick(wxMouseEvent& event)
{
    auto hit = HitTest(event.GetPosition());
    if (hit.node == nullptr)
    {
        return;
    }

    if (hit.node->is_folder())
    {
        // Double-click on folder toggles it (same as single click)
        hit.node->is_open = !hit.node->is_open;
        UpdateVirtualHeight();
        Refresh();
    }
    else
    {
        // Double-click on file: open in tab
        if (on_file_open_)
        {
            on_file_open_(*hit.node);
        }
        MARKAMP_LOG_DEBUG("File double-clicked (open): {}", hit.node->name);
    }
}

// --- Right-click context menu (QoL feature 6) ---

namespace
{
constexpr int kCtxOpen = 100;
constexpr int kCtxRevealInFinder = 101;
constexpr int kCtxCopyPath = 102;
constexpr int kCtxCopyRelativePath = 103;
constexpr int kCtxCollapseAll = 104;
constexpr int kCtxExpandAll = 105;
constexpr int kCtxNewFile = 106;
constexpr int kCtxDeleteFile = 107;
constexpr int kCtxRename = 108;
constexpr int kCtxNewFolder = 109;
constexpr int kCtxOpenInTerminal = 110;       // R4 Fix 5
constexpr int kCtxOpenContainingFolder = 111; // R4 Fix 6
constexpr int kCtxCompareWith = 112;
constexpr int kCtxCopy = 113;
constexpr int kCtxCut = 114;
constexpr int kCtxPaste = 115;
constexpr int kCtxSortByName = 116;
constexpr int kCtxSortByType = 117;
constexpr int kCtxSortByModified = 118;
} // namespace

void FileTreeCtrl::OnRightClick(wxMouseEvent& event)
{
    // Fix 1: Acquire focus on right-click too
    SetFocus();

    auto hit = HitTest(event.GetPosition());
    core::FileNode* clicked_node = hit.node;

    if (clicked_node == nullptr)
    {
        // R2 Fix 1: Show workspace-level context menu on empty area
        ShowEmptyAreaContextMenu();
        return;
    }

    // Select the node
    if (clicked_node)
    {
        // Ensure right-clicked node is in selection
        if (std::find(selected_node_ids_.begin(), selected_node_ids_.end(), clicked_node->id) ==
            selected_node_ids_.end())
        {
            selected_node_ids_.clear();
            selected_node_ids_.push_back(clicked_node->id);
            active_file_id_ = clicked_node->id;
            Refresh();
        }

        ShowFileContextMenu(*clicked_node);
    }
}

void FileTreeCtrl::ShowFileContextMenu(core::FileNode& node)
{
    wxMenu menu;

    if (node.is_file())
    {
        menu.Append(kCtxOpen, "Open");
        menu.AppendSeparator();
    }
    else
    {
        menu.Append(kCtxExpandAll, "Expand All");
        menu.Append(kCtxCollapseAll, "Collapse All");
        menu.AppendSeparator();

        wxMenu* sort_menu = new wxMenu();
        sort_menu->AppendCheckItem(kCtxSortByName, "Name");
        sort_menu->AppendCheckItem(kCtxSortByType, "Type");
        sort_menu->AppendCheckItem(kCtxSortByModified, "Modified Date");
        sort_menu->Check(kCtxSortByName + static_cast<int>(sort_order_), true);
        menu.AppendSubMenu(sort_menu, "Sort By");
        menu.AppendSeparator();
    }

    // R2 Fix 6: "New Folder…" option
    menu.Append(kCtxNewFile, "New File\u2026");
    menu.Append(kCtxNewFolder, "New Folder\u2026");
    menu.AppendSeparator();
    // R2 Fix 3: "Rename…" option — R5 Fix 8: keyboard shortcut hint
    menu.Append(kCtxRename, "Rename\u2026\tF2");
    // R2 Fix 2: "Delete File…" option — R5 Fix 8: keyboard shortcut hint
    menu.Append(kCtxDeleteFile, "Delete\u2026\t\u232B");
    menu.AppendSeparator();
    menu.Append(kCtxRevealInFinder, "Reveal in Finder");
    menu.AppendSeparator();
    menu.Append(kCtxCut, "Cut\tCmd+X");
    menu.Append(kCtxCopy, "Copy\tCmd+C");
    menu.Append(kCtxPaste, "Paste\tCmd+V");
    menu.AppendSeparator();
    menu.Append(kCtxCopyPath, "Copy Path");
    menu.Append(kCtxCopyRelativePath, "Copy Relative Path");

    // R4 Fix 5: Open in Terminal
    menu.AppendSeparator();
    menu.Append(kCtxOpenInTerminal, "Open in Terminal");
    // R4 Fix 6: Open Containing Folder (for files, opens parent directory)
    if (node.is_file())
    {
        menu.Append(kCtxOpenContainingFolder, "Open Containing Folder");
        menu.AppendSeparator();
        menu.Append(kCtxCompareWith, "Compare with\u2026");
    }

    const std::string node_path = node.id;
    const bool is_file = node.is_file();

    menu.Bind(
        wxEVT_MENU,
        [this, node_path, is_file, &node](wxCommandEvent& cmd_event)
        {
            switch (cmd_event.GetId())
            {
                case kCtxOpen:
                    if (is_file && on_file_open_)
                    {
                        on_file_open_(node);
                    }
                    break;
                case kCtxRevealInFinder:
                {
#ifdef __APPLE__
                    wxExecute(wxString::Format("open -R \"%s\"", node_path));
#elif defined(__linux__)
                    wxExecute(
                        wxString::Format("xdg-open \"%s\"",
                                         std::filesystem::path(node_path).parent_path().string()));
#endif
                    break;
                }
                case kCtxCopyPath:
                    if (wxTheClipboard->Open())
                    {
                        wxTheClipboard->SetData(new wxTextDataObject(node_path));
                        wxTheClipboard->Close();
                    }
                    break;
                case kCtxCopyRelativePath:
                {
                    std::string relative_path = node_path;
                    if (!workspace_root_.empty())
                    {
                        const auto rel = std::filesystem::relative(node_path, workspace_root_);
                        relative_path = rel.string();
                    }
                    if (wxTheClipboard->Open())
                    {
                        wxTheClipboard->SetData(new wxTextDataObject(relative_path));
                        wxTheClipboard->Close();
                    }
                    break;
                }
                case kCtxCollapseAll:
                {
                    // Recursively collapse all folders
                    std::function<void(core::FileNode&)> collapse_all;
                    collapse_all = [&collapse_all](core::FileNode& target)
                    {
                        if (target.is_folder())
                        {
                            target.is_open = false;
                            for (auto& child : target.children)
                            {
                                collapse_all(child);
                            }
                        }
                    };
                    collapse_all(node);
                    UpdateVirtualHeight();
                    Refresh();
                    break;
                }
                case kCtxNewFile:
                {
                    wxTextEntryDialog name_dlg(this, "Enter file name:", "New File", "untitled.md");
                    if (name_dlg.ShowModal() == wxID_OK)
                    {
                        const std::string file_name = name_dlg.GetValue().ToStdString();
                        std::string dir_path = node_path;
                        if (is_file)
                        {
                            dir_path = std::filesystem::path(node_path).parent_path().string();
                        }
                        const std::string new_file_path =
                            (std::filesystem::path(dir_path) / file_name).string();

                        // Create the file on disk
                        std::ofstream new_stream(new_file_path);
                        new_stream.close();

                        // Open in tab via the file open callback
                        if (on_file_open_)
                        {
                            core::FileNode new_node;
                            new_node.id = new_file_path;
                            new_node.name = file_name;
                            new_node.type = core::FileNodeType::File;
                            on_file_open_(new_node);
                        }

                        // R2 Fix 5: Trigger workspace refresh
                        core::events::WorkspaceRefreshRequestEvent refresh_evt;
                        event_bus_.publish(refresh_evt);
                    }
                    break;
                }
                case kCtxExpandAll:
                {
                    // Recursively expand all folders
                    std::function<void(core::FileNode&)> expand_all;
                    expand_all = [&expand_all](core::FileNode& target)
                    {
                        if (target.is_folder())
                        {
                            target.is_open = true;
                            for (auto& child : target.children)
                            {
                                expand_all(child);
                            }
                        }
                    };
                    expand_all(node);
                    UpdateVirtualHeight();
                    Refresh();
                    break;
                }
                // R2 Fix 2: Delete file/folder
                case kCtxDeleteFile:
                {
                    DeleteSelectedNodes();
                    break;
                }
                // R2 Fix 3: Rename file/folder
                case kCtxRename:
                {
                    const std::string current_name =
                        std::filesystem::path(node_path).filename().string();
                    wxTextEntryDialog rename_dlg(
                        this, "New name:", "Rename", wxString(current_name));
                    if (rename_dlg.ShowModal() == wxID_OK)
                    {
                        const std::string new_name = rename_dlg.GetValue().ToStdString();
                        if (!new_name.empty() && new_name != current_name)
                        {
                            const std::string new_path =
                                (std::filesystem::path(node_path).parent_path() / new_name)
                                    .string();
                            std::error_code err_code;
                            std::filesystem::rename(node_path, new_path, err_code);
                            if (!err_code)
                            {
                                core::events::WorkspaceRefreshRequestEvent refresh_evt;
                                event_bus_.publish(refresh_evt);
                            }
                        }
                    }
                    break;
                }
                // R2 Fix 6: New Folder
                case kCtxNewFolder:
                {
                    wxTextEntryDialog folder_dlg(
                        this, "Enter folder name:", "New Folder", "new-folder");
                    if (folder_dlg.ShowModal() == wxID_OK)
                    {
                        const std::string folder_name = folder_dlg.GetValue().ToStdString();
                        std::string dir_path = node_path;
                        if (is_file)
                        {
                            dir_path = std::filesystem::path(node_path).parent_path().string();
                        }
                        const std::string new_dir_path =
                            (std::filesystem::path(dir_path) / folder_name).string();

                        std::error_code err_code;
                        std::filesystem::create_directory(new_dir_path, err_code);
                        if (!err_code)
                        {
                            core::events::WorkspaceRefreshRequestEvent refresh_evt;
                            event_bus_.publish(refresh_evt);
                        }
                    }
                    break;
                }
                // R4 Fix 5: Open in Terminal
                case kCtxOpenInTerminal:
                {
                    std::string dir_path = node_path;
                    if (is_file)
                    {
                        dir_path = std::filesystem::path(node_path).parent_path().string();
                    }
#ifdef __APPLE__
                    wxExecute(wxString::Format("open -a Terminal \"%s\"", dir_path));
#elif defined(__linux__)
                    wxExecute(wxString::Format("x-terminal-emulator --working-directory=\"%s\"",
                                               dir_path));
#endif
                    break;
                }
                // R4 Fix 6: Open Containing Folder (opens parent in Finder)
                case kCtxOpenContainingFolder:
                {
                    const std::string parent_dir =
                        std::filesystem::path(node_path).parent_path().string();
#ifdef __APPLE__
                    wxExecute(wxString::Format("open \"%s\"", parent_dir));
#elif defined(__linux__)
                    wxExecute(wxString::Format("xdg-open \"%s\"", parent_dir));
#endif
                    break;
                }
                case kCtxCompareWith:
                {
                    wxFileDialog openFileDialog(this,
                                                "Compare With",
                                                "",
                                                "",
                                                "All files (*.*)|*.*",
                                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                    if (openFileDialog.ShowModal() == wxID_CANCEL)
                        return;

                    std::string right_path = openFileDialog.GetPath().ToStdString();
                    std::string left_path = node_path;

                    core::events::OpenDiffRequestEvent diff_evt;
                    diff_evt.left_path = left_path;
                    diff_evt.right_path = right_path;
                    event_bus_.publish(diff_evt);
                    break;
                }
                case kCtxCopy:
                    CopySelectedNodes();
                    break;
                case kCtxCut:
                    CutSelectedNodes();
                    break;
                case kCtxPaste:
                    PasteNodes();
                    break;
                case kCtxSortByName:
                    SetSortOrder(SortOrder::Name);
                    break;
                case kCtxSortByType:
                    SetSortOrder(SortOrder::TypeFirst);
                    break;
                case kCtxSortByModified:
                    SetSortOrder(SortOrder::ModifiedDate);
                    break;
                default:
                    break;
            }
        });

    PopupMenu(&menu);
}

// --- Keyboard navigation (QoL feature 7) ---

void FileTreeCtrl::OnKeyDown(wxKeyEvent& event)
{
    const int key_code = event.GetKeyCode();
    auto visible_nodes = GetVisibleNodes();

    if (visible_nodes.empty())
    {
        event.Skip();
        return;
    }

    // Clamp focused index
    if (focused_node_index_ < 0)
    {
        focused_node_index_ = 0;
    }
    if (focused_node_index_ >= static_cast<int>(visible_nodes.size()))
    {
        focused_node_index_ = static_cast<int>(visible_nodes.size()) - 1;
    }

    switch (key_code)
    {
        case WXK_UP:
            if (focused_node_index_ > 0)
            {
                --focused_node_index_;
                auto* up_node = visible_nodes[static_cast<size_t>(focused_node_index_)];
                active_file_id_ = up_node->id;
                EnsureNodeVisible(active_file_id_);
                Refresh();

                // Announce new focus
                accessibility::AccessibilityController::get().announce_focus(
                    up_node->name,
                    "Tree Item",
                    up_node->is_folder() ? (up_node->is_open ? "Expanded" : "Collapsed") : "");

                // Fix 2: Fire select callback so keyboard nav opens files
                if (up_node->is_file() && on_file_select_)
                {
                    on_file_select_(*up_node);
                }
            }
            break;

        case WXK_DOWN:
            if (focused_node_index_ < static_cast<int>(visible_nodes.size()) - 1)
            {
                ++focused_node_index_;
                auto* down_node = visible_nodes[static_cast<size_t>(focused_node_index_)];
                active_file_id_ = down_node->id;
                EnsureNodeVisible(active_file_id_);
                Refresh();

                // Announce new focus
                accessibility::AccessibilityController::get().announce_focus(
                    down_node->name,
                    "Tree Item",
                    down_node->is_folder() ? (down_node->is_open ? "Expanded" : "Collapsed") : "");

                // Fix 2: Fire select callback so keyboard nav opens files
                if (down_node->is_file() && on_file_select_)
                {
                    on_file_select_(*down_node);
                }
            }
            break;

        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
        {
            auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
            if (node->is_folder())
            {
                node->is_open = !node->is_open;

                // Announce state change
                accessibility::AccessibilityController::get().notify_state_change(
                    node->name, node->is_open ? "Expanded" : "Collapsed");

                UpdateVirtualHeight();
                Refresh();
            }
            else if (on_file_open_)
            {
                on_file_open_(*node);
            }
            break;
        }

        case WXK_SPACE:
        {
            auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
            if (node->is_folder())
            {
                node->is_open = !node->is_open;
                accessibility::AccessibilityController::get().notify_state_change(
                    node->name, node->is_open ? "Expanded" : "Collapsed");
                UpdateVirtualHeight();
                Refresh();
            }
            break;
        }

        case WXK_RIGHT:
        {
            auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
            if (node->is_folder() && !node->is_open)
            {
                node->is_open = true;
                accessibility::AccessibilityController::get().notify_state_change(node->name,
                                                                                  "Expanded");
                UpdateVirtualHeight();
                Refresh();
            }
            break;
        }

        case WXK_LEFT:
        {
            auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
            if (node->is_folder() && node->is_open)
            {
                // Collapse open folder
                node->is_open = false;
                accessibility::AccessibilityController::get().notify_state_change(node->name,
                                                                                  "Collapsed");
                UpdateVirtualHeight();
                Refresh();
            }
            else
            {
                // Fix 3: Navigate to parent folder (like VS Code)
                const int parent_idx = FindParentIndex(visible_nodes, focused_node_index_);
                if (parent_idx >= 0)
                {
                    focused_node_index_ = parent_idx;
                    active_file_id_ = visible_nodes[static_cast<size_t>(parent_idx)]->id;
                    EnsureNodeVisible(active_file_id_);
                    Refresh();

                    accessibility::AccessibilityController::get().announce_focus(
                        visible_nodes[static_cast<size_t>(parent_idx)]->name,
                        "Tree Item",
                        "Expanded");
                }
            }
            break;
        }

        // Fix 8: Home/End keys jump to first/last node
        case WXK_HOME:
        {
            if (!visible_nodes.empty())
            {
                focused_node_index_ = 0;
                active_file_id_ = visible_nodes[0]->id;
                EnsureNodeVisible(active_file_id_);
                Refresh();

                auto* node = visible_nodes[0];
                accessibility::AccessibilityController::get().announce_focus(
                    node->name,
                    "Tree Item",
                    node->is_folder() ? (node->is_open ? "Expanded" : "Collapsed") : "");
            }
            break;
        }

        case WXK_END:
        {
            if (!visible_nodes.empty())
            {
                focused_node_index_ = static_cast<int>(visible_nodes.size()) - 1;
                active_file_id_ = visible_nodes[static_cast<size_t>(focused_node_index_)]->id;
                EnsureNodeVisible(active_file_id_);
                Refresh();

                auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
                accessibility::AccessibilityController::get().announce_focus(
                    node->name,
                    "Tree Item",
                    node->is_folder() ? (node->is_open ? "Expanded" : "Collapsed") : "");
            }
            break;
        }

        // R4 Fix 2: Delete key deletes focused file/folder
        case WXK_DELETE:
        case WXK_BACK:
        {
            if (!selected_node_ids_.empty())
            {
                DeleteSelectedNodes();
            }
            break;
        }

        // R4 Fix 3: F2 key renames focused file/folder
        case WXK_F2:
        {
            RenameActiveNode();
            event.Skip(false);
            break;
        }

        default:
        {
            // Handle Cmd/Ctrl C, X, V
            if (event.CmdDown() || event.ControlDown())
            {
                if (key_code == 'C')
                {
                    CopySelectedNodes();
                    return;
                }
                else if (key_code == 'X')
                {
                    CutSelectedNodes();
                    return;
                }
                else if (key_code == 'V')
                {
                    PasteNodes();
                    return;
                }
            }

            // R3 Fix 4: Type-ahead search — letter keys jump to matching node
            const int unicode_key = event.GetUnicodeKey();
            if (unicode_key >= 32 && unicode_key < 127)
            {
                const char typed = static_cast<char>(std::tolower(unicode_key));
                type_ahead_buffer_ += typed;
                type_ahead_timer_.Start(500, wxTIMER_ONE_SHOT);

                // Find first visible node whose name starts with the buffer
                for (size_t idx = 0; idx < visible_nodes.size(); ++idx)
                {
                    std::string lower_name = visible_nodes[idx]->name;
                    std::transform(
                        lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
                    if (lower_name.rfind(type_ahead_buffer_, 0) == 0)
                    {
                        focused_node_index_ = static_cast<int>(idx);
                        active_file_id_ = visible_nodes[idx]->id;
                        EnsureNodeVisible(active_file_id_);
                        Refresh();
                        break;
                    }
                }
            }
            else
            {
                event.Skip();
            }
            break;
        }
    }
}

auto FileTreeCtrl::GetVisibleNodes() -> std::vector<core::FileNode*>
{
    std::vector<core::FileNode*> result;
    CollectVisibleNodes(result, roots_);
    return result;
}

void FileTreeCtrl::CollectVisibleNodes(std::vector<core::FileNode*>& result,
                                       std::vector<core::FileNode>& nodes)
{
    for (auto& node : nodes)
    {
        if (!node.filter_visible)
        {
            continue;
        }
        result.push_back(&node);
        if (node.is_folder() && node.is_open)
        {
            CollectVisibleNodes(result, node.children);
        }
    }
}

// Fix 3: Find the parent folder index in the visible node list
auto FileTreeCtrl::FindParentIndex(const std::vector<core::FileNode*>& visible, int child_index)
    -> int
{
    if (child_index <= 0 || child_index >= static_cast<int>(visible.size()))
    {
        return -1;
    }

    // The child's id contains the parent directory path
    const std::string child_path = visible[static_cast<size_t>(child_index)]->id;
    const std::string parent_dir = std::filesystem::path(child_path).parent_path().string();

    // Walk backward to find a folder whose id matches the parent directory
    for (int idx = child_index - 1; idx >= 0; --idx)
    {
        if (visible[static_cast<size_t>(idx)]->id == parent_dir &&
            visible[static_cast<size_t>(idx)]->is_folder())
        {
            return idx;
        }
    }
    return -1;
}

// R2 Fix 1: Show context menu when right-clicking empty area below the tree
void FileTreeCtrl::ShowEmptyAreaContextMenu()
{
    wxMenu menu;
    menu.Append(kCtxNewFile, "New File\u2026");
    menu.Append(kCtxNewFolder, "New Folder\u2026");
    menu.AppendSeparator();
    menu.Append(kCtxExpandAll, "Expand All");
    menu.Append(kCtxCollapseAll, "Collapse All");
    menu.AppendSeparator();

    wxMenu* sort_menu = new wxMenu();
    sort_menu->AppendCheckItem(kCtxSortByName, "Name");
    sort_menu->AppendCheckItem(kCtxSortByType, "Type");
    sort_menu->AppendCheckItem(kCtxSortByModified, "Modified Date");
    sort_menu->Check(kCtxSortByName + static_cast<int>(sort_order_), true);
    menu.AppendSubMenu(sort_menu, "Sort By");

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& cmd_event)
        {
            switch (cmd_event.GetId())
            {
                case kCtxNewFile:
                {
                    wxTextEntryDialog name_dlg(this, "Enter file name:", "New File", "untitled.md");
                    if (name_dlg.ShowModal() == wxID_OK)
                    {
                        const std::string file_name = name_dlg.GetValue().ToStdString();
                        if (!workspace_root_.empty())
                        {
                            const std::string new_file_path =
                                (std::filesystem::path(workspace_root_) / file_name).string();
                            std::ofstream new_stream(new_file_path);
                            new_stream.close();

                            if (on_file_open_)
                            {
                                core::FileNode new_node;
                                new_node.id = new_file_path;
                                new_node.name = file_name;
                                new_node.type = core::FileNodeType::File;
                                on_file_open_(new_node);
                            }

                            core::events::WorkspaceRefreshRequestEvent refresh_evt;
                            event_bus_.publish(refresh_evt);
                        }
                    }
                    break;
                }
                case kCtxNewFolder:
                {
                    wxTextEntryDialog folder_dlg(
                        this, "Enter folder name:", "New Folder", "new-folder");
                    if (folder_dlg.ShowModal() == wxID_OK)
                    {
                        const std::string folder_name = folder_dlg.GetValue().ToStdString();
                        if (!workspace_root_.empty())
                        {
                            const std::string new_dir_path =
                                (std::filesystem::path(workspace_root_) / folder_name).string();
                            std::error_code err_code;
                            std::filesystem::create_directory(new_dir_path, err_code);
                            if (!err_code)
                            {
                                core::events::WorkspaceRefreshRequestEvent refresh_evt;
                                event_bus_.publish(refresh_evt);
                            }
                        }
                    }
                    break;
                }
                case kCtxExpandAll:
                {
                    std::function<void(core::FileNode&)> expand_all;
                    expand_all = [&expand_all](core::FileNode& target)
                    {
                        if (target.is_folder())
                        {
                            target.is_open = true;
                            for (auto& child : target.children)
                            {
                                expand_all(child);
                            }
                        }
                    };
                    for (auto& root : roots_)
                    {
                        expand_all(root);
                    }
                    UpdateVirtualHeight();
                    Refresh();
                    break;
                }
                case kCtxCollapseAll:
                {
                    std::function<void(core::FileNode&)> collapse_all;
                    collapse_all = [&collapse_all](core::FileNode& target)
                    {
                        if (target.is_folder())
                        {
                            target.is_open = false;
                            for (auto& child : target.children)
                            {
                                collapse_all(child);
                            }
                        }
                    };
                    for (auto& root : roots_)
                    {
                        collapse_all(root);
                    }
                    UpdateVirtualHeight();
                    Refresh();
                    break;
                }
                case kCtxSortByName:
                    SetSortOrder(SortOrder::Name);
                    break;
                case kCtxSortByType:
                    SetSortOrder(SortOrder::TypeFirst);
                    break;
                case kCtxSortByModified:
                    SetSortOrder(SortOrder::ModifiedDate);
                    break;
                default:
                    break;
            }
        });

    PopupMenu(&menu);
}

void FileTreeCtrl::OnSetFocus(wxFocusEvent& event)
{
    if (focused_node_index_ < 0 && !roots_.empty())
    {
        focused_node_index_ = 0;
    }
    FocusManager::get().set_focus(FocusZoneId::kSidebar, focused_node_index_);
    Refresh();

    // Announce initially focused item
    auto visible_nodes = GetVisibleNodes();
    if (focused_node_index_ >= 0 && focused_node_index_ < static_cast<int>(visible_nodes.size()))
    {
        auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
        accessibility::AccessibilityController::get().announce_focus(
            node->name,
            "Tree Item",
            node->is_folder() ? (node->is_open ? "Expanded" : "Collapsed") : "");
    }

    event.Skip();
}

void FileTreeCtrl::OnKillFocus(wxFocusEvent& event)
{
    if (FocusManager::get().current_zone() == FocusZoneId::kSidebar)
    {
        FocusManager::get().set_item(-1);
    }
    Refresh();
    event.Skip();
}

} // namespace markamp::ui

// R3 Fix 4: Clear the type-ahead buffer when the timer fires
void markamp::ui::FileTreeCtrl::OnTypeAheadTimerExpired(wxTimerEvent& /*event*/)
{
    type_ahead_buffer_.clear();
}

// R3 Fix 3: Return the id of the node with keyboard focus
auto markamp::ui::FileTreeCtrl::GetFocusedNodeId() const -> std::string
{
    if (focused_node_index_ < 0)
    {
        return {};
    }
    // We need a const version of GetVisibleNodes; build inline
    std::vector<const core::FileNode*> nodes;
    std::function<void(const std::vector<core::FileNode>&)> collect;
    collect = [&](const std::vector<core::FileNode>& src)
    {
        for (const auto& node : src)
        {
            if (!node.filter_visible)
            {
                continue;
            }
            nodes.push_back(&node);
            if (node.is_folder() && node.is_open)
            {
                collect(node.children);
            }
        }
    };
    collect(roots_);
    if (focused_node_index_ < static_cast<int>(nodes.size()))
    {
        return nodes[static_cast<size_t>(focused_node_index_)]->id;
    }
    return {};
}

// R3 Fix 2: Expand ancestor folders so that a given node_id becomes visible
auto markamp::ui::FileTreeCtrl::ExpandAncestors(const std::string& node_id) -> bool
{
    // Recursively walk the tree; on the way back up, open each ancestor
    std::function<bool(std::vector<core::FileNode>&)> expand;
    expand = [&](std::vector<core::FileNode>& nodes) -> bool
    {
        for (auto& node : nodes)
        {
            if (node.id == node_id)
            {
                return true; // Found
            }
            if (node.is_folder())
            {
                if (expand(node.children))
                {
                    node.is_open = true;
                    return true; // Ancestor
                }
            }
        }
        return false;
    };
    return expand(roots_);
}
