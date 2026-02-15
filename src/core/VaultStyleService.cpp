/// @file VaultStyleService.cpp
/// @brief V4 Phase 22 – Custom CSS Per Vault implementation.

#include "core/VaultStyleService.h"

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"
#include "core/VaultService.h"

#include <fstream>
#include <regex>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

VaultStyleService::VaultStyleService(EventBus& event_bus,
                                     Config& config,
                                     VaultService& vault_service,
                                     ThemeEngine& theme_engine)
    : event_bus_(event_bus)
    , config_(config)
    , vault_service_(vault_service)
    , theme_engine_(theme_engine)
{
}

// ============================================================================
// Load vault style
// ============================================================================

auto VaultStyleService::load_vault_style(const std::filesystem::path& vault_path)
    -> std::expected<VaultStyle, std::string>
{
    auto css_path = vault_css_path(vault_path);

    if (!std::filesystem::exists(css_path))
    {
        // No custom CSS is not an error
        current_style_ = VaultStyle{};
        current_style_.file_path = css_path;
        current_style_.is_valid = true;
        return current_style_;
    }

    std::ifstream file(css_path);
    if (!file.is_open())
    {
        return std::unexpected("Failed to open CSS file: " + css_path.string());
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string css_content = buffer.str();

    auto [valid, error_msg] = validate_css(css_content);

    VaultStyle style;
    style.css_content = std::move(css_content);
    style.file_path = css_path;
    style.variables = extract_variables(style.css_content);
    style.is_valid = valid;
    style.error = error_msg;

    current_style_ = style;

    events::VaultStyleLoadedEvent event;
    event.vault_path = vault_path.string();
    event.has_custom_css = !style.css_content.empty();
    event_bus_.publish(event);

    return style;
}

// ============================================================================
// Get merged CSS
// ============================================================================

auto VaultStyleService::get_merged_css(const std::string& base_theme_css) const -> std::string
{
    if (current_style_.css_content.empty())
    {
        return base_theme_css;
    }

    // Vault CSS overrides base theme
    return base_theme_css + "\n\n/* === Vault Custom CSS === */\n" + current_style_.css_content;
}

// ============================================================================
// Save vault style
// ============================================================================

auto VaultStyleService::save_vault_style(const std::filesystem::path& vault_path,
                                         const std::string& css_content)
    -> std::expected<void, std::string>
{
    auto css_path = vault_css_path(vault_path);

    // Ensure .markamp directory exists
    auto dir = css_path.parent_path();
    if (!std::filesystem::exists(dir))
    {
        std::error_code error_code;
        std::filesystem::create_directories(dir, error_code);
        if (error_code)
        {
            return std::unexpected("Failed to create directory: " + dir.string());
        }
    }

    std::ofstream file(css_path);
    if (!file.is_open())
    {
        return std::unexpected("Failed to write CSS file: " + css_path.string());
    }

    file << css_content;
    file.close();

    // Update current style
    auto [valid, error_msg] = validate_css(css_content);
    current_style_.css_content = css_content;
    current_style_.file_path = css_path;
    current_style_.variables = extract_variables(css_content);
    current_style_.is_valid = valid;
    current_style_.error = error_msg;

    events::VaultStyleChangedEvent event;
    event.css_content = css_content;
    event_bus_.publish(event);

    return {};
}

// ============================================================================
// Validate CSS
// ============================================================================

auto VaultStyleService::validate_css(const std::string& css) const -> std::pair<bool, std::string>
{
    if (css.empty())
    {
        return {true, ""};
    }

    // Check balanced braces
    int brace_count = 0;
    for (char chr : css)
    {
        if (chr == '{')
        {
            ++brace_count;
        }
        else if (chr == '}')
        {
            --brace_count;
        }
        if (brace_count < 0)
        {
            return {false, "Unexpected closing brace '}'"};
        }
    }
    if (brace_count != 0)
    {
        return {false, "Unbalanced braces: " + std::to_string(brace_count) + " unclosed"};
    }

    // Check for at least one rule (selector { ... })
    const std::regex rule_re(R"(\S+\s*\{)");
    if (!std::regex_search(css, rule_re))
    {
        return {false, "No valid CSS rules found"};
    }

    return {true, ""};
}

// ============================================================================
// Extract CSS variables
// ============================================================================

auto VaultStyleService::extract_variables(const std::string& css) const -> std::vector<CssVariable>
{
    std::vector<CssVariable> variables;

    // Match --variable-name: value; patterns
    const std::regex var_re(R"((--[\w-]+)\s*:\s*([^;]+);)");
    auto begin = std::sregex_iterator(css.begin(), css.end(), var_re);
    const auto end = std::sregex_iterator();

    for (auto iter = begin; iter != end; ++iter)
    {
        CssVariable var;
        var.name = (*iter)[1].str();

        // Trim value
        var.value = (*iter)[2].str();
        while (!var.value.empty() && var.value.front() == ' ')
        {
            var.value.erase(var.value.begin());
        }
        while (!var.value.empty() && var.value.back() == ' ')
        {
            var.value.pop_back();
        }

        variables.push_back(std::move(var));
    }

    return variables;
}

// ============================================================================
// Vault CSS path
// ============================================================================

auto VaultStyleService::vault_css_path(const std::filesystem::path& vault_path)
    -> std::filesystem::path
{
    return vault_path / ".markamp" / "styles.css";
}

// ============================================================================
// Generate default CSS
// ============================================================================

auto VaultStyleService::generate_default_css() -> std::string
{
    return R"(/* MarkAmp Vault Custom Styles
 * Place custom CSS here to override the base theme.
 * Use CSS variables for easy customization.
 */

:root {
    /* -- Colors -- */
    /* --primary-color: #007acc; */
    /* --background-color: #1e1e1e; */
    /* --text-color: #d4d4d4; */
    /* --accent-color: #569cd6; */

    /* -- Typography -- */
    /* --font-family: 'Inter', sans-serif; */
    /* --font-size: 14px; */
    /* --line-height: 1.6; */

    /* -- Editor -- */
    /* --editor-background: #1e1e1e; */
    /* --editor-foreground: #d4d4d4; */
    /* --editor-selection: rgba(38, 79, 120, 0.5); */

    /* -- Preview -- */
    /* --preview-background: #ffffff; */
    /* --preview-foreground: #333333; */
    /* --preview-heading-color: #111111; */
    /* --preview-link-color: #007acc; */
    /* --preview-code-background: #f5f5f5; */
}
)";
}

// ============================================================================
// Access current style
// ============================================================================

auto VaultStyleService::current_style() const -> const VaultStyle&
{
    return current_style_;
}

} // namespace markamp::core
