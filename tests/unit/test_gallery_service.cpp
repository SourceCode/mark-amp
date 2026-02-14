#include "core/GalleryService.h"
#include "core/HttpClient.h"

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

using namespace markamp::core;

// ── Query Builder Tests ──

TEST_CASE("GalleryService: build query with search text", "[gallery]")
{
    GalleryQueryOptions options;
    options.filters.push_back({GalleryFilterType::kSearchText, "markdown"});
    options.page_size = 10;
    options.page_number = 1;
    options.sort_by = GallerySortBy::kInstallCount;
    options.sort_order = GallerySortOrder::kDescending;

    const auto json_str = ExtensionGalleryService::build_query_json(options);
    const auto parsed = nlohmann::json::parse(json_str);

    REQUIRE(parsed.contains("filters"));
    REQUIRE(parsed["filters"].is_array());
    REQUIRE(parsed["filters"].size() == 1);

    const auto& filter = parsed["filters"][0];
    REQUIRE(filter["pageSize"] == 10);
    REQUIRE(filter["pageNumber"] == 1);
    REQUIRE(filter["sortBy"] == static_cast<int>(GallerySortBy::kInstallCount));
    REQUIRE(filter["sortOrder"] == static_cast<int>(GallerySortOrder::kDescending));

    // Criteria should include search text + platform target
    REQUIRE(filter["criteria"].is_array());
    REQUIRE(filter["criteria"].size() == 2); // search text + platform

    bool found_search = false;
    bool found_platform = false;
    for (const auto& criterion : filter["criteria"])
    {
        if (criterion["filterType"] == 10) // SearchText
        {
            found_search = true;
            REQUIRE(criterion["value"] == "markdown");
        }
        if (criterion["filterType"] == 8) // Target
        {
            found_platform = true;
            REQUIRE(criterion["value"] == "Microsoft.VisualStudio.Code");
        }
    }
    REQUIRE(found_search);
    REQUIRE(found_platform);
}

TEST_CASE("GalleryService: build query with multiple filters", "[gallery]")
{
    GalleryQueryOptions options;
    options.filters.push_back({GalleryFilterType::kCategory, "Themes"});
    options.filters.push_back({GalleryFilterType::kTag, "dark"});

    const auto json_str = ExtensionGalleryService::build_query_json(options);
    const auto parsed = nlohmann::json::parse(json_str);

    // 2 user filters + 1 platform target
    REQUIRE(parsed["filters"][0]["criteria"].size() == 3);
}

TEST_CASE("GalleryService: build query with extension name lookup", "[gallery]")
{
    GalleryQueryOptions options;
    options.filters.push_back({GalleryFilterType::kExtensionName, "ms-python.python"});
    options.page_size = 1;

    const auto json_str = ExtensionGalleryService::build_query_json(options);
    const auto parsed = nlohmann::json::parse(json_str);

    bool found_name = false;
    for (const auto& criterion : parsed["filters"][0]["criteria"])
    {
        if (criterion["filterType"] == 7) // ExtensionName
        {
            found_name = true;
            REQUIRE(criterion["value"] == "ms-python.python");
        }
    }
    REQUIRE(found_name);
}

TEST_CASE("GalleryService: build query includes flags", "[gallery]")
{
    GalleryQueryOptions options;
    const auto json_str = ExtensionGalleryService::build_query_json(options);
    const auto parsed = nlohmann::json::parse(json_str);

    // Flags should include stats, versions, categories, files, properties
    REQUIRE(parsed.contains("flags"));
    REQUIRE(parsed["flags"].get<int>() == 0x1F);
}

// ── Response Parser Tests ──

TEST_CASE("GalleryService: parse empty response", "[gallery]")
{
    const std::string json_body = R"({
        "results": [{
            "extensions": [],
            "resultMetadata": [{
                "metadataType": "ResultCount",
                "metadataItems": [{"count": 0, "name": "TotalCount"}]
            }]
        }]
    })";

    const auto result = ExtensionGalleryService::parse_query_response(json_body);
    REQUIRE(result.has_value());
    REQUIRE(result.value().extensions.empty());
    REQUIRE(result.value().total_count == 0);
}

