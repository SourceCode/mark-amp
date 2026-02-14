#include "GalleryService.h"

#include "Logger.h"

#include <nlohmann/json.hpp>

namespace markamp::core
{

namespace
{

constexpr auto kDefaultMarketplaceEndpoint =
    "https://marketplace.visualstudio.com/_apis/public/gallery/extensionquery";

auto string_to_asset_type(const std::string& asset_str) -> GalleryAssetType
{
    if (asset_str == "Microsoft.VisualStudio.Code.Manifest")
        return GalleryAssetType::kManifest;
    if (asset_str == "Microsoft.VisualStudio.Services.Content.Details")
        return GalleryAssetType::kReadme;
    if (asset_str == "Microsoft.VisualStudio.Services.Content.Changelog")
        return GalleryAssetType::kChangelog;
    if (asset_str == "Microsoft.VisualStudio.Services.Content.License")
        return GalleryAssetType::kLicense;
    if (asset_str == "Microsoft.VisualStudio.Services.VSIXPackage")
        return GalleryAssetType::kDownload;
    if (asset_str == "Microsoft.VisualStudio.Services.Icons.Default")
        return GalleryAssetType::kIcon;
    if (asset_str == "Microsoft.VisualStudio.Services.VsixSignature")
        return GalleryAssetType::kSignature;
    return GalleryAssetType::kManifest; // fallback
}

auto filter_type_to_int(GalleryFilterType filter_type) -> int
{
    return static_cast<int>(filter_type);
}

/// Parse a single extension from the raw gallery JSON.
auto parse_gallery_extension(const nlohmann::json& raw) -> GalleryExtension
{
    GalleryExtension ext;

    ext.name = raw.value("extensionName", "");
    ext.display_name = raw.value("displayName", "");
    ext.short_description = raw.value("shortDescription", "");
    ext.description = ext.short_description;
    ext.release_date = raw.value("releaseDate", "");
    ext.last_updated = raw.value("lastUpdated", "");

    // Extension ID / UUID
    if (raw.contains("extensionId"))
    {
        ext.uuid = raw.value("extensionId", "");
    }

    // Publisher
    if (raw.contains("publisher") && raw["publisher"].is_object())
    {
        ext.publisher = raw["publisher"].value("publisherName", "");
        ext.publisher_display = raw["publisher"].value("displayName", "");
    }

    ext.identifier = ext.publisher + "." + ext.name;

    // Categories & Tags
    if (raw.contains("categories") && raw["categories"].is_array())
    {
        for (const auto& cat : raw["categories"])
        {
            if (cat.is_string())
            {
                ext.categories.push_back(cat.get<std::string>());
            }
        }
    }
    if (raw.contains("tags") && raw["tags"].is_array())
    {
        for (const auto& tag : raw["tags"])
        {
            if (tag.is_string())
            {
                ext.tags.push_back(tag.get<std::string>());
            }
        }
    }

    // Statistics
    if (raw.contains("statistics") && raw["statistics"].is_array())
    {
        for (const auto& stat : raw["statistics"])
        {
            const auto stat_name = stat.value("statisticName", "");
            const auto stat_value = stat.value("value", 0.0);

            if (stat_name == "install")
            {
                ext.install_count = static_cast<std::int64_t>(stat_value);
            }
            else if (stat_name == "averagerating")
            {
                ext.rating = stat_value;
            }
            else if (stat_name == "ratingcount")
            {
                ext.rating_count = static_cast<std::int64_t>(stat_value);
            }
        }
    }

    // Versions → take first (latest) version
    if (raw.contains("versions") && raw["versions"].is_array() && !raw["versions"].empty())
    {
        const auto& first_version = raw["versions"][0];
        ext.version = first_version.value("version", "");

        // Assets
        if (first_version.contains("files") && first_version["files"].is_array())
        {
            for (const auto& file_entry : first_version["files"])
            {
                GalleryAsset asset;
                const auto asset_type_str = file_entry.value("assetType", "");
                asset.type = string_to_asset_type(asset_type_str);
                asset.uri = file_entry.value("source", "");
                ext.assets.push_back(std::move(asset));
            }
        }

        // Properties
        if (first_version.contains("properties") && first_version["properties"].is_array())
        {
            for (const auto& prop_entry : first_version["properties"])
            {
                GalleryProperty prop;
                prop.key = prop_entry.value("key", "");
                prop.value = prop_entry.value("value", "");
                ext.properties.push_back(std::move(prop));
            }
        }
    }

    return ext;
}

} // anonymous namespace

// ── ExtensionGalleryService ──

ExtensionGalleryService::ExtensionGalleryService()
    : api_endpoint_(kDefaultMarketplaceEndpoint)
{
}

ExtensionGalleryService::ExtensionGalleryService(std::string api_endpoint)
    : api_endpoint_(std::move(api_endpoint))
{
}

auto ExtensionGalleryService::build_query_json(const GalleryQueryOptions& options) -> std::string
{
    nlohmann::json query;

    // Build filters array
    nlohmann::json criteria = nlohmann::json::array();
    for (const auto& filter : options.filters)
    {
        nlohmann::json criterion;
        criterion["filterType"] = filter_type_to_int(filter.type);
        criterion["value"] = filter.value;
        criteria.push_back(std::move(criterion));
    }

    // Target VS Code platform
    nlohmann::json platform_criterion;
    platform_criterion["filterType"] = 8; // Target
    platform_criterion["value"] = "Microsoft.VisualStudio.Code";
    criteria.push_back(std::move(platform_criterion));

    nlohmann::json filter_obj;
    filter_obj["criteria"] = std::move(criteria);
    filter_obj["pageNumber"] = options.page_number;
    filter_obj["pageSize"] = options.page_size;
    filter_obj["sortBy"] = static_cast<int>(options.sort_by);
    filter_obj["sortOrder"] = static_cast<int>(options.sort_order);

    query["filters"] = nlohmann::json::array({std::move(filter_obj)});

    // Request flags: include stats (0x1), versions (0x2), categories (0x4),
    //                files (0x8), properties (0x10)
    query["assetTypes"] = nlohmann::json::array();
    query["flags"] = 0x1 | 0x2 | 0x4 | 0x8 | 0x10; // 0x1F = 31

    return query.dump();
}

auto ExtensionGalleryService::parse_query_response(const std::string& json_body)
    -> std::expected<GalleryQueryResult, std::string>
{
    GalleryQueryResult result;

    try
    {
        auto root = nlohmann::json::parse(json_body);

        if (!root.contains("results") || !root["results"].is_array() || root["results"].empty())
        {
            return std::unexpected("Invalid gallery response: missing results array");
        }

        const auto& first_result = root["results"][0];

        if (first_result.contains("resultMetadata") && first_result["resultMetadata"].is_array())
        {
            for (const auto& meta : first_result["resultMetadata"])
            {
                if (meta.value("metadataType", "") == "ResultCount" &&
                    meta.contains("metadataItems") && meta["metadataItems"].is_array() &&
                    !meta["metadataItems"].empty())
                {
                    result.total_count = meta["metadataItems"][0].value("count", 0);
                }
            }
        }

        if (first_result.contains("extensions") && first_result["extensions"].is_array())
        {
            for (const auto& raw_ext : first_result["extensions"])
            {
                result.extensions.push_back(parse_gallery_extension(raw_ext));
            }
        }
    }
    catch (const nlohmann::json::exception& json_err)
    {
        return std::unexpected(std::string("JSON parse error: ") + json_err.what());
    }

    return result;
}

auto ExtensionGalleryService::query(const GalleryQueryOptions& options)
    -> std::expected<GalleryQueryResult, std::string>
{
    const auto request_body = build_query_json(options);
    const auto response = HttpClient::post_json(api_endpoint_, request_body);

    if (!response.error.empty())
    {
        return std::unexpected(response.error);
    }

    if (response.status_code != 200)
    {
        return std::unexpected("Gallery API returned status " +
                               std::to_string(response.status_code));
    }

    return parse_query_response(response.body);
}

auto ExtensionGalleryService::get_extensions(const std::vector<std::string>& identifiers)
    -> std::expected<std::vector<GalleryExtension>, std::string>
{
    GalleryQueryOptions options;
    for (const auto& ext_id : identifiers)
    {
        options.filters.push_back({GalleryFilterType::kExtensionName, ext_id});
    }
    options.page_size = static_cast<std::int32_t>(identifiers.size());

    auto result = query(options);
    if (!result)
    {
        return std::unexpected(result.error());
    }

    return std::move(result.value().extensions);
}

auto ExtensionGalleryService::download(const GalleryExtension& extension,
                                       const std::string& dest_path)
    -> std::expected<void, std::string>
{
    const auto* download_url = find_asset(extension, GalleryAssetType::kDownload);
    if (download_url == nullptr || download_url->empty())
    {
        // Construct fallback download URL
        const auto fallback_url =
            "https://marketplace.visualstudio.com/_apis/public/gallery/publishers/" +
            extension.publisher + "/vsextensions/" + extension.name + "/" + extension.version +
            "/vspackage";

        return HttpClient::download(fallback_url, dest_path);
    }

    return HttpClient::download(*download_url, dest_path);
}

auto ExtensionGalleryService::get_readme(const GalleryExtension& extension)
    -> std::expected<std::string, std::string>
{
    const auto* readme_url = find_asset(extension, GalleryAssetType::kReadme);
    if (readme_url == nullptr || readme_url->empty())
    {
        return std::unexpected("No README asset available");
    }

    const auto response = HttpClient::get(*readme_url);
    if (!response.error.empty())
    {
        return std::unexpected(response.error);
    }

    if (response.status_code != 200)
    {
        return std::unexpected("Failed to fetch README: HTTP " +
                               std::to_string(response.status_code));
    }

    return response.body;
}

auto ExtensionGalleryService::get_changelog(const GalleryExtension& extension)
    -> std::expected<std::string, std::string>
{
    const auto* changelog_url = find_asset(extension, GalleryAssetType::kChangelog);
    if (changelog_url == nullptr || changelog_url->empty())
    {
        return std::unexpected("No changelog asset available");
    }

    const auto response = HttpClient::get(*changelog_url);
    if (!response.error.empty())
    {
        return std::unexpected(response.error);
    }

    if (response.status_code != 200)
    {
        return std::unexpected("Failed to fetch changelog: HTTP " +
                               std::to_string(response.status_code));
    }

    return response.body;
}

auto ExtensionGalleryService::find_asset(const GalleryExtension& extension,
                                         GalleryAssetType asset_type) -> const std::string*
{
    for (const auto& asset : extension.assets)
    {
        if (asset.type == asset_type)
        {
            return &asset.uri;
        }
    }
    return nullptr;
}

} // namespace markamp::core
