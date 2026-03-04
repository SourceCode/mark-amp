#include "OutlinePanel.h"

#include "../core/Events.h"

#include <wx/sizer.h>
#include <wx/stattext.h>

#include <algorithm>

namespace markamp::ui
{

OutlinePanel::OutlinePanel(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           core::EventBus& event_bus,
                           core::OutlineService& outline_service)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , outline_service_(outline_service)
    , search_debounce_timer_(this)
    , cursor_throttle_timer_(this)
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // ---- Search bar ----
    search_ctrl_ = new wxSearchCtrl(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    search_ctrl_->SetDescriptiveText("Filter outline");
    main_sizer->Add(search_ctrl_, 0, wxEXPAND | wxALL, 4);

    // ---- Stats label ----
    stats_label_ = new wxStaticText(this, wxID_ANY, "No headings");
    main_sizer->Add(stats_label_, 0, wxEXPAND | wxLEFT | wxRIGHT, 8);

    // ---- Tree control ----
    tree_ctrl_ = new wxTreeCtrl(this,
                                wxID_ANY,
                                wxDefaultPosition,
                                wxDefaultSize,
                                wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT |
                                    wxTR_SINGLE | wxTR_NO_LINES);
    main_sizer->Add(tree_ctrl_, 1, wxEXPAND | wxALL, 2);

    // ---- Empty state label (hidden by default) ----
    empty_label_ = new wxStaticText(this,
                                    wxID_ANY,
                                    "Open a document to view its outline.",
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    wxALIGN_CENTER_HORIZONTAL);
    empty_label_->Hide();
    main_sizer->Add(empty_label_, 1, wxEXPAND | wxALL, 20);

    SetSizer(main_sizer);

    // Bind events
    tree_ctrl_->Bind(wxEVT_TREE_SEL_CHANGED, &OutlinePanel::OnTreeSelectionChanged, this);
    search_ctrl_->Bind(wxEVT_TEXT, &OutlinePanel::OnSearchTextChanged, this);
    Bind(wxEVT_TIMER, &OutlinePanel::OnSearchDebounceTimer, this, search_debounce_timer_.GetId());
    Bind(wxEVT_TIMER, &OutlinePanel::OnCursorThrottleTimer, this, cursor_throttle_timer_.GetId());

    // Subscribe to outline changes
    outline_changed_sub_ = event_bus_.subscribe<core::events::OutlineChangedEvent>(
        [this](const core::events::OutlineChangedEvent& /*event*/)
        {
            is_stale_ = false;
            RebuildTree();
        });

    // Subscribe to cursor movement for follow-cursor
    cursor_moved_sub_ = event_bus_.subscribe<core::events::CursorPositionChangedEvent>(
        [this](const core::events::CursorPositionChangedEvent& event)
        {
            if (!follow_cursor_ || active_root_id_.empty())
            {
                return;
            }
            // Throttle cursor updates
            if (!cursor_throttle_timer_.IsRunning())
            {
                auto heading = outline_service_.get_heading_at_line(active_root_id_, event.line);
                if (heading.has_value())
                {
                    // Find and select the matching tree item
                    wxTreeItemIdValue cookie;
                    auto root = tree_ctrl_->GetRootItem();
                    auto item = tree_ctrl_->GetFirstChild(root, cookie);
                    while (item.IsOk())
                    {
                        auto* data = dynamic_cast<OutlineItemData*>(tree_ctrl_->GetItemData(item));
                        if (data != nullptr && data->entry.source_line == heading->source_line)
                        {
                            tree_ctrl_->SelectItem(item);
                            tree_ctrl_->EnsureVisible(item);
                            break;
                        }
                        item = tree_ctrl_->GetNextSibling(item);
                    }
                }
                cursor_throttle_timer_.StartOnce(100);
            }
        });

    ApplyTheme();
}

void OutlinePanel::ApplyTheme()
{
    const auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    const auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);

    SetBackgroundColour(bg_color);
    SetForegroundColour(fg_color);

    if (tree_ctrl_ != nullptr)
    {
        tree_ctrl_->SetBackgroundColour(bg_color);
        tree_ctrl_->SetForegroundColour(fg_color);
    }
    if (search_ctrl_ != nullptr)
    {
        search_ctrl_->SetBackgroundColour(bg_color);
        search_ctrl_->SetForegroundColour(fg_color);
    }
    if (stats_label_ != nullptr)
    {
        stats_label_->SetForegroundColour(fg_color);
    }
    if (empty_label_ != nullptr)
    {
        empty_label_->SetForegroundColour(fg_color);
    }

