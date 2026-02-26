#include "FileTypeIconRegistry.h"

#include <algorithm>
#include <filesystem>

namespace markamp::ui
{

FileTypeIconRegistry::FileTypeIconRegistry()
{
    RegisterExtensions();
}

void FileTypeIconRegistry::RegisterExtensions()
{
    extension_map_ = {{".cpp", FileTypeIconId::kCpp},
                      {".cc", FileTypeIconId::kCpp},
                      {".cxx", FileTypeIconId::kCpp},
                      {".c", FileTypeIconId::kCpp},
                      {".h", FileTypeIconId::kHeader},
                      {".hpp", FileTypeIconId::kHeader},
                      {".hxx", FileTypeIconId::kHeader},
                      {".py", FileTypeIconId::kPython},
                      {".pyw", FileTypeIconId::kPython},
                      {".js", FileTypeIconId::kJavascript},
                      {".jsx", FileTypeIconId::kJavascript},
                      {".ts", FileTypeIconId::kTypescript},
                      {".tsx", FileTypeIconId::kTypescript},
                      {".md", FileTypeIconId::kMarkdown},
                      {".markdown", FileTypeIconId::kMarkdown},
                      {".json", FileTypeIconId::kJson},
                      {".yml", FileTypeIconId::kYaml},
                      {".yaml", FileTypeIconId::kYaml},
                      {".html", FileTypeIconId::kHtml},
                      {".htm", FileTypeIconId::kHtml},
                      {".css", FileTypeIconId::kCss},
                      {".scss", FileTypeIconId::kCss},
                      {".sass", FileTypeIconId::kCss},
                      {".xml", FileTypeIconId::kXml},
                      {".rs", FileTypeIconId::kRust},
                      {".go", FileTypeIconId::kGo},
                      {".java", FileTypeIconId::kJava},
                      {".rb", FileTypeIconId::kRuby},
                      {".sh", FileTypeIconId::kShell},
                      {".bash", FileTypeIconId::kShell},
                      {".zsh", FileTypeIconId::kShell},
                      {".toml", FileTypeIconId::kToml},
                      {".txt", FileTypeIconId::kText},
                      {".log", FileTypeIconId::kText},
                      {"CMakeLists.txt", FileTypeIconId::kCMake},
                      {".cmake", FileTypeIconId::kCMake}};
}

auto FileTypeIconRegistry::GetExtensionBytes(const std::string& filename) -> std::string
{
    // Special case exact matches like CMakeLists.txt
    if (filename == "CMakeLists.txt")
    {
        return filename;
    }

    std::filesystem::path p(filename);
    if (!p.has_extension())
    {
        return "";
    }

    std::string ext = p.extension().string();
    std::transform(
        ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
    return ext;
}

auto FileTypeIconRegistry::GetIconId(const std::string& filename) const -> FileTypeIconId
{
    std::string ext = GetExtensionBytes(filename);
    auto it = extension_map_.find(ext);
    if (it != extension_map_.end())
    {
        return it->second;
    }
    return FileTypeIconId::kUnknown;
}

void FileTypeIconRegistry::DrawFileIcon(wxGraphicsContext& gc,
                                        const std::string& filename,
                                        double x,
                                        double y,
                                        double size,
                                        const core::ThemeEngine& theme) const
{
    FileTypeIconId id = GetIconId(filename);
    switch (id)
    {
        case FileTypeIconId::kCpp:
            DrawCppIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kHeader:
            DrawHeaderIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kPython:
            DrawPythonIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kJavascript:
            DrawJsTsIcon(gc, x, y, size, theme, false);
            break;
        case FileTypeIconId::kTypescript:
            DrawJsTsIcon(gc, x, y, size, theme, true);
            break;
        case FileTypeIconId::kMarkdown:
            DrawMarkdownIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kJson:
            DrawJsonIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kYaml:
            DrawYamlIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kHtml:
        case FileTypeIconId::kXml:
            DrawWebIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kCss:
            DrawStyleIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kRust:
            DrawRustIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kGo:
            DrawGoIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kJava:
            DrawJavaIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kRuby:
            DrawRubyIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kShell:
            DrawShellIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kToml:
        case FileTypeIconId::kCMake:
            DrawConfigIcon(gc, x, y, size, theme);
            break;
        case FileTypeIconId::kText:
        case FileTypeIconId::kUnknown:
        default:
            DrawGenericTextIcon(gc, x, y, size, theme);
            break;
    }
}

// ============================================================================
// Drawing Routines
// ============================================================================

void FileTypeIconRegistry::DrawCppIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxKeyword); // Blue typically
    gc.SetPen(wxPen(color, 2));
    gc.SetBrush(wxNullBrush);

