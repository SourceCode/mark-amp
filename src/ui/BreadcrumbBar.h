#pragma once

#include "core/ThemeEngine.h"

#include <wx/panel.h>
#include <wx/stattext.h>

#include <string>
#include <vector>

namespace markamp::ui
{

/// A breadcrumb navigation bar that shows the current file path and heading hierarchy.
/// Inspired by VSCode's breadcrumb bar above the editor.
class BreadcrumbBar : public wxPanel
{
public:
    BreadcrumbBar(wxWindow* parent, core::ThemeEngine& theme_engine);

    /// Set the file path segments (e.g. ["src", "ui", "EditorPanel.cpp"])
    void SetFilePath(const std::vector<std::string>& segments);

    /// Set the heading path (e.g. ["Chapter 1", "Section 1.1"])
    void SetHeadingPath(const std::vector<std::string>& headings);

    /// Update theme colors
    void ApplyTheme();

private:
    void Rebuild();

    core::ThemeEngine& theme_engine_;
    std::vector<std::string> file_segments_;
    std::vector<std::string> heading_segments_;
    wxStaticText* label_{nullptr};
};

} // namespace markamp::ui
