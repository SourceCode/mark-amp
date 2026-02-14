#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace markamp::core
{

/// Workspace extension recommendations from `.markamp/extensions.json` (#40).
/// Mirrors VS Code's `.vscode/extensions.json` concept.
class ExtensionRecommendations
{
public:
    ExtensionRecommendations() = default;

    /// Load recommendations from a file path (e.g. workspace/.markamp/extensions.json).
    auto load_from_file(const std::string& file_path) -> bool;

    /// Load from raw JSON content.
    void load_from_json(const nlohmann::json& json_obj);

    /// Get recommended extension IDs.
    [[nodiscard]] auto recommended() const -> const std::vector<std::string>&;

    /// Get unwanted extension IDs (should not be installed).
    [[nodiscard]] auto unwanted() const -> const std::vector<std::string>&;

    /// Check if an extension ID is recommended.
    [[nodiscard]] auto is_recommended(const std::string& extension_id) const -> bool;

    /// Check if an extension ID is unwanted.
    [[nodiscard]] auto is_unwanted(const std::string& extension_id) const -> bool;

    /// Get the file path that was loaded.
    [[nodiscard]] auto file_path() const -> const std::string&;

private:
    std::vector<std::string> recommended_;
    std::vector<std::string> unwanted_;
    std::string file_path_;
};

} // namespace markamp::core
