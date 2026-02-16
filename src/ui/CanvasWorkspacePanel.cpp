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
    CreateLayout();
    ApplyTheme();
    MARKAMP_LOG_DEBUG("CanvasWorkspacePanel created");
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
}

void CanvasWorkspacePanel::ToggleMinimap()
{
    minimap_visible_ = !minimap_visible_;
    if (minimap_strip_ != nullptr)
    {
        minimap_strip_->Show(minimap_visible_);
        root_sizer_->Layout();
    }
}

bool CanvasWorkspacePanel::is_inspector_visible() const
{
    return inspector_visible_;
}

bool CanvasWorkspacePanel::is_minimap_visible() const
{
    return minimap_visible_;
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

    Refresh();
}

} // namespace markamp::ui
