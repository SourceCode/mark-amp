/// @file VaultStyleService.h
/// @brief V4 Phase 22 – Custom CSS Per Vault.

#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class VaultService;
class ThemeEngine;

// ============================================================================
// CssVariable: a single CSS custom property
// ============================================================================

struct CssVariable
{
    std::string name;  // e.g. "--primary-color"
    std::string value; // e.g. "#ff0000"
    std::string description;
};

// ============================================================================
// VaultStyle: loaded vault CSS with metadata
// ============================================================================

struct VaultStyle
{
    std::string css_content;
    std::filesystem::path file_path;
    std::vector<CssVariable> variables;
    bool is_valid{false};
    std::string error;
};

// ============================================================================
// VaultStyleService
// ============================================================================

class VaultStyleService
{
public:
    VaultStyleService(EventBus& event_bus,
                      Config& config,
                      VaultService& vault_service,
                      ThemeEngine& theme_engine);

    /// Load the vault's custom CSS file.
    [[nodiscard]] auto load_vault_style(const std::filesystem::path& vault_path)
        -> std::expected<VaultStyle, std::string>;

    /// Get the current vault's merged CSS (theme + vault custom).
    [[nodiscard]] auto get_merged_css(const std::string& base_theme_css) const -> std::string;

    /// Save custom CSS content to the vault.
    [[nodiscard]] auto save_vault_style(const std::filesystem::path& vault_path,
                                        const std::string& css_content)
        -> std::expected<void, std::string>;

    /// Validate CSS content (basic syntax check).
    [[nodiscard]] auto validate_css(const std::string& css) const -> std::pair<bool, std::string>;

    /// Extract CSS variables from content.
    [[nodiscard]] auto extract_variables(const std::string& css) const -> std::vector<CssVariable>;

    /// Get the path to the vault's CSS file.
    [[nodiscard]] static auto vault_css_path(const std::filesystem::path& vault_path)
        -> std::filesystem::path;

    /// Generate a default CSS template with documented variable overrides.
    [[nodiscard]] static auto generate_default_css() -> std::string;

    /// Access current style.
    [[nodiscard]] auto current_style() const -> const VaultStyle&;

private:
    EventBus& event_bus_;
    Config& config_;
    VaultService& vault_service_;
    ThemeEngine& theme_engine_;

    VaultStyle current_style_;
};

} // namespace markamp::core
