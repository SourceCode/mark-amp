#include "ui/CanvasWorkspacePanel.h"

#include "core/Config.h"
#include "core/Events.h"
#include "core/Logger.h"
#include "ui/CanvasPanel.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

namespace markamp::ui
{

// ── Tool descriptors ──────────────────────────────────────────────

struct ToolDescriptor
{
    std::string name;
    std::string icon; // Unicode icon placeholder
    std::string tooltip;
};

static const std::vector<ToolDescriptor>& tool_descriptors()
{
    static const std::vector<ToolDescriptor> tools = {
        {"Select", "⇲", "Select & Move (V)"},
        {"Pan", "✋", "Pan Canvas (H)"},
        {"Sticky", "📝", "Sticky Note (S)"},
        {"Text", "T", "Text Box (T)"},
        {"Shape", "◻", "Shape (R)"},
        {"Connector", "↗", "Connector (C)"},
        {"Draw", "✏", "Freehand Draw (P)"},
        {"Erase", "⌫", "Erase (E)"},
    };
    return tools;
}

// ── Constructor & Layout ──────────────────────────────────────────

CanvasWorkspacePanel::CanvasWorkspacePanel(wxWindow* parent,
                                           core::EventBus& event_bus,
                                           core::ThemeEngine& theme_engine,
                                           core::Config* config)
    : wxPanel(parent, wxID_ANY)
    , event_bus_(event_bus)
    , theme_engine_(theme_engine)
    , config_(config)
{
    // Restore persisted state from config
    if (config_ != nullptr)
    {
        inspector_visible_ = config_->get_bool("canvas.inspector_visible", true);
        minimap_visible_ = config_->get_bool("canvas.minimap_visible", true);
    }

    CreateLayout();
    SubscribeEvents();
    ApplyTheme();
    MARKAMP_LOG_INFO("CanvasWorkspacePanel created — inspector:{} minimap:{}",
                     inspector_visible_,
                     minimap_visible_);
}

// ── Layout Construction ───────────────────────────────────────────

void CanvasWorkspacePanel::CreateLayout()
{
    root_sizer_ = new wxBoxSizer(wxVERTICAL);

    // Top: contextual bar
    CreateContextBar();
    root_sizer_->Add(context_bar_, 0, wxEXPAND);

    // Middle: tool rail | canvas | inspector
    middle_sizer_ = new wxBoxSizer(wxHORIZONTAL);

    CreateToolRail();
    middle_sizer_->Add(tool_rail_, 0, wxEXPAND);

    // Canvas panel (center, flex)
    auto event_bus_shared = std::make_shared<core::EventBus>();
    canvas_panel_ = new canvas::CanvasPanel(this, wxID_ANY, event_bus_shared);
    middle_sizer_->Add(canvas_panel_, 1, wxEXPAND);

    CreateInspector();
    middle_sizer_->Add(inspector_, 0, wxEXPAND);

    root_sizer_->Add(middle_sizer_, 1, wxEXPAND);

    // Bottom: minimap strip
    CreateMinimapStrip();
    root_sizer_->Add(minimap_strip_, 0, wxEXPAND);

    SetSizer(root_sizer_);
}

void CanvasWorkspacePanel::CreateContextBar()
{
    context_bar_ = new wxPanel(this, wxID_ANY);
    context_bar_->SetMinSize(wxSize(-1, kContextBarHeight));

    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    board_title_label_ = new wxStaticText(context_bar_, wxID_ANY, "Untitled Board");
    sizer->Add(board_title_label_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 12);

    sizer->AddStretchSpacer(1);

    zoom_label_ = new wxStaticText(context_bar_, wxID_ANY, "100%");
    sizer->Add(zoom_label_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    context_bar_->SetSizer(sizer);
}

void CanvasWorkspacePanel::CreateToolRail()
{
    tool_rail_ = new wxPanel(this, wxID_ANY);
    tool_rail_->SetMinSize(wxSize(kToolRailWidth, -1));

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->AddSpacer(8);

    const auto& tools = tool_descriptors();
    for (size_t idx = 0; idx < tools.size(); ++idx)
    {
        auto* btn = new wxButton(tool_rail_,
                                 wxID_ANY,
                                 tools[idx].icon,
                                 wxDefaultPosition,
                                 wxSize(kToolRailWidth - 4, kToolRailWidth - 4));
        btn->SetToolTip(tools[idx].tooltip);

        const int tool_idx = static_cast<int>(idx);
        btn->Bind(wxEVT_BUTTON,
                  [this, tool_idx](wxCommandEvent& /*evt*/) { OnToolSelected(tool_idx); });

        sizer->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 2);
    }

    tool_rail_->SetSizer(sizer);
}

void CanvasWorkspacePanel::CreateInspector()
{
    inspector_ = new wxPanel(this, wxID_ANY);
    inspector_->SetMinSize(wxSize(kInspectorWidth, -1));

    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* title = new wxStaticText(inspector_, wxID_ANY, "Properties");
    sizer->Add(title, 0, wxEXPAND | wxALL, 8);

    auto* placeholder =
        new wxStaticText(inspector_, wxID_ANY, "Select an object to\nsee its properties");
    sizer->Add(placeholder, 1, wxEXPAND | wxALL, 12);

    inspector_->SetSizer(sizer);
}

void CanvasWorkspacePanel::CreateMinimapStrip()
{
    minimap_strip_ = new wxPanel(this, wxID_ANY);
    minimap_strip_->SetMinSize(wxSize(-1, kMinimapHeight));

    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* label = new wxStaticText(minimap_strip_, wxID_ANY, "Navigator");
    sizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);

