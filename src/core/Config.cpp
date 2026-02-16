#include "Config.h"

#include "Logger.h"

#include <nlohmann/json.hpp> // For migration

#include <array>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <variant>

#ifdef __APPLE__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#endif

namespace markamp::core
{

auto Config::config_directory() -> std::filesystem::path
{
    std::filesystem::path dir;

#ifdef __APPLE__
    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        dir = std::filesystem::path(home) / "Library" / "Application Support" / "MarkAmp";
    }
#elif defined(_WIN32)
    const char* appData = std::getenv("APPDATA");
    if (appData != nullptr)
    {
        dir = std::filesystem::path(appData) / "MarkAmp";
    }
#else
    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        dir = std::filesystem::path(home) / ".config" / "markamp";
    }
#endif

    if (dir.empty())
    {
        // R20 Fix 10: current_path() can throw if CWD is deleted
        try
        {
            dir = std::filesystem::current_path() / ".markamp";
        }
        catch (const std::filesystem::filesystem_error&)
        {
            dir = std::filesystem::path(".") / ".markamp";
        }
    }

    return dir;
}

auto Config::config_file_path() -> std::filesystem::path
{
    return config_directory() / "config.md";
}

auto Config::defaults_file_path() -> std::filesystem::path
{
    // 1. macOS app bundle: <bundle>/Contents/Resources/config_defaults.json
#ifdef __APPLE__
    {
        // Use _NSGetExecutablePath to find the executable location
        // Executable is at <bundle>/Contents/MacOS/<exe>
        // Resources  is at <bundle>/Contents/Resources/
        std::array<char, 1024> exe_buf{};
        uint32_t buf_size = exe_buf.size();
        if (_NSGetExecutablePath(exe_buf.data(), &buf_size) == 0)
        {
            std::error_code canon_ec;
            auto exe_path = std::filesystem::canonical(exe_buf.data(), canon_ec);
            if (!canon_ec)
            {
                auto bundle_res =
                    exe_path.parent_path().parent_path() / "Resources" / "config_defaults.json";
                std::error_code exists_ec;
                if (std::filesystem::exists(bundle_res, exists_ec))
                {
                    return bundle_res;
                }
            }
        }
    }
#endif

    // 2. Build tree / source tree: <binary_dir>/resources/config_defaults.json
    std::error_code dir_ec;
    try
    {
        auto cwd = std::filesystem::current_path();
        auto dev_path = cwd / "resources" / "config_defaults.json";
        if (std::filesystem::exists(dev_path, dir_ec))
        {
            return dev_path;
        }
    }
    catch (const std::filesystem::filesystem_error&)
    {
        // CWD unavailable
    }

    // 3. Relative to source root (common in dev)
    std::filesystem::path source_path =
        std::filesystem::path(CMAKE_SOURCE_DIR) / "resources" / "config_defaults.json";
    if (std::filesystem::exists(source_path, dir_ec))
    {
        return source_path;
    }

    // 4. Give up — return empty path (caller will use hardcoded defaults)
    return {};
}

auto Config::load_defaults_from_json(const std::filesystem::path& path)
    -> std::expected<void, std::string>
{
    if (path.empty())
    {
        return std::unexpected("defaults file path is empty");
    }

    std::error_code exists_ec;
    if (!std::filesystem::exists(path, exists_ec))
    {
        return std::unexpected("defaults file not found: " + path.string());
    }

    try
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            return std::unexpected("failed to open defaults file: " + path.string());
        }

        auto json_defaults = nlohmann::json::parse(file, nullptr, true, true);

        for (const auto& [key, value] : json_defaults.items())
        {
            // Only set keys that are not already present in data_
            if (data_[key])
            {
                continue;
            }

            if (value.is_string())
            {
                data_[key] = value.get<std::string>();
            }
            else if (value.is_boolean())
            {
                data_[key] = value.get<bool>();
            }
            else if (value.is_number_float())
            {
                data_[key] = value.get<double>();
            }
            else if (value.is_number_integer())
            {
                data_[key] = value.get<int>();
            }
        }

        return {};
    }
    catch (const nlohmann::json::parse_error& parse_err)
    {
        return std::unexpected(std::string("JSON parse error in defaults: ") + parse_err.what());
    }
    catch (const std::exception& ex)
    {
        return std::unexpected(std::string("error loading defaults: ") + ex.what());
    }
}