    // Draw "C"
    auto path = gc.CreatePath();
    path.AddArc(x + size * 0.5, y + size * 0.5, size * 0.35, -M_PI / 4, M_PI / 4 + M_PI, false);
    gc.StrokePath(path);

    // ++
    gc.SetPen(wxPen(color, 1));
    gc.StrokeLine(x + size * 0.6, y + size * 0.4, x + size * 0.8, y + size * 0.4);
    gc.StrokeLine(x + size * 0.7, y + size * 0.3, x + size * 0.7, y + size * 0.5);

    gc.StrokeLine(x + size * 0.85, y + size * 0.45, x + size * 0.95, y + size * 0.45);
    gc.StrokeLine(x + size * 0.9, y + size * 0.4, x + size * 0.9, y + size * 0.5);
}

void FileTypeIconRegistry::DrawHeaderIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxType); // Cyan/Purple
    gc.SetPen(wxPen(color, 2));
    gc.SetBrush(wxNullBrush);

    // Draw "H"
    gc.StrokeLine(x + size * 0.3, y + size * 0.2, x + size * 0.3, y + size * 0.8);
    gc.StrokeLine(x + size * 0.7, y + size * 0.2, x + size * 0.7, y + size * 0.8);
    gc.StrokeLine(x + size * 0.3, y + size * 0.5, x + size * 0.7, y + size * 0.5);
}

void FileTypeIconRegistry::DrawPythonIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxString); // Green/Yellow
    gc.SetPen(wxPen(color, 2));

    // Snake S curve
    auto path = gc.CreatePath();
    path.MoveToPoint(x + size * 0.7, y + size * 0.2);
    path.AddCurveToPoint(x + size * 0.2,
                         y + size * 0.2,
                         x + size * 0.2,
                         y + size * 0.5,
                         x + size * 0.5,
                         y + size * 0.5);
    path.AddCurveToPoint(x + size * 0.8,
                         y + size * 0.5,
                         x + size * 0.8,
                         y + size * 0.8,
                         x + size * 0.3,
                         y + size * 0.8);
    gc.StrokePath(path);
}

void FileTypeIconRegistry::DrawJsTsIcon(wxGraphicsContext& gc,
                                        double x,
                                        double y,
                                        double size,
                                        const core::ThemeEngine& theme,
                                        bool is_ts) const
{
    wxColour color = is_ts ? theme.color(core::ThemeColorToken::SyntaxKeyword) // TS: Blue
                           : theme.color(core::ThemeColorToken::SyntaxNumber); // JS: Yellow/Orange

    gc.SetPen(wxPen(color, 1));
    // Square background
    gc.DrawRectangle(x + size * 0.1, y + size * 0.1, size * 0.8, size * 0.8);

    // J/T and S inside
    gc.SetFont(wxFontInfo(8).Family(wxFONTFAMILY_TELETYPE).Bold(), color);
    if (is_ts)
    {
        gc.DrawText("TS", x + size * 0.15, y + size * 0.2);
    }
    else
    {
        gc.DrawText("JS", x + size * 0.15, y + size * 0.2);
    }
}