    sizer->AddStretchSpacer(1);

    minimap_strip_->SetSizer(sizer);
}

// ── Tool Selection ────────────────────────────────────────────────

void CanvasWorkspacePanel::OnToolSelected(int tool_index)
{
    const auto& tools = tool_descriptors();
    if (tool_index < 0 || static_cast<size_t>(tool_index) >= tools.size())
    {
        return;
    }

    core::events::CanvasToolChangedEvent evt;
    evt.tool_mode = static_cast<uint8_t>(tool_index);
    evt.tool_name = tools[static_cast<size_t>(tool_index)].name;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Canvas tool selected: {}", evt.tool_name);
}

void CanvasWorkspacePanel::OnZoomChanged(int zoom_percent)
{
    if (zoom_label_ != nullptr)
    {
        zoom_label_->SetLabel(std::to_string(zoom_percent) + "%");
    }
}

// ── Board Management ──────────────────────────────────────────────

void CanvasWorkspacePanel::LoadBoard(const std::string& board_id)
{
    current_board_id_ = board_id;
    if (board_title_label_ != nullptr)
    {
        board_title_label_->SetLabel("Board: " + board_id);
    }
    MARKAMP_LOG_DEBUG("CanvasWorkspacePanel loading board: {}", board_id);
}

void CanvasWorkspacePanel::NewBoard()
{
    static int board_counter = 0;
    ++board_counter;
    current_board_id_ = "board_" + std::to_string(board_counter);

    if (board_title_label_ != nullptr)
    {
        board_title_label_->SetLabel("Untitled Board " + std::to_string(board_counter));
    }
    MARKAMP_LOG_DEBUG("CanvasWorkspacePanel created new board: {}", current_board_id_);
}

// ── Accessors ─────────────────────────────────────────────────────

auto CanvasWorkspacePanel::canvas_panel() -> canvas::CanvasPanel*
{
    return canvas_panel_;
}

auto CanvasWorkspacePanel::canvas_panel() const -> const canvas::CanvasPanel*
{
    return canvas_panel_;
}

auto CanvasWorkspacePanel::board_id() const -> const std::string&
{
    return current_board_id_;
}

// ── Inspector / Minimap ───────────────────────────────────────────