void Config::apply_defaults()
{
    // Try loading defaults from external JSON file first
    auto json_path = defaults_file_path();
    auto json_result = load_defaults_from_json(json_path);
    if (json_result)
    {
        MARKAMP_LOG_INFO("Loaded config defaults from: " + json_path.string());
    }
    else
    {
        MARKAMP_LOG_INFO("Using hardcoded config defaults (" + json_result.error() + ")");
    }

    // Data-driven hardcoded fallback table.
    // Each entry is {key, default_value} — applied only if key is absent.
    struct DefaultEntry
    {
        const char* key;
        std::variant<const char*, int, bool, double> value;
    };

    static constexpr std::array<DefaultEntry, 28> kDefaults = {{
        {"theme", "midnight-neon"},
        {"view_mode", "split"},
        {"sidebar_visible", true},
        {"font_size", 14},
        {"word_wrap", true},
        {"auto_save", false},
        {"show_line_numbers", true},
        {"highlight_current_line", true},
        {"show_whitespace", false},
        {"tab_size", 4},
        {"show_minimap", false},
        {"last_workspace", ""},
        {"last_open_files", ""},
        {"auto_indent", true},
        {"indent_guides", true},
        {"bracket_matching", true},
        {"code_folding", true},
        {"edge_column", 80},
        {"font_family", "Menlo"},
        {"auto_save_interval_seconds", 60},
        {"show_status_bar", true},
        {"show_tab_bar", true},
        {"editor.cursor_blinking", "blink"},
        {"editor.cursor_width", 2},
        {"editor.mouse_wheel_zoom", false},
        {"editor.word_wrap_column", 80},
        {"editor.line_height", 0},
        {"editor.letter_spacing", 0.0},
    }};

    // Additional entries that don't fit in constexpr array due to variant limitations
    static const std::array<DefaultEntry, 4> kExtraDefaults = {{
        {"editor.padding_top", 0},
        {"editor.padding_bottom", 0},
        {"editor.bracket_pair_colorization", false},
        {"syntax.dim_whitespace", false},
    }};

    auto apply_entry = [this](const DefaultEntry& entry)
    {
        if (!data_[entry.key])
        {
            std::visit([this, &entry](const auto& val) { data_[entry.key] = val; }, entry.value);
        }
    };

    for (const auto& entry : kDefaults)
    {
        apply_entry(entry);
    }
    for (const auto& entry : kExtraDefaults)
    {
        apply_entry(entry);
    }
}

auto Config::migrate_from_json(const std::filesystem::path& json_path)
    -> std::expected<void, std::string>
{
    MARKAMP_LOG_INFO("Migrating config.json to config.md...");
    try
    {
        std::ifstream json_file(json_path);
        nlohmann::json json_data = nlohmann::json::parse(json_file, nullptr, true, true);

        // Transfer known keys from JSON config
        static constexpr std::array<const char*, 6> kMigrationKeys = {
            "theme",
            "view_mode",
            "sidebar_visible",
            "font_size",
            "word_wrap",
            "auto_save",
        };

        for (const auto* migration_key : kMigrationKeys)
        {
            if (json_data.contains(migration_key))
            {
                const auto& val = json_data[migration_key];
                if (val.is_string())
                {
                    data_[migration_key] = val.get<std::string>();
                }
                else if (val.is_boolean())
                {
                    data_[migration_key] = val.get<bool>();
                }
                else if (val.is_number_integer())
                {
                    data_[migration_key] = val.get<int>();
                }
            }
        }

        apply_defaults();
        return save();
    }
    catch (const std::exception& e)
    {
        return std::unexpected("Migration failed: " + std::string(e.what()));
    }
}

auto Config::parse_frontmatter(const std::string& content) -> bool
{
    if (content.rfind("---", 0) != 0)
    {
        return false;
    }

    auto end_pos = content.find("\n---", 3);
    if (end_pos == std::string::npos)
    {
        return false;
    }

    const std::string yaml_content = content.substr(3, end_pos - 3);
    data_ = YAML::Load(yaml_content);
    return true;
}

