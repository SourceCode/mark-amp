#pragma once

#include "DesignTokenRegistry.h"
#include "core/ThemeEngine.h"

#include <wx/graphics.h>

#include <string>
#include <unordered_map>

namespace markamp::ui
{

enum class FileTypeIconId
{
    kUnknown,
    kCpp,
    kHeader,
    kPython,
    kJavascript,
    kTypescript,
    kMarkdown,
    kJson,
    kYaml,
    kHtml,
    kCss,
    kXml,
    kRust,
    kGo,
    kJava,
    kRuby,
    kShell,
    kToml,
    kCMake,
    kText
};

/// Maps file extensions to small 14x14 themed icon rendering routines.
class FileTypeIconRegistry
{
public:
    FileTypeIconRegistry();

    /// Draw the icon corresponding to the filename's extension.
    void DrawFileIcon(wxGraphicsContext& gc,
                      const std::string& filename,
                      double x,
                      double y,
                      double size,
                      const core::ThemeEngine& theme) const;

    [[nodiscard]] auto GetIconId(const std::string& filename) const -> FileTypeIconId;

private:
    std::unordered_map<std::string, FileTypeIconId> extension_map_;

    void RegisterExtensions();

    // Individual drawing routines
    void DrawCppIcon(wxGraphicsContext& gc,
                     double x,
                     double y,
                     double size,
                     const core::ThemeEngine& theme) const;
    void DrawHeaderIcon(wxGraphicsContext& gc,
                        double x,
                        double y,
                        double size,
                        const core::ThemeEngine& theme) const;
    void DrawPythonIcon(wxGraphicsContext& gc,
                        double x,
                        double y,
                        double size,
                        const core::ThemeEngine& theme) const;
    void DrawJsTsIcon(wxGraphicsContext& gc,
                      double x,
                      double y,
                      double size,
                      const core::ThemeEngine& theme,
                      bool is_ts) const;
    void DrawMarkdownIcon(wxGraphicsContext& gc,
                          double x,
                          double y,
                          double size,
                          const core::ThemeEngine& theme) const;
    void DrawJsonIcon(wxGraphicsContext& gc,
                      double x,
                      double y,
                      double size,
                      const core::ThemeEngine& theme) const;
    void DrawYamlIcon(wxGraphicsContext& gc,
                      double x,
                      double y,
                      double size,
                      const core::ThemeEngine& theme) const;
    void DrawWebIcon(wxGraphicsContext& gc,
                     double x,
                     double y,
                     double size,
                     const core::ThemeEngine& theme) const; // HTML/XML
    void DrawStyleIcon(wxGraphicsContext& gc,
                       double x,
                       double y,
                       double size,
                       const core::ThemeEngine& theme) const; // CSS
    void DrawRustIcon(wxGraphicsContext& gc,
                      double x,
                      double y,
                      double size,
                      const core::ThemeEngine& theme) const;
    void DrawGoIcon(wxGraphicsContext& gc,
                    double x,
                    double y,
                    double size,
                    const core::ThemeEngine& theme) const;
    void DrawJavaIcon(wxGraphicsContext& gc,
                      double x,
                      double y,
                      double size,
                      const core::ThemeEngine& theme) const;
    void DrawRubyIcon(wxGraphicsContext& gc,
                      double x,
                      double y,
                      double size,
                      const core::ThemeEngine& theme) const;
    void DrawShellIcon(wxGraphicsContext& gc,
                       double x,
                       double y,
                       double size,
                       const core::ThemeEngine& theme) const;
    void DrawConfigIcon(wxGraphicsContext& gc,
                        double x,
                        double y,
                        double size,
                        const core::ThemeEngine& theme) const; // TOML/CMake
    void DrawGenericTextIcon(wxGraphicsContext& gc,
                             double x,
                             double y,
                             double size,
                             const core::ThemeEngine& theme) const;

    // Helper to extract extension
    [[nodiscard]] static auto GetExtensionBytes(const std::string& filename) -> std::string;
};

} // namespace markamp::ui