void CanvasWorkspacePanel::ToggleInspector()
{
    inspector_visible_ = !inspector_visible_;
    if (inspector_ != nullptr)
    {
        inspector_->Show(inspector_visible_);
        middle_sizer_->Layout();
    }

    // Persist to config
    if (config_ != nullptr)
    {
        config_->set("canvas.inspector_visible", inspector_visible_);
    }

    // Publish toggle event
    core::events::CanvasInspectorToggledEvent evt;
    evt.visible = inspector_visible_;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Canvas inspector toggled: {}", inspector_visible_);
}

void CanvasWorkspacePanel::ToggleMinimap()
{
    minimap_visible_ = !minimap_visible_;
    if (minimap_strip_ != nullptr)
    {
        minimap_strip_->Show(minimap_visible_);
        root_sizer_->Layout();
    }

    // Persist to config
    if (config_ != nullptr)
    {
        config_->set("canvas.minimap_visible", minimap_visible_);
    }

    // Publish toggle event
    core::events::CanvasMinimapToggledEvent evt;
    evt.visible = minimap_visible_;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Canvas minimap toggled: {}", minimap_visible_);
}

bool CanvasWorkspacePanel::is_inspector_visible() const
{
    return inspector_visible_;
}

bool CanvasWorkspacePanel::is_minimap_visible() const
{
    return minimap_visible_;
}

// ── Status Display ───────────────────────────────────────────────────────────

void CanvasWorkspacePanel::SetBoardTitle(const std::string& title)
{
    if (board_title_label_ != nullptr)
    {
        std::string display = dirty_ ? "● " + title : title;
        board_title_label_->SetLabel(display);
    }
}

void CanvasWorkspacePanel::SetZoomLevel(double zoom)
{
    if (zoom_label_ != nullptr)
    {
        int zoom_pct = static_cast<int>(zoom * 100.0);
        zoom_label_->SetLabel(std::to_string(zoom_pct) + "%");
    }
}

void CanvasWorkspacePanel::SetObjectCount(size_t count)
{
    object_count_ = count;
}

void CanvasWorkspacePanel::SetDirtyIndicator(bool dirty)
{
    if (dirty_ == dirty)
    {
        return;
    }
    dirty_ = dirty;
    // Re-render the title with or without the dirty dot
    if (board_title_label_ != nullptr)
    {
        std::string current_title = board_title_label_->GetLabel().ToStdString();
        // Strip existing dirty prefix if present
        if (current_title.substr(0, 4) == "● ")
        {
            current_title = current_title.substr(4);
        }
        SetBoardTitle(current_title);
    }
}

// ── Event Subscriptions ───────────────────────────────────────────────────

void CanvasWorkspacePanel::SubscribeEvents()
{
    // Tool changes: update context bar mode label
    subscriptions_.emplace_back(
        event_bus_.subscribe<core::events::CanvasToolChangedEvent>(
            [](const core::events::CanvasToolChangedEvent& evt)
            {
                MARKAMP_LOG_DEBUG("Workspace shell tool changed: {}", evt.tool_name);
            }));

    // Viewport changes: update zoom label
    subscriptions_.emplace_back(
        event_bus_.subscribe<core::events::CanvasViewportChangedEvent>(
            [this](const core::events::CanvasViewportChangedEvent& evt)
            {
                SetZoomLevel(evt.zoom);
            }));

    // Object added: increment count
    subscriptions_.emplace_back(
        event_bus_.subscribe<core::events::CanvasObjectAddedEvent>(
            [this](const core::events::CanvasObjectAddedEvent& /*evt*/)
            {
                ++object_count_;
            }));

    // Object removed: decrement count
    subscriptions_.emplace_back(
        event_bus_.subscribe<core::events::CanvasObjectRemovedEvent>(
            [this](const core::events::CanvasObjectRemovedEvent& /*evt*/)
            {
                if (object_count_ > 0)
                {
                    --object_count_;
                }
            }));

    // Board renamed: update title
    subscriptions_.emplace_back(
        event_bus_.subscribe<core::events::BoardRenamedEvent>(
            [this](const core::events::BoardRenamedEvent& evt)
            {
                SetBoardTitle(evt.new_name);
                MARKAMP_LOG_DEBUG("Board renamed in workspace panel: {}", evt.new_name);
            }));

    // Board saved: clear dirty indicator
    subscriptions_.emplace_back(
        event_bus_.subscribe<core::events::BoardSavedEvent>(
            [this](const core::events::BoardSavedEvent& /*evt*/)
            {
                SetDirtyIndicator(false);
            }));
}