void FileTypeIconRegistry::DrawMarkdownIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxType); // Purple
    gc.SetPen(wxPen(color, 1));

    // Box
    gc.DrawRectangle(x + size * 0.1, y + size * 0.25, size * 0.8, size * 0.5);

    // M
    gc.StrokeLine(x + size * 0.2, y + size * 0.65, x + size * 0.2, y + size * 0.35);
    gc.StrokeLine(x + size * 0.2, y + size * 0.35, x + size * 0.4, y + size * 0.55);
    gc.StrokeLine(x + size * 0.4, y + size * 0.55, x + size * 0.6, y + size * 0.35);
    gc.StrokeLine(x + size * 0.6, y + size * 0.35, x + size * 0.6, y + size * 0.65);

    // Down arrow
    gc.StrokeLine(x + size * 0.8, y + size * 0.35, x + size * 0.8, y + size * 0.65);
    gc.StrokeLine(x + size * 0.75, y + size * 0.55, x + size * 0.8, y + size * 0.65);
    gc.StrokeLine(x + size * 0.85, y + size * 0.55, x + size * 0.8, y + size * 0.65);
}

void FileTypeIconRegistry::DrawJsonIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxKeyword); // Usually cyan or orange
    gc.SetPen(wxPen(color, 1));

    // Curly braces { }
    gc.SetFont(wxFontInfo(10).Family(wxFONTFAMILY_TELETYPE).Bold(), color);
    gc.DrawText("{}", x + size * 0.1, y + size * 0.1);
}

void FileTypeIconRegistry::DrawYamlIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxKeyword);
    gc.SetPen(wxPen(color, 1));

    // Y
    gc.StrokeLine(x + size * 0.3, y + size * 0.2, x + size * 0.5, y + size * 0.5);
    gc.StrokeLine(x + size * 0.7, y + size * 0.2, x + size * 0.5, y + size * 0.5);
    gc.StrokeLine(x + size * 0.5, y + size * 0.5, x + size * 0.5, y + size * 0.8);

    // List dash
    gc.StrokeLine(x + size * 0.6, y + size * 0.8, x + size * 0.8, y + size * 0.8);
}

void FileTypeIconRegistry::DrawWebIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxOperator); // Orange/Red
    gc.SetPen(wxPen(color, 1));

    // < >
    gc.StrokeLine(x + size * 0.4, y + size * 0.2, x + size * 0.2, y + size * 0.5);
    gc.StrokeLine(x + size * 0.2, y + size * 0.5, x + size * 0.4, y + size * 0.8);

    gc.StrokeLine(x + size * 0.6, y + size * 0.2, x + size * 0.8, y + size * 0.5);
    gc.StrokeLine(x + size * 0.8, y + size * 0.5, x + size * 0.6, y + size * 0.8);

    // /
    gc.StrokeLine(x + size * 0.55, y + size * 0.2, x + size * 0.45, y + size * 0.8);
}

void FileTypeIconRegistry::DrawStyleIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxKeyword); // Blue
    gc.SetPen(wxPen(color, 1));

    // #
    gc.StrokeLine(x + size * 0.3, y + size * 0.2, x + size * 0.3, y + size * 0.8);
    gc.StrokeLine(x + size * 0.7, y + size * 0.2, x + size * 0.7, y + size * 0.8);
    gc.StrokeLine(x + size * 0.2, y + size * 0.4, x + size * 0.8, y + size * 0.4);
    gc.StrokeLine(x + size * 0.2, y + size * 0.6, x + size * 0.8, y + size * 0.6);
}

void FileTypeIconRegistry::DrawRustIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxNumber); // Orange
    gc.SetPen(wxPen(color, 1));

    // R
    gc.StrokeLine(x + size * 0.3, y + size * 0.2, x + size * 0.3, y + size * 0.8);
    auto p = gc.CreatePath();
    p.MoveToPoint(x + size * 0.3, y + size * 0.2);
    p.AddCurveToPoint(x + size * 0.7,
                      y + size * 0.2,
                      x + size * 0.7,
                      y + size * 0.5,
                      x + size * 0.3,
                      y + size * 0.5);
    gc.StrokePath(p);
    gc.StrokeLine(x + size * 0.4, y + size * 0.5, x + size * 0.7, y + size * 0.8);
}

