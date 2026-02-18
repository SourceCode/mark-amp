#include "WalkthroughPanel.h"

#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

namespace markamp::ui
{

// ── Data-only constructor (for tests) ──

WalkthroughPanel::WalkthroughPanel()
    : wxPanel()
{
}

// ── UI constructor ──

WalkthroughPanel::WalkthroughPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    CreateLayout();
}

void WalkthroughPanel::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // ── Progress gauge ──
    progress_gauge_ = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 16));
    sizer->Add(progress_gauge_, 0, wxEXPAND | wxALL, 4);

    // ── Scrollable step list ──
    scroll_area_ = new wxScrolledWindow(this, wxID_ANY);
    scroll_area_->SetScrollRate(0, 10);
    sizer->Add(scroll_area_, 1, wxEXPAND);

    SetSizer(sizer);
}

void WalkthroughPanel::RefreshContent()
{
    if (scroll_area_ == nullptr)
    {
        return;
    }

    scroll_area_->DestroyChildren();
    auto* step_sizer = new wxBoxSizer(wxVERTICAL);

    // Find the active walkthrough
    const core::ExtensionWalkthrough* active = nullptr;
    for (const auto& walk : walkthroughs_)
    {
        if (walk.walkthrough_id == active_walkthrough_)
        {
            active = &walk;
            break;
        }
    }

    if (active != nullptr)
    {
        for (const auto& step : active->steps)
        {
            auto* row = new wxBoxSizer(wxHORIZONTAL);

            const bool completed = is_step_completed(active->walkthrough_id, step.step_id);
            const wxString check_label = completed ? wxString::FromUTF8("✓ ") : wxString("○ ");

            auto* label =
                new wxStaticText(scroll_area_, wxID_ANY, check_label + wxString(step.title));
            row->Add(label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 4);

            step_sizer->Add(row, 0, wxEXPAND);

            if (!step.description.empty())
            {
                auto* desc =
                    new wxStaticText(scroll_area_, wxID_ANY, wxString("    " + step.description));
                desc->SetForegroundColour(wxColour(150, 150, 150));
                step_sizer->Add(desc, 0, wxEXPAND | wxLEFT, 20);
            }
        }
    }

    scroll_area_->SetSizer(step_sizer);
    scroll_area_->FitInside();

    // Update progress gauge
    if (progress_gauge_ != nullptr && active != nullptr && !active->steps.empty())
    {
        double progress = completion_progress(active->walkthrough_id);
        progress_gauge_->SetValue(static_cast<int>(progress * 100.0));
    }
}

void WalkthroughPanel::ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour)
{
    SetBackgroundColour(bg_colour);
    if (scroll_area_ != nullptr)
    {
        scroll_area_->SetBackgroundColour(bg_colour);
        scroll_area_->SetForegroundColour(fg_colour);
    }
    Refresh();
}

// ── Data-layer API (unchanged for test compatibility) ──

void WalkthroughPanel::set_walkthroughs(std::vector<core::ExtensionWalkthrough> walkthroughs)
{
    walkthroughs_ = std::move(walkthroughs);
    if (!walkthroughs_.empty() && active_walkthrough_.empty())
    {
        active_walkthrough_ = walkthroughs_.front().walkthrough_id;
    }
}

auto WalkthroughPanel::walkthroughs() const -> const std::vector<core::ExtensionWalkthrough>&
{
    return walkthroughs_;
}

void WalkthroughPanel::complete_step(const std::string& walkthrough_id, const std::string& step_id)
{
    completed_steps_.insert(walkthrough_id + ":" + step_id);
}

auto WalkthroughPanel::is_step_completed(const std::string& walkthrough_id,
                                         const std::string& step_id) const -> bool
{
    return completed_steps_.contains(walkthrough_id + ":" + step_id);
}