    Refresh();
}

void OutlinePanel::SetActiveDocument(const std::string& root_id)
{
    if (root_id == active_root_id_)
    {
        return;
    }

    // Save collapse state for current document
    if (!active_root_id_.empty())
    {
        SaveCollapseState();
    }

    active_root_id_ = root_id;

    if (root_id.empty())
    {
        ShowEmptyState("Open a document to view its outline.");
        return;
    }

    RebuildTree();
    RestoreCollapseState();
}

void OutlinePanel::set_sort_mode(OutlineSortMode mode)
{
    if (mode == sort_mode_)
    {
        return;
    }
    sort_mode_ = mode;
    RebuildTree();
}

void OutlinePanel::set_follow_cursor(bool enabled)
{
    follow_cursor_ = enabled;
}

auto OutlinePanel::heading_stats_text() const -> std::string
{
    if (active_root_id_.empty())
    {
        return "No headings";
    }

    auto outline = outline_service_.get_outline(active_root_id_);
    if (outline.is_empty())
    {
        return "No headings";
    }

    std::string result = std::to_string(outline.total_heading_count) + " headings";
    std::string level_detail;
    for (std::size_t level_idx = 0; level_idx < 6; ++level_idx)
    {
        if (outline.level_counts[level_idx] > 0)
        {
            if (!level_detail.empty())
            {
                level_detail += ", ";
            }
            level_detail += std::to_string(outline.level_counts[level_idx]) + " H" +
                            std::to_string(level_idx + 1);
        }
    }
    if (!level_detail.empty())
    {
        result += " (" + level_detail + ")";
    }
    return result;
}

void OutlinePanel::RebuildTree()
{
    RebuildTree(pending_search_query_);
}

void OutlinePanel::RebuildTree(const std::string& filter_query)
{
    if (active_root_id_.empty())
    {
        ShowEmptyState("Open a document to view its outline.");
        return;
    }

    std::vector<core::OutlineEntry> entries;

    if (filter_query.empty())
    {
        entries = outline_service_.get_flat_outline(active_root_id_);
    }
    else
    {
        entries = outline_service_.search_headings(active_root_id_, filter_query);
    }

    if (entries.empty() && filter_query.empty())
    {
        ShowEmptyState("No headings found in this document.\n\n"
                       "Add Markdown headings (# H1, ## H2, etc.) to see\n"
                       "the document structure here.");
        return;
    }

    if (entries.empty())
    {
        ShowEmptyState("No headings match '" + filter_query + "'.");
        return;
    }

    // Sort if needed
    if (sort_mode_ == OutlineSortMode::kByName)
    {
        std::sort(entries.begin(),
                  entries.end(),
                  [](const core::OutlineEntry& lhs, const core::OutlineEntry& rhs)
                  {
                      // Case-insensitive compare
                      auto lhs_lower = lhs.text;
                      auto rhs_lower = rhs.text;
                      std::transform(lhs_lower.begin(),
                                     lhs_lower.end(),
                                     lhs_lower.begin(),
                                     [](unsigned char chr) { return std::tolower(chr); });
                      std::transform(rhs_lower.begin(),
                                     rhs_lower.end(),
                                     rhs_lower.begin(),
                                     [](unsigned char chr) { return std::tolower(chr); });
                      return lhs_lower < rhs_lower;
                  });
    }

    HideEmptyState();
    PopulateTree(entries, filter_query);

    // Update stats label
    stats_label_->SetLabel(heading_stats_text());
}