void FileTypeIconRegistry::DrawGoIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxType); // Cyan
    gc.SetPen(wxPen(color, 1));

    // G
    auto p = gc.CreatePath();
    p.AddArc(x + size * 0.5, y + size * 0.5, size * 0.3, M_PI * 0.2, M_PI * 1.8, false);
    gc.StrokePath(p);
    gc.StrokeLine(x + size * 0.5, y + size * 0.5, x + size * 0.8, y + size * 0.5);
}

void FileTypeIconRegistry::DrawJavaIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxOperator); // Red/Orange
    gc.SetPen(wxPen(color, 1));

    // Coffee cup
    gc.StrokeLine(x + size * 0.3, y + size * 0.5, x + size * 0.3, y + size * 0.7);
    gc.StrokeLine(x + size * 0.7, y + size * 0.5, x + size * 0.7, y + size * 0.7);
    gc.StrokeLine(x + size * 0.3, y + size * 0.7, x + size * 0.7, y + size * 0.7);

    // Steam
    gc.StrokeLine(x + size * 0.4, y + size * 0.2, x + size * 0.4, y + size * 0.4);
    gc.StrokeLine(x + size * 0.6, y + size * 0.2, x + size * 0.6, y + size * 0.4);
}

void FileTypeIconRegistry::DrawRubyIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxOperator); // Red
    gc.SetPen(wxPen(color, 1));

    // Gem shape
    gc.StrokeLine(x + size * 0.3, y + size * 0.2, x + size * 0.7, y + size * 0.2);
    gc.StrokeLine(x + size * 0.7, y + size * 0.2, x + size * 0.9, y + size * 0.4);
    gc.StrokeLine(x + size * 0.9, y + size * 0.4, x + size * 0.5, y + size * 0.8);
    gc.StrokeLine(x + size * 0.5, y + size * 0.8, x + size * 0.1, y + size * 0.4);
    gc.StrokeLine(x + size * 0.1, y + size * 0.4, x + size * 0.3, y + size * 0.2);
}

void FileTypeIconRegistry::DrawShellIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::SyntaxString); // Green
    gc.SetPen(wxPen(color, 2));

    // >_
    gc.StrokeLine(x + size * 0.2, y + size * 0.3, x + size * 0.5, y + size * 0.5);
    gc.StrokeLine(x + size * 0.5, y + size * 0.5, x + size * 0.2, y + size * 0.7);

    gc.StrokeLine(x + size * 0.6, y + size * 0.7, x + size * 0.9, y + size * 0.7);
}

void FileTypeIconRegistry::DrawConfigIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::TextMuted);
    gc.SetPen(wxPen(color, 1));

    // Gear shape
    gc.DrawEllipse(x + size * 0.3, y + size * 0.3, size * 0.4, size * 0.4);
    gc.StrokeLine(x + size * 0.5, y + size * 0.1, x + size * 0.5, y + size * 0.3);
    gc.StrokeLine(x + size * 0.5, y + size * 0.7, x + size * 0.5, y + size * 0.9);
    gc.StrokeLine(x + size * 0.1, y + size * 0.5, x + size * 0.3, y + size * 0.5);
    gc.StrokeLine(x + size * 0.7, y + size * 0.5, x + size * 0.9, y + size * 0.5);
}

void FileTypeIconRegistry::DrawGenericTextIcon(
    wxGraphicsContext& gc, double x, double y, double size, const core::ThemeEngine& theme) const
{
    wxColour color = theme.color(core::ThemeColorToken::TextMuted);
    gc.SetPen(wxPen(color, 1));

    // Document outline
    gc.DrawRectangle(x + size * 0.2, y + size * 0.1, size * 0.6, size * 0.8);
    // Lines
    gc.StrokeLine(x + size * 0.3, y + size * 0.3, x + size * 0.7, y + size * 0.3);
    gc.StrokeLine(x + size * 0.3, y + size * 0.5, x + size * 0.7, y + size * 0.5);
    gc.StrokeLine(x + size * 0.3, y + size * 0.7, x + size * 0.5, y + size * 0.7);
}

} // namespace markamp::ui
