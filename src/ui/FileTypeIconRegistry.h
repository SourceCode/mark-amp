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

enum class FolderIconType
{
    kNormal,
    kGit,
    kNodeModules,
    kBuild,
    kSrc,
    kDocs,
    kTests,
    kConfig
};

/// Maps file extensions to small 14x14 themed icon rendering routines.
class FileTypeIconRegistry
{
public:
    FileTypeIconRegistry();

    /// Draw the icon corresponding to the filename's extension.
    void DrawFileIcon(wxGraphicsContext& ctx,
                      const std::string& filename,
                      double pos_x,
                      double pos_y,
                      double size,
                      const core::ThemeEngine& theme) const;

    [[nodiscard]] auto GetIconId(const std::string& filename) const -> FileTypeIconId;

    /// Get the specific folder icon type based on the folder name.
    [[nodiscard]] static auto GetFolderIconType(const std::string& folder_name) -> FolderIconType;

    /// Draw the icon for an open folder.
    void DrawOpenFolderIcon(wxGraphicsContext& ctx,
                            FolderIconType type,
                            double pos_x,
                            double pos_y,
                            double size,
                            const core::ThemeEngine& theme) const;

    /// Draw the icon for a closed folder.
    void DrawClosedFolderIcon(wxGraphicsContext& ctx,
                              FolderIconType type,
                              double pos_x,
                              double pos_y,
                              double size,
                              const core::ThemeEngine& theme) const;

private:
    std::unordered_map<std::string, FileTypeIconId> extension_map_;

    void RegisterExtensions();

    // Individual drawing routines
    void DrawCppIcon(wxGraphicsContext& ctx,
                     double pos_x,
                     double pos_y,
                     double size,
                     const core::ThemeEngine& theme) const;
    void DrawHeaderIcon(wxGraphicsContext& ctx,
                        double pos_x,
                        double pos_y,
                        double size,
                        const core::ThemeEngine& theme) const;
    void DrawPythonIcon(wxGraphicsContext& ctx,
                        double pos_x,
                        double pos_y,
                        double size,
                        const core::ThemeEngine& theme) const;
    void DrawJsTsIcon(wxGraphicsContext& ctx,
                      double pos_x,
                      double pos_y,
                      double size,
                      const core::ThemeEngine& theme,
                      bool is_ts) const;
    void DrawMarkdownIcon(wxGraphicsContext& ctx,
                          double pos_x,
                          double pos_y,
                          double size,
                          const core::ThemeEngine& theme) const;
    void DrawJsonIcon(wxGraphicsContext& ctx,
                      double pos_x,
                      double pos_y,
                      double size,
                      const core::ThemeEngine& theme) const;
    void DrawYamlIcon(wxGraphicsContext& ctx,
                      double pos_x,
                      double pos_y,
                      double size,
                      const core::ThemeEngine& theme) const;
    void DrawWebIcon(wxGraphicsContext& ctx,
                     double pos_x,
                     double pos_y,
                     double size,
                     const core::ThemeEngine& theme) const; // HTML/XML
    void DrawStyleIcon(wxGraphicsContext& ctx,
                       double pos_x,
                       double pos_y,
                       double size,
                       const core::ThemeEngine& theme) const; // CSS
    void DrawRustIcon(wxGraphicsContext& ctx,
                      double pos_x,
                      double pos_y,
                      double size,
                      const core::ThemeEngine& theme) const;
    void DrawGoIcon(wxGraphicsContext& ctx,
                    double pos_x,
                    double pos_y,
                    double size,
                    const core::ThemeEngine& theme) const;
    void DrawJavaIcon(wxGraphicsContext& ctx,
                      double pos_x,
                      double pos_y,
                      double size,
                      const core::ThemeEngine& theme) const;
    void DrawRubyIcon(wxGraphicsContext& ctx,
                      double pos_x,
                      double pos_y,
                      double size,
                      const core::ThemeEngine& theme) const;
    void DrawShellIcon(wxGraphicsContext& ctx,
                       double pos_x,
                       double pos_y,
                       double size,
                       const core::ThemeEngine& theme) const;
    void DrawConfigIcon(wxGraphicsContext& ctx,
                        double pos_x,
                        double pos_y,
                        double size,
                        const core::ThemeEngine& theme) const; // TOML/CMake
    void DrawGenericTextIcon(wxGraphicsContext& ctx,
                             double pos_x,
                             double pos_y,
                             double size,
                             const core::ThemeEngine& theme) const;

    // Helper to extract extension
    [[nodiscard]] static auto GetExtensionBytes(const std::string& filename) -> std::string;
};

} // namespace markamp::ui
