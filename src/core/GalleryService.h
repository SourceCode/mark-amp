#pragma once

#include "HttpClient.h"

#include <cstdint>
#include <expected>
#include <string>
#include <vector>

namespace markamp::core
{

// ── Gallery Data Structures ──

/// Asset types available for a gallery extension (matches VS Code).
enum class GalleryAssetType
{
    kManifest,
    kReadme,
    kChangelog,
    kLicense,
    kDownload,
    kIcon,
    kSignature
};

/// An asset URL for a gallery extension.
struct GalleryAsset
{
    GalleryAssetType type;
    std::string uri;
};

/// Property of a gallery extension.
struct GalleryProperty
{
    std::string key;
    std::string value;
};

/// A gallery extension as returned by the VS Code Marketplace API.
/// Matches VS Code's `IGalleryExtension` interface.
struct GalleryExtension
{
    std::string identifier;        // publisher.name
    std::string uuid;              // Extension GUID
    std::string name;              // Extension name
    std::string display_name;      // Human-readable name
    std::string publisher;         // Publisher identifier
    std::string publisher_display; // Publisher display name
    std::string version;           // Latest version
    std::string description;       // Short description
    std::string short_description; // Even shorter description
    std::int64_t install_count{0}; // Marketplace install count
    double rating{0.0};            // Average rating (0-5)
    std::int64_t rating_count{0};  // Number of ratings
    std::vector<std::string> categories;
    std::vector<std::string> tags;
    std::string release_date; // ISO 8601
    std::string last_updated; // ISO 8601
    std::vector<GalleryAsset> assets;
    std::vector<GalleryProperty> properties;
};

/// Sort options for gallery queries.
enum class GallerySortBy
{
    kNone = 0,
    kInstallCount = 4,
    kAverageRating = 6,
    kTitle = 2,
    kPublishedDate = 10,
    kLastUpdatedDate = 1
};

/// Sort order for gallery queries.
enum class GallerySortOrder
{
    kDefault = 0,
    kAscending = 1,
    kDescending = 2
};

/// Filter type for gallery queries.
enum class GalleryFilterType
{
    kSearchText = 10,
    kCategory = 5,
    kExtensionName = 7,
    kExtensionId = 4,
    kTag = 1
};

/// A single filter criterion for gallery queries.
struct GalleryFilter
{
    GalleryFilterType type;
    std::string value;
};

/// Options for querying the gallery.
struct GalleryQueryOptions
{
    std::vector<GalleryFilter> filters;
    GallerySortBy sort_by{GallerySortBy::kNone};
    GallerySortOrder sort_order{GallerySortOrder::kDefault};
    std::int32_t page_number{1};
    std::int32_t page_size{50};
};

/// Result of a gallery query.
struct GalleryQueryResult
{
    std::vector<GalleryExtension> extensions;
    std::int64_t total_count{0};
};

// ── Gallery Service Interface ──

/// Interface for VS Code Marketplace gallery operations.
class IExtensionGalleryService
{
public:
    virtual ~IExtensionGalleryService() = default;

    IExtensionGalleryService(const IExtensionGalleryService&) = delete;
    auto operator=(const IExtensionGalleryService&) -> IExtensionGalleryService& = delete;
    IExtensionGalleryService(IExtensionGalleryService&&) = delete;
    auto operator=(IExtensionGalleryService&&) -> IExtensionGalleryService& = delete;

    /// Query the marketplace with filter options.
    [[nodiscard]] virtual auto query(const GalleryQueryOptions& options)
        -> std::expected<GalleryQueryResult, std::string> = 0;

    /// Batch-lookup extensions by their identifiers.
    [[nodiscard]] virtual auto get_extensions(const std::vector<std::string>& identifiers)
        -> std::expected<std::vector<GalleryExtension>, std::string> = 0;

    /// Download a VSIX to a destination path.
    [[nodiscard]] virtual auto download(const GalleryExtension& extension,
                                        const std::string& dest_path)
        -> std::expected<void, std::string> = 0;

    /// Get the README markdown for an extension.
    [[nodiscard]] virtual auto get_readme(const GalleryExtension& extension)
        -> std::expected<std::string, std::string> = 0;

    /// Get the changelog markdown for an extension.
    [[nodiscard]] virtual auto get_changelog(const GalleryExtension& extension)
        -> std::expected<std::string, std::string> = 0;

protected:
    IExtensionGalleryService() = default;
};

// ── Gallery Service Implementation ──

/// Concrete implementation of IExtensionGalleryService that talks to
/// the VS Code Marketplace REST API.
class ExtensionGalleryService : public IExtensionGalleryService
{
public:
    /// Default: targets the official VS Code marketplace.
    ExtensionGalleryService();

    /// Custom endpoint (useful for testing or private registries).
    explicit ExtensionGalleryService(std::string api_endpoint);

    auto query(const GalleryQueryOptions& options)
        -> std::expected<GalleryQueryResult, std::string> override;

    auto get_extensions(const std::vector<std::string>& identifiers)
        -> std::expected<std::vector<GalleryExtension>, std::string> override;

    auto download(const GalleryExtension& extension, const std::string& dest_path)
        -> std::expected<void, std::string> override;

    auto get_readme(const GalleryExtension& extension)
        -> std::expected<std::string, std::string> override;

    auto get_changelog(const GalleryExtension& extension)
        -> std::expected<std::string, std::string> override;

    // ── Query Builder (public for testability) ──

    /// Build the JSON request body for a marketplace query.
    [[nodiscard]] static auto build_query_json(const GalleryQueryOptions& options) -> std::string;

    /// Parse a marketplace JSON response into GalleryQueryResult.
    [[nodiscard]] static auto parse_query_response(const std::string& json_body)
        -> std::expected<GalleryQueryResult, std::string>;

private:
    std::string api_endpoint_;

    /// Get a specific asset URL from a gallery extension.
    [[nodiscard]] static auto find_asset(const GalleryExtension& extension,
                                         GalleryAssetType asset_type) -> const std::string*;
};

} // namespace markamp::core
