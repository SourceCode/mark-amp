/// @file ThemeExporter.h
/// @brief V9 Phase 3 — Export themes to VSCode-compatible JSON format.

#pragma once

#include "Theme.h"

#include <expected>
#include <filesystem>
#include <string>

namespace markamp::core
{

/// Export internal themes to VSCode-compatible JSON.
class ThemeExporter
{
public:
    /// Export a theme to VSCode-compatible JSON at the given path.
    [[nodiscard]] static auto export_to_vscode(const Theme& theme,
                                               const std::filesystem::path& output_path)
        -> std::expected<void, std::string>;

    /// Generate VSCode-compatible JSON string from a Theme.
    [[nodiscard]] static auto to_vscode_json(const Theme& theme) -> std::string;
};

} // namespace markamp::core