auto WalkthroughPanel::completion_progress(const std::string& walkthrough_id) const -> double
{
    for (const auto& walk : walkthroughs_)
    {
        if (walk.walkthrough_id == walkthrough_id && !walk.steps.empty())
        {
            int completed = 0;
            for (const auto& step : walk.steps)
            {
                if (is_step_completed(walkthrough_id, step.step_id))
                {
                    ++completed;
                }
            }
            return static_cast<double>(completed) / static_cast<double>(walk.steps.size());
        }
    }
    return 0.0;
}

auto WalkthroughPanel::active_walkthrough() const -> const std::string&
{
    return active_walkthrough_;
}

void WalkthroughPanel::set_active_walkthrough(const std::string& walkthrough_id)
{
    active_walkthrough_ = walkthrough_id;
}

void WalkthroughPanel::reset_walkthrough(const std::string& walkthrough_id)
{
    auto iter = completed_steps_.begin();
    const std::string prefix = walkthrough_id + ":";
    while (iter != completed_steps_.end())
    {
        if (iter->substr(0, prefix.size()) == prefix)
        {
            iter = completed_steps_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

// ── Phase 35: Interactive tutorial support ──

void WalkthroughPanel::register_built_in(BuiltInWalkthrough walkthrough)
{
    built_in_walkthroughs_.push_back(std::move(walkthrough));
}

auto WalkthroughPanel::built_in_walkthroughs() const -> const std::vector<BuiltInWalkthrough>&
{
    return built_in_walkthroughs_;
}

auto WalkthroughPanel::get_built_in(const std::string& walkthrough_id) const
    -> const BuiltInWalkthrough*
{
    for (const auto& wlk : built_in_walkthroughs_)
    {
        if (wlk.walkthrough_id == walkthrough_id)
        {
            return &wlk;
        }
    }
    return nullptr;
}

void WalkthroughPanel::set_spotlight_target(const std::string& element_id)
{
    spotlight_target_ = element_id;
}

auto WalkthroughPanel::spotlight_target() const -> const std::string&
{
    return spotlight_target_;
}

void WalkthroughPanel::try_action(const std::string& walkthrough_id, const std::string& step_id)
{
    const auto* wlk = get_built_in(walkthrough_id);
    if (wlk == nullptr)
    {
        return;
    }
    for (const auto& step : wlk->steps)
    {
        if (step.step_id == step_id && step.action)
        {
            step.action();
            complete_step(walkthrough_id, step_id);
            break;
        }
    }
}

auto WalkthroughPanel::built_in_count() const -> std::size_t
{
    return built_in_walkthroughs_.size();
}

void WalkthroughPanel::load_built_in_walkthroughs()
{
    built_in_walkthroughs_.clear();

    // Getting Started walkthrough
    BuiltInWalkthrough getting_started;
    getting_started.walkthrough_id = "getting_started";
    getting_started.title = "Getting Started with MarkAmp";
    getting_started.description = "Learn the basics of MarkAmp in 5 steps.";
    getting_started.steps = {
        {.step_id = "open_file",
         .title = "Open a File",
         .description = "Use Cmd+O or the file explorer to open a Markdown file.",
         .action_label = "Open File Explorer",
         .target_element = "sidebar.explorer"},
        {.step_id = "edit_markdown",
         .title = "Edit Markdown",
         .description = "Start typing in the editor. Syntax highlighting is automatic.",
         .action_label = "Try Editing",
         .target_element = "editor.area"},
        {.step_id = "preview",
         .title = "Preview Your Work",
         .description = "Press Cmd+Shift+V to see a live preview of your Markdown.",
         .action_label = "Toggle Preview",
         .target_element = "toolbar.preview"},
        {.step_id = "command_palette",
         .title = "Command Palette",
         .description = "Press Cmd+Shift+P to access any command in MarkAmp.",
         .action_label = "Open Palette",
         .target_element = "toolbar.palette"},
        {.step_id = "shortcuts",
         .title = "Keyboard Shortcuts",
         .description = "Press F1 to see all keyboard shortcuts.",
         .action_label = "Show Shortcuts",
         .target_element = "menu.help"},
    };
    built_in_walkthroughs_.push_back(std::move(getting_started));

    // Editor Basics walkthrough
    BuiltInWalkthrough editor_basics;
    editor_basics.walkthrough_id = "editor_basics";
    editor_basics.title = "Editor Basics";
    editor_basics.description = "Master the Markdown editor features.";
    editor_basics.steps = {
        {.step_id = "bold_italic",
         .title = "Bold & Italic",
         .description = "Use Cmd+B for bold and Cmd+I for italic.",
         .action_label = "Try Bold",
         .target_element = "toolbar.bold"},
        {.step_id = "headings",
         .title = "Headings",
         .description = "Use # symbols for headings (1-6 levels).",
         .action_label = "Insert Heading",
         .target_element = "toolbar.heading"},
        {.step_id = "links",
         .title = "Links & Wiki-Links",
         .description = "Use Cmd+K for links or [[page]] for wiki-links.",
         .action_label = "Insert Link",
         .target_element = "toolbar.link"},
    };
    built_in_walkthroughs_.push_back(std::move(editor_basics));

    // Canvas walkthrough
    BuiltInWalkthrough canvas_intro;
    canvas_intro.walkthrough_id = "canvas_intro";
    canvas_intro.title = "Canvas Mode";
    canvas_intro.description = "Visual thinking on an infinite canvas.";
    canvas_intro.steps = {
        {.step_id = "open_canvas",
         .title = "Open Canvas",
         .description = "Create or open a .board file to start.",
         .action_label = "Open Canvas",
         .target_element = "sidebar.canvas"},
        {.step_id = "add_note",
         .title = "Add a Sticky Note",
         .description = "Double-click on the canvas to add a note.",
         .action_label = "Try It",
         .target_element = "canvas.area"},
        {.step_id = "connect",
         .title = "Connect Objects",
         .description = "Drag from one object edge to another to create a connector.",
         .action_label = "Try Connecting",
         .target_element = "canvas.connector_tool"},
    };
    built_in_walkthroughs_.push_back(std::move(canvas_intro));

    // Notebooks walkthrough
    BuiltInWalkthrough notebooks_intro;
    notebooks_intro.walkthrough_id = "notebooks_intro";
    notebooks_intro.title = "Notebooks";
    notebooks_intro.description = "Combine prose with executable code.";
    notebooks_intro.steps = {
        {.step_id = "create_notebook",
         .title = "Create a Notebook",
         .description = "Create a .notebook file to get started.",
         .action_label = "New Notebook",
         .target_element = "menu.file.new"},
        {.step_id = "add_cell",
         .title = "Add a Code Cell",
         .description = "Click + to add a code cell, then type your code.",
         .action_label = "Add Cell",
         .target_element = "notebook.add_cell"},
        {.step_id = "run_cell",
         .title = "Run a Cell",
         .description = "Press Shift+Enter to execute the cell.",
         .action_label = "Run Cell",
         .target_element = "notebook.run"},
    };
    built_in_walkthroughs_.push_back(std::move(notebooks_intro));

    // Graph walkthrough
    BuiltInWalkthrough graph_intro;
    graph_intro.walkthrough_id = "graph_intro";
    graph_intro.title = "Knowledge Graph";
    graph_intro.description = "Visualise connections in your knowledge base.";
    graph_intro.steps = {
        {.step_id = "open_graph",
         .title = "Open Graph View",
         .description = "Click the graph icon in the sidebar to view connections.",
         .action_label = "Open Graph",
         .target_element = "sidebar.graph"},
        {.step_id = "create_links",
         .title = "Create Wiki-Links",
         .description = "Use [[page-name]] in your documents to create connections.",
         .action_label = "Try Linking",
         .target_element = "editor.area"},
        {.step_id = "filter_graph",
         .title = "Filter the Graph",
         .description = "Use the filter bar to focus on specific connections.",
         .action_label = "Try Filtering",
         .target_element = "graph.filter"},
    };
    built_in_walkthroughs_.push_back(std::move(graph_intro));
}

} // namespace markamp::ui