auto Config::load() -> std::expected<void, std::string>
{
    auto path = config_file_path();

    // R20 Fix 7: Use error_code overload — exists() can throw on bad permissions
    std::error_code exists_ec;
    if (!std::filesystem::exists(path, exists_ec))
    {
        // Try migrating from legacy JSON config
        auto json_path = config_directory() / "config.json";
        if (std::filesystem::exists(json_path, exists_ec))
        {
            auto migration_result = migrate_from_json(json_path);
            if (migration_result)
            {
                return {};
            }
            MARKAMP_LOG_ERROR("{}", migration_result.error());
        }

        MARKAMP_LOG_INFO("Config file not found, creating defaults at: {}", path.string());
        data_ = YAML::Node();
        apply_defaults();
        return save();
    }

    try
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            return std::unexpected("Failed to open config file: " + path.string());
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        const std::string content = buffer.str();

        if (parse_frontmatter(content))
        {
            apply_defaults();
            rebuild_cache();
            MARKAMP_LOG_INFO("Config loaded from: {}", path.string());
            return {};
        }

        // Fallback for partial/invalid files
        MARKAMP_LOG_WARN(
            "Invalid config format (missing frontmatter), loading as plain YAML or empty");
        data_ = YAML::Load(content);
        if (data_.IsNull())
        {
            data_ = YAML::Node();
        }
        apply_defaults();
        rebuild_cache();
        return {};
    }
    catch (const YAML::Exception& e)
    {
        MARKAMP_LOG_WARN("Corrupt config file, resetting to defaults: {}", e.what());
        data_ = YAML::Node();
        apply_defaults();
        rebuild_cache();
        return save();
    }
}

auto Config::save() const -> std::expected<void, std::string>
{
    auto dir = config_directory();
    try
    {
        std::filesystem::create_directories(dir);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        return std::unexpected("Failed to create config directory: " + std::string(e.what()));
    }

    auto path = config_file_path();
    std::ofstream file(path);
    if (!file.is_open())
    {
        return std::unexpected("Failed to write config file: " + path.string());
    }

    file << "---\n";
    file << data_;
    file << "\n---\n\n";
    file << "# MarkAmp Configuration\n";
    file << "Edit the values above to customize your experience.\n";
    file << "Supported themes: midnight-neon, synth-wave, retro-terminal\n";

    MARKAMP_LOG_DEBUG("Config saved to: {}", path.string());
    return {};
}

auto Config::get_string(std::string_view key, std::string_view default_val) const -> std::string
{
    auto key_str = std::string(key);
    if (data_[key_str] && data_[key_str].IsScalar())
    {
        return data_[key_str].as<std::string>();
    }
    return std::string(default_val);
}

auto Config::get_int(std::string_view key, int default_val) const -> int
{
    auto key_str = std::string(key);
    if (data_[key_str] && data_[key_str].IsScalar())
    {
        try
        {
            return data_[key_str].as<int>();
        }
        catch (const YAML::Exception&)
        {
            // R20 Fix 1: Typed catch — only swallow YAML conversion errors
        }
    }
    return default_val;
}

auto Config::get_bool(std::string_view key, bool default_val) const -> bool
{
    auto key_str = std::string(key);
    if (data_[key_str] && data_[key_str].IsScalar())
    {
        try
        {
            return data_[key_str].as<bool>();
        }
        catch (const YAML::Exception&)
        {
            // R20 Fix 2: Typed catch — only swallow YAML conversion errors
        }
    }
    return default_val;
}

auto Config::get_double(std::string_view key, double default_val) const -> double
{
    auto key_str = std::string(key);
    if (data_[key_str] && data_[key_str].IsScalar())
    {
        try
        {
            return data_[key_str].as<double>();
        }
        catch (const YAML::Exception&)
        {
            // R20 Fix 3: Typed catch — only swallow YAML conversion errors
        }
    }
    return default_val;
}

void Config::set(std::string_view key, std::string_view value)
{
    data_[std::string(key)] = std::string(value);
    rebuild_cache();
}

void Config::set(std::string_view key, int value)
{
    data_[std::string(key)] = value;
    rebuild_cache();
}

void Config::set(std::string_view key, bool value)
{
    data_[std::string(key)] = value;
    rebuild_cache();
}

void Config::set(std::string_view key, double value)
{
    data_[std::string(key)] = value;
    rebuild_cache();
}

auto Config::cached() const -> const Config::CachedValues&
{
    return cached_;
}

