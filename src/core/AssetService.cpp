// ============================================================================
// File: src/core/AssetService.cpp
// Phase 36: Asset Management — AssetService implementation
// ============================================================================

#include "AssetService.h"

#include "Events.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace markamp::core
{

// ── AssetInfo ───────────────────────────────────────────────────────────────

auto AssetInfo::formatted_size() const -> std::string
{
    if (file_size < 1024)
        return std::to_string(file_size) + " B";
    if (file_size < 1024 * 1024)
        return std::to_string(file_size / 1024) + " KB";
    if (file_size < 1024 * 1024 * 1024)
        return std::to_string(file_size / (1024 * 1024)) + " MB";
    return std::to_string(file_size / (1024 * 1024 * 1024)) + " GB";
}

// ── AssetService ────────────────────────────────────────────────────────────

AssetService::AssetService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
    load_index();
}

auto AssetService::register_asset(const std::filesystem::path& source_path,
                                  const std::string& notebook_id)
    -> std::expected<AssetInfo, std::string>
{
    std::lock_guard lock(mutex_);

    if (!std::filesystem::exists(source_path))
    {
        return std::unexpected("Source file not found: " + source_path.string());
    }

    auto file_size = std::filesystem::file_size(source_path);
    if (file_size > static_cast<uintmax_t>(service_config_.max_file_size_mb * 1024 * 1024))
    {
        return std::unexpected("File exceeds maximum size limit");
    }

    // Compute hash for dedup.
    auto hash = compute_hash(source_path);

    // Check for duplicate.
    if (service_config_.enable_dedup)
    {
        auto dup_it = hash_index_.find(hash);
        if (dup_it != hash_index_.end())
        {
            auto asset_it = assets_.find(dup_it->second);
            if (asset_it != assets_.end())
            {
                // Return existing asset (dedup).
                auto info = asset_it->second;
                info.reference_count++;
                assets_[info.asset_id] = info;
                return info;
            }
        }
    }

    // Generate target path.
    auto target = generate_asset_path(source_path.filename().string(), notebook_id);

    // Copy file.
    std::error_code ec;
    std::filesystem::create_directories(target.parent_path(), ec);
    std::filesystem::copy_file(
        source_path, target, std::filesystem::copy_options::overwrite_existing, ec);
    if (ec)
    {
        return std::unexpected("Failed to copy asset: " + ec.message());
    }

    // Build asset info.
    AssetInfo info;
    info.asset_id = hash.substr(0, 16); // Use hash prefix as ID
    info.file_path = target;
    info.original_name = source_path.filename().string();
    info.mime_type = detect_mime_type(source_path);
    info.category = categorize(info.mime_type);
    info.sha256_hash = hash;
    info.file_size = static_cast<int64_t>(file_size);
    info.reference_count = 1;
    info.created_at = std::chrono::duration_cast<std::chrono::seconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();
    info.modified_at = info.created_at;

    assets_[info.asset_id] = info;
    hash_index_[hash] = info.asset_id;
    save_index();

    return info;
}

auto AssetService::register_asset_data(const std::string& filename,
                                       const std::vector<uint8_t>& data,
                                       const std::string& notebook_id)
    -> std::expected<AssetInfo, std::string>
{
    // Write data to a temp file, then register.
    auto temp = std::filesystem::temp_directory_path() / filename;
    {
        std::ofstream ofs(temp, std::ios::binary);
        if (!ofs)
            return std::unexpected("Failed to write temp file");
        ofs.write(reinterpret_cast<const char*>(data.data()),
                  static_cast<std::streamsize>(data.size()));
    }
    auto result = register_asset(temp, notebook_id);
    std::filesystem::remove(temp);
    return result;
}

auto AssetService::get_asset(const std::string& asset_id) const -> std::optional<AssetInfo>
{
    std::lock_guard lock(mutex_);
    auto it = assets_.find(asset_id);
    return it != assets_.end() ? std::optional(it->second) : std::nullopt;
}

auto AssetService::find_by_hash(const std::string& sha256_hash) const -> std::optional<AssetInfo>
{
    std::lock_guard lock(mutex_);
    auto idx_it = hash_index_.find(sha256_hash);
    if (idx_it == hash_index_.end())
        return std::nullopt;
    auto asset_it = assets_.find(idx_it->second);
    return asset_it != assets_.end() ? std::optional(asset_it->second) : std::nullopt;
}