void OutlinePanel::PopulateTree(const std::vector<core::OutlineEntry>& entries,
                                const std::string& /*filter_query*/)
{
    tree_ctrl_->Freeze();
    tree_ctrl_->DeleteAllItems();

    auto root_item = tree_ctrl_->AddRoot("Root");

    // In position-sort mode, build hierarchy using depth.
    // In name-sort mode, everything is flat.
    if (sort_mode_ == OutlineSortMode::kByName)
    {
        for (const auto& entry : entries)
        {
            auto icon_type = icon_for_heading_level(entry.level);
            auto label = outline_icon_char(icon_type) + "  " + entry.text + "  :" +
                         std::to_string(entry.source_line + 1);
            tree_ctrl_->AppendItem(root_item, label, -1, -1, new OutlineItemData(entry));
        }
    }
    else
    {
        // Build hierarchy: track parent items per depth level.
        std::vector<wxTreeItemId> depth_stack;
        depth_stack.push_back(root_item);

        for (const auto& entry : entries)
        {
            auto icon_type = icon_for_heading_level(entry.level);
            auto label = outline_icon_char(icon_type) + "  " + entry.text + "  :" +
                         std::to_string(entry.source_line + 1);

            // Determine parent: use depth to find the right parent
            int target_depth = entry.depth;
            while (static_cast<int>(depth_stack.size()) > target_depth + 1 &&
                   depth_stack.size() > 1)
            {
                depth_stack.pop_back();
            }

            auto parent_item = depth_stack.back();
            auto new_item =
                tree_ctrl_->AppendItem(parent_item, label, -1, -1, new OutlineItemData(entry));
            depth_stack.push_back(new_item);
        }
    }

    tree_ctrl_->ExpandAll();
    tree_ctrl_->Thaw();
}

void OutlinePanel::ShowEmptyState(const std::string& message)
{
    tree_ctrl_->Hide();
    empty_label_->SetLabel(message);
    empty_label_->Show();
    stats_label_->SetLabel("No headings");
    GetSizer()->Layout();
}

void OutlinePanel::HideEmptyState()
{
    empty_label_->Hide();
    tree_ctrl_->Show();
    GetSizer()->Layout();
}

void OutlinePanel::SaveCollapseState()
{
    // Store which items are collapsed for the current document
    // (simplified: store heading texts that are collapsed)
    if (active_root_id_.empty() || tree_ctrl_ == nullptr)
    {
        return;
    }

    std::set<std::string> collapsed;
    wxTreeItemIdValue cookie;
    auto root = tree_ctrl_->GetRootItem();
    auto item = tree_ctrl_->GetFirstChild(root, cookie);
    while (item.IsOk())
    {
        if (tree_ctrl_->ItemHasChildren(item) && !tree_ctrl_->IsExpanded(item))
        {
            auto* data = dynamic_cast<OutlineItemData*>(tree_ctrl_->GetItemData(item));
            if (data != nullptr)
            {
                collapsed.insert(data->entry.text);
            }
        }
        item = tree_ctrl_->GetNextSibling(item);
    }
    collapse_state_[active_root_id_] = std::move(collapsed);
}

void OutlinePanel::RestoreCollapseState()
{
    auto state_iter = collapse_state_.find(active_root_id_);
    if (state_iter == collapse_state_.end())
    {
        return;
    }

    const auto& collapsed = state_iter->second;
    wxTreeItemIdValue cookie;
    auto root = tree_ctrl_->GetRootItem();
    auto item = tree_ctrl_->GetFirstChild(root, cookie);
    while (item.IsOk())
    {
        auto* data = dynamic_cast<OutlineItemData*>(tree_ctrl_->GetItemData(item));
        if (data != nullptr && collapsed.count(data->entry.text) > 0)
        {
            tree_ctrl_->Collapse(item);
        }
        item = tree_ctrl_->GetNextSibling(item);
    }
}

void OutlinePanel::OnTreeSelectionChanged(wxTreeEvent& event)
{
    auto selected = event.GetItem();
    if (!selected.IsOk())
    {
        return;
    }

    auto* data = dynamic_cast<OutlineItemData*>(tree_ctrl_->GetItemData(selected));
    if (data == nullptr)
    {
        return;
    }

    // Emit scroll-to event
    if (data->entry.block_id.has_value())
    {
        core::events::OutlineScrollToEvent scroll_evt;
        scroll_evt.block_id = data->entry.block_id.value().value;
        event_bus_.publish(scroll_evt);
    }
}

void OutlinePanel::OnSearchTextChanged(wxCommandEvent& /*event*/)
{
    pending_search_query_ = search_ctrl_->GetValue().ToStdString();
    // Debounce: restart the timer
    search_debounce_timer_.StartOnce(200);
}

void OutlinePanel::OnSearchDebounceTimer(wxTimerEvent& /*event*/)
{
    RebuildTree(pending_search_query_);
}

void OutlinePanel::OnCursorThrottleTimer(wxTimerEvent& /*event*/)
{
    // Timer expired, next cursor event will process immediately
}

} // namespace markamp::ui
