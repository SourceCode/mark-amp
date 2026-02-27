#include "ui/EditorGroupManager.h"

#include "ui/DesignSystemContext.h"

#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/dnd.h>
#include <wx/graphics.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/tokenzr.h>

namespace markamp::ui
{

EditorGroupManager::EditorGroupManager(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       core::EventBus& event_bus,
                                       core::Config* config,
                                       core::IMermaidRenderer* mermaid_renderer,
                                       core::IMathRenderer* math_renderer,
                                       DesignSystemContext& ds_context)
    : ThemeAwareWindow(parent, theme_engine, wxID_ANY)
    , event_bus_(event_bus)
    , config_(config)
    , mermaid_renderer_(mermaid_renderer)
    , math_renderer_(math_renderer)
    , ds_context_(ds_context)
{
    // Task 3: Initialize with one single group
    root_ = std::make_unique<EditorGroupNode>();
    root_->id = next_group_id_++;

    root_->leaf = CreateGroupLeaf(root_->id);
    focused_group_id_ = root_->leaf->group_id;

    Bind(wxEVT_SIZE, &EditorGroupManager::OnSize, this);
    Bind(wxEVT_PAINT, &EditorGroupManager::OnPaint, this);
    Bind(wxEVT_MOTION, &EditorGroupManager::OnMouseEvent, this);
    Bind(wxEVT_LEFT_DOWN, &EditorGroupManager::OnMouseEvent, this);
    Bind(wxEVT_LEFT_UP, &EditorGroupManager::OnMouseEvent, this);
    Bind(wxEVT_LEAVE_WINDOW, &EditorGroupManager::OnMouseEvent, this);
    Bind(wxEVT_LEFT_DCLICK, &EditorGroupManager::OnMouseEvent, this);
    Bind(wxEVT_ENTER_WINDOW, &EditorGroupManager::OnMouseEvent, this);

    Bind(wxEVT_TIMER, &EditorGroupManager::OnSymbolUpdateTimer, this, symbol_update_timer_.GetId());

    split_req_sub_ = event_bus_.subscribe<core::events::EditorGroupSplitRequestEvent>(
        [this](const auto& evt) { OnSplitRequest(evt); });

    open_diff_sub_ = event_bus_.subscribe<core::events::OpenDiffRequestEvent>(
        [this](const auto& evt)
        {
            OpenDiffInGroup(focused_group_id_,
                            evt.left_path,
                            evt.right_path,
                            evt.left_content,
                            evt.right_content,
                            evt.title);
        });

    more_actions_sub_ = event_bus_.subscribe<core::events::EditorGroupMoreActionsEvent>(
        [this](const auto& evt) { OnMoreActions(evt); });

    focus_req_sub_ = event_bus_.subscribe<core::events::EditorGroupFocusRequestEvent>(
        [this](const auto& evt) { OnFocusGroupRequest(evt); });

    toggle_max_sub_ = event_bus_.subscribe<core::events::EditorGroupToggleMaximizeEvent>(
        [this](const auto& evt) { OnToggleMaximizeEvent(evt); });

    breadcrumb_nav_sub_ = event_bus_.subscribe<core::events::BreadcrumbNavigateEvent>(
        [this](const auto& evt) { OnBreadcrumbNavigate(evt); });

    open_git_log_sub_ = event_bus_.subscribe<core::events::OpenGitLogRequestEvent>(
        [this](const auto& evt) { OnOpenGitLogRequest(evt); });

    cursor_pos_sub_ = event_bus_.subscribe<core::events::CursorPositionChangedEvent>(
        [this](const auto& evt) { OnCursorPositionChanged(evt); });

    SetDropTarget(new EditorGroupDropTarget(this));
}

auto EditorGroupManager::CreateGroupLeaf(int group_id) -> std::unique_ptr<EditorGroupLeaf>
{
    auto leaf = std::make_unique<EditorGroupLeaf>();
    leaf->group_id = group_id;
    leaf->container = new wxPanel(this);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    leaf->tab_bar = new TabBar(leaf->container, ds_context_, event_bus_);
    leaf->breadcrumb = new BreadcrumbBar(leaf->container, ds_context_, event_bus_);
    leaf->editor = new EditorPanel(leaf->container, theme_engine(), event_bus_);
    leaf->diff_panel = new DiffPanel(leaf->container, event_bus_);
    leaf->git_log_panel = new GitLogPanel(leaf->container, theme_engine(), event_bus_, "");

    leaf->diff_panel->Hide();
    leaf->git_log_panel->Hide();

    // Phase 12 Task 19: Group Focus Follows Mouse
    auto on_enter = [this, group_id](wxMouseEvent& e)
    {
        if (config_ && config_->get_bool("editor.focus_follows_mouse", true))
        {
            SetFocusedGroup(group_id);
        }
        e.Skip();
    };

    leaf->container->Bind(wxEVT_ENTER_WINDOW, on_enter);
    leaf->tab_bar->Bind(wxEVT_ENTER_WINDOW, on_enter);
    leaf->breadcrumb->Bind(wxEVT_ENTER_WINDOW, on_enter);
    leaf->editor->Bind(wxEVT_ENTER_WINDOW, on_enter);
    leaf->diff_panel->Bind(wxEVT_ENTER_WINDOW, on_enter);
    leaf->git_log_panel->Bind(wxEVT_ENTER_WINDOW, on_enter);

    sizer->Add(leaf->tab_bar, 0, wxEXPAND);
    sizer->Add(leaf->breadcrumb, 0, wxEXPAND);
    sizer->Add(leaf->editor, 1, wxEXPAND);
    sizer->Add(leaf->diff_panel, 1, wxEXPAND);
    sizer->Add(leaf->git_log_panel, 1, wxEXPAND);
    leaf->container->SetSizer(sizer);

    // Phase 12 Task 25: Editor Group Events
    core::events::EditorGroupChangedEvent evt;
    evt.group_id = std::to_string(group_id);
    evt.action = "created";
    event_bus_.publish(evt);

    return leaf;
}

void EditorGroupManager::OnSize(wxSizeEvent& event)
{
    UpdateLayout();
    event.Skip();
}

void EditorGroupManager::UpdateLayout()
{
    if (root_)
    {
        if (maximized_group_id_ != -1)
        {
            auto rect = GetClientRect();
            auto apply_max = [&](auto& self, EditorGroupNode* node) -> void
            {
                if (node == nullptr)
                    return;
                if (node->leaf && node->leaf->container)
                {
                    if (node->leaf->group_id == maximized_group_id_)
                    {
                        node->leaf->container->SetSize(rect);
                        if (!node->leaf->container->IsShown())
                            node->leaf->container->Show();
                    }
                    else
                    {
                        node->leaf->container->SetSize(wxRect(0, 0, 0, 0));
                        if (node->leaf->container->IsShown())
                            node->leaf->container->Hide();
                    }
                }
                self(self, node->first_child.get());
                self(self, node->second_child.get());
            };
            apply_max(apply_max, root_.get());
        }
        else
        {
            // Restore visibility
            auto show_all = [&](auto& self, EditorGroupNode* node) -> void
            {
                if (node == nullptr)
                    return;
                if (node->leaf && node->leaf->container && !node->leaf->container->IsShown())
                {
                    node->leaf->container->Show();
                }
                self(self, node->first_child.get());
                self(self, node->second_child.get());
            };
            show_all(show_all, root_.get());

            LayoutNode(root_.get(), GetClientRect());
        }
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
void EditorGroupManager::LayoutNode(EditorGroupNode* node, const wxRect& rect)
{
    if (node == nullptr)
    {
        return;
    }

    if (node->leaf)
    {
        node->leaf->container->SetSize(rect);
    }
    else
    {
        const int kDividerSize = 4;
        if (node->split == SplitOrientation::kHorizontal)
        {
            const int kSplitPos = rect.x + static_cast<int>(rect.width * node->split_ratio);
            const wxRect kLeftRect(rect.x, rect.y, kSplitPos - rect.x, rect.height);
            const wxRect kRightRect(kSplitPos + kDividerSize,
                                    rect.y,
                                    rect.GetRight() - (kSplitPos + kDividerSize),
                                    rect.height);

            LayoutNode(node->first_child.get(), kLeftRect);
            LayoutNode(node->second_child.get(), kRightRect);
        }
        else
        {
            const int kSplitPos = rect.y + static_cast<int>(rect.height * node->split_ratio);
            const wxRect kTopRect(rect.x, rect.y, rect.width, kSplitPos - rect.y);
            const wxRect kBottomRect(rect.x,
                                     kSplitPos + kDividerSize,
                                     rect.width,
                                     rect.GetBottom() - (kSplitPos + kDividerSize));

            LayoutNode(node->first_child.get(), kTopRect);
            LayoutNode(node->second_child.get(), kBottomRect);
        }
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
auto EditorGroupManager::FindNode(EditorGroupNode* node, int group_id) -> EditorGroupNode*
{
    if (node == nullptr)
    {
        return nullptr;
    }
    if (node->leaf && node->leaf->group_id == group_id)
    {
        return node;
    }

    if (auto* found = FindNode(node->first_child.get(), group_id))
    {
        return found;
    }
    if (auto* found = FindNode(node->second_child.get(), group_id))
    {
        return found;
    }

    return nullptr;
}

void EditorGroupManager::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

// Stubs for later
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void EditorGroupManager::SplitActiveGroupRight()
{
    SplitGroup(focused_group_id_, SplitOrientation::kHorizontal);
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void EditorGroupManager::SplitActiveGroupDown()
{
    SplitGroup(focused_group_id_, SplitOrientation::kVertical);
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void EditorGroupManager::SplitGroup(int group_id, SplitOrientation orientation)
{
    EditorGroupNode* target = FindNode(root_.get(), group_id);
    if (target == nullptr || !target->leaf)
    {
        return;
    }

    // 1. Move leaf to first_child
    target->first_child = std::make_unique<EditorGroupNode>();
    target->first_child->leaf = std::move(target->leaf);

    // 2. Create new leaf for second_child
    target->second_child = std::make_unique<EditorGroupNode>();
    target->second_child->leaf = CreateGroupLeaf(next_group_id_++);

    // 3. Set split orientation
    target->split = orientation;
    target->split_ratio = 0.5;

    // 4. Force layout
    UpdateLayout();
    Refresh();
}

void EditorGroupManager::ResetGroupSizes()
{
    auto reset_node = [&](auto& self, EditorGroupNode* node) -> void
    {
        if (node == nullptr)
            return;
        if (!node->leaf)
        {
            node->split_ratio = 0.5;
            self(self, node->first_child.get());
            self(self, node->second_child.get());
        }
    };
    reset_node(reset_node, root_.get());
    UpdateLayout();
    Refresh();
}

void EditorGroupManager::ApplyGridLayout(EditorGridLayout layout)
{
    std::vector<std::unique_ptr<EditorGroupLeaf>> existing_leaves;

    auto collect_leaves = [&](auto& self, std::unique_ptr<EditorGroupNode>& node) -> void
    {
        if (node == nullptr)
        {
            return;
        }
        if (node->leaf)
        {
            existing_leaves.push_back(std::move(node->leaf));
        }
        self(self, node->first_child);
        self(self, node->second_child);
    };
    collect_leaves(collect_leaves, root_);

    root_ = std::make_unique<EditorGroupNode>();
    root_->id = next_group_id_++;

    auto get_next_leaf = [&]() -> std::unique_ptr<EditorGroupLeaf>
    {
        if (!existing_leaves.empty())
        {
            auto l = std::move(existing_leaves.front());
            existing_leaves.erase(existing_leaves.begin());
            return l;
        }
        return CreateGroupLeaf(next_group_id_++);
    };

    switch (layout)
    {
        case EditorGridLayout::kSingle:
            root_->leaf = get_next_leaf();
            break;

        case EditorGridLayout::kTwoColumns:
            root_->split = SplitOrientation::kHorizontal;
            root_->first_child = std::make_unique<EditorGroupNode>();
            root_->first_child->leaf = get_next_leaf();
            root_->second_child = std::make_unique<EditorGroupNode>();
            root_->second_child->leaf = get_next_leaf();
            break;

        case EditorGridLayout::kTwoRows:
            root_->split = SplitOrientation::kVertical;
            root_->first_child = std::make_unique<EditorGroupNode>();
            root_->first_child->leaf = get_next_leaf();
            root_->second_child = std::make_unique<EditorGroupNode>();
            root_->second_child->leaf = get_next_leaf();
            break;

        case EditorGridLayout::kGrid2x2:
            root_->split = SplitOrientation::kHorizontal;
            root_->first_child = std::make_unique<EditorGroupNode>();
            root_->first_child->split = SplitOrientation::kVertical;
            root_->first_child->first_child = std::make_unique<EditorGroupNode>();
            root_->first_child->first_child->leaf = get_next_leaf();
            root_->first_child->second_child = std::make_unique<EditorGroupNode>();
            root_->first_child->second_child->leaf = get_next_leaf();

            root_->second_child = std::make_unique<EditorGroupNode>();
            root_->second_child->split = SplitOrientation::kVertical;
            root_->second_child->first_child = std::make_unique<EditorGroupNode>();
            root_->second_child->first_child->leaf = get_next_leaf();
            root_->second_child->second_child = std::make_unique<EditorGroupNode>();
            root_->second_child->second_child->leaf = get_next_leaf();
            break;

        case EditorGridLayout::kLeftPlusTwoRight:
            root_->split = SplitOrientation::kHorizontal;
            root_->first_child = std::make_unique<EditorGroupNode>();
            root_->first_child->leaf = get_next_leaf();

            root_->second_child = std::make_unique<EditorGroupNode>();
            root_->second_child->split = SplitOrientation::kVertical;
            root_->second_child->first_child = std::make_unique<EditorGroupNode>();
            root_->second_child->first_child->leaf = get_next_leaf();
            root_->second_child->second_child = std::make_unique<EditorGroupNode>();
            root_->second_child->second_child->leaf = get_next_leaf();
            break;
    }

    for (auto& leaf : existing_leaves)
    {
        if (leaf && leaf->container)
        {
            leaf->container->Destroy();
        }
    }
    existing_leaves.clear();

    bool focus_found = false;
    auto check_focus = [&](auto& self, EditorGroupNode* node) -> void
    {
        if (node == nullptr)
        {
            return;
        }
        if (node->leaf && node->leaf->group_id == focused_group_id_)
        {
            focus_found = true;
        }
        self(self, node->first_child.get());
        self(self, node->second_child.get());
    };
    check_focus(check_focus, root_.get());

    if (!focus_found)
    {
        auto find_first_leaf = [&](auto& self, EditorGroupNode* node) -> EditorGroupNode*
        {
            if (node == nullptr)
            {
                return nullptr;
            }
            if (node->leaf)
            {
                return node;
            }
            if (auto* res = self(self, node->first_child.get()))
            {
                return res;
            }
            return self(self, node->second_child.get());
        };
        auto* first_leaf_node = find_first_leaf(find_first_leaf, root_.get());
        if (first_leaf_node != nullptr && first_leaf_node->leaf)
        {
            focused_group_id_ = first_leaf_node->leaf->group_id;
        }
    }

    UpdateLayout();
    Refresh();
}

void EditorGroupManager::OnSplitRequest(const core::events::EditorGroupSplitRequestEvent& evt)
{
    int target_group_id = -1;
    auto find_group = [&](auto& self, EditorGroupNode* node) -> void
    {
        if (node == nullptr)
        {
            return;
        }
        if (node->leaf && node->leaf->tab_bar &&
            node->leaf->tab_bar->GetId() == evt.source_tabbar_id)
        {
            target_group_id = node->leaf->group_id;
        }
        self(self, node->first_child.get());
        self(self, node->second_child.get());
    };
    find_group(find_group, root_.get());

    if (target_group_id != -1)
    {
        SplitGroup(target_group_id,
                   evt.is_horizontal_split ? SplitOrientation::kHorizontal
                                           : SplitOrientation::kVertical);
    }
}

void EditorGroupManager::OnMoreActions(const core::events::EditorGroupMoreActionsEvent& evt)
{
    int target_group_id = -1;
    auto find_group = [&](auto& self, EditorGroupNode* node) -> void
    {
        if (node == nullptr)
        {
            return;
        }
        if (node->leaf && node->leaf->tab_bar &&
            node->leaf->tab_bar->GetId() == evt.source_tabbar_id)
        {
            target_group_id = node->leaf->group_id;
        }
        self(self, node->first_child.get());
        self(self, node->second_child.get());
    };
    find_group(find_group, root_.get());

    if (target_group_id != -1)
    {
        wxMenu menu;
        menu.Append(1001, "Split Up");
        menu.Append(1002, "Split Down");
        menu.Append(1003, "Split Left");
        menu.Append(1004, "Split Right");
        menu.AppendSeparator();
        menu.Append(1005, "Close Group");

        menu.Bind(wxEVT_MENU,
                  [this, target_group_id](wxCommandEvent& e)
                  {
                      switch (e.GetId())
                      {
                          case 1001:
                          case 1002:
                              SplitGroup(target_group_id, SplitOrientation::kVertical);
                              break;
                          case 1003:
                          case 1004:
                              SplitGroup(target_group_id, SplitOrientation::kHorizontal);
                              break;
                          case 1005:
                              CloseGroup(target_group_id);
                              break;
                      }
                  });

        PopupMenu(&menu, ScreenToClient(wxPoint(evt.screen_x, evt.screen_y)));
    }
}

void EditorGroupManager::OnFocusGroupRequest(const core::events::EditorGroupFocusRequestEvent& evt)
{
    FocusGroupByIndex(evt.group_index);
}

void EditorGroupManager::OnToggleMaximizeEvent(
    const core::events::EditorGroupToggleMaximizeEvent& evt)
{
    (void)evt; // Used in binding
    if (maximized_group_id_ != -1)
    {
        maximized_group_id_ = -1;
    }
    else if (focused_group_id_ != -1)
    {
        // Only maximize if there is more than one group alive
        int leaf_count = 0;
        auto count_leaves = [&](auto& self, EditorGroupNode* node) -> void
        {
            if (!node)
                return;
            if (node->leaf)
                leaf_count++;
            self(self, node->first_child.get());
            self(self, node->second_child.get());
        };
        count_leaves(count_leaves, root_.get());

        if (leaf_count > 1)
        {
            maximized_group_id_ = focused_group_id_;
        }
    }

    UpdateLayout();
}

void EditorGroupManager::JoinGroupWithNext(int group_id)
{
    EditorGroupNode* src_node = FindNode(root_.get(), group_id);
    if (src_node == nullptr || src_node->leaf == nullptr)
        return;

    EditorGroupNode* target_leaf = nullptr;
    auto find_sibling = [&](auto& self, EditorGroupNode* node) -> void
    {
        if (node == nullptr)
            return;
        if (node->leaf && node->leaf->group_id != group_id)
        {
            target_leaf = node;
        }
        else if (target_leaf == nullptr && !node->leaf)
        {
            self(self, node->first_child.get());
            self(self, node->second_child.get());
        }
    };
    find_sibling(find_sibling, root_.get());

    if (target_leaf != nullptr && target_leaf->leaf != nullptr)
    {
        for (const auto& tab : src_node->leaf->tab_bar->tabs())
        {
            OpenFileInGroup(target_leaf->leaf->group_id, tab.file_path);
        }
    }
    CloseGroup(group_id);
}

void EditorGroupManager::CloseGroup(int group_id)
{
    EditorGroupNode* target = nullptr;
    EditorGroupNode* parent = nullptr;

    auto find_target_and_parent =
        [&](auto& self, EditorGroupNode* current, EditorGroupNode* current_parent) -> void
    {
        if (current == nullptr)
            return;
        if (current->leaf && current->leaf->group_id == group_id)
        {
            target = current;
            parent = current_parent;
            return;
        }
        if (target == nullptr)
            self(self, current->first_child.get(), current);
        if (target == nullptr)
            self(self, current->second_child.get(), current);
    };

    find_target_and_parent(find_target_and_parent, root_.get(), nullptr);

    if (target == nullptr)
        return;

    if (parent == nullptr)
    {
        // Cannot close the root/only split leaf
        return;
    }

    if (target->leaf && target->leaf->container)
    {
        target->leaf->container->Destroy();
    }

    if (maximized_group_id_ == group_id)
    {
        maximized_group_id_ = -1;
    }

    std::unique_ptr<EditorGroupNode> sibling;
    if (parent->first_child.get() == target)
    {
        sibling = std::move(parent->second_child);
    }
    else
    {
        sibling = std::move(parent->first_child);
    }

    parent->split = sibling->split;
    parent->split_ratio = sibling->split_ratio;
    parent->leaf = std::move(sibling->leaf);
    parent->first_child = std::move(sibling->first_child);
    parent->second_child = std::move(sibling->second_child);

    if (focused_group_id_ == group_id)
    {
        auto find_first_leaf = [&](auto& self, EditorGroupNode* node) -> EditorGroupNode*
        {
            if (node == nullptr)
                return nullptr;
            if (node->leaf)
                return node;
            if (auto* res = self(self, node->first_child.get()))
                return res;
            return self(self, node->second_child.get());
        };
        auto* first_leaf_node = find_first_leaf(find_first_leaf, parent);
        if (first_leaf_node != nullptr && first_leaf_node->leaf)
        {
            SetFocusedGroup(first_leaf_node->leaf->group_id);
        }
    }

    // Phase 12 Task 25: Editor Group Events
    core::events::EditorGroupChangedEvent evt;
    evt.group_id = std::to_string(group_id);
    evt.action = "closed";
    event_bus_.publish(evt);

    UpdateLayout();
    Refresh();
}

void EditorGroupManager::GoBackInGroupHistory()
{
    if (history_index_ > 0 && history_index_ < static_cast<int>(focus_history_.size()))
    {
        history_index_--;
        int back_id = focus_history_[static_cast<std::size_t>(history_index_)];
        SetFocusedGroup(back_id);
    }
}

void EditorGroupManager::GoForwardInGroupHistory()
{
    if (history_index_ >= 0 && history_index_ < static_cast<int>(focus_history_.size()) - 1)
    {
        history_index_++;
        int forward_id = focus_history_[static_cast<std::size_t>(history_index_)];
        SetFocusedGroup(forward_id);
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void EditorGroupManager::SetFocusedGroup(int group_id)
{
    if (focused_group_id_ == group_id)
        return;

    // Task 23: History
    if (history_index_ < 0 || static_cast<std::size_t>(history_index_) >= focus_history_.size() ||
        focus_history_[static_cast<std::size_t>(history_index_)] != group_id)
    {
        if (history_index_ >= 0 && history_index_ < static_cast<int>(focus_history_.size()) - 1)
        {
            focus_history_.erase(focus_history_.begin() + history_index_ + 1, focus_history_.end());
        }
        focus_history_.push_back(group_id);
        history_index_ = static_cast<int>(focus_history_.size()) - 1;
    }

    focused_group_id_ = group_id;

    // Phase 12 Task 25: Editor Group Events
    core::events::EditorGroupChangedEvent evt;
    evt.group_id = std::to_string(group_id);
    evt.action = "focused";
    event_bus_.publish(evt);

    auto update_focus = [&](auto& self, EditorGroupNode* node) -> void
    {
        if (node == nullptr)
        {
            return;
        }
        if (node->leaf)
        {
            const bool is_focused = (node->leaf->group_id == group_id);
            node->leaf->is_focused = is_focused;

            if (node->leaf->tab_bar)
            {
                node->leaf->tab_bar->set_group_focused(is_focused);
            }
            if (is_focused && node->leaf->editor)
            {
                node->leaf->editor->SetFocus();
            }
        }
        self(self, node->first_child.get());
        self(self, node->second_child.get());
    };

    update_focus(update_focus, root_.get());
}
auto EditorGroupManager::GetFocusedGroupId() const -> int
{
    return focused_group_id_;
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto EditorGroupManager::GetGroupCount() const -> int
{
    return 1;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void EditorGroupManager::OpenFileInGroup(int group_id, const std::string& path)
{
    EditorGroupNode* target = FindNode(root_.get(), group_id);
    if (target == nullptr || target->leaf == nullptr)
        return;

    // Phase 12 Tasks 13/14: Populate group-specific Tab/Breadcrumbs
    std::string display_name;
    if (path.rfind("untitled:", 0) == 0)
    {
        display_name = path.substr(9);
        if (display_name.empty())
            display_name = "Untitled.md";
    }
    else
    {
        display_name = std::filesystem::path(path).filename().string();
    }

    if (target->leaf->diff_panel && target->leaf->diff_panel->IsShown())
        target->leaf->diff_panel->Hide();

    if (target->leaf->git_log_panel && target->leaf->git_log_panel->IsShown())
        target->leaf->git_log_panel->Hide();

    if (target->leaf->editor && !target->leaf->editor->IsShown())
        target->leaf->editor->Show();

    if (target->leaf->tab_bar)
    {
        target->leaf->tab_bar->AddTab(path, display_name);
        target->leaf->tab_bar->SetActiveTab(path);
    }

    if (target->leaf->breadcrumb)
    {
        std::vector<std::string> segments;
        if (path.rfind("untitled:", 0) == 0)
        {
            segments.push_back(display_name);
        }
        target->leaf->breadcrumb->SetFilePath(path, "");
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void EditorGroupManager::OpenFileInFocusedGroup(const std::string& path)
{
    OpenFileInGroup(focused_group_id_, path);
}

void EditorGroupManager::OpenDiffInGroup(int group_id,
                                         const std::string& left_path,
                                         const std::string& right_path,
                                         const std::string& left_content,
                                         const std::string& right_content,
                                         const std::string& title)
{
    EditorGroupNode* target = FindNode(root_.get(), group_id);
    if (target == nullptr || target->leaf == nullptr)
        return;

    std::string display_name = title;
    if (display_name.empty())
    {
        display_name = std::filesystem::path(left_path).filename().string() + " (Diff)";
    }

    if (target->leaf->tab_bar)
    {
        // Don't hide the tab bar, add a diff tab instead
        target->leaf->tab_bar->Show();
        std::string diff_id = "diff:" + left_path + ":" + right_path;
        target->leaf->tab_bar->AddTab(diff_id, display_name);
        target->leaf->tab_bar->SetActiveTab(diff_id);
    }

    if (target->leaf->editor)
        target->leaf->editor->Hide();

    if (target->leaf->git_log_panel && target->leaf->git_log_panel->IsShown())
        target->leaf->git_log_panel->Hide();

    if (target->leaf->diff_panel)
    {
        target->leaf->diff_panel->Show();
        if (!left_content.empty() || !right_content.empty())
        {
            target->leaf->diff_panel->open_diff_content(left_content, right_content);
        }
        else
        {
            target->leaf->diff_panel->open_diff(left_path, right_path);
        }
    }

    target->leaf->container->Layout();
}

void EditorGroupManager::OnOpenGitLogRequest(const core::events::OpenGitLogRequestEvent& evt)
{
    OpenGitLogInGroup(focused_group_id_, evt.workspace_root);
}

void EditorGroupManager::OpenGitLogInGroup(int group_id, const std::string& workspace_root)
{
    EditorGroupNode* target = FindNode(root_.get(), group_id);
    if (target == nullptr || target->leaf == nullptr)
        return;

    if (target->leaf->tab_bar)
    {
        target->leaf->tab_bar->Show();
        std::string log_id = "git-log:" + workspace_root;
        target->leaf->tab_bar->AddTab(log_id, "Git Log");
        target->leaf->tab_bar->SetActiveTab(log_id);
    }

    if (target->leaf->editor)
        target->leaf->editor->Hide();

    if (target->leaf->diff_panel && target->leaf->diff_panel->IsShown())
        target->leaf->diff_panel->Hide();

    if (target->leaf->git_log_panel)
    {
        target->leaf->git_log_panel->SetWorkspaceRoot(workspace_root);
        target->leaf->git_log_panel->Show();
    }

    if (target->leaf->container)
    {
        target->leaf->container->Layout();
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void EditorGroupManager::FocusNextGroup() {}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void EditorGroupManager::FocusPreviousGroup() {}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void EditorGroupManager::FocusGroupByIndex(int index)
{
    // Collect all leaf nodes
    std::vector<EditorGroupNode*> leaves;
    auto collect = [&](auto& self, EditorGroupNode* node) -> void
    {
        if (node == nullptr)
            return;
        if (node->leaf)
            leaves.push_back(node);
        self(self, node->first_child.get());
        self(self, node->second_child.get());
    };
    collect(collect, root_.get());

    if (index >= 0 && index < static_cast<int>(leaves.size()))
    {
        SetFocusedGroup(leaves[static_cast<size_t>(index)]->leaf->group_id);
    }
}

void EditorGroupManager::OnBreadcrumbNavigate(const core::events::BreadcrumbNavigateEvent& evt)
{
    EditorGroupNode* target = FindNode(root_.get(), focused_group_id_);
    if (target == nullptr || target->leaf == nullptr)
        return;

    if (!evt.heading_text.empty() && target->leaf->editor)
    {
        target->leaf->editor->GoToHeading(evt.source_line);
        target->leaf->editor->SetFocus();
    }
    else if (!evt.root_id.empty())
    {
        core::events::FileOpenRequestEvent open_evt;
        open_evt.file_path = evt.root_id;
        event_bus_.publish(open_evt);
    }
}

void EditorGroupManager::OnCursorPositionChanged(
    const core::events::CursorPositionChangedEvent& /*evt*/)
{
    // Debounce the symbol hierarchy extraction to avoid parsing delays on every keystroke
    symbol_update_timer_.StartOnce(300);
}

void EditorGroupManager::OnSymbolUpdateTimer(wxTimerEvent& /*evt*/)
{
    // Update the breadcrumb symbol path based on the focused editor's cursor
    EditorGroupNode* target = FindNode(root_.get(), focused_group_id_);
    if (target == nullptr || target->leaf == nullptr || target->leaf->editor == nullptr ||
        target->leaf->breadcrumb == nullptr)
    {
        return;
    }

    auto symbols = target->leaf->editor->GetHeadingSymbols();
    const int kActiveLine = target->leaf->editor->GetCursorLine();

    std::vector<BreadcrumbBar::SymbolItem> breadcrumb_symbols;
    breadcrumb_symbols.reserve(symbols.size());
    for (const auto& sym : symbols)
    {
        breadcrumb_symbols.push_back({sym.text, sym.level, sym.line});
    }

    target->leaf->breadcrumb->SetDocumentSymbols(breadcrumb_symbols, kActiveLine);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto EditorGroupManager::GetEditorForGroup(int group_id) -> EditorPanel*
{
    if (root_ && root_->leaf && root_->leaf->group_id == group_id)
    {
        return root_->leaf->editor;
    }
    return nullptr;
}

auto EditorGroupManager::GetFocusedEditor() -> EditorPanel*
{
    if (auto* node = FindNode(root_.get(), focused_group_id_))
    {
        if (node->leaf)
            return node->leaf->editor;
    }
    return nullptr;
}

auto EditorGroupManager::GetFocusedTabBar() -> TabBar*
{
    if (auto* node = FindNode(root_.get(), focused_group_id_))
    {
        if (node->leaf)
            return node->leaf->tab_bar;
    }
    return nullptr;
}

auto EditorGroupManager::GetFocusedBreadcrumbBar() -> BreadcrumbBar*
{
    if (auto* node = FindNode(root_.get(), focused_group_id_))
    {
        if (node->leaf)
            return node->leaf->breadcrumb;
    }
    return nullptr;
}

auto EditorGroupManager::GetAllGroupIds() const -> std::vector<int>
{
    std::vector<int> ids;
    auto collect = [&](auto& self, EditorGroupNode* node) -> void
    {
        if (node == nullptr)
            return;
        if (node->leaf)
            ids.push_back(node->leaf->group_id);
        self(self, node->first_child.get());
        self(self, node->second_child.get());
    };
    collect(collect, root_.get());
    return ids;
}

// ----------------------------------------------------------------------------
// Divider Drag & Visuals
// ----------------------------------------------------------------------------

static const int kDividerHitTestMargin = 6;
static const int kDividerDrawSize = 4;

auto EditorGroupManager::HitTestDivider(EditorGroupNode* node,
                                        const wxPoint& pos,
                                        const wxRect& rect) -> EditorGroupNode*
{
    if (node == nullptr || node->leaf)
        return nullptr;

    if (node->split == SplitOrientation::kHorizontal)
    {
        const int split_x = rect.x + static_cast<int>(rect.width * node->split_ratio);
        if (pos.x >= split_x - kDividerHitTestMargin &&
            pos.x <= split_x + kDividerDrawSize + kDividerHitTestMargin)
        {
            if (pos.y >= rect.y && pos.y <= rect.GetBottom())
            {
                return node;
            }
        }

        const wxRect left_rect(rect.x, rect.y, split_x - rect.x, rect.height);
        if (auto* found = HitTestDivider(node->first_child.get(), pos, left_rect))
            return found;

        const wxRect right_rect(split_x + kDividerDrawSize,
                                rect.y,
                                rect.GetRight() - (split_x + kDividerDrawSize),
                                rect.height);
        return HitTestDivider(node->second_child.get(), pos, right_rect);
    }
    else
    {
        const int split_y = rect.y + static_cast<int>(rect.height * node->split_ratio);
        if (pos.y >= split_y - kDividerHitTestMargin &&
            pos.y <= split_y + kDividerDrawSize + kDividerHitTestMargin)
        {
            if (pos.x >= rect.x && pos.x <= rect.GetRight())
            {
                return node;
            }
        }

        const wxRect top_rect(rect.x, rect.y, rect.width, split_y - rect.y);
        if (auto* found = HitTestDivider(node->first_child.get(), pos, top_rect))
            return found;

        const wxRect bottom_rect(rect.x,
                                 split_y + kDividerDrawSize,
                                 rect.width,
                                 rect.GetBottom() - (split_y + kDividerDrawSize));
        return HitTestDivider(node->second_child.get(), pos, bottom_rect);
    }
}

void EditorGroupManager::DrawDividers(wxDC& dc, EditorGroupNode* node, const wxRect& rect)
{
    if (node == nullptr || node->leaf)
        return;

    if (node->split == SplitOrientation::kHorizontal)
    {
        const int split_x = rect.x + static_cast<int>(rect.width * node->split_ratio);
        dc.DrawRectangle(split_x, rect.y, kDividerDrawSize, rect.height);

        const wxRect left_rect(rect.x, rect.y, split_x - rect.x, rect.height);
        DrawDividers(dc, node->first_child.get(), left_rect);

        const wxRect right_rect(split_x + kDividerDrawSize,
                                rect.y,
                                rect.GetRight() - (split_x + kDividerDrawSize),
                                rect.height);
        DrawDividers(dc, node->second_child.get(), right_rect);
    }
    else
    {
        const int split_y = rect.y + static_cast<int>(rect.height * node->split_ratio);
        dc.DrawRectangle(rect.x, split_y, rect.width, kDividerDrawSize);

        const wxRect top_rect(rect.x, rect.y, rect.width, split_y - rect.y);
        DrawDividers(dc, node->first_child.get(), top_rect);

        const wxRect bottom_rect(rect.x,
                                 split_y + kDividerDrawSize,
                                 rect.width,
                                 rect.GetBottom() - (split_y + kDividerDrawSize));
        DrawDividers(dc, node->second_child.get(), bottom_rect);
    }
}

void EditorGroupManager::OnPaint(wxPaintEvent& /*evt*/)
{
    wxAutoBufferedPaintDC dc(this);
    dc.SetBackground(wxBrush(theme_engine().color(core::ThemeColorToken::BgApp)));
    dc.Clear();

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::BorderLight)));

    if (maximized_group_id_ == -1 && root_)
    {
        DrawDividers(dc, root_.get(), GetClientRect());
    }
}

void EditorGroupManager::OnMouseEvent(wxMouseEvent& evt)
{
    if (maximized_group_id_ != -1)
    {
        evt.Skip();
        return;
    }

    const wxPoint pos = evt.GetPosition();

    if (evt.LeftDown())
    {
        if (auto* node = HitTestDivider(root_.get(), pos, GetClientRect()))
        {
            dragging_node_ = node;
            is_dragging_ = true;
            CaptureMouse();
        }
        else
        {
            evt.Skip();
        }
    }
    else if (evt.LeftUp())
    {
        if (is_dragging_)
        {
            is_dragging_ = false;
            dragging_node_ = nullptr;
            if (HasCapture())
                ReleaseMouse();
        }
        else
        {
            evt.Skip();
        }
    }
    else if (evt.LeftDClick())
    {
        if (auto* node = HitTestDivider(root_.get(), pos, GetClientRect()))
        {
            node->split_ratio = 0.5;
            UpdateLayout();
            Refresh();
        }
        else
        {
            evt.Skip();
        }
    }
    else if (evt.Dragging() && is_dragging_ && dragging_node_ != nullptr)
    {
        auto client_rect = GetClientRect();
        wxRect node_rect;

        auto find_rect =
            [&](auto& self, EditorGroupNode* current, const wxRect& current_rect) -> wxRect
        {
            if (current == dragging_node_)
                return current_rect;
            if (current == nullptr || current->leaf)
                return wxRect();

            if (current->split == SplitOrientation::kHorizontal)
            {
                const int split_x =
                    current_rect.x + static_cast<int>(current_rect.width * current->split_ratio);
                wxRect r1 = self(self,
                                 current->first_child.get(),
                                 wxRect(current_rect.x,
                                        current_rect.y,
                                        split_x - current_rect.x,
                                        current_rect.height));
                if (r1.width > 0)
                    return r1;

                wxRect r2 = self(self,
                                 current->second_child.get(),
                                 wxRect(split_x + kDividerDrawSize,
                                        current_rect.y,
                                        current_rect.GetRight() - (split_x + kDividerDrawSize),
                                        current_rect.height));
                return r2;
            }
            else
            {
                const int split_y =
                    current_rect.y + static_cast<int>(current_rect.height * current->split_ratio);
                wxRect r1 = self(self,
                                 current->first_child.get(),
                                 wxRect(current_rect.x,
                                        current_rect.y,
                                        current_rect.width,
                                        split_y - current_rect.y));
                if (r1.height > 0)
                    return r1;

                wxRect r2 = self(self,
                                 current->second_child.get(),
                                 wxRect(current_rect.x,
                                        split_y + kDividerDrawSize,
                                        current_rect.width,
                                        current_rect.GetBottom() - (split_y + kDividerDrawSize)));
                return r2;
            }
        };

        node_rect = find_rect(find_rect, root_.get(), client_rect);

        if (dragging_node_->split == SplitOrientation::kHorizontal)
        {
            if (node_rect.width > 0)
            {
                double ratio = static_cast<double>(pos.x - node_rect.x) / node_rect.width;
                if (ratio < 0.1)
                    ratio = 0.1;
                if (ratio > 0.9)
                    ratio = 0.9;
                dragging_node_->split_ratio = ratio;
                UpdateLayout();
                Refresh();
            }
        }
        else
        {
            if (node_rect.height > 0)
            {
                double ratio = static_cast<double>(pos.y - node_rect.y) / node_rect.height;
                if (ratio < 0.1)
                    ratio = 0.1;
                if (ratio > 0.9)
                    ratio = 0.9;
                dragging_node_->split_ratio = ratio;
                UpdateLayout();
                Refresh();
            }
        }
    }
    else if (evt.Moving() || evt.Entering())
    {
        if (auto* node = HitTestDivider(root_.get(), pos, GetClientRect()))
        {
            if (node->split == SplitOrientation::kHorizontal)
            {
                SetCursor(wxCursor(wxCURSOR_SIZEWE));
            }
            else
            {
                SetCursor(wxCursor(wxCURSOR_SIZENS));
            }
        }
        else
        {
            SetCursor(wxNullCursor);
            evt.Skip();
        }
    }
    else if (evt.Leaving())
    {
        if (!is_dragging_)
            SetCursor(wxNullCursor);
        evt.Skip();
    }
    else
    {
        evt.Skip();
    }
}

EditorGroupDropTarget::EditorGroupDropTarget(EditorGroupManager* manager)
    : manager_(manager)
{
}

bool EditorGroupDropTarget::OnDropText(wxCoord coord_x, wxCoord coord_y, const wxString& text)
{
    if (!text.StartsWith("markamp_tab:"))
    {
        return false;
    }

    // Parse markamp_tab:PATH:ID
    wxStringTokenizer tokenizer(text, ":");
    const wxString prefix = tokenizer.GetNextToken();
    const wxString path = tokenizer.GetNextToken();
    const wxString source_id_str = tokenizer.GetNextToken();

    if (path.IsEmpty())
    {
        return false;
    }

    // Find dropping quadrant on manager_
    const wxSize client_size = manager_->GetClientSize();
    if (client_size.GetWidth() == 0 || client_size.GetHeight() == 0)
    {
        return false;
    }

    const float ratio_x = static_cast<float>(coord_x) / static_cast<float>(client_size.GetWidth());
    const float ratio_y = static_cast<float>(coord_y) / static_cast<float>(client_size.GetHeight());

    // For Phase 12 just use fixed logic: dropping in left 20% splits left, etc.
    // For single group, split.
    SplitOrientation split_orient = SplitOrientation::kHorizontal;
    if (ratio_x > 0.8F)
    {
        split_orient = SplitOrientation::kHorizontal; // Split Right
    }
    else if (ratio_y > 0.8F)
    {
        split_orient = SplitOrientation::kVertical; // Split Down
    }
    else
    {
        // Drag to move (Task 20)
        // If dropped in center, open file in focused group, but ideally the group at x, y
        // For now, let's just assume move to focused group if ratio is in center
        manager_->OpenFileInFocusedGroup(path.ToStdString());
        return true;
    }

    manager_->SplitGroup(manager_->GetFocusedGroupId(), split_orient);
    // Move the tab to new group
    manager_->OpenFileInFocusedGroup(path.ToStdString());

    return true; // Returns true to trigger RemoveTab on source
}

// ----------------------------------------------------------------------------
// State Persistence
// ----------------------------------------------------------------------------

auto EditorGroupManager::SerializeState() const -> nlohmann::json
{
    auto serialize_node = [&](auto& self, EditorGroupNode* node) -> nlohmann::json
    {
        if (node == nullptr)
        {
            return nlohmann::json();
        }

        nlohmann::json res;
        res["id"] = node->id;
        res["split"] = (node->split == SplitOrientation::kHorizontal) ? "horizontal" : "vertical";
        res["split_ratio"] = node->split_ratio;

        if (node->leaf)
        {
            res["type"] = "leaf";
            res["group_id"] = node->leaf->group_id;

            if (node->leaf->tab_bar)
            {
                res["open_files"] = node->leaf->tab_bar->GetAllTabPaths();
                res["active_file"] = node->leaf->tab_bar->GetActiveTabPath();
            }
            res["is_focused"] = (node->leaf->group_id == focused_group_id_);
        }
        else
        {
            res["type"] = "node";
            res["first_child"] = self(self, node->first_child.get());
            res["second_child"] = self(self, node->second_child.get());
        }
        return res;
    };

    nlohmann::json state;
    state["focused_group_id"] = focused_group_id_;
    state["next_group_id"] = next_group_id_;
    state["maximized_group_id"] = maximized_group_id_;

    if (root_)
    {
        state["root"] = serialize_node(serialize_node, root_.get());
    }

    return state;
}

void EditorGroupManager::RestoreState(const nlohmann::json& state)
{
    if (state.is_null())
    {
        return;
    }

    focused_group_id_ = state.value("focused_group_id", 1);
    next_group_id_ = state.value("next_group_id", 1);
    maximized_group_id_ = state.value("maximized_group_id", -1);

    auto restore_node = [&](auto& self,
                            const nlohmann::json& j_node) -> std::unique_ptr<EditorGroupNode>
    {
        if (j_node.is_null() || !j_node.is_object())
        {
            return nullptr;
        }

        auto node = std::make_unique<EditorGroupNode>();
        node->id = j_node.value("id", 0);
        node->split = j_node.value("split", "horizontal") == "horizontal"
                          ? SplitOrientation::kHorizontal
                          : SplitOrientation::kVertical;
        node->split_ratio = j_node.value("split_ratio", 0.5);

        const std::string type = j_node.value("type", "leaf");
        if (type == "leaf")
        {
            const int group_id = j_node.value("group_id", node->id);
            node->leaf = CreateGroupLeaf(group_id);

            if (j_node.contains("open_files") && j_node["open_files"].is_array())
            {
                node->leaf->open_files = j_node["open_files"].get<std::vector<std::string>>();
            }
            node->leaf->active_file = j_node.value("active_file", "");
        }
        else
        {
            if (j_node.contains("first_child"))
            {
                node->first_child = self(self, j_node["first_child"]);
            }
            if (j_node.contains("second_child"))
            {
                node->second_child = self(self, j_node["second_child"]);
            }
        }
        return node;
    };

    if (state.contains("root"))
    {
        root_ = restore_node(restore_node, state["root"]);

        // Reopen files in the tree
        auto reopen_files = [&](auto& self, EditorGroupNode* node) -> void
        {
            if (node == nullptr)
                return;
            if (node->leaf)
            {
                const std::vector<std::string> files_to_open = node->leaf->open_files;
                node->leaf->open_files.clear();

                for (const auto& path : files_to_open)
                {
                    OpenFileInGroup(node->leaf->group_id, path);
                }

                if (!node->leaf->active_file.empty())
                {
                    OpenFileInGroup(node->leaf->group_id, node->leaf->active_file);
                }
            }
            self(self, node->first_child.get());
            self(self, node->second_child.get());
        };

        reopen_files(reopen_files, root_.get());
    }

    UpdateLayout();
    Refresh();
}

} // namespace markamp::ui