auto AssetService::search(const std::string& query, AssetCategory category, int limit) const
    -> AssetSearchResult
{
    std::lock_guard lock(mutex_);
    AssetSearchResult result;
    auto start = std::chrono::steady_clock::now();

    for (const auto& [_, info] : assets_)
    {
        if (category != AssetCategory::Other && info.category != category)
            continue;
        if (!query.empty())
        {
            // Simple substring search on name and path.
            auto lower_query = query;
            auto lower_name = info.original_name;
            std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
            std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
            if (lower_name.find(lower_query) == std::string::npos)
                continue;
        }
        result.assets.push_back(info);
        if (static_cast<int>(result.assets.size()) >= limit)
            break;
    }

    result.total_count = static_cast<int32_t>(result.assets.size());
    auto end = std::chrono::steady_clock::now();
    result.elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    return result;
}

auto AssetService::update_reference_count(const std::string& asset_id, int32_t delta) -> void
{
    std::lock_guard lock(mutex_);
    auto it = assets_.find(asset_id);
    if (it != assets_.end())
    {
        it->second.reference_count += delta;
        if (it->second.reference_count < 0)
            it->second.reference_count = 0;
    }
}

auto AssetService::rename_asset(const std::string& asset_id, const std::string& new_name)
    -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);
    auto it = assets_.find(asset_id);
    if (it == assets_.end())
        return std::unexpected("Asset not found: " + asset_id);

    auto old_path = it->second.file_path;
    auto new_path = old_path.parent_path() / new_name;

    std::error_code ec;
    std::filesystem::rename(old_path, new_path, ec);
    if (ec)
        return std::unexpected("Rename failed: " + ec.message());

    it->second.file_path = new_path;
    it->second.original_name = new_name;
    save_index();
    return {};
}

auto AssetService::delete_asset(const std::string& asset_id) -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);
    auto it = assets_.find(asset_id);
    if (it == assets_.end())
        return std::unexpected("Asset not found: " + asset_id);

    std::error_code ec;
    std::filesystem::remove(it->second.file_path, ec);

    hash_index_.erase(it->second.sha256_hash);
    assets_.erase(it);
    save_index();
    return {};
}

auto AssetService::deduplicate() -> int32_t
{
    std::lock_guard lock(mutex_);
    // Group assets by hash, merge references to keep only the first.
    std::unordered_map<std::string, std::vector<std::string>> by_hash;
    for (const auto& [id, info] : assets_)
    {
        by_hash[info.sha256_hash].push_back(id);
    }

    int32_t removed = 0;
    for (const auto& [hash, ids] : by_hash)
    {
        if (ids.size() <= 1)
            continue;
        // Keep first, remove rest.
        for (size_t i = 1; i < ids.size(); ++i)
        {
            auto it = assets_.find(ids[i]);
            if (it != assets_.end())
            {
                std::error_code ec;
                std::filesystem::remove(it->second.file_path, ec);
                assets_.erase(it);
                ++removed;
            }
        }
    }
    if (removed > 0)
        save_index();
    return removed;
}

auto AssetService::find_orphans() const -> std::vector<AssetInfo>
{
    std::lock_guard lock(mutex_);
    std::vector<AssetInfo> orphans;
    for (const auto& [_, info] : assets_)
    {
        if (info.is_orphan())
            orphans.push_back(info);
    }
    return orphans;
}

auto AssetService::cleanup_orphans(int32_t grace_days) -> int32_t
{
    std::lock_guard lock(mutex_);
    auto now = std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();
    auto grace_seconds = static_cast<int64_t>(grace_days) * 86400;

    std::vector<std::string> to_remove;
    for (const auto& [id, info] : assets_)
    {
        if (info.is_orphan() && (now - info.created_at) > grace_seconds)
        {
            to_remove.push_back(id);
        }
    }

    for (const auto& id : to_remove)
    {
        auto it = assets_.find(id);
        if (it != assets_.end())
        {
            std::error_code ec;
            std::filesystem::remove(it->second.file_path, ec);
            hash_index_.erase(it->second.sha256_hash);
            assets_.erase(it);
        }
    }

    if (!to_remove.empty())
        save_index();
    return static_cast<int32_t>(to_remove.size());
}