// ── Theme ─────────────────────────────────────────────────────────

void CanvasWorkspacePanel::ApplyTheme()
{
    const auto panel_bg = theme_engine_.color(core::ThemeColorToken::BgPanel);
    const auto text_fg = theme_engine_.color(core::ThemeColorToken::TextMain);
    const auto header_bg = theme_engine_.color(core::ThemeColorToken::BgHeader);
    const auto border_clr = theme_engine_.color(core::ThemeColorToken::BorderLight);

    SetBackgroundColour(panel_bg);

    if (context_bar_ != nullptr)
    {
        context_bar_->SetBackgroundColour(header_bg);
        if (board_title_label_ != nullptr)
        {
            board_title_label_->SetForegroundColour(text_fg);
        }
        if (zoom_label_ != nullptr)
        {
            zoom_label_->SetForegroundColour(text_fg);
        }
    }

    if (tool_rail_ != nullptr)
    {
        tool_rail_->SetBackgroundColour(header_bg);
    }

    if (inspector_ != nullptr)
    {
        inspector_->SetBackgroundColour(panel_bg);
    }

    if (minimap_strip_ != nullptr)
    {
        minimap_strip_->SetBackgroundColour(border_clr);
    }

    if (canvas_panel_ != nullptr)
    {
        auto& renderer = canvas_panel_->renderer();

        // Background
        const auto bg_col = theme_engine_.color(core::ThemeColorToken::BgApp);
        renderer.set_background_color(
            canvas::CanvasColor{bg_col.Red(), bg_col.Green(), bg_col.Blue(), bg_col.Alpha()});

        // Grid
        auto grid = renderer.grid_settings();
        const auto grid_col = theme_engine_.color(core::ThemeColorToken::BorderLight);
        grid.color = canvas::CanvasColor{grid_col.Red(),
                                         grid_col.Green(),
                                         grid_col.Blue(),
                                         static_cast<uint8_t>(grid_col.Alpha() * 0.4)};
        grid.major_color = canvas::CanvasColor{grid_col.Red(),
                                               grid_col.Green(),
                                               grid_col.Blue(),
                                               static_cast<uint8_t>(grid_col.Alpha() * 0.8)};
        grid.background =
            canvas::CanvasColor{bg_col.Red(), bg_col.Green(), bg_col.Blue(), bg_col.Alpha()};
        grid.style = canvas::GridStyle::kDots;
        renderer.set_grid_settings(grid);

        // Minimap
        auto minimap = renderer.minimap_settings();
        minimap.background = canvas::CanvasColor{bg_col.Red(), bg_col.Green(), bg_col.Blue(), 180};
        minimap.border =
            canvas::CanvasColor{grid_col.Red(), grid_col.Green(), grid_col.Blue(), 200};

        const auto accent = theme_engine_.color(core::ThemeColorToken::AccentPrimary);
        minimap.viewport_rect =
            canvas::CanvasColor{accent.Red(), accent.Green(), accent.Blue(), 200};
        minimap.object_rect = canvas::CanvasColor{accent.Red(), accent.Green(), accent.Blue(), 100};
        renderer.set_minimap_settings(minimap);

        canvas_panel_->request_repaint();
    }

    Refresh();
}

} // namespace markamp::ui
