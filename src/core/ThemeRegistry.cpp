#include "ThemeRegistry.h"

#include "BuiltInThemes.h"
#include "Logger.h"
#include "ThemeValidator.h"
#include "loader/ThemeLoader.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>

namespace markamp::core
{

ThemeRegistry::ThemeRegistry() = default;

auto ThemeRegistry::initialize() -> std::expected<void, std::string>
{
    load_builtin_themes();

    auto userResult = load_user_themes();
    if (!userResult)
    {
        MARKAMP_LOG_WARN("Could not load user themes: {}", userResult.error());
        // Non-fatal: built-in themes are still available
    }

    MARKAMP_LOG_INFO("ThemeRegistry: {} themes loaded ({} built-in)",
                     themes_.size(),
                     get_builtin_themes().size());
    return {};
}

auto ThemeRegistry::get_theme(const std::string& id) const -> std::optional<Theme>
{
    auto it =
        std::find_if(themes_.begin(), themes_.end(), [&id](const Theme& t) { return t.id == id; });
    if (it != themes_.end())
    {
        return *it;
    }
    return std::nullopt;
}

auto ThemeRegistry::list_themes() const -> std::vector<ThemeInfo>
{
    std::vector<ThemeInfo> infos;
    infos.reserve(themes_.size());

    const auto& builtins = get_builtin_themes();

    for (const auto& theme : themes_)
    {
        bool builtin = std::any_of(builtins.begin(),
                                   builtins.end(),
                                   [&theme](const Theme& b) { return b.id == theme.id; });
        infos.push_back(ThemeInfo{
            .id = theme.id,
            .name = theme.name,
            .is_builtin = builtin,
            .file_path = std::nullopt,
        });
    }

    return infos;
}

auto ThemeRegistry::theme_count() const -> size_t
{
    return themes_.size();
}

auto ThemeRegistry::has_theme(const std::string& id) const -> bool
{
    return std::any_of(
        themes_.begin(), themes_.end(), [&id](const Theme& t) { return t.id == id; });
}

auto ThemeRegistry::is_builtin(const std::string& id) const -> bool
{
    const auto& builtins = get_builtin_themes();
    return std::any_of(
        builtins.begin(), builtins.end(), [&id](const Theme& b) { return b.id == id; });
}

void ThemeRegistry::load_builtin_themes()
{
    const auto& builtins = get_builtin_themes();
    themes_.insert(themes_.end(), builtins.begin(), builtins.end());
}

auto ThemeRegistry::load_user_themes() -> std::expected<void, std::string>
{
    auto dir = user_themes_directory();
    // R20 Fix 9: Use error_code overload — exists() can throw on bad permissions
    std::error_code exists_ec;
    if (!std::filesystem::exists(dir, exists_ec))
    {
        return {}; // No user themes directory yet, not an error
    }

    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(dir, ec))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        auto ext = entry.path().extension().string();
        std::expected<Theme, std::string> result = std::unexpected("Unknown file type");

        if (ext == ".json")
        {
            // Legacy JSON loading
            // We can keep the old import_theme for JSON or refactor it.
            // For now, let's keep the old logic but invoke it here.
            result = import_theme(entry.path());
        }
        else if (ext == ".md")
        {
            // New Markdown/YAML loading
            result = ThemeLoader::load_from_file(entry.path());
        }
        else
        {
            continue;
        }

        if (result)
        {
            themes_.push_back(*result);
        }
        else
        {
            MARKAMP_LOG_WARN("Skipping invalid theme file {}: {}",
                             entry.path().filename().string(),
                             result.error());
        }
    }

    if (ec)
    {
        return std::unexpected("Error reading user themes directory: " + ec.message());
    }

    return {};
}

