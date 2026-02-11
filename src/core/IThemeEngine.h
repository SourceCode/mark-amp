#pragma once

#include "Theme.h"

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

/// Interface for the theming subsystem.
/// Manages loading, switching, importing, and exporting visual themes.
class IThemeEngine
{
public:
    virtual ~IThemeEngine() = default;
    [[nodiscard]] virtual auto current_theme() const -> const Theme& = 0;
    virtual void apply_theme(const std::string& theme_id) = 0;
    [[nodiscard]] virtual auto available_themes() const -> std::vector<ThemeInfo> = 0;
    virtual void import_theme(const std::filesystem::path& path) = 0;
    virtual void export_theme(const std::string& theme_id, const std::filesystem::path& path) = 0;
};

} // namespace markamp::core
