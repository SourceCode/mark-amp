#include "GraphSidebarPanel.h"

#include "core/Events.h"
#include "core/Logger.h"

#include <wx/sizer.h>

#include <filesystem>
#include <fstream>

namespace markamp::ui
{

GraphSidebarPanel::GraphSidebarPanel(wxWindow* parent,
                                     core::ThemeEngine& theme_engine,
                                     core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Title
    title_label_ = new wxStaticText(this, wxID_ANY, "BACKLINKS");
    auto title_font = title_label_->GetFont();
    title_font.SetWeight(wxFONTWEIGHT_BOLD);
    title_label_->SetFont(title_font);
    main_sizer->Add(title_label_, 0, wxALL, 12);

    // Status indicator — shows count of backlinks
    status_label_ = new wxStaticText(this, wxID_ANY, "Open a file to view backlinks");
    main_sizer->Add(status_label_, 0, wxLEFT | wxRIGHT | wxBOTTOM, 12);

    // Document links list
    link_list_ = new wxListBox(this, wxID_ANY);
    link_list_->SetName("Graph Links List");
    link_list_->SetHelpText("Contains a list of backwards references to this document.");
    main_sizer->Add(link_list_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);

    SetSizer(main_sizer);

    ApplyTheme();

    // Improvement 2: Subscribe to FileOpenedEvent to auto-refresh backlinks
    file_opened_sub_ = event_bus_.subscribe<core::events::FileOpenedEvent>(
        [this](const core::events::FileOpenedEvent& evt) { RefreshBacklinks(evt.file_path); });

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });

    // Improvement 3: Double-click handler for navigation
    link_list_->Bind(wxEVT_LISTBOX_DCLICK, &GraphSidebarPanel::OnLinkDoubleClicked, this);
}

void GraphSidebarPanel::AddDocumentLink(const std::string& label)
{
    if (link_list_ != nullptr)
    {
        link_list_->Append(label);
    }
}

void GraphSidebarPanel::ClearLinks()
{
    if (link_list_ != nullptr)
    {
        link_list_->Clear();
    }
    link_paths_.clear();
}

void GraphSidebarPanel::RefreshBacklinks(const std::string& file_path)
{
    current_file_path_ = file_path;
    ClearLinks();

    // Extract the filename for display in status
    const std::filesystem::path fs_path(file_path);
    const std::string filename = fs_path.filename().string();

    // Scan the workspace for files that reference the current file
    // This is a lightweight scan looking for the filename in other markdown files
    auto parent_dir = fs_path.parent_path();
    if (!std::filesystem::exists(parent_dir))
    {
        status_label_->SetLabel("No backlinks");
        return;
    }

    int found_count = 0;
    const std::string stem = fs_path.stem().string();

    // Search for [[wikilink]] or [link](path) references to this file
    for (const auto& entry : std::filesystem::recursive_directory_iterator(
             parent_dir, std::filesystem::directory_options::skip_permission_denied))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        auto ext = entry.path().extension().string();
        if (ext != ".md" && ext != ".markdown" && ext != ".txt")
        {
            continue;
        }

        // Skip the file itself
        if (std::filesystem::equivalent(entry.path(), fs_path))
        {
            continue;
        }

        // Read the file and check for references
        std::ifstream in_file(entry.path());
        if (!in_file.is_open())
        {
            continue;
        }

        std::string line;
        bool references_found = false;
        while (std::getline(in_file, line))
        {
            // Check for [[stem]] wiki-link or (filename) markdown link
            if (line.find("[[" + stem + "]]") != std::string::npos ||
                line.find("(" + filename + ")") != std::string::npos ||
                line.find("(" + file_path + ")") != std::string::npos)
            {
                references_found = true;
                break;
            }
        }

        if (references_found)
        {
            const std::string label = entry.path().filename().string();
            link_list_->Append(label);
            link_paths_.push_back(entry.path().string());
            ++found_count;
        }
    }

    // Update status label
    if (found_count == 0)
    {
        status_label_->SetLabel("No backlinks found for " + filename);
    }
    else
    {
        status_label_->SetLabel(std::to_string(found_count) + " backlink" +
                                (found_count > 1 ? "s" : "") + " for " + filename);
    }
}

void GraphSidebarPanel::OnLinkDoubleClicked(wxCommandEvent& /*event*/)
{
    const int selection = link_list_->GetSelection();
    if (selection == wxNOT_FOUND || selection < 0)
    {
        return;
    }

    auto idx = static_cast<size_t>(selection);
    if (idx >= link_paths_.size())
    {
        return;
    }

    // Publish a FileOpenRequestEvent to navigate to the backlinked document
    core::events::FileOpenRequestEvent open_evt;
    open_evt.file_path = link_paths_[idx];
    event_bus_.publish(open_evt);
}

void GraphSidebarPanel::ApplyTheme()
{
    SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgPanel));

    if (title_label_ != nullptr)
    {
        title_label_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMain));
    }

    if (status_label_ != nullptr)
    {
        status_label_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    }

    Refresh();
}

} // namespace markamp::ui
