/// @file IconManifest.cpp
/// @brief V16 Phase 01-04 — Manifest-driven icon lookup implementation.
///
/// Parses a JSON manifest mapping canonical icon IDs to SVG assets,
/// file extensions to icon IDs, and folder names to icon IDs.

#include "ui/IconManifest.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <fstream>
#include <sstream>

namespace markamp::ui
{

// ── ManifestVersion ──

auto ManifestVersion::to_string() const -> std::string
{
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}

// ── IconManifest: Loading ──

auto IconManifest::load_from_file(const std::filesystem::path& path) -> bool
{
    errors_.clear();

    if (!std::filesystem::exists(path))
    {
        errors_.emplace_back("Manifest file not found: " + path.string());
        return false;
    }

    std::ifstream file(path);
    if (!file.is_open())
    {
        errors_.emplace_back("Failed to open manifest file: " + path.string());
        return false;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return load_from_string(ss.str());
}

auto IconManifest::load_from_string(const std::string& json_content) -> bool
{
    errors_.clear();
    icons_.clear();
    extension_map_.clear();
    filename_map_.clear();
    folder_map_.clear();
    command_map_.clear();

    try
    {
        auto doc = nlohmann::json::parse(json_content);

        // Parse version
        if (doc.contains("version"))
        {
            const auto& ver = doc["version"];
            version_.major = ver.value("major", 1);
            version_.minor = ver.value("minor", 0);
            version_.patch = ver.value("patch", 0);
        }

        // Parse icon entries
        if (doc.contains("icons") && doc["icons"].is_array())
        {
            for (const auto& icon_json : doc["icons"])
            {
                IconEntry entry;
                entry.canonical_id = icon_json.value("id", "");
                entry.asset_path = icon_json.value("asset", "");
                entry.display_name = icon_json.value("name", "");

                if (icon_json.contains("colorOverrides") && icon_json["colorOverrides"].is_object())
                {
                    for (const auto& [variant, color] : icon_json["colorOverrides"].items())
                    {
                        entry.color_overrides[variant] = color.get<std::string>();
                    }
                }

                if (entry.canonical_id.empty())
                {
                    errors_.emplace_back("Icon entry missing 'id' field");
                    continue;
                }

                if (icons_.contains(entry.canonical_id))
                {
                    errors_.emplace_back("Duplicate icon ID: " + entry.canonical_id);
                }

                icons_[entry.canonical_id] = std::move(entry);
            }
        }

        // Parse file extension mappings
        if (doc.contains("fileExtensions") && doc["fileExtensions"].is_object())
        {
            for (const auto& [ext, icon_id] : doc["fileExtensions"].items())
            {
                extension_map_[ext] = icon_id.get<std::string>();
            }
        }

        // Parse special filename mappings
        if (doc.contains("fileNames") && doc["fileNames"].is_object())
        {
            for (const auto& [name, icon_id] : doc["fileNames"].items())
            {
                filename_map_[name] = icon_id.get<std::string>();
            }
        }

        // Parse folder mappings
        if (doc.contains("folders") && doc["folders"].is_array())
        {
            for (const auto& folder_json : doc["folders"])
            {
                FolderIconEntry entry;
                entry.canonical_id = folder_json.value("id", "");
                entry.open_asset = folder_json.value("openAsset", "");
                entry.closed_asset = folder_json.value("closedAsset", "");
                entry.display_name = folder_json.value("name", "");

                if (!entry.canonical_id.empty())
                {
                    folder_map_[entry.canonical_id] = entry;
                }
            }
        }

        // Parse folder name → folder icon mappings
        if (doc.contains("folderNames") && doc["folderNames"].is_object())
        {
            // Stored as folderName → folderId, resolved through folder_map_
            // We store a parallel map for name lookups
            for (const auto& [name, folder_id] : doc["folderNames"].items())
            {
                auto id = folder_id.get<std::string>();
                // Store folder name → folder id mapping in folder_map_ entries via
                // a secondary index. For simplicity, we store in the existing folder_map_
                // if the folder_id entry doesn't exist yet.
                if (!folder_map_.contains(id))
                {
                    FolderIconEntry placeholder;
                    placeholder.canonical_id = id;
                    placeholder.display_name = name;
                    folder_map_[id] = placeholder;
                }
                // Also store the name→id mapping for resolve_folder_icon
                folder_map_["__name__" + name] = folder_map_[id];
            }
        }

        // Parse command icon mappings
        if (doc.contains("commandIcons") && doc["commandIcons"].is_object())
        {
            for (const auto& [cmd_id, icon_id] : doc["commandIcons"].items())
            {
                command_map_[cmd_id] = icon_id.get<std::string>();
            }
        }

        // Parse default folder icon override
        if (doc.contains("defaultFolder") && doc["defaultFolder"].is_object())
        {
            const auto& df = doc["defaultFolder"];
            default_folder_.open_asset = df.value("openAsset", default_folder_.open_asset);
            default_folder_.closed_asset = df.value("closedAsset", default_folder_.closed_asset);
        }

        validate();
        return errors_.empty();
    }
    catch (const nlohmann::json::parse_error& e)
    {
        errors_.emplace_back(std::string("JSON parse error: ") + e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        errors_.emplace_back(std::string("Manifest load error: ") + e.what());
        return false;
    }
}

// ── IconManifest: Resolution ──

auto IconManifest::resolve_file_icon(const std::string& filename) const -> std::string
{
    // 1) Check special filenames first (e.g., "Dockerfile", ".gitignore")
    auto normalized_name = normalize_filename(filename);
    if (auto it = filename_map_.find(normalized_name); it != filename_map_.end())
    {
        return it->second;
    }

    // 2) Check file extension
    auto ext = normalize_extension(filename);
    if (auto it = extension_map_.find(ext); it != extension_map_.end())
    {
        return it->second;
    }

    // 3) Fallback
    return kFallbackFileIcon;
}

auto IconManifest::get_entry(const std::string& canonical_id) const -> std::optional<IconEntry>
{
    if (auto it = icons_.find(canonical_id); it != icons_.end())
    {
        return it->second;
    }
    return std::nullopt;
}

auto IconManifest::resolve_folder_icon(const std::string& folder_name) const
    -> std::optional<FolderIconEntry>
{
    auto key = "__name__" + normalize_filename(folder_name);
    if (auto it = folder_map_.find(key); it != folder_map_.end())
    {
        return it->second;
    }
    return std::nullopt;
}

auto IconManifest::default_folder_icon() const -> const FolderIconEntry&
{
    return default_folder_;
}

auto IconManifest::resolve_command_icon(const std::string& command_id) const -> std::string
{
    if (auto it = command_map_.find(command_id); it != command_map_.end())
    {
        return it->second;
    }
    return kFallbackCommandIcon;
}

// ── IconManifest: Introspection ──

auto IconManifest::version() const -> const ManifestVersion&
{
    return version_;
}

auto IconManifest::icon_count() const -> size_t
{
    return icons_.size();
}

auto IconManifest::extension_count() const -> size_t
{
    return extension_map_.size();
}

auto IconManifest::folder_mapping_count() const -> size_t
{
    size_t count = 0;
    for (const auto& [key, _] : folder_map_)
    {
        if (key.substr(0, 8) != "__name__")
        {
            ++count;
        }
    }
    return count;
}

auto IconManifest::command_mapping_count() const -> size_t
{
    return command_map_.size();
}

auto IconManifest::all_icon_ids() const -> std::vector<std::string>
{
    std::vector<std::string> ids;
    ids.reserve(icons_.size());
    for (const auto& [id, _] : icons_)
    {
        ids.push_back(id);
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

auto IconManifest::validation_errors() const -> const std::vector<std::string>&
{
    return errors_;
}

// ── Private helpers ──

auto IconManifest::normalize_extension(const std::string& filename) -> std::string
{
    auto dot_pos = filename.rfind('.');
    if (dot_pos == std::string::npos || dot_pos == filename.size() - 1)
    {
        return {};
    }

    std::string ext = filename.substr(dot_pos + 1);
    std::transform(ext.begin(),
                   ext.end(),
                   ext.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return ext;
}

auto IconManifest::normalize_filename(const std::string& filename) -> std::string
{
    // Extract just the filename portion (no directory path)
    auto slash_pos = filename.rfind('/');
    auto backslash_pos = filename.rfind('\\');

    size_t name_start = 0;
    if (slash_pos != std::string::npos)
    {
        name_start = slash_pos + 1;
    }
    if (backslash_pos != std::string::npos && backslash_pos >= name_start)
    {
        name_start = backslash_pos + 1;
    }

    std::string name = filename.substr(name_start);
    std::transform(name.begin(),
                   name.end(),
                   name.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return name;
}

void IconManifest::validate()
{
    // Check extension mappings reference existing icon IDs
    for (const auto& [ext, icon_id] : extension_map_)
    {
        if (!icons_.contains(icon_id) && icon_id != kFallbackFileIcon)
        {
            errors_.emplace_back("Extension '" + ext + "' references unknown icon ID: " + icon_id);
        }
    }

    // Check filename mappings reference existing icon IDs
    for (const auto& [name, icon_id] : filename_map_)
    {
        if (!icons_.contains(icon_id) && icon_id != kFallbackFileIcon)
        {
            errors_.emplace_back("Filename '" + name + "' references unknown icon ID: " + icon_id);
        }
    }

    // Check command mappings reference existing icon IDs
    for (const auto& [cmd, icon_id] : command_map_)
    {
        if (!icons_.contains(icon_id) && icon_id != kFallbackCommandIcon)
        {
            errors_.emplace_back("Command '" + cmd + "' references unknown icon ID: " + icon_id);
        }
    }

    // Check all icons have non-empty asset paths
    for (const auto& [id, entry] : icons_)
    {
        if (entry.asset_path.empty())
        {
            errors_.emplace_back("Icon '" + id + "' has empty asset path");
        }
    }
}

} // namespace markamp::ui