TEST_CASE("GalleryService: parse single extension", "[gallery]")
{
    const std::string json_body = R"({
        "results": [{
            "extensions": [{
                "extensionId": "abc-123",
                "extensionName": "markdown-all-in-one",
                "displayName": "Markdown All in One",
                "shortDescription": "All you need to write Markdown",
                "publisher": {
                    "publisherName": "yzhang",
                    "displayName": "Yu Zhang"
                },
                "versions": [{
                    "version": "3.5.0",
                    "files": [
                        {
                            "assetType": "Microsoft.VisualStudio.Services.VSIXPackage",
                            "source": "https://example.com/download.vsix"
                        },
                        {
                            "assetType": "Microsoft.VisualStudio.Services.Content.Details",
                            "source": "https://example.com/readme.md"
                        }
                    ],
                    "properties": [
                        {"key": "Microsoft.VisualStudio.Code.Engine", "value": "^1.75.0"},
                        {"key": "Microsoft.VisualStudio.Code.ExtensionDependencies", "value": ""}
                    ]
                }],
                "statistics": [
                    {"statisticName": "install", "value": 5000000.0},
                    {"statisticName": "averagerating", "value": 4.6},
                    {"statisticName": "ratingcount", "value": 250.0}
                ],
                "categories": ["Programming Languages", "Formatters"],
                "tags": ["markdown", "md", "toc"],
                "releaseDate": "2017-01-01T00:00:00Z",
                "lastUpdated": "2023-06-15T00:00:00Z"
            }],
            "resultMetadata": [{
                "metadataType": "ResultCount",
                "metadataItems": [{"count": 1, "name": "TotalCount"}]
            }]
        }]
    })";

    const auto result = ExtensionGalleryService::parse_query_response(json_body);
    REQUIRE(result.has_value());
    REQUIRE(result.value().extensions.size() == 1);
    REQUIRE(result.value().total_count == 1);

    const auto& ext = result.value().extensions[0];
    REQUIRE(ext.name == "markdown-all-in-one");
    REQUIRE(ext.display_name == "Markdown All in One");
    REQUIRE(ext.publisher == "yzhang");
    REQUIRE(ext.publisher_display == "Yu Zhang");
    REQUIRE(ext.identifier == "yzhang.markdown-all-in-one");
    REQUIRE(ext.uuid == "abc-123");
    REQUIRE(ext.version == "3.5.0");
    REQUIRE(ext.install_count == 5000000);
    REQUIRE(ext.rating == 4.6);
    REQUIRE(ext.rating_count == 250);
    REQUIRE(ext.categories.size() == 2);
    REQUIRE(ext.tags.size() == 3);
    REQUIRE(ext.assets.size() == 2);
    REQUIRE(ext.properties.size() == 2);
    REQUIRE(ext.release_date == "2017-01-01T00:00:00Z");
    REQUIRE(ext.last_updated == "2023-06-15T00:00:00Z");
}

TEST_CASE("GalleryService: parse multiple extensions", "[gallery]")
{
    const std::string json_body = R"({
        "results": [{
            "extensions": [
                {
                    "extensionName": "ext-a",
                    "publisher": {"publisherName": "pub-a", "displayName": "Pub A"},
                    "versions": [{"version": "1.0.0"}]
                },
                {
                    "extensionName": "ext-b",
                    "publisher": {"publisherName": "pub-b", "displayName": "Pub B"},
                    "versions": [{"version": "2.0.0"}]
                }
            ],
            "resultMetadata": [{
                "metadataType": "ResultCount",
                "metadataItems": [{"count": 2, "name": "TotalCount"}]
            }]
        }]
    })";

    const auto result = ExtensionGalleryService::parse_query_response(json_body);
    REQUIRE(result.has_value());
    REQUIRE(result.value().extensions.size() == 2);
    REQUIRE(result.value().extensions[0].identifier == "pub-a.ext-a");
    REQUIRE(result.value().extensions[1].identifier == "pub-b.ext-b");
}

TEST_CASE("GalleryService: parse invalid JSON", "[gallery]")
{
    const auto result = ExtensionGalleryService::parse_query_response("not json");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("GalleryService: parse missing results", "[gallery]")
{
    const auto result = ExtensionGalleryService::parse_query_response(R"({"foo": "bar"})");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("GalleryService: parse empty results array", "[gallery]")
{
    const auto result = ExtensionGalleryService::parse_query_response(R"({"results": []})");
    REQUIRE_FALSE(result.has_value());
}

// ── HttpResponse / HttpClient structure tests ──

TEST_CASE("HttpResponse default values", "[gallery]")
{
    HttpResponse resp;
    REQUIRE(resp.status_code == 0);
    REQUIRE(resp.body.empty());
    REQUIRE(resp.headers.empty());
    REQUIRE(resp.error.empty());
}

TEST_CASE("GalleryExtension default values", "[gallery]")
{
    GalleryExtension ext;
    REQUIRE(ext.identifier.empty());
    REQUIRE(ext.install_count == 0);
    REQUIRE(ext.rating == 0.0);
    REQUIRE(ext.rating_count == 0);
    REQUIRE(ext.assets.empty());
    REQUIRE(ext.properties.empty());
}

// ── GalleryQueryOptions defaults ──

TEST_CASE("GalleryQueryOptions defaults", "[gallery]")
{
    GalleryQueryOptions options;
    REQUIRE(options.filters.empty());
    REQUIRE(options.sort_by == GallerySortBy::kNone);
    REQUIRE(options.sort_order == GallerySortOrder::kDefault);
    REQUIRE(options.page_number == 1);
    REQUIRE(options.page_size == 50);
}
