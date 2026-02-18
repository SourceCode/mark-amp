#include "ExtensionStorage.h"

#include "Logger.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <sstream>

namespace markamp::core
{

namespace fs = std::filesystem;

namespace
{

auto source_to_string(ExtensionSource src) -> std::string
{
    switch (src)
    {
        case ExtensionSource::kGallery:
            return "gallery";
        case ExtensionSource::kVsix:
            return "vsix";
        case ExtensionSource::kBuiltin:
            return "builtin";
    }
    return "builtin";
}

auto string_to_source(const std::string& str) -> ExtensionSource
{
    if (str == "gallery")
    {
        return ExtensionSource::kGallery;
    }
    if (str == "vsix")
    {
        return ExtensionSource::kVsix;
    }
    return ExtensionSource::kBuiltin;
}

auto get_json_string(const nlohmann::json& obj, const std::string& field_key) -> std::string
{
    if (obj.contains(field_key) && obj[field_key].is_string())
    {
        return obj[field_key].get<std::string>();
    }
    return {};
}

} // anonymous namespace

ExtensionStorageService::ExtensionStorageService(fs::path storage_path)
    : storage_path_(std::move(storage_path))
{
}

void ExtensionStorageService::load()
{
    entries_.clear();

    if (!fs::exists(storage_path_))
    {
        return;
    }

    try
    {
        std::ifstream file_stream(storage_path_);
        if (!file_stream.is_open())
        {
            MARKAMP_LOG_WARN("Cannot open extension storage file: {}", storage_path_.string());
            return;
        }

        std::ostringstream buffer;
        buffer << file_stream.rdbuf();

        const auto root = nlohmann::json::parse(buffer.str());
        if (!root.is_object() || !root.contains("extensions") || !root["extensions"].is_array())
        {
            MARKAMP_LOG_WARN("Invalid extension storage format in: {}", storage_path_.string());
            return;
        }

        for (const auto& entry : root["extensions"])
        {
            ExtensionMetadata meta;
            meta.extension_id = get_json_string(entry, "id");
            meta.version = get_json_string(entry, "version");
            meta.source = string_to_source(get_json_string(entry, "source"));
            meta.installed_at = get_json_string(entry, "installedAt");
            meta.location = get_json_string(entry, "location");

            if (entry.contains("enabled") && entry["enabled"].is_boolean())
            {
                meta.enabled = entry["enabled"].get<bool>();
            }

            if (!meta.extension_id.empty())
            {
                entries_[meta.extension_id] = std::move(meta);
            }
        }
    }
    catch (const std::exception& load_err)
    {
        MARKAMP_LOG_WARN("Failed to load extension storage: {}", load_err.what());
    }
}

void ExtensionStorageService::save() const
{
    nlohmann::json root;
    nlohmann::json extensions_array = nlohmann::json::array();

    for (const auto& [ext_id, meta] : entries_)
    {
        nlohmann::json entry;
        entry["id"] = meta.extension_id;
        entry["version"] = meta.version;
        entry["source"] = source_to_string(meta.source);
        entry["installedAt"] = meta.installed_at;
        entry["enabled"] = meta.enabled;
        entry["location"] = meta.location;
        extensions_array.push_back(std::move(entry));
    }

    root["extensions"] = std::move(extensions_array);

    try
    {
        // Ensure parent directory exists
        const auto parent = storage_path_.parent_path();
        if (!parent.empty() && !fs::exists(parent))
        {
            fs::create_directories(parent);
        }

        std::ofstream file_stream(storage_path_);
        if (!file_stream.is_open())
        {
            MARKAMP_LOG_WARN("Cannot write extension storage file: {}", storage_path_.string());
            return;
        }
        file_stream << root.dump(2) << "\n";
    }
    catch (const std::exception& save_err)
    {
        MARKAMP_LOG_WARN("Failed to save extension storage: {}", save_err.what());
    }
}

void ExtensionStorageService::upsert(const ExtensionMetadata& metadata)
{
    entries_[metadata.extension_id] = metadata;
}

void ExtensionStorageService::remove(const std::string& extension_id)
{
    entries_.erase(extension_id);
}

auto ExtensionStorageService::get(const std::string& extension_id) const -> const ExtensionMetadata*
{
    const auto iter = entries_.find(extension_id);
    if (iter != entries_.end())
    {
        return &iter->second;
    }
    return nullptr;
}

auto ExtensionStorageService::get_all() const -> std::vector<ExtensionMetadata>
{
    std::vector<ExtensionMetadata> result;
    result.reserve(entries_.size());
    for (const auto& [ext_id, meta] : entries_)
    {
        result.push_back(meta);
    }
    return result;
}

auto ExtensionStorageService::contains(const std::string& extension_id) const -> bool
{
    return entries_.find(extension_id) != entries_.end();
}

auto ExtensionStorageService::count() const -> size_t
{
    return entries_.size();
}

// ── V9 Phase 04 Task 9: Secret storage with encryption ──

void ExtensionStorageService::set_encryption_key(const std::string& key)
{
    encryption_key_ = key;
}

void ExtensionStorageService::store_secret(const std::string& extension_id,
                                           const std::string& secret_key,
                                           const std::string& secret_value)
{
    if (encryption_key_.empty())
    {
        MARKAMP_LOG_WARN("No encryption key set — storing secret in plaintext for '{}'",
                         extension_id);
        secrets_[extension_id][secret_key] = secret_value;
        return;
    }
    secrets_[extension_id][secret_key] = xor_cipher(secret_value, encryption_key_);
}

auto ExtensionStorageService::retrieve_secret(const std::string& extension_id,
                                              const std::string& secret_key) const -> std::string
{
    const auto ext_it = secrets_.find(extension_id);
    if (ext_it == secrets_.end())
    {
        return {};
    }
    const auto key_it = ext_it->second.find(secret_key);
    if (key_it == ext_it->second.end())
    {
        return {};
    }

    if (encryption_key_.empty())
    {
        return key_it->second; // Plaintext fallback
    }
    return xor_cipher(key_it->second, encryption_key_);
}

void ExtensionStorageService::delete_secret(const std::string& extension_id,
                                            const std::string& secret_key)
{
    const auto ext_it = secrets_.find(extension_id);
    if (ext_it != secrets_.end())
    {
        ext_it->second.erase(secret_key);
        if (ext_it->second.empty())
        {
            secrets_.erase(ext_it);
        }
    }
}

auto ExtensionStorageService::xor_cipher(const std::string& data, const std::string& key)
    -> std::string
{
    std::string result = data;
    for (std::size_t idx = 0; idx < data.size(); ++idx)
    {
        result[idx] = static_cast<char>(data[idx] ^ key[idx % key.size()]);
    }
    return result;
}

} // namespace markamp::core