void Config::rebuild_cache()
{
    cached_.theme = get_string("theme", "midnight-neon");
    cached_.view_mode = get_string("view_mode", "split");
    cached_.font_family = get_string("font_family", "Menlo");
    cached_.last_workspace = get_string("last_workspace", "");
    cached_.cursor_blinking = get_string("editor.cursor_blinking", "blink");

    cached_.font_size = get_int("font_size", 14);
    cached_.tab_size = get_int("tab_size", 4);
    cached_.edge_column = get_int("edge_column", 80);
    cached_.auto_save_interval_seconds = get_int("auto_save_interval_seconds", 60);
    cached_.cursor_width = get_int("editor.cursor_width", 2);
    cached_.word_wrap_column = get_int("editor.word_wrap_column", 80);
    cached_.line_height = get_int("editor.line_height", 0);
    cached_.padding_top = get_int("editor.padding_top", 0);
    cached_.padding_bottom = get_int("editor.padding_bottom", 0);

    cached_.letter_spacing = get_double("editor.letter_spacing", 0.0);

    cached_.sidebar_visible = get_bool("sidebar_visible", true);
    cached_.word_wrap = get_bool("word_wrap", true);
    cached_.auto_save = get_bool("auto_save", false);
    cached_.show_line_numbers = get_bool("show_line_numbers", true);
    cached_.highlight_current_line = get_bool("highlight_current_line", true);
    cached_.show_whitespace = get_bool("show_whitespace", false);
    cached_.show_minimap = get_bool("show_minimap", false);
    cached_.auto_indent = get_bool("auto_indent", true);
    cached_.indent_guides = get_bool("indent_guides", true);
    cached_.bracket_matching = get_bool("bracket_matching", true);
    cached_.code_folding = get_bool("code_folding", true);
    cached_.show_status_bar = get_bool("show_status_bar", true);
    cached_.show_tab_bar = get_bool("show_tab_bar", true);
    cached_.mouse_wheel_zoom = get_bool("editor.mouse_wheel_zoom", false);
    cached_.bracket_pair_colorization = get_bool("editor.bracket_pair_colorization", false);
    cached_.dim_whitespace = get_bool("syntax.dim_whitespace", false);
    cached_.format_on_save = get_bool("editor.formatOnSave", false);
    cached_.format_on_paste = get_bool("editor.formatOnPaste", false);
    cached_.linked_editing = get_bool("editor.linkedEditing", false);
}

// ── Batch 9: Additional Config methods ──

auto Config::has_key(std::string_view key) const -> bool
{
    auto key_str = std::string(key);
    return data_[key_str] && data_[key_str].IsDefined();
}

void Config::remove(std::string_view key)
{
    auto key_str = std::string(key);
    if (data_[key_str])
    {
        data_.remove(key_str);
    }
    rebuild_cache();
}

auto Config::all_keys() const -> std::vector<std::string>
{
    std::vector<std::string> keys;
    if (data_.IsMap())
    {
        for (const auto& pair : data_)
        {
            keys.push_back(pair.first.as<std::string>());
        }
    }
    return keys;
}

auto Config::key_count() const -> std::size_t
{
    if (data_.IsMap())
    {
        return data_.size();
    }
    return 0;
}

// ── New Batch 9: Config profile support implementations (#55-58) ──

void Config::export_to_json(const std::filesystem::path& path) const
{
    std::ofstream ofs(path);
    if (!ofs.is_open())
    {
        return;
    }
    // Simple JSON export from YAML data
    ofs << "{\n";
    bool first = true;
    if (data_.IsMap())
    {
        for (const auto& pair : data_)
        {
            if (!first)
            {
                ofs << ",\n";
            }
            first = false;
            ofs << "  \"" << pair.first.as<std::string>() << "\": \""
                << pair.second.as<std::string>("") << "\"";
        }
    }
    ofs << "\n}\n";
}

void Config::import_from_json(const std::filesystem::path& path)
{
    std::ifstream ifs(path);
    if (!ifs.is_open())
    {
        return;
    }
    // Simple JSON import — parse key:value pairs
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    // Basic JSON parser for flat key-value objects
    std::size_t pos = 0;
    while ((pos = content.find('"', pos)) != std::string::npos)
    {
        ++pos;
        const auto key_end = content.find('"', pos);
        if (key_end == std::string::npos)
        {
            break;
        }
        const std::string key = content.substr(pos, key_end - pos);
        pos = key_end + 1;

        const auto val_start = content.find('"', pos);
        if (val_start == std::string::npos)
        {
            break;
        }
        const auto val_end = content.find('"', val_start + 1);
        if (val_end == std::string::npos)
        {
            break;
        }
        const std::string value = content.substr(val_start + 1, val_end - val_start - 1);
        pos = val_end + 1;

        data_[key] = value;
    }
    rebuild_cache();
}

auto Config::diff(const Config& other) const -> std::vector<std::string>
{
    std::vector<std::string> differing_keys;
    if (!data_.IsMap())
    {
        return differing_keys;
    }
    for (const auto& pair : data_)
    {
        const auto key = pair.first.as<std::string>();
        const auto this_val = pair.second.as<std::string>("");
        const auto other_val = other.get_string(key, "");
        if (this_val != other_val)
        {
            differing_keys.push_back(key);
        }
    }
    return differing_keys;
}

auto Config::snapshot() const -> Config
{
    Config copy;
    copy.data_ = YAML::Clone(data_);
    copy.rebuild_cache();
    return copy;
}

} // namespace markamp::core