auto ThemeRegistry::import_theme(const std::filesystem::path& path)
    -> std::expected<Theme, std::string>
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return std::unexpected("Could not open file: " + path.string());
    }

    nlohmann::json j;
    try
    {
        file >> j;
    }
    catch (const nlohmann::json::parse_error&)
    {
        return std::unexpected("Error parsing theme file. Ensure it is valid JSON.");
    }

    // Validate JSON structure before deserialization
    ThemeValidator validator;
    auto validation = validator.validate_json(j);
    if (!validation.is_valid)
    {
        return std::unexpected(validation.errors.front());
    }

    // Auto-generate ID from name if missing
    if (!j.contains("id") || !j["id"].is_string() || j["id"].get<std::string>().empty())
    {
        auto name = j["name"].get<std::string>();
        std::string gen_id;
        for (char ch : name)
        {
            if (ch == ' ')
            {
                gen_id += '-';
            }
            else if (std::isalnum(static_cast<unsigned char>(ch)) || ch == '-')
            {
                gen_id += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            }
        }
        j["id"] = gen_id;
    }

    Theme theme;
    try
    {
        theme = j.get<Theme>();
    }
    catch (const nlohmann::json::exception& e)
    {
        return std::unexpected(std::string("Invalid theme format: ") + e.what());
    }

    if (!theme.is_valid())
    {
        auto errors = theme.validation_errors();
        return std::unexpected("Theme validation failed: " + errors.front());
    }

    // Duplicate handling
    auto existing = std::find_if(
        themes_.begin(), themes_.end(), [&theme](const Theme& t) { return t.id == theme.id; });
    if (existing != themes_.end())
    {
        if (is_builtin(theme.id))
        {
            // Built-in theme: generate a custom ID and rename
            theme.id = generate_unique_id(theme.id);
            theme.name += " (Custom)";
            themes_.push_back(theme);
        }
        else
        {
            // User theme: overwrite
            *existing = theme;
        }
    }
    else
    {
        themes_.push_back(theme);
    }

    // Persist to user themes directory
    auto persist_result = persist_theme(theme);
    if (!persist_result)
    {
        MARKAMP_LOG_WARN("Could not persist theme: {}", persist_result.error());
        // Non-fatal: theme is loaded in memory
    }

    return theme;
}

auto ThemeRegistry::export_theme(const std::string& id, const std::filesystem::path& path)
    -> std::expected<void, std::string>
{
    auto theme = get_theme(id);
    if (!theme)
    {
        return std::unexpected("Theme not found: " + id);
    }

    nlohmann::json j = *theme;

    std::ofstream file(path);
    if (!file.is_open())
    {
        return std::unexpected("Could not open file for writing: " + path.string());
    }

    file << j.dump(2); // 2-space indent per PRD
    if (!file.good())
    {
        return std::unexpected("Error writing theme file");
    }

    return {};
}

auto ThemeRegistry::delete_theme(const std::string& id) -> std::expected<void, std::string>
{
    if (is_builtin(id))
    {
        return std::unexpected("Cannot delete built-in theme: " + id);
    }

    auto it =
        std::find_if(themes_.begin(), themes_.end(), [&id](const Theme& t) { return t.id == id; });
    if (it == themes_.end())
    {
        return std::unexpected("Theme not found: " + id);
    }

    themes_.erase(it);

    // Delete persisted file
    auto file_path = user_themes_directory() / (sanitize_filename(id) + ".theme.json");
    std::error_code ec;
    std::filesystem::remove(file_path, ec);
    if (ec)
    {
        MARKAMP_LOG_WARN("Could not delete theme file {}: {}", file_path.string(), ec.message());
    }

    MARKAMP_LOG_INFO("ThemeRegistry: deleted theme '{}'", id);
    return {};
}

auto ThemeRegistry::user_themes_directory() -> std::filesystem::path
{
#if defined(__APPLE__)
    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        return std::filesystem::path(home) / "Library" / "Application Support" / "MarkAmp" /
               "themes";
    }
#elif defined(_WIN32)
    const char* appdata = std::getenv("APPDATA");
    if (appdata != nullptr)
    {
        return std::filesystem::path(appdata) / "MarkAmp" / "themes";
    }
#else
    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        return std::filesystem::path(home) / ".config" / "markamp" / "themes";
    }
#endif
    return std::filesystem::path("themes");
}

auto ThemeRegistry::sanitize_filename(const std::string& name) -> std::string
{
    std::string result;
    result.reserve(name.size());
    bool prev_was_sep = false;
    for (char ch : name)
    {
        if (ch == ' ' || ch == '-')
        {
            if (!result.empty() && !prev_was_sep)
            {
                result += '_';
            }
            prev_was_sep = true;
        }
        else if (std::isalnum(static_cast<unsigned char>(ch)))
        {
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            prev_was_sep = false;
        }
    }
    if (!result.empty() && result.back() == '_')
    {
        result.pop_back();
    }
    return result;
}

auto ThemeRegistry::persist_theme(const Theme& theme) -> std::expected<void, std::string>
{
    auto dir = user_themes_directory();
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    if (ec)
    {
        return std::unexpected("Could not create themes directory: " + ec.message());
    }

    auto file_path = dir / (sanitize_filename(theme.id) + ".theme.json");
    return export_theme(theme.id, file_path);
}

auto ThemeRegistry::generate_unique_id(const std::string& base_id) const -> std::string
{
    auto now = std::chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
    return base_id + "-custom-" + std::to_string(millis);
}

} // namespace markamp::core