auto AssetService::stats() const -> AssetStats
{
    std::lock_guard lock(mutex_);
    AssetStats s;
    std::unordered_map<std::string, int> hash_counts;

    for (const auto& [_, info] : assets_)
    {
        ++s.total_assets;
        s.total_size_bytes += info.file_size;
        if (info.is_orphan())
            ++s.orphans;

        switch (info.category)
        {
            case AssetCategory::Image:
                ++s.images;
                break;
            case AssetCategory::Document:
                ++s.documents;
                break;
            default:
                ++s.other;
                break;
        }

        hash_counts[info.sha256_hash]++;
    }

    for (const auto& [hash, count] : hash_counts)
    {
        if (count > 1)
        {
            s.duplicates += count - 1;
        }
    }

    return s;
}

auto AssetService::compute_hash(const std::filesystem::path& path) -> std::string
{
    // Simplified hash: read file and compute a basic hash.
    // In production, use OpenSSL SHA-256.
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return "";

    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    uint64_t hash = 14695981039346656037ULL; // FNV-1a offset basis
    char buf[8192];
    while (file.read(buf, sizeof(buf)) || file.gcount() > 0)
    {
        for (std::streamsize i = 0; i < file.gcount(); ++i)
        {
            hash ^= static_cast<uint8_t>(buf[i]);
            hash *= 1099511628211ULL; // FNV-1a prime
        }
    }

    ss << std::setw(16) << hash;
    return ss.str();
}

auto AssetService::detect_mime_type(const std::filesystem::path& path) -> std::string
{
    auto ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    static const std::unordered_map<std::string, std::string> mime_map = {
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},
        {".svg", "image/svg+xml"},
        {".webp", "image/webp"},
        {".bmp", "image/bmp"},
        {".ico", "image/x-icon"},
        {".pdf", "application/pdf"},
        {".doc", "application/msword"},
        {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
        {".xls", "application/vnd.ms-excel"},
        {".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
        {".ppt", "application/vnd.ms-powerpoint"},
        {".mp3", "audio/mpeg"},
        {".wav", "audio/wav"},
        {".ogg", "audio/ogg"},
        {".mp4", "video/mp4"},
        {".webm", "video/webm"},
        {".avi", "video/x-msvideo"},
        {".zip", "application/zip"},
        {".tar", "application/x-tar"},
        {".gz", "application/gzip"},
        {".txt", "text/plain"},
        {".md", "text/markdown"},
        {".html", "text/html"},
        {".css", "text/css"},
        {".js", "text/javascript"},
        {".json", "application/json"},
        {".xml", "application/xml"},
        {".woff", "font/woff"},
        {".woff2", "font/woff2"},
        {".ttf", "font/ttf"},
        {".otf", "font/otf"},
    };

    auto it = mime_map.find(ext);
    return it != mime_map.end() ? it->second : "application/octet-stream";
}

auto AssetService::categorize(const std::string& mime_type) -> AssetCategory
{
    if (mime_type.starts_with("image/"))
        return AssetCategory::Image;
    if (mime_type.starts_with("audio/"))
        return AssetCategory::Audio;
    if (mime_type.starts_with("video/"))
        return AssetCategory::Video;
    if (mime_type.starts_with("font/"))
        return AssetCategory::Font;
    if (mime_type.find("pdf") != std::string::npos ||
        mime_type.find("document") != std::string::npos ||
        mime_type.find("word") != std::string::npos)
        return AssetCategory::Document;
    if (mime_type.find("zip") != std::string::npos || mime_type.find("tar") != std::string::npos ||
        mime_type.find("gzip") != std::string::npos)
        return AssetCategory::Archive;
    return AssetCategory::Other;
}

auto AssetService::generate_asset_path(const std::string& original_name,
                                       const std::string& notebook_id) const
    -> std::filesystem::path
{
    auto dir = service_config_.assets_root / notebook_id;
    return dir / original_name;
}

auto AssetService::load_index() -> void
{
    // Stub: in production, load from SQLite or JSON index file.
}

auto AssetService::save_index() const -> void
{
    // Stub: in production, persist to SQLite or JSON index file.
}

} // namespace markamp::core
